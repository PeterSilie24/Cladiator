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

#include <QPolygon>

#include "content/content.hpp"

class EndNode
{
public:
	EndNode();

	EndNode(const Identifier& connectedId, const QPointF& position = QPointF());

	EndNode(const QPointF& position);

	EndNode(QDomElement element);

	bool hasConnectedId() const;

	quint64 getConnectedId() const;

	QPointF getPosition() const;

	void appendXml(QDomElement element);

private:
	quint64 connectedId;
	QPointF position;
};

class Connection : public Content
{
public:
	Connection();

	Connection(QDomElement element);

	static QString getTagName();

	void appendXml(QDomElement element, const Identifier& id, quint64 level, bool perpendicular, EndNode& beginNode, EndNode& endNode, const QPolygonF& nodes);

	bool isRequestedPerpendicular() const;

	EndNode getRequestedBeginNode() const;

	EndNode getRequestedEndNode() const;

	const QPolygonF& getRequestedNodes() const;

private:
	bool requestedPerpendicular;
	EndNode requestedBeginNode;
	EndNode requestedEndNode;

	QPolygonF requestedNodes;
};
