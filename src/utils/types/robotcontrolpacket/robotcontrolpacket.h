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


#ifndef ROBOTCONTROLPACKET_H
#define ROBOTCONTROLPACKET_H

#include <QtGlobal>

/*!
 * \brief The RobotControlpacket class provides an interface to enhance robot control packet data,
 * allowing to center all information to control a robot in the field (team color, player id and
 * wheels).
 */
class RobotControlPacket
{
public:
    /*!
     * \brief RobotControlPacket class constructor. It receives all the required data to control
     * a robot.
     * \param isTeamBlue True if the control packet is sent to the blue team and False otherwise.
     * \param playerId The player id which this packet is aimed for.
     * \param wheelLeft The wheel left speed (rad/s).
     * \param wheelRight The wheel right speed (rad/s).
     */
    RobotControlPacket(const bool& isTeamBlue, const quint8& playerId,
                       const float& wheelLeft, const float& wheelRight);

    /*!
     * \return True if the control packet is aimed to team blue and False otherwise.
     */
    bool isTeamBlue() const;

    /*!
     * \return The player id related to this control packet.
     */
    quint8 getPlayerId() const;

    /*!
     * \return The wheel left speed (rad/s) that was set in this command.
     */
    float getWheelLeft() const;

    /*!
     * \return The wheel right speed (rad/s) that was set in this command.
     */
    float getWheelRight() const;

private:
    // Internal vars
    bool _isTeamBlue;
    quint8 _playerId;
    float _wheelLeft;
    float _wheelRight;
};

#endif // ROBOTCONTROLPACKET_H
