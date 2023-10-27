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


#include "robotcontrolpacket.h"

RobotControlPacket::RobotControlPacket(const bool &isTeamBlue,
                                       const quint8 &playerId,
                                       const float &forward,
                                       const float &left,
                                       const float &angular,
                                       const float &kickSpeed,
                                       const float &chipKick,
                                       const float &dribblerSpeed)
    : _isTeamBlue(isTeamBlue)
    , _playerId(playerId)
    , _forward(forward)
    , _left(left)
    , _angular(angular)
    , _kickSpeed(kickSpeed)
    , _chipKick(chipKick)
    , _dribblerSpeed(dribblerSpeed)
{

}

bool RobotControlPacket::isTeamBlue() const {
    return _isTeamBlue;
}

quint8 RobotControlPacket::getPlayerId() const {
    return _playerId;
}

float RobotControlPacket::forwardSpeed() const
{
    return _forward;
}

float RobotControlPacket::leftSpeed() const
{
    return _left;
}

float RobotControlPacket::angularSpeed() const
{
    return _angular;
}

float RobotControlPacket::kickSpeed() const
{
    return _kickSpeed;
}

float RobotControlPacket::chipKick() const
{
    return _chipKick;
}

float RobotControlPacket::dribblerSpeed() const
{
    return _dribblerSpeed;
}
