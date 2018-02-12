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

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QSharedPointer>
#include <QPainter>
#include <QMenu>

#include "utils/xml.hpp"

#include "utils/icons.hpp"

#include "item-scene/identifier.hpp"

#include "item-scene/undo-redo.hpp"

#include "utils/action.hpp"

#include "utils/math.hpp"

#include "config/settings.hpp"

template <typename TInstance, typename TPtr>
inline bool instanceof(const TPtr* ptr)
{
	return (ptr && dynamic_cast<const TInstance*>(ptr));
}

template <typename TInstance, typename TPtr>
inline bool instanceof(const QSharedPointer<TPtr>& ptr)
{
	return instanceof<TInstance>(ptr.data());
}

class ItemScene;

class Item : public QGraphicsObject
{
	Q_OBJECT

public:
	Item(ItemScene* scene, const Identifier& id);

	virtual void postConstruction();

	const Identifier& getId() const;

	virtual QRectF boundingRect() const override;

	virtual QPainterPath shape() const override;

	virtual QPolygonF outline() const;

	void updateItem();

	virtual void appendXml(QDomElement parentElement);

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

	virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

	virtual void updateGeometry();

	virtual void geometryChange();

	ItemScene* const scene;
	const Identifier id;
};

class SelectableItem : public Item
{
	Q_OBJECT

public:
	SelectableItem(ItemScene* scene, const Identifier& id);

	bool isHovered() const;

protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

	QColor getBackgroundColor() const;

private:
	bool hovered;
};

class EditableItem : public SelectableItem
{
	Q_OBJECT

public:
	EditableItem(ItemScene* scene, const Identifier& id);

	virtual QVector<UndoRedoEvent> deleteItemEvent();
	void deleteItem();

	virtual void edit();

	bool isEditable() const;

	void setEditable(bool editable);

	bool isCopyable() const;

	void setCopyable(bool copyable);

protected:
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

	virtual void contextMenuAdditions(QMenu& contextMenu);

	virtual void contextMenuAdditionsGlobal(QMenu& contextMenu);

private:
	Action actionEdit;
	Action actionCut;
	Action actionCopy;
	Action actionPaste;
	Action actionDelete;

	bool editable;
	bool copyable;
};

class ArrangeableItem : public EditableItem
{
	Q_OBJECT

public:
	ArrangeableItem(ItemScene* scene, const Identifier& id, quint64 level);

	static const quint64 alwaysOnTop;

	QVector<UndoRedoEvent> toBottomItemEvent();
	void toBottomItem();

	QVector<UndoRedoEvent> lowerItemEvent();
	void lowerItem();

	QVector<UndoRedoEvent> raiseItemEvent();
	void raiseItem();

	QVector<UndoRedoEvent> toTopItemEvent();
	void toTopItem();

	quint64 getLevel() const;

	void setLevel(quint64 level);

	bool isAlwaysOnTop() const;

protected:
	virtual void contextMenuAdditions(QMenu& contextMenu) override;

	virtual void contextMenuAdditionsGlobal(QMenu& contextMenu) override;

private:
	Action actionToBottom;
	Action actionLower;
	Action actionRaise;
	Action actionToTop;

	QMenu orderMenu;

	quint64 level;
};

class MovableItem : public ArrangeableItem
{
	Q_OBJECT

public:
	MovableItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position);

	virtual QVector<UndoRedoEvent> deleteItemEvent() override;

	virtual QVector<UndoRedoEvent> moveItemEvent(const QPointF& newPosition);
	void moveItem(const QPointF& newPosition);

	const QPointF& getPosition() const;

	virtual bool checkMoved(QVector<UndoRedoEvent>& events);

	bool isGrabbed() const;

	bool isMoving() const;

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

	virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

	void setPosition(const QPointF& position);

	void beginMove();
	void move();
	void endMove();

private:
	void setMoving(bool moving);

	QPointF position;

	QPointF cursorPosition;
	bool grabbed;
	bool moving;

	bool grid;
};
