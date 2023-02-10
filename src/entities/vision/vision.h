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

#ifndef VISION_H
#define VISION_H

#include <QTimer>
#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QNetworkInterface>

#include <include/proto/packet.pb.h>
#include <src/utils/types/robotdetectionpacket/robotdetectionpacket.h>

/*!
 * \brief The Vision class provides a implementation to receive vision data from the simulator,
 * allowing to obtain information both of field objects (such as the ball or robots) and also
 * from the field geometry (length, width, etc.).
 */
class Vision : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Vision class constructor. It receives the network address to access the vision server
     * and make the connections to enable data reception.
     * \param visionAddress The vision network address.
     * \param visionPort The vision network port.
     */
    Vision(const QString& visionAddress = "224.0.0.1", const quint16& visionPort = 10002);

    /*!
     * \brief Vision class destructor. It disconnects from the vision network and destroys the
     * created socket and deallocate objects if any.
     */
    ~Vision();

protected:
    /*!
     * \return The vision address that was given in the constructor.
     */
    QString getVisionAddress() const;

    /*!
     * \return The vision port that was given in the constructor.
     */
    quint16 getVisionPort() const;

private:
    // Internal network address variables
    QString _visionAddress;
    quint16 _visionPort;

    // Socket and timer for data acquisition
    QUdpSocket *_visionSocket;

    // Interface discovery
    QTimer *_interfaceDiscoverTimer;
    bool _receivedAnyPackets = false;
    int _interfaceIndex = 0;

signals:
    /*!
     * \brief Send a robot detection packet signal to connected QObjects.
     * \param robotDetectionPacket The robot detection packet which will be sent.
     */
    void sendRobotDetection(const RobotDetectionPacket& robotDetectionPacket);

    /*!
     * \brief Send a ball detection packet signal to connected QObjects.
     * \param ballDetectionPacket The ball detection packet which will be sent.
     */
    void sendBallDetection(const fira_message::Ball& ballDetectionPacket);

    /*!
     * \brief Send a field detection packet signal to connected QObjects.
     * \param fieldDetectionPacket The field detection packet which will be sent.
     */
    void sendFieldDetection(const fira_message::Field& fieldDetectionPacket);

    /*!
     * \brief Send the given host address to further connection with the simulator environment.
     * \param hostAddress The given host address.
     */
    void sendHostAddress(const QHostAddress& hostAddress);

private slots:
    /*!
     * \brief Receive packets from the socket connected to the Vision server.
     * \note This slot is called when the socket is in the ReadyRead state.
     */
    void receivePackets();

    /*!
     * \brief Makes the connection to network using the given network address and port.
     * \return A boolean that indicates if the connection was succesfully or not.
     */
    void connectToNetwork();
};

#endif // VISION_H
