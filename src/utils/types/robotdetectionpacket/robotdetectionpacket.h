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


#ifndef ROBOTDETECTIONPACKET_H
#define ROBOTDETECTIONPACKET_H

#include <include/proto/ssl_vision_wrapper.pb.h>

/*!
 * \brief The RobotDetectionPacket class provides an interface to enhance fira_message::Robot packet
 * adding the team color data.
 */
class RobotDetectionPacket
{
public:
    /*!
     * \brief RobotDetectionPacket class constructor.
     * \param isTeamBlue True if the detection packet is from team blue and False otherwise.
     * \param robotDetectionPacket The robot detection packet itself (from Vision server).
     */
    RobotDetectionPacket(const bool &isTeamBlue, const SSL_DetectionRobot &robotDetectionPacket);

    /*!
     * \return True if the robot detection packet is from team blue and False otherwise.
     */
    bool isTeamBlue() const;

    /*!
     * \return The robot detection packet associated with this class.
     */
    SSL_DetectionRobot getRobotDetectionPacket() const;

private:
    // Internal vars
    bool _isTeamBlue;
    SSL_DetectionRobot _robotDetectionPacket;
};

#endif // ROBOTDETECTIONPACKET_H
