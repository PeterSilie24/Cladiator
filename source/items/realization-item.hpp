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

#include "items/connection-item.hpp"

#include "content/realization.hpp"

class RealizationItem : public ConnectionItem
{
	Q_OBJECT

public:
	RealizationItem(ItemScene* scene, const Identifier& id, quint64 level, const Realization& realization, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes, bool perpendicular);

	RealizationItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position);

	static QVector<UndoRedoEvent> loadItems(ItemScene* scene, QDomElement element);

	static void createItem(ItemScene* scene, const QPointF& position);

	virtual QPainterPath shape() const override;

	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	virtual void appendXml(QDomElement parentElement, EndNode& beginNode, EndNode& endNode, const QPolygonF& nodes) override;

protected:
	virtual UndoRedoEvent anchor(ItemScene* scene, const Identifier& id, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes) override;

	virtual void updateGeometry() override;

private:
	Realization realization;

	QPolygonF arrow;
};
