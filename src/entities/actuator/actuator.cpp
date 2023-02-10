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

#include "actuator.h"

#include <spdlog/spdlog.h>

Actuator::Actuator(const quint16& simPort)
    : _simPort(simPort)
{
    // Default set actuator socket to nullptr
    _actuatorSocket = nullptr;

    // Setup timer
    _actuatorTimer = new QTimer();
    QObject::connect(_actuatorTimer, &QTimer::timeout, this, &Actuator::sendControlPacketsToNetwork);
    _actuatorTimer->start(CONTROL_PACKET_SEND_INTERVAL_MS);
}

Actuator::~Actuator() {
    // Disconnect the vision socket from host
    _actuatorSocket->disconnectFromHost();

    // Delete timer and socket
    delete _actuatorTimer;
    delete _actuatorSocket;
}

void Actuator::connectToNetwork(const QHostAddress& simHostAddress) {
    // Create socket
    _actuatorSocket = new QUdpSocket();

    // Set sim host address
    _simHostAddress = simHostAddress.toString();

    // Bind socket
    _actuatorSocket->connectToHost(_simHostAddress, _simPort);

    // Wait for connected status
    if(!_actuatorSocket->waitForConnected()) {
        spdlog::critical("Could not connect to actuator at address '{}' and port '{}'.",
                                 _simHostAddress.toStdString(), _simPort);
    }
    else {
        spdlog::info("Connected to actuator at address '{}' and port '{}'.",
                             _simHostAddress.toStdString(), _simPort);
    }
}

void Actuator::sendControlPacketsToNetwork() {
    // If not have created the socket yet, just discard the control packets
    if(_actuatorSocket == nullptr) {
        _actuatorMutex.lock();
        _controlPackets.clear();
        _actuatorMutex.unlock();

        return ;
    }

    // Creating packet
    fira_message::sim_to_ref::Packet packet;

    // Locking mutex, reading from control packets and setting commands
    _actuatorMutex.lock();
    for (const auto& pkt : _controlPackets) {
        fira_message::sim_to_ref::Command *command = packet.mutable_cmd()->add_robot_commands();

        // Setting macro informations (team and robot id)
        command->set_id(pkt.getPlayerId());
        command->set_yellowteam(!pkt.isTeamBlue());

        // Set wheels speed
        command->set_wheel_left(pkt.getWheelLeft());
        command->set_wheel_right(pkt.getWheelRight());
    }

    // Clear packets
    _controlPackets.clear();

    // Unlock mutex
    _actuatorMutex.unlock();

    // Parse to datagram
    QByteArray buffer(packet.ByteSizeLong(), 0);
    packet.SerializeToArray(buffer.data(), buffer.size());
    QNetworkDatagram datagram(buffer);

    // Try to send data to simulator
    bool sentDatagram = (_actuatorSocket->writeDatagram(datagram) != -1);
    if(!sentDatagram) {
        spdlog::warn("Failed to sent datagram to simulator. Is simulator running?");
    }
}

void Actuator::receiveControlPacket(const RobotControlPacket& robotControlPacket) {
    // Lock mutex and insert packet into control packets list
    _actuatorMutex.lock();
    _controlPackets.push_back(robotControlPacket);
    _actuatorMutex.unlock();
}
