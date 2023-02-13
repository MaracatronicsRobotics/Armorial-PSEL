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

#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QVector2D>

#include <include/proto/packet.pb.h>

#include <src/utils/types/robotcontrolpacket/robotcontrolpacket.h>
#include <src/utils/types/robotdetectionpacket/robotdetectionpacket.h>

// Constants for Player detection
static constexpr QVector2D OUT_OF_FIELD = QVector2D(std::numeric_limits<float>::max(),
                                                    std::numeric_limits<float>::max());
static constexpr int PACKETS_TILL_MISSING = 60;
static constexpr float KP = 25.0f;
static constexpr float KI = 0.0f;
static constexpr float KD = 2.5f;
static constexpr float BASE_SPEED = 30.0f;

/*!
 * \brief The Player class provides a implementation to manage a robot in the field, providing
 * from simple getter methods (for position, id, orientation) to the robot control.
 */
class Player : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Player class constructor.
     * \param isTeamBlue If this player belongs to the team blue.
     * \param playerId This Player instance id.
     */
    Player(const bool& isTeamBlue, const quint8& playerId);

    /*!
     * \return True if the player is missing from detection and False otherwise.
     */
    bool isMissing() const;

    /*!
     * \return This Player instance object position at the field.
     */
    QVector2D getPosition() const;

    /*!
     * \return This Player instance object orientation at the field.
     * \note For reference:
     *             pi/2   pi/4
     *               |   /
     *               |  /
     *               | /
     *               |/
     * +- pi ------------------ 0
     *               |\
     *               | \
     *               |  \
     *               |   \
     *            -pi/2  -pi/4
     */
    float getOrientation() const;

    /*!
     * \return True if this Player instance belongs to team blue and False otherwise.
     */
    bool isTeamBlue() const;

    /*!
     * \return This Player instance id.
     */
    quint8 getPlayerId() const;

protected:
    // Mark Coach as a friend class so it can call this methods from Player
    friend class Coach;

    /*!
     * \brief Make this Player go to a given target position.
     * \param targetPosition The given target position.
     */
    void goTo(const QVector2D& targetPosition);

    /*!
     * \brief Make this Player rotate to a given target position.
     * \param targetPosition The given target position.
     */
    void rotateTo(const QVector2D& targetPosition);

    /*!
     * \brief Make this Player rotate to a given orientation.
     * \param orientation The given orientation.
     */
    void rotateTo(const float& orientation);

private:
    // Player internal variables
    QVector2D _position;
    float _orientation;
    bool _isTeamBlue;
    quint8 _playerId;

    // Internal detection management
    int _missingPackets;

    // PID
    float _lastError;
    float _cumulativeError;

signals:
    /*!
     * \brief Send robot control packet to the Actuator implementation.
     * \param robotControlPacket The given robot control packet.
     */
    void sendControlPacket(const RobotControlPacket& robotControlPacket);

public slots:
    /*!
     * \brief Update this Player instance with a given detection packet.
     * \param robotDetectionPacket The given detection packet.
     */
    void updateFromDetection(const RobotDetectionPacket& robotDetectionPacket);
};

#endif // PLAYER_H
