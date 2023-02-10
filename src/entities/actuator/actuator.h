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

#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <QMutex>
#include <QTimer>
#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QNetworkInterface>

#include <include/proto/packet.pb.h>

#include <src/utils/types/robotcontrolpacket/robotcontrolpacket.h>

#define CONTROL_PACKET_SEND_INTERVAL_MS 16

/*!
 * \brief The Actuator class provides an interface to control the robots in the simulated
 * environment.
 */
class Actuator : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Actuator class constructor. It receives the host port to connect and send packets.
     * \param simPort The sim port to connect.
     * \note The address will be taken from the Vision module, so the network connection will be
     * made later.
     */
    Actuator(const quint16& simPort = 20011);

    /*!
     * \brief Actuator class destructor.
     */
    ~Actuator();

private:
    // Internal network address variables
    QString _simHostAddress;
    quint16 _simPort;

    // Socket implementation to send data
    QUdpSocket *_actuatorSocket;

    // QTimer implementation for sync
    QTimer *_actuatorTimer;

    // Control packet data management
    QList<RobotControlPacket> _controlPackets;
    QMutex _actuatorMutex;

public slots:
    /*!
     * \brief Receive a control packet from a Player signal implementation.
     * \param robotControlPacket The robot control packet which will be sent.
     */
    void receiveControlPacket(const RobotControlPacket& robotControlPacket);

    /*!
     * \brief Connect to the sim network with the given sim host address
     * \param simHostAddress The given sim host address.
     */
    void connectToNetwork(const QHostAddress& simHostAddress);

private slots:
    /*!
     * \brief Send control packets to network through the socket.
     * \note This method should be called/connected with the QTimer timeout signal.
     */
    void sendControlPacketsToNetwork();
};

#endif // ACTUATOR_H
