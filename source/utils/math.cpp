/* Cladiator - Class Diagram Editor
 * Copyright (C) 2017 Phil Badura
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "math.hpp"

qreal Math::length(const QPointF& point)
{
	return qSqrt(point.x() * point.x() + point.y() * point.y());
}

qreal Math::dot(const QPointF& firstPoint, const QPointF& secondPoint)
{
	return firstPoint.x() * secondPoint.x() + firstPoint.y() * secondPoint.y();
}

qreal Math::angle(const QPointF& firstPoint, const QPointF& secondPoint)
{
	qreal firstLength = length(firstPoint);
	qreal secondLength = length(secondPoint);

	if (firstLength > 0.0 && secondLength > 0.0)
	{
		return qAcos(dot(firstPoint, secondPoint) / firstLength / secondLength);
	}

	return 0.0;
}

QPointF Math::normalize(const QPointF& point)
{
	qreal len = length(point);

	if (len > 0.0)
	{
		return point / len;
	}

	return point;
}

QPointF Math::orthogonal(const QPointF& point)
{
	return QPointF(point.y(), -point.x());
}

QPair<QPointF, bool> Math::intersectionOfStraights(const QPointF& firstPosition, const QPointF& firstDirection, const QPointF& secondPosition, const QPointF& secondDirection)
{
	if ((firstDirection.x() * secondDirection.y() == firstDirection.y() * secondDirection.x()) || (firstDirection.x() == 0.0f && firstDirection.y() == 0.0f) || (secondDirection.x() == 0.0f && secondDirection.y() == 0.0f))
	{
		return QPair<QPointF, bool>(QPointF(), false);
	}

	QPointF intersection;
	intersection.setX((firstPosition.x() * firstDirection.y() * secondDirection.x() - firstPosition.y() * firstDirection.x() * secondDirection.x() - firstDirection.x() * secondPosition.x() * secondDirection.y() + firstDirection.x() * secondPosition.y() * secondDirection.x()) / (firstDirection.y() * secondDirection.x() - firstDirection.x() * secondDirection.y()));
	intersection.setY((firstPosition.x() * firstDirection.y() * secondDirection.y() - firstPosition.y() * firstDirection.x() * secondDirection.y() - firstDirection.y() * secondPosition.x() * secondDirection.y() + firstDirection.y() * secondPosition.y() * secondDirection.x()) / (firstDirection.y() * secondDirection.x() - firstDirection.x() * secondDirection.y()));

	return QPair<QPointF, bool>(intersection, true);
}

QPair<QPointF, bool> Math::intersectionOfLines(const QPointF& firstBegin, const QPointF& firstEnd, const QPointF& secondBegin, const QPointF& secondEnd)
{
	QPointF firstDirection = firstBegin - firstEnd;
	QPointF secondDirection = secondBegin - secondEnd;

	QPair<QPointF, bool> intersection = intersectionOfStraights(firstBegin, firstDirection, secondBegin, secondDirection);

	if (intersection.second)
	{
		QPointF buff1 = intersection.first - firstBegin;
		QPointF buff2 = intersection.first - firstEnd;

		qreal firstDistance = qMax(length(buff1), length(buff2));

		buff1 = intersection.first - secondBegin;
		buff2 = intersection.first - secondEnd;

		qreal secondDistance = qMax(length(buff1), length(buff2));

		qreal firstLength = length(firstDirection);
		qreal secondLength = length(secondDirection);

		if (firstDistance <= firstLength && secondDistance <= secondLength)
		{
			return intersection;
		}
	}

	return QPair<QPointF, bool>(QPointF(), false);
}

QPolygonF Math::intersectionOfPolygons(const QPolygonF& firstPolygon, const QPolygonF& secondPolygon)
{
	QPolygonF points;

	for (int first = 0; first < firstPolygon.count() - 1; first++)
	{
		for (int second = 0; second < secondPolygon.count() - 1; second++)
		{
			QPair<QPointF, bool> intersection = intersectionOfLines(firstPolygon[first], firstPolygon[first + 1], secondPolygon[second], secondPolygon[second + 1]);

			if (intersection.second)
			{
				points.append(intersection.first);
			}
		}
	}

	return points;
}

qreal Math::distanceToLine(const QPointF& begin, const QPointF& end, const QPointF& point)
{
	qreal distance = qMin(length(begin - point), length(end - point));

	QPointF hypotenuseA = point - begin;
	QPointF hypotenuseB = point - end;

	qreal alpha = angle(end - begin, hypotenuseA);
	qreal beta = angle(begin - end, hypotenuseB);

	qreal parallelA = qCos(alpha) * length(hypotenuseA);
	qreal parallelB = qCos(beta) * length(hypotenuseB);

	qreal parallel = qMax(parallelA, parallelB);

	if (parallel <= length(end - begin))
	{
		qreal orthogonal = qMin(qSin(alpha) * length(hypotenuseA), qSin(beta) * length(hypotenuseB));

		distance = qMin(distance, orthogonal);
	}

	return distance;
}
