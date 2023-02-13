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

#include <QCoreApplication>

#include <src/entities/player/player.h>
#include <src/entities/vision/vision.h>
#include <src/entities/worldmap/worldmap.h>
#include <src/entities/actuator/actuator.h>
#include <src/entities/coach/coach.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Create Vision module
    Vision *vision = new Vision();

    // Create actuator module and connect it with the Vision to receive the host address
    Actuator* actuator = new Actuator();
    QObject::connect(vision, &Vision::sendHostAddress, actuator, &Actuator::connectToNetwork);

    // Create WorldMap instance and connect it with the Vision module to receive detection packets
    // from ball and field.
    WorldMap *worldMap = new WorldMap(false);
    QObject::connect(vision, &Vision::sendFieldDetection, worldMap, &WorldMap::updateFieldDetection);
    QObject::connect(vision, &Vision::sendBallDetection, worldMap, &WorldMap::updateBallDetection);

    // Create Player pointers
    QMap<bool, QList<Player*>> _players;
    for(int i = 0; i <= 1; i++) {
        _players.insert(i, QList<Player*>());
        for(int j = 0; j < 3; j++) {
            _players[i].push_back(new Player(i, j));
            QObject::connect(vision, &Vision::sendRobotDetection, _players[i][j], &Player::updateFromDetection);
            QObject::connect(_players[i][j], &Player::sendControlPacket, actuator, &Actuator::receiveControlPacket);
        }
    }

    // Create Coach module instance and allocate players and worldmap to it.
    Coach* coach = new Coach(_players, worldMap);

    // Hold exec
    bool exec = a.exec();

    // Delete pointers
    delete coach;
    delete worldMap;

    for (auto& team : _players) {
        for (auto& player : team) {
            delete player;
        }
        team.clear();
    }

    delete actuator;
    delete vision;

    return exec;
}
