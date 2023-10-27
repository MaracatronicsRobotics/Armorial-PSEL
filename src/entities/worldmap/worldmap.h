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


#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <QObject>
#include <QMap>

#include <include/proto/ssl_vision_wrapper.pb.h>

#include <src/entities/player/player.h>

/*!
 * \brief The WorldMap class provides an interface to access field data and objects, such as ball
 * and some pre-defined field locations.
 */
class WorldMap : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief WorldMap
     * \param isPlayingLeft
     */
    WorldMap(const bool& isPlayingLeft);

    /*!
     * \return True if we are playing at left side and False otherwise.
     */
    bool playingLeftSide() const;

    /*!
     * \return The minimum x-axis value of the field.
     */
    float minX() const;

    /*!
     * \return The maximum x-axis value of the field.
     */
    float maxX() const;

    /*!
     * \return The minimum y-axis value of the field.
     */
    float minY() const;

    /*!
     * \return The maximum y-axis value of the field.
     */
    float maxY() const;

    /*!
     * \return The length of the field.
     */
    float length() const;

    /*!
     * \return The width of the field.
     */
    float width() const;

    /*!
     * \return The goal depth.
     */
    float goalDepth() const;

    /*!
     * \return The goal width.
     */
    float goalWidth() const;

    /*!
     * \return The penalty depth.
     */
    float penaltyDepth() const;

    /*!
     * \return The penalty width.
     */
    float penaltyWidth() const;

    /*!
     * \return The penalty mark distance from goal center.
     */
    float penaltyMarkDistanceFromGoal() const;

    /*!
     * \return The center radius.
     */
    float centerRadius() const;

    /*!
     * \return The position of the center of our goal.
     */
    QVector2D ourGoalCenter() const;

    /*!
     * \return The position of the right post of our goal.
     */
    QVector2D ourGoalRightPost() const;

    /*!
     * \return The position of the left post of our goal.
     */
    QVector2D ourGoalLeftPost() const;

    /*!
     * \return The position of our penalty mark.
     */
    QVector2D ourPenaltyMark() const;

    /*!
     * \return The position of the center of their goal.
     */
    QVector2D theirGoalCenter() const;

    /*!
     * \return The position of the right post of their goal.
     */
    QVector2D theirGoalRightPost() const;

    /*!
     * \return The position of the left post of their goal.
     */
    QVector2D theirGoalLeftPost() const;

    /*!
     * \return The position of their penalty mark.
     */
    QVector2D theirPenaltyMark() const;

    /*!
     * \return The position of the ball.
     */
    QVector2D ballPosition() const;

private:
    // Internal variables
    SSL_GeometryData _field;
    SSL_DetectionBall _ball;
    bool _isPlayingLeft;

public slots:
    void updateBallDetection(const SSL_DetectionBall &ball);
    void updateFieldDetection(const SSL_GeometryData &field);
};

#endif // WORLDMAP_H
