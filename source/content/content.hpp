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

#include <QString>
#include <QPointF>

#include "utils/xml.hpp"

#include "item-scene/identifier.hpp"

#include "utils/reg-exp.hpp"

class Content
{
public:
	Content();

	Content(QDomElement element);

	void appendXml(QDomElement element, const Identifier& id, quint64 level);

	quint64 getRequestedId() const;

	quint64 getRequestedLevel() const;

private:
	quint64 requestedId;
	quint64 requestedLevel;
};

class PositionableContent : public Content
{
public:
	PositionableContent();

	PositionableContent(QDomElement element);

	void appendXml(QDomElement element, const Identifier& id, quint64 level, const QPointF& position);

	const QPointF& getRequestedPosition() const;

private:
	QPointF requestedPosition;
};
