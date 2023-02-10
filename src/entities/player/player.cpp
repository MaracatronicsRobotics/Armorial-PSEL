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

#include "player.h"

#include <spdlog/spdlog.h>

Player::Player(const bool& isTeamBlue, const quint8& playerId)
    : _isTeamBlue(isTeamBlue), _playerId(playerId)
{
    _position = OUT_OF_FIELD;
    _orientation = 0.0f;
    _missingPackets = 0;
}

bool Player::isMissing() const {
    return (_position == OUT_OF_FIELD);
}

QVector2D Player::getPosition() const {
    return _position;
}

float Player::getOrientation() const {
    return _orientation;
}

bool Player::isTeamBlue() const {
    return _isTeamBlue;
}

quint8 Player::getPlayerId() const {
    return _playerId;
}

void Player::updateFromDetection(const RobotDetectionPacket& robotDetectionPacket) {
    if(robotDetectionPacket.isTeamBlue() != this->isTeamBlue()) return;

    // Get robot detection packet and parse
    fira_message::Robot robotDetectPacket = robotDetectionPacket.getRobotDetectionPacket();
    if(robotDetectPacket.robot_id() != this->getPlayerId()) {
        if(++_missingPackets >= PACKETS_TILL_MISSING) {
            _position = OUT_OF_FIELD;
            _orientation = 0.0f;
        }
        return ;
    }

    // Update detection
    _missingPackets = 0;
    _position = QVector2D(robotDetectPacket.x(), robotDetectPacket.y());
    _orientation = robotDetectPacket.orientation();
}
