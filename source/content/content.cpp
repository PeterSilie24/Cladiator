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

#include "content.hpp"

Content::Content() : requestedId(0), requestedLevel(0)
{

}

Content::Content(QDomElement element) : requestedId(element.attribute("id").toULongLong(nullptr, 16)), requestedLevel(element.attribute("level").toULongLong(nullptr, 16))
{

}

void Content::appendXml(QDomElement element, const Identifier& id, quint64 level)
{
	element.setAttribute("id", id.toString());
	element.setAttribute("level", QString::number(level));
}

quint64 Content::getRequestedId() const
{
	return this->requestedId;
}

quint64 Content::getRequestedLevel() const
{
	return this->requestedLevel;
}

PositionableContent::PositionableContent()
{

}

PositionableContent::PositionableContent(QDomElement element) : Content(element), requestedPosition(static_cast<qreal>(static_cast<int>(element.attribute("position-x", "0").toDouble())), static_cast<qreal>(static_cast<int>(element.attribute("position-y", "0").toDouble())))
{

}

void PositionableContent::appendXml(QDomElement element, const Identifier& id, quint64 level, const QPointF& position)
{
	Content::appendXml(element, id, level);

	element.setAttribute("position-x", QString::number(static_cast<int>(position.x())));
	element.setAttribute("position-y", QString::number(static_cast<int>(position.y())));
}

const QPointF& PositionableContent::getRequestedPosition() const
{
	return this->requestedPosition;
}
