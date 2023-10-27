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
    _controlPacket = new RobotControlPacket(isTeamBlue, playerId, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
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

float normalize(const float &angle)
{
    float _angle = angle;
    while (_angle > M_PI) {
        _angle -= 2 * M_PI;
    }
    while (_angle < -M_PI) {
        _angle += 2 * M_PI;
    }

    return _angle;
}

void Player::goTo(const QVector2D &targetPosition)
{
    QVector2D error = targetPosition - this->getPosition();
    QVector2D desiredVelocity = error * LINEAR_KP;

    const QVector2D robotYVector(cos(this->getOrientation()), sin(this->getOrientation()));
    const float c = cos(-M_PI / 2);
    const float s = sin(-M_PI / 2);
    const QVector2D robotXVector = QVector2D(robotYVector.x() * c - robotYVector.y() * s,
                                             robotYVector.x() * s + robotYVector.y() * c);

    const float angleX = normalize(acos(QVector2D::dotProduct(robotXVector, desiredVelocity)
                                        / (robotXVector.length() * desiredVelocity.length())));
    const float angleY = normalize(acos(QVector2D::dotProduct(robotYVector, desiredVelocity)
                                        / (robotYVector.length() * desiredVelocity.length())));

    if (isnanf(angleX) || isnanf(angleY)) {
        return;
    }

    QVector2D decomposedVel(desiredVelocity.length() * cos(angleX),
                            desiredVelocity.length() * cos(angleY));

    if (decomposedVel.length() > MAX_SPEED) {
        decomposedVel = decomposedVel.normalized() * MAX_SPEED;
    }

    _controlMutex.lock();
    _controlPacket->setForwardSpeed(decomposedVel.y());
    _controlPacket->setLeftSpeed(-decomposedVel.x());
    _controlMutex.unlock();

    emit sendControlPacket(*_controlPacket);
}

void Player::rotateTo(const QVector2D &targetPosition)
{
    QVector2D distToTarget = targetPosition - this->getPosition();

    float playerAngle = this->getOrientation();
    float targetAngle = atan2(distToTarget.y(), distToTarget.x());

    float angleDelta = targetAngle - playerAngle;

    if (abs(angleDelta) > M_PI) {
        if (targetAngle < playerAngle) {
            angleDelta += 2 * M_PI;
        } else {
            angleDelta -= 2 * M_PI;
        }
    }

    _controlMutex.lock();
    _controlPacket->setAngularSpeed(angleDelta * ANGULAR_KP);
    _controlMutex.unlock();

    emit sendControlPacket(*_controlPacket);
}

void Player::kick(const float kickSpeed, bool isChipped)
{
    float chipAngle = 0.0f;
    if (isChipped) {
        chipAngle = M_PI_4;
    }

    _controlMutex.lock();
    _controlPacket->setKickSpeed(kickSpeed);
    _controlPacket->setChipKickAngle(chipAngle);
    _controlMutex.unlock();

    emit sendControlPacket(*_controlPacket);
}

void Player::dribble(const bool enable)
{
    float dribbleSpeed = 0.0f;
    if (enable) {
        dribbleSpeed = DRIBBLE_SPEED;
    }

    _controlMutex.lock();
    _controlPacket->setDribblerSpeed(dribbleSpeed);
    _controlMutex.unlock();

    emit sendControlPacket(*_controlPacket);
}

void Player::updateFromDetection(const RobotDetectionPacket& robotDetectionPacket) {
    if(robotDetectionPacket.isTeamBlue() != this->isTeamBlue()) return;

    // Get robot detection packet and parse
    SSL_DetectionRobot robotDetectPacket = robotDetectionPacket.getRobotDetectionPacket();
    if (robotDetectPacket.robot_id() != this->getPlayerId()) {
        if (++_missingPackets >= PACKETS_TILL_MISSING) {
            _position = OUT_OF_FIELD;
            _orientation = 0.0f;
        }
        return;
    }

    // Update detection
    _missingPackets = 0;
    _position = QVector2D(robotDetectPacket.x() / 1000.0f, robotDetectPacket.y() / 1000.0f);
    _orientation = robotDetectPacket.orientation();

    _controlMutex.lock();
    delete _controlPacket;
    _controlPacket = new RobotControlPacket(this->isTeamBlue(),
                                            this->getPlayerId(),
                                            0.0f,
                                            0.0f,
                                            0.0f,
                                            0.0f,
                                            0.0f,
                                            0.0f);
    _controlMutex.unlock();
}
