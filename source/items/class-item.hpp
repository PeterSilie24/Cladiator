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

#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include "content/class.hpp"

#include "dialogs/class-editor.hpp"

#include "item-scene/item-scene.hpp"

#include "items/connection-item.hpp"

class ClassItem : public ConnectableItem
{
	Q_OBJECT

public:
	ClassItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position, const Class& c);

	static QVector<UndoRedoEvent> loadItems(ItemScene* scene, QDomElement element);

	static QVector<UndoRedoEvent> createItemEvent(ItemScene* scene, const QPointF& position);
	static void createItem(ItemScene* scene, const QPointF& position);

	QVector<UndoRedoEvent> changeClassEvent(const Class& newClass);
	void changeClass(const Class& newClass);

	const Class& getClass() const;

	void edit() override;

	QPolygonF outline() const override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	void appendXml(QDomElement parentElement) override;

protected:
	virtual void updateGeometry() override;

private:
	void setClass(const Class& c);

	Class c;

	QTextDocument documentHead;
	QTextDocument documentAttributes;
	QTextDocument documentMethods;

	QRectF rectHead;
	QRectF rectAttributes;
	QRectF rectMethods;

	QRectF rect;
};
