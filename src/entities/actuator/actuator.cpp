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

Actuator::Actuator(const quint16 &simBluePort, const quint16 &simYellowPort)
    : _simBluePort(simBluePort)
    , _simYellowPort(simYellowPort)
{
    // Default set actuator socket to nullptr
    _actuatorBlueSocket = nullptr;
    _actuatorYellowSocket = nullptr;

    // Setup timer
    _actuatorTimer = new QTimer();
    QObject::connect(_actuatorTimer, &QTimer::timeout, this, &Actuator::sendControlPacketsToNetwork);
    _actuatorTimer->start(CONTROL_PACKET_SEND_INTERVAL_MS);
}

Actuator::~Actuator() {
    // Disconnect the vision socket from host
    _actuatorBlueSocket->disconnectFromHost();
    _actuatorYellowSocket->disconnectFromHost();

    // Delete timer and socket
    delete _actuatorTimer;
    delete _actuatorBlueSocket;
    delete _actuatorYellowSocket;
}

void Actuator::connectToNetwork(const QHostAddress& simHostAddress) {
    // Create socket
    _actuatorBlueSocket = new QUdpSocket();
    _actuatorYellowSocket = new QUdpSocket();

    // Set sim host address
    _simHostAddress = simHostAddress.toString();

    // Bind socket
    _actuatorBlueSocket->connectToHost(_simHostAddress, _simBluePort);
    _actuatorYellowSocket->connectToHost(_simHostAddress, _simYellowPort);

    // Wait for connected status
    if (!_actuatorBlueSocket->waitForConnected()) {
        spdlog::critical("Could not connect to blue actuator at address '{}' and port '{}'.",
                         _simHostAddress.toStdString(),
                         _simBluePort);
    } else {
        spdlog::info("Connected to blue actuator at address '{}' and port '{}'.",
                     _simHostAddress.toStdString(),
                     _simBluePort);
    }
    if (!_actuatorYellowSocket->waitForConnected()) {
        spdlog::critical("Could not connect to yellow actuator at address '{}' and port '{}'.",
                         _simHostAddress.toStdString(),
                         _simYellowPort);
    } else {
        spdlog::info("Connected to yellow actuator at address '{}' and port '{}'.",
                     _simHostAddress.toStdString(),
                     _simYellowPort);
    }
}

void Actuator::sendControlPacketsToNetwork() {
    // If not have created the socket yet, just discard the control packets
    if (_actuatorBlueSocket == nullptr || _actuatorYellowSocket == nullptr) {
        _actuatorMutex.lock();
        _controlPackets.clear();
        _actuatorMutex.unlock();

        return ;
    }

    // Creating packet
    RobotControl bluePacket;
    RobotControl yellowPacket;

    // Locking mutex, reading from control packets and setting commands
    _actuatorMutex.lock();
    for (const auto &pkt : _controlPackets) {
        if (pkt.isTeamBlue()) {
            RobotCommand *command = bluePacket.add_robot_commands();

            // Setting macro informations (team and robot id)
            command->set_id(pkt.getPlayerId());

            // Set wheels speed
            MoveLocalVelocity *localVel = command->mutable_move_command()->mutable_local_velocity();
            localVel->set_forward(pkt.forwardSpeed());
            localVel->set_left(pkt.leftSpeed());
            localVel->set_angular(pkt.angularSpeed());

            // Set kick parameters
            command->set_kick_speed(pkt.kickSpeed());
            command->set_kick_angle(pkt.chipKick());

            // Set dribble parameters
            command->set_dribbler_speed(pkt.dribblerSpeed());
        } else {
            RobotCommand *command = yellowPacket.add_robot_commands();

            // Setting macro informations (team and robot id)
            command->set_id(pkt.getPlayerId());

            // Set wheels speed
            MoveLocalVelocity *localVel = command->mutable_move_command()->mutable_local_velocity();
            localVel->set_forward(pkt.forwardSpeed());
            localVel->set_left(pkt.leftSpeed());
            localVel->set_angular(pkt.angularSpeed());

            // Set kick parameters
            command->set_kick_speed(pkt.kickSpeed());
            command->set_kick_angle(pkt.chipKick());

            // Set dribble parameters
            command->set_dribbler_speed(pkt.dribblerSpeed());
        }
    }

    // Clear packets
    _controlPackets.clear();

    // Unlock mutex
    _actuatorMutex.unlock();

    // Parse to datagram
    QByteArray blueBuffer(bluePacket.ByteSizeLong(), 0);
    bluePacket.SerializeToArray(blueBuffer.data(), blueBuffer.size());
    QNetworkDatagram blueDatagram(blueBuffer);

    QByteArray yellowBuffer(yellowPacket.ByteSizeLong(), 0);
    yellowPacket.SerializeToArray(yellowBuffer.data(), yellowBuffer.size());
    QNetworkDatagram yellowDatagram(yellowBuffer);

    // Try to send data to simulator
    bool sentBlueDatagram = (_actuatorBlueSocket->writeDatagram(blueDatagram) != -1);
    bool sentYellowDatagram = (_actuatorYellowSocket->writeDatagram(yellowDatagram) != -1);
    if (!sentBlueDatagram) {
        spdlog::warn("Failed to sent blue datagram to simulator. Is simulator running?");
    }
    if (!sentYellowDatagram) {
        spdlog::warn("Failed to sent yellow datagram to simulator. Is simulator running?");
    }
}

void Actuator::receiveControlPacket(const RobotControlPacket& robotControlPacket) {
    // Lock mutex and insert packet into control packets list
    _actuatorMutex.lock();
    _controlPackets.push_back(robotControlPacket);
    _actuatorMutex.unlock();
}
