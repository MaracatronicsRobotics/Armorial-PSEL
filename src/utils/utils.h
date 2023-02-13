#ifndef UTILS_H
#define UTILS_H

#include <QVector2D>

/*!
 * \brief The Utils class provides util methods to make some calculations, generally related
 * to geometry or more generic methods that are used in all classes.
 */
class Utils
{
public:
    /*!
     * \brief Computes the smallest angle diff between two given angles.
     * \param targetAngle The target angle.
     * \param sourceAngle The source angle.
     * \return The smallest angle diff between the two given angles.
     */
    static float smallestAngleDiff(const float& targetAngle, const float& sourceAngle);

    /*!
     * \brief Compute the angle normalization to the range [-π, π).
     * \return The normalized angle in the range.
     */
    static float normalizeAngle(const float& angle);

    /*!
     * \brief Check if robot front can be reversed with a given target angle.
     * \param robotAngle The robot angle.
     * \param targetAngle The angle to the target.
     * \return True if robot front can be reversed and False otherwise.
     */
    static bool checkIfCanBeReversed(const float& robotAngle, const float& targetAngle);

    /*!
     * \brief Get the angle between the currentPosition and the targetPosition.
     * \param currentPosition The given current position.
     * \param targetPosition The given target position.
     * \return The angle value between the currentPosition and targetPosition.
     */
    static float getAngle(const QVector2D& currentPosition, const QVector2D& targetPosition);
};

#endif // UTILS_H
