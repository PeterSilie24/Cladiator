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

#include "connection.hpp"

EndNode::EndNode() : connectedId(0)
{

}

EndNode::EndNode(const Identifier& connectedId, const QPointF& position) : connectedId(connectedId.getValue()), position(position)
{

}

EndNode::EndNode(const QPointF& position) : connectedId(0), position(position)
{

}

EndNode::EndNode(QDomElement element) : connectedId(0)
{
	this->connectedId = element.attribute("id").toULongLong(nullptr, 16);

	this->position = QPointF(static_cast<qreal>(static_cast<int>(element.attribute("position-x", "0").toDouble())), static_cast<qreal>(static_cast<int>(element.attribute("position-y", "0").toDouble())));
}

bool EndNode::hasConnectedId() const
{
	return this->connectedId != 0;
}

quint64 EndNode::getConnectedId() const
{
	return this->connectedId;
}

QPointF EndNode::getPosition() const
{
	return this->position;
}

void EndNode::appendXml(QDomElement element)
{
	if (this->hasConnectedId())
	{
		element.setAttribute("id", QString::number(this->connectedId, 16).toUpper());
	}
	
	element.setAttribute("position-x", QString::number(static_cast<int>(this->position.x())));
	element.setAttribute("position-y", QString::number(static_cast<int>(this->position.y())));
}

Connection::Connection() : requestedPerpendicular(false)
{

}

Connection::Connection(QDomElement element) : Content(element)
{
	this->requestedPerpendicular = (element.attribute("perpendicular") == "true") ? true : false;

	QDomNodeList list = element.elementsByTagName("begin-node");

	if (list.count())
	{
		QDomNode node = list.item(0);

		if (node.isElement())
		{
			requestedBeginNode = EndNode(node.toElement());
		}
	}

	list = element.elementsByTagName("node");

	for (int i = 0; i < list.count(); i++)
	{
		QDomNode node = list.item(i);

		if (node.isElement())
		{
			QDomElement element = node.toElement();

			this->requestedNodes.append(QPointF(static_cast<qreal>(static_cast<int>(element.attribute("position-x", "0").toDouble())), static_cast<qreal>(static_cast<int>(element.attribute("position-y", "0").toDouble()))));
		}
	}

	list = element.elementsByTagName("end-node");

	if (list.count())
	{
		QDomNode node = list.item(0);

		if (node.isElement())
		{
			requestedEndNode = EndNode(node.toElement());
		}
	}
}

QString Connection::getTagName()
{
	return "connection";
}

void Connection::appendXml(QDomElement element, const Identifier& id, quint64 level, bool perpendicular, EndNode& beginNode, EndNode& endNode, const QPolygonF& nodes)
{
	Content::appendXml(element, id, level);

	element.setAttribute("perpendicular", perpendicular ? "true" : "false");

	QDomElement childElement = element.ownerDocument().createElement("begin-node");

	beginNode.appendXml(childElement);

	element.appendChild(childElement);

	for (auto& node : nodes)
	{
		QDomElement childElement = element.ownerDocument().createElement("node");

		childElement.setAttribute("position-x", QString::number(static_cast<int>(node.x())));
		childElement.setAttribute("position-y", QString::number(static_cast<int>(node.y())));

		element.appendChild(childElement);
	}

	childElement = element.ownerDocument().createElement("end-node");

	endNode.appendXml(childElement);

	element.appendChild(childElement);
}

bool Connection::isRequestedPerpendicular() const
{
	return this->requestedPerpendicular;
}

EndNode Connection::getRequestedBeginNode() const
{
	return this->requestedBeginNode;
}

EndNode Connection::getRequestedEndNode() const
{
	return this->requestedEndNode;
}

const QPolygonF& Connection::getRequestedNodes() const
{
	return this->requestedNodes;
}
