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

#include "realization-item.hpp"

RealizationItem::RealizationItem(ItemScene* scene, const Identifier& id, quint64 level, const Realization& realization, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes, bool perpendicular) : ConnectionItem(scene, id, level, realization, beginNode, endNode, nodes, perpendicular), realization(realization)
{
	
}

RealizationItem::RealizationItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position) : ConnectionItem(scene, id, level, position, true)
{
	
}

QVector<UndoRedoEvent> RealizationItem::loadItems(ItemScene* scene, QDomElement element)
{
	QVector<UndoRedoEvent> events;

	if (scene)
	{
		QVector<Realization> realizations = Xml::loadObjects<Realization>(element);

		for (auto& realization : realizations)
		{
			ItemScene* cScene = scene;
			Identifier cId = scene->newIdentifier(realization.getRequestedId());
			Realization cRealization = realization;

			if (cId)
			{
				std::function<void()> undo = [cScene, cId]()
				{
					cScene->removeItemById(cId);
				};

				std::function<void()> redo = [cScene, cId, cRealization]()
				{
					cScene->addItem(QSharedPointer<Item>(new RealizationItem(cScene, cId, cRealization.getRequestedLevel(), cRealization, cRealization.getRequestedBeginNode(), cRealization.getRequestedEndNode(), cRealization.getRequestedNodes(), cRealization.isRequestedPerpendicular())));
				};

				events.append(UndoRedoEvent(undo, redo));
			}
		}
	}

	return events;
}

void RealizationItem::createItem(ItemScene* scene, const QPointF& position)
{
	if (scene)
	{
		Identifier id = scene->newIdentifier();

		if (id)
		{
			scene->addItem(QSharedPointer<Item>(new RealizationItem(scene, id, scene->queryLevel(), position)));
		}
	}
}

QPainterPath RealizationItem::shape() const
{
	QPainterPath path;

	path.addPolygon(this->arrow);

	path = path.united(ConnectionItem::shape());

	return path;
}

void RealizationItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QPen pen;
	pen.setWidth(Settings::getBorderWidth());
	pen.setColor(Settings::getColor());
	pen.setStyle(Qt::PenStyle::DashLine);
	pen.setDashPattern({ Settings::getBorderWidth() * 2.0, Settings::getBorderWidth() * 2.0 });

	painter->setPen(pen);
	painter->setRenderHints(QPainter::RenderHint::Antialiasing | QPainter::RenderHint::SmoothPixmapTransform | QPainter::RenderHint::HighQualityAntialiasing);

	QPolygonF nodes = this->getNodes();

	QPainterPath path;
	path.addPolygon(nodes);

	painter->drawPath(path);

	path = QPainterPath();
	path.addPolygon(this->arrow);

	pen = QPen();
	pen.setWidth(Settings::getBorderWidth());
	pen.setColor(Settings::getColor());
	pen.setJoinStyle(Qt::PenJoinStyle::SvgMiterJoin);

	painter->setPen(pen);

	painter->fillPath(path, this->getBackgroundColor());

	painter->drawPath(path);
}

void RealizationItem::appendXml(QDomElement parentElement, EndNode& beginNode, EndNode& endNode, const QPolygonF& nodes)
{
	this->realization.appendXml(Xml::appendObject<Realization>(parentElement), this->id, this->getLevel(), this->isPerpendicular(), beginNode, endNode, nodes);
}

UndoRedoEvent RealizationItem::anchor(ItemScene* scene, const Identifier& id, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes)
{
	ItemScene* cScene = scene;
	Identifier cId = id;
	Realization cRealization = Realization();
	EndNode cBeginNode = beginNode;
	EndNode cEndNode = endNode;
	QPolygonF cNodes = nodes;

	std::function<void()> undo = [cScene, cId]()
	{
		cScene->removeItemById(cId);
	};

	std::function<void()> redo = [cScene, cId, cRealization, cBeginNode, cEndNode, cNodes]()
	{
		cScene->addItem(QSharedPointer<Item>(new RealizationItem(cScene, cId, cScene->queryLevel(), cRealization, cBeginNode, cEndNode, cNodes, true)));
	};

	return UndoRedoEvent(undo, redo);
}

void RealizationItem::updateGeometry()
{
	ConnectionItem::updateGeometry();

	qreal size = Settings::getBorderWidth() * 5.0;

	this->arrow.clear();

	QPolygonF nodes = this->getNodes();

	if (nodes.size() >= 2)
	{
		QPointF first = nodes[0];
		QPointF second = nodes[1];

		QPointF direction = second - first;

		if (Math::length(direction) > 0.0)
		{
			direction = Math::normalize(direction);

			QPointF orthogonal = Math::orthogonal(direction);

			this->arrow << first << (first + direction * size * 2.0 + orthogonal * size) << (first + direction * size * 2.0 - orthogonal * size) << first;
		}
	}
}
