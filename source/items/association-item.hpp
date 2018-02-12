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

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include "items/connection-item.hpp"

#include "content/association.hpp"

#include "dialogs/association-editor.hpp"

class AssociationItem : public ConnectionItem
{
	Q_OBJECT

public:
	AssociationItem(ItemScene* scene, const Identifier& id, quint64 level, const Association& association, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes, bool perpendicular);

	AssociationItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position, const Association::Type& type = Association::Type::Default);

	static QVector<UndoRedoEvent> loadItems(ItemScene* scene, QDomElement element);

	static void createItem(ItemScene* scene, const QPointF& position);

	QVector<UndoRedoEvent> changeAssociationEvent(const Association& newAssociation);
	void changeAssociation(const Association& newAssociation);

	void edit() override;

	virtual QPainterPath shape() const override;

	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	virtual void appendXml(QDomElement parentElement, EndNode& beginNode, EndNode& endNode, const QPolygonF& nodes) override;

protected:
	virtual UndoRedoEvent anchor(ItemScene* scene, const Identifier& id, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes) override;

	virtual void updateGeometry() override;

private:
	void calculateDirectionArrow();

	QPointF calculateTextPosition(const QPointF& first, const QPointF& second, const QPointF& offset, const QSizeF& size, bool under = false);

	void calculateArrows();

	void calculateSides();

	void setAssociation(const Association& association);

	Association association;

	QPolygonF directionArrow;
	QTextDocument documentName;
	QRectF rectName;

	QPair<QPolygonF, QPolygonF> arrowSideA;
	qreal offsetXSideA;

	QTextDocument documentSideA;
	QRect rectSideA;

	QTextDocument documentMultiplicitySideA;
	QRect rectMultiplicitySideA;

	QPair<QPolygonF, QPolygonF> arrowSideB;
	qreal offsetXSideB;

	QTextDocument documentSideB;
	QRect rectSideB;

	QTextDocument documentMultiplicitySideB;
	QRect rectMultiplicitySideB;
};
