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
#include <src/utils/utils.h>

Player::Player(const bool& isTeamBlue, const quint8& playerId)
    : _isTeamBlue(isTeamBlue), _playerId(playerId)
{
    _position = OUT_OF_FIELD;
    _orientation = 0.0f;
    _missingPackets = 0;
    _lastError = 0.0f;
    _cumulativeError = 0.0f;
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

void Player::goTo(const QVector2D &targetPosition) {
    // Define speed and reverse vars
    float rightMotorSpeed;
    float leftMotorSpeed;
    bool reversed = false;

    // Get normalized robot angle and angle to target
    float robotAngle = Utils::normalizeAngle(getOrientation());
    float angleToTarget = Utils::getAngle(this->getPosition(), targetPosition);

    // Check if front of robot can be reversed
    if(Utils::checkIfCanBeReversed(robotAngle, angleToTarget)) {
        // Reverse the front of the robot
        reversed = true;
        robotAngle = Utils::normalizeAngle(robotAngle + M_PI);
    }

    // Calculate error
    float angError = Utils::smallestAngleDiff(robotAngle, angleToTarget);

    // Get PID output and update error
    float motorSpeed = (KP * angError) + (KI * _cumulativeError) + (KD * (angError - _lastError));
    _lastError = angError;
    _cumulativeError += angError;

    // Normalize
    motorSpeed = motorSpeed > BASE_SPEED ? BASE_SPEED : motorSpeed;
    motorSpeed = motorSpeed < -BASE_SPEED ? -BASE_SPEED : motorSpeed;

    if (!reversed) {
        if (motorSpeed > 0) {
            leftMotorSpeed = BASE_SPEED ;
            rightMotorSpeed = BASE_SPEED - motorSpeed;
        } else {
            leftMotorSpeed = BASE_SPEED + motorSpeed;
            rightMotorSpeed = BASE_SPEED;
        }
    } else {
        if (motorSpeed > 0) {
            leftMotorSpeed = -BASE_SPEED + motorSpeed;
            rightMotorSpeed = -BASE_SPEED ;
        } else {
            leftMotorSpeed = -BASE_SPEED ;
            rightMotorSpeed = -BASE_SPEED - motorSpeed;
        }
    }

    emit sendControlPacket(RobotControlPacket(isTeamBlue(), getPlayerId(),
                                              leftMotorSpeed, rightMotorSpeed));
}

void Player::rotateTo(const QVector2D &targetPosition) {
    this->rotateTo(Utils::getAngle(this->getPosition(), targetPosition));
}

void Player::rotateTo(const float &orientation) {
    // Define speed and reverse vars
    float rightMotorSpeed;
    float leftMotorSpeed;
    bool reversed = false;

    // Get normalized robot angle and angle to target
    float robotAngle = Utils::normalizeAngle(getOrientation());

    // Check if front of robot can be reversed
    if(Utils::checkIfCanBeReversed(robotAngle, orientation)) {
        // Reverse the front of the robot
        reversed = true;
        robotAngle = Utils::normalizeAngle(robotAngle + M_PI);
    }

    // Calculate error
    float angError = Utils::smallestAngleDiff(robotAngle, orientation);

    // Get PID output and update error
    float motorSpeed = (KP * angError) + (KI * _cumulativeError) + (KD * (angError - _lastError));
    _lastError = angError;
    _cumulativeError += angError;

    // The standard movement of the robots will be a clockwise rotation
    leftMotorSpeed = motorSpeed ;
    rightMotorSpeed = -motorSpeed;

    if (reversed) {
        leftMotorSpeed = -motorSpeed ;
        rightMotorSpeed = motorSpeed;
    }

    emit sendControlPacket(RobotControlPacket(isTeamBlue(), getPlayerId(),
                                              leftMotorSpeed, rightMotorSpeed));
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
