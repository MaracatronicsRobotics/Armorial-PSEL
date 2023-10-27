/***
 * Maracatronics Robotics
 * Federal University of Pernambuco (UFPE) at Recife
 * http://www.maracatronics.com/
 *
 * This file is part of Armorial project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#include "vision.h"

#include <include/proto/ssl_vision_wrapper.pb.h>
#include <spdlog/spdlog.h>

Vision::Vision(const QString& visionAddress, const quint16& visionPort)
    : _visionAddress(visionAddress), _visionPort(visionPort)
{
    // Allocate vision socket pointer
    _visionSocket = new QUdpSocket();

    // Bind socket
    _visionSocket->bind(QHostAddress::AnyIPv4, getVisionPort(), QUdpSocket::ShareAddress);

    // Connects interface discover timer
    _interfaceDiscoverTimer = new QTimer();
    QObject::connect(_interfaceDiscoverTimer, &QTimer::timeout, this, &Vision::connectToNetwork);
    _interfaceDiscoverTimer->start(200);
}

Vision::~Vision() {
    // Disconnect the vision socket from host
    _visionSocket->disconnectFromHost();

    // Delete timer and socket
    delete _interfaceDiscoverTimer;
    delete _visionSocket;
}

QString Vision::getVisionAddress() const {
    return _visionAddress;
}

quint16 Vision::getVisionPort() const {
    return _visionPort;
}

void Vision::receivePackets() {
    // If reached here, the QUdpSocket has received a packet from the network. So, we mark the
    // received any packets variable as true and disconnect the interface discover timer signal.
    if(!_receivedAnyPackets) {
        // Mark as received packets
        _receivedAnyPackets = true;
        _interfaceDiscoverTimer->disconnect(SIGNAL(timeout()));
        spdlog::info("Received vision packet!");

        // Discover and get host address
        QList<QNetworkInterface> availableInterfaces = QNetworkInterface::allInterfaces();
        const QNetworkAddressEntry addrEntry = availableInterfaces.at(_interfaceIndex - 1)
                                                .addressEntries().constFirst();
        emit sendHostAddress(addrEntry.broadcast());
    }

    // Process the pending packets
    while(_visionSocket->hasPendingDatagrams()) {
        // Take datagram
        QNetworkDatagram datagram = _visionSocket->receiveDatagram();

        // Check if datagram is valid
        if(!datagram.isValid()) continue;

        // Try to parse to detection packet
        SSL_WrapperPacket wrapperPacket;

        if (wrapperPacket.ParseFromArray(datagram.data().constData(), datagram.data().size())) {
            if (wrapperPacket.has_detection()) {
                // Get frame object
                SSL_DetectionFrame visionFrame = wrapperPacket.detection();

                // Send detected ball
                if (visionFrame.balls_size()) {
                    SSL_DetectionBall ball = visionFrame.balls(0);

                    emit sendBallDetection(ball);
                }

                // Send blue robots
                for (int i = 0; i < visionFrame.robots_blue_size(); i++) {
                    SSL_DetectionRobot blueRobot = visionFrame.robots_blue(i);

                    emit sendRobotDetection(RobotDetectionPacket(true, blueRobot));
                }

                // Send yellow robots
                for (int i = 0; i < visionFrame.robots_yellow_size(); i++) {
                    SSL_DetectionRobot yellowRobot = visionFrame.robots_yellow(i);

                    emit sendRobotDetection(RobotDetectionPacket(false, yellowRobot));
                }
            }

            // Send field geometry
            if (wrapperPacket.has_geometry()) {
                SSL_GeometryData fieldGeometry = wrapperPacket.geometry();

                emit sendFieldDetection(fieldGeometry);
            }
        } else {
            spdlog::warn("Failed to convert vision datagram");
            continue;
        }
    }
}

void Vision::connectToNetwork() {
    // If has already connected to a valid interface, ignore
    if(_receivedAnyPackets) return ;

    // Try to found and connect the vision interface
    QList<QNetworkInterface> availableInterfaces = QNetworkInterface::allInterfaces();

    // Debug when not received any packet from the last try
    if(_interfaceIndex != 0) {
        spdlog::warn("Could not receive any vision packets from Vision at address '{}' and interface '{}'.",
                     getVisionAddress().toStdString(),
                     availableInterfaces.at(_interfaceIndex - 1).humanReadableName().toStdString());
    }

    // If could not connected to any interface, ignore
    if(_interfaceIndex >= availableInterfaces.size()) {
        spdlog::critical("Could not receive vision packets in any network interface.");
        exit(-1);
    }

    // Get the next interface to try connection
    const QNetworkInterface interface = availableInterfaces.at(_interfaceIndex);
    QHostAddress visionHostAddress(getVisionAddress());
    spdlog::info("Trying to connect to Vision at address '{}' and interface '{}'.",
                 getVisionAddress().toStdString(), interface.humanReadableName().toStdString());

    // Disconnect the readyRead signal from the vision socket and clear pending datagrams
    _visionSocket->disconnect(SIGNAL(readyRead()));
    while(_visionSocket->hasPendingDatagrams()) _visionSocket->receiveDatagram();

    // Try to join the multicast group with the given address and interface
    if(_visionSocket->joinMulticastGroup(visionHostAddress, interface)) {
        _receivedAnyPackets = false;
        spdlog::info("Connected to Vision at address '{}', port '{}' and interface '{}'.",
                     getVisionAddress().toStdString(), getVisionPort(),
                     interface.humanReadableName().toStdString());
        QObject::connect(_visionSocket, &QUdpSocket::readyRead, this, &Vision::receivePackets);
    }
    else {
        spdlog::error("Failed to join vision multicast group at address '{}' and interface '{}'.",
                      getVisionAddress().toStdString(),
                      interface.humanReadableName().toStdString());
    }

    _interfaceIndex++;
}
