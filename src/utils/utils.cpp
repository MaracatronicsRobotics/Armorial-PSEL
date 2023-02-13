#include "utils.h"

#include <math.h>

float Utils::smallestAngleDiff(const float &targetAngle, const float &sourceAngle) {
    float a = fmod(targetAngle + 2*M_PI, 2*M_PI) - fmod(sourceAngle + 2*M_PI, 2*M_PI);

    if (a > M_PI) {
        a = a - 2 * M_PI;
    } else if (a < -M_PI) {
        a = a + 2 * M_PI;
    }
    return a;
}

float Utils::normalizeAngle(const float &angle) {
    float angleRet = fmod(angle + M_PI, 2.0 * M_PI);
    angleRet = (angleRet < 0.0) ? angleRet + M_PI : angleRet - M_PI;

    return angleRet;
}

bool Utils::checkIfCanBeReversed(const float &robotAngle, const float &targetAngle) {
    float angDiff = Utils::smallestAngleDiff(robotAngle, targetAngle);

    return (fabs(angDiff) > M_PI/2.0 + M_PI/20.0);
}

float Utils::getAngle(const QVector2D &currentPosition, const QVector2D &targetPosition) {
    return atan2(targetPosition.y() - currentPosition.y(), targetPosition.x() -currentPosition.x());
}
