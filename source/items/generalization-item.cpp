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

#include "generalization-item.hpp"

GeneralizationItem::GeneralizationItem(ItemScene* scene, const Identifier& id, quint64 level, const Generalization& generalization, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes, bool perpendicular) : ConnectionItem(scene, id, level, generalization, beginNode, endNode, nodes, perpendicular), generalization(generalization)
{
	
}

GeneralizationItem::GeneralizationItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position) : ConnectionItem(scene, id, level, position, true)
{
	
}

QVector<UndoRedoEvent> GeneralizationItem::loadItems(ItemScene* scene, QDomElement element)
{
	QVector<UndoRedoEvent> events;

	if (scene)
	{
		QVector<Generalization> generalizations = Xml::loadObjects<Generalization>(element);

		for (auto& generalization : generalizations)
		{
			ItemScene* cScene = scene;
			Identifier cId = scene->newIdentifier(generalization.getRequestedId());
			Generalization cGeneralization = generalization;

			if (cId)
			{
				std::function<void()> undo = [cScene, cId]()
				{
					cScene->removeItemById(cId);
				};

				std::function<void()> redo = [cScene, cId, cGeneralization]()
				{
					cScene->addItem(QSharedPointer<Item>(new GeneralizationItem(cScene, cId, cGeneralization.getRequestedLevel(), cGeneralization, cGeneralization.getRequestedBeginNode(), cGeneralization.getRequestedEndNode(), cGeneralization.getRequestedNodes(), cGeneralization.isRequestedPerpendicular())));
				};

				events.append(UndoRedoEvent(undo, redo));
			}
		}
	}

	return events;
}

void GeneralizationItem::createItem(ItemScene* scene, const QPointF& position)
{
	if (scene)
	{
		Identifier id = scene->newIdentifier();

		if (id)
		{
			scene->addItem(QSharedPointer<Item>(new GeneralizationItem(scene, id, scene->queryLevel(), position)));
		}
	}
}

QPainterPath GeneralizationItem::shape() const
{
	QPainterPath path;

	path.addPolygon(this->arrow);

	path = path.united(ConnectionItem::shape());

	return path;
}

void GeneralizationItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QPen pen;
	pen.setColor(Settings::getColor());
	pen.setWidth(Settings::getBorderWidth());

	painter->setPen(pen);
	painter->setRenderHints(QPainter::RenderHint::Antialiasing | QPainter::RenderHint::SmoothPixmapTransform | QPainter::RenderHint::HighQualityAntialiasing);

	QPolygonF nodes = this->getNodes();

	QPainterPath path;
	path.addPolygon(nodes);

	painter->drawPath(path);

	path = QPainterPath();
	path.addPolygon(this->arrow);

	pen.setJoinStyle(Qt::PenJoinStyle::SvgMiterJoin);

	painter->setPen(pen);

	painter->fillPath(path, this->getBackgroundColor());

	painter->drawPath(path);
}

void GeneralizationItem::appendXml(QDomElement parentElement, EndNode& beginNode, EndNode& endNode, const QPolygonF& nodes)
{
	this->generalization.appendXml(Xml::appendObject<Generalization>(parentElement), this->id, this->getLevel(), this->isPerpendicular(), beginNode, endNode, nodes);
}

UndoRedoEvent GeneralizationItem::anchor(ItemScene* scene, const Identifier& id, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes)
{
	ItemScene* cScene = scene;
	Identifier cId = id;
	Generalization cGeneralization = Generalization();
	EndNode cBeginNode = beginNode;
	EndNode cEndNode = endNode;
	QPolygonF cNodes = nodes;

	std::function<void()> undo = [cScene, cId]()
	{
		cScene->removeItemById(cId);
	};

	std::function<void()> redo = [cScene, cId, cGeneralization, cBeginNode, cEndNode, cNodes]()
	{
		cScene->addItem(QSharedPointer<Item>(new GeneralizationItem(cScene, cId, cScene->queryLevel(), cGeneralization, cBeginNode, cEndNode, cNodes, true)));
	};

	return UndoRedoEvent(undo, redo);
}

void GeneralizationItem::updateGeometry()
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
