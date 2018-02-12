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

#pragma once

#include <QtMath>
#include <QPair>
#include <QPointF>
#include <QPolygon>
#include <QPainterPath>

class Math
{
public:
	static qreal length(const QPointF& point);

	static qreal dot(const QPointF& firstPoint, const QPointF& secondPoint);

	static qreal angle(const QPointF& firstPoint, const QPointF& secondPoint);

	static QPointF normalize(const QPointF& point);

	static QPointF orthogonal(const QPointF& point);

	static QPair<QPointF, bool> intersectionOfStraights(const QPointF& firstPosition, const QPointF& firstDirection, const QPointF& secondPosition, const QPointF& secondDirection);

	static QPair<QPointF, bool> intersectionOfLines(const QPointF& firstBegin, const QPointF& firstEnd, const QPointF& secondBegin, const QPointF& secondEnd);

	static QPolygonF intersectionOfPolygons(const QPolygonF& firstPolygon, const QPolygonF& secondPolygon);

	static qreal distanceToLine(const QPointF& begin, const QPointF& end, const QPointF& point);
};
