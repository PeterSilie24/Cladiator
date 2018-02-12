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

#include "item.hpp"

#include "item-scene/item-scene.hpp"

Item::Item(ItemScene* scene, const Identifier& id)
	: scene(scene), id(id)
{
	this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void Item::postConstruction()
{
	this->geometryChange();
}

const Identifier& Item::getId() const
{
	return this->id;
}

QRectF Item::boundingRect() const
{
	QRectF rect = this->shape().boundingRect();

	rect.setLeft(rect.left() - 40.0);
	rect.setRight(rect.right() + 40.0);
	rect.setTop(rect.top() - 40.0);
	rect.setBottom(rect.bottom() + 40.0);

	return rect;
}

QPainterPath Item::shape() const
{
	QPainterPath path;
	path.addPolygon(this->outline());

	return path;
}

QPolygonF Item::outline() const
{
	return QPolygonF();
}

void Item::updateItem()
{
	this->prepareGeometryChange();

	this->updateGeometry();

	this->update();

	this->geometryChange();
}

void Item::appendXml(QDomElement parentElement)
{
	Q_UNUSED(parentElement);
}

void Item::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() != Qt::MouseButton::RightButton)
	{
		QGraphicsObject::mousePressEvent(event);
	}
}

void Item::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() != Qt::MouseButton::RightButton)
	{
		QGraphicsObject::mouseReleaseEvent(event);
	}
}

QVariant Item::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == ItemPositionHasChanged ||
		change == ItemRotationHasChanged ||
		change == ItemScaleHasChanged ||
		change == ItemTransformHasChanged)
	{
		this->geometryChange();
	}

	return QGraphicsObject::itemChange(change, value);
}

void Item::updateGeometry()
{

}

void Item::geometryChange()
{
	if (this->scene)
	{
		QRectF boundingRect = this->boundingRect();

		boundingRect.moveTo(this->pos());

		this->scene->recalculateRect(boundingRect);
	}
}

SelectableItem::SelectableItem(ItemScene* scene, const Identifier& id) : Item(scene, id), hovered(false)
{
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);

	this->setAcceptHoverEvents(true);
}

bool SelectableItem::isHovered() const
{
	return this->hovered;
}

void SelectableItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	this->hovered = true;

	this->update();

	Item::hoverEnterEvent(event);
}

void SelectableItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	this->hovered = false;

	this->update();

	Item::hoverLeaveEvent(event);
}

QColor SelectableItem::getBackgroundColor() const
{
	QColor color = Settings::getColor(Colors::Background);

	qreal avarage = (color.redF() + color.greenF() + color.blueF()) / 3.0;

	qreal factor = -avarage * 3.0 + 2.0;

	if (factor < 1.0 && factor > -1.0)
	{
		factor = 1.0 * qAbs(factor) / factor;
	}

	int offset = 0;

	if (this->isSelected())
	{
		offset = 32;
	}
	else if (this->isHovered())
	{
		offset = 16;
	}

	offset = static_cast<int>(static_cast<qreal>(offset) * factor);

	return QColor(qMax(qMin(color.red() + offset, 255), 0), qMax(qMin(color.green() + offset, 255), 0), qMax(qMin(color.blue() + offset, 255), 0));
}

EditableItem::EditableItem(ItemScene* scene, const Identifier& id)
	: SelectableItem(scene, id),
	actionEdit(this, tr("Edit"), Icons::loadIcon("document-edit")),
	actionCut(this, tr("Cut"), Icons::loadIcon("edit-cut"), QKeySequence("Ctrl+X")),
	actionCopy(this, tr("Copy"), Icons::loadIcon("edit-copy"), QKeySequence("Ctrl+C")),
	actionPaste(this, tr("Paste"), Icons::loadIcon("edit-paste"), QKeySequence("Ctrl+V")),
	actionDelete(this, tr("Delete"), Icons::loadIcon("edit-delete"), QKeySequence("Delete")),
	editable(true), copyable(true)
{
	
}

QVector<UndoRedoEvent> EditableItem::deleteItemEvent()
{
	if (this->scene)
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;

		std::function<void()> undo = [cScene, cId]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<EditableItem>(item))
			{
				item.dynamicCast<EditableItem>()->show();
			}
		};

		std::function<void()> redo = [cScene, cId]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<EditableItem>(item))
			{
				item.dynamicCast<EditableItem>()->hide();
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

void EditableItem::deleteItem()
{
	if (this->scene)
	{
		this->scene->change(this->deleteItemEvent());
	}
}

void EditableItem::edit()
{

}

bool EditableItem::isEditable() const
{
	return this->editable;
}

void EditableItem::setEditable(bool editable)
{
	this->editable = editable;
}

bool EditableItem::isCopyable() const
{
	return this->copyable;
}

void EditableItem::setCopyable(bool copyable)
{
	this->copyable = copyable;
}

void EditableItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	if (this->scene)
	{
		QList<QSharedPointer<EditableItem>> items = this->scene->getSelectedItems<EditableItem>();

		QMenu contextMenu(this->scene->getWidget());
		
		this->actionCut.setTriggeredFunction(std::function<void()>([&]() { this->scene->cut(); }));

		this->actionCopy.setTriggeredFunction(std::function<void()>([&]() { this->scene->copy(); }));

		this->actionPaste.setTriggeredFunction(std::function<void()>([&]() { this->scene->paste(); }));

		if (items.size() >= 2 && this->isSelected())
		{
			if (this->copyable)
			{
				this->actionCut.setEnabled(this->scene->isCopyable());

				contextMenu.addAction(&(this->actionCut));

				this->actionCopy.setEnabled(this->scene->isCopyable());

				contextMenu.addAction(&(this->actionCopy));
			}

			this->actionPaste.setEnabled(this->scene->isInsertable());

			contextMenu.addAction(&(this->actionPaste));

			contextMenu.addSeparator();

			contextMenuAdditionsGlobal(contextMenu);

			this->actionDelete.setTriggeredFunction(std::function<void()>([&]() { this->scene->deleteAll(); }));

			contextMenu.addAction(&(this->actionDelete));
		}
		else
		{
			this->scene->deselectAll();

			this->setSelected(true);

			if (this->editable)
			{
				this->actionEdit.setTriggeredFunction(std::function<void()>([&]() { this->edit(); }));

				contextMenu.addAction(&(this->actionEdit));

				contextMenu.addSeparator();
			}

			if (this->copyable)
			{
				this->actionCut.setEnabled(true);

				contextMenu.addAction(&(this->actionCut));

				this->actionCopy.setEnabled(true);

				contextMenu.addAction(&(this->actionCopy));
			}

			this->actionPaste.setEnabled(this->scene->isInsertable());

			contextMenu.addAction(&(this->actionPaste));

			contextMenu.addSeparator();

			contextMenuAdditions(contextMenu);

			this->actionDelete.setTriggeredFunction(std::function<void()>([&]() { this->deleteItem(); }));

			contextMenu.addAction(&(this->actionDelete));
		}

		contextMenu.exec(event->screenPos());

		event->accept();
	}
}

void EditableItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		this->edit();
	}

	SelectableItem::mouseDoubleClickEvent(event);
}

void EditableItem::contextMenuAdditions(QMenu& contextMenu)
{
	Q_UNUSED(contextMenu);
}

void EditableItem::contextMenuAdditionsGlobal(QMenu& contextMenu)
{
	Q_UNUSED(contextMenu);
}

ArrangeableItem::ArrangeableItem(ItemScene* scene, const Identifier& id, quint64 level)
	: EditableItem(scene, id),
	actionToBottom(this, tr("To Bottom"), Icons::loadIcon("layer-bottom"), QKeySequence("Ctrl+End")),
	actionLower(this, tr("Lower"), Icons::loadIcon("layer-lower"), QKeySequence("Ctrl+PgDown")),
	actionRaise(this, tr("Raise"), Icons::loadIcon("layer-raise"), QKeySequence("Ctrl+PgUp")),
	actionToTop(this, tr("To Top"), Icons::loadIcon("layer-top"), QKeySequence("Ctrl+Home")),
	orderMenu(tr("Order"))
{
	this->setLevel(level);

	this->orderMenu.addActions({ &(this->actionToTop), &(this->actionRaise), &(this->actionLower), &(this->actionToBottom) });
}

const quint64 ArrangeableItem::alwaysOnTop = ~quint64(0);

QVector<UndoRedoEvent> ArrangeableItem::toBottomItemEvent()
{
	if (this->scene && !this->isAlwaysOnTop())
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;
		QSharedPointer<int> cSteps = QSharedPointer<int>(new int(0));

		std::function<void()> undo = [cScene, cId, cSteps]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ArrangeableItem>(item))
			{
				for (int i = 0; i < *cSteps; i++)
				{
					cScene->raiseItem(item.dynamicCast<ArrangeableItem>().data());
				}
			}
		};

		std::function<void()> redo = [cScene, cId, cSteps]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			*cSteps = 0;

			if (instanceof<ArrangeableItem>(item))
			{
				while (cScene->lowerItem(item.dynamicCast<ArrangeableItem>().data()))
				{
					(*cSteps)++;
				}
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

void ArrangeableItem::toBottomItem()
{
	if (this->scene)
	{
		this->scene->change(this->toBottomItemEvent());
	}
}

QVector<UndoRedoEvent> ArrangeableItem::lowerItemEvent()
{
	if (this->scene && !this->isAlwaysOnTop())
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;

		std::function<void()> undo = [cScene, cId]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ArrangeableItem>(item))
			{
				cScene->raiseItem(item.dynamicCast<ArrangeableItem>().data());
			}
		};

		std::function<void()> redo = [cScene, cId]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ArrangeableItem>(item))
			{
				cScene->lowerItem(item.dynamicCast<ArrangeableItem>().data());
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

void ArrangeableItem::lowerItem()
{
	if (this->scene)
	{
		this->scene->change(this->lowerItemEvent());
	}
}

QVector<UndoRedoEvent> ArrangeableItem::raiseItemEvent()
{
	if (this->scene && !this->isAlwaysOnTop())
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;

		std::function<void()> undo = [cScene, cId]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ArrangeableItem>(item))
			{
				cScene->lowerItem(item.dynamicCast<ArrangeableItem>().data());
			}
		};

		std::function<void()> redo = [cScene, cId]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ArrangeableItem>(item))
			{
				cScene->raiseItem(item.dynamicCast<ArrangeableItem>().data());
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

void ArrangeableItem::raiseItem()
{
	if (this->scene)
	{
		this->scene->change(this->raiseItemEvent());
	}
}

QVector<UndoRedoEvent> ArrangeableItem::toTopItemEvent()
{
	if (this->scene && !this->isAlwaysOnTop())
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;
		QSharedPointer<int> cSteps = QSharedPointer<int>(new int(0));

		std::function<void()> undo = [cScene, cId, cSteps]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ArrangeableItem>(item))
			{
				for (int i = 0; i < *cSteps; i++)
				{
					cScene->lowerItem(item.dynamicCast<ArrangeableItem>().data());
				}
			}
		};

		std::function<void()> redo = [cScene, cId, cSteps]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			*cSteps = 0;

			if (instanceof<ArrangeableItem>(item))
			{
				while (cScene->raiseItem(item.dynamicCast<ArrangeableItem>().data()))
				{
					(*cSteps)++;
				}
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

void ArrangeableItem::toTopItem()
{
	if (this->scene)
	{
		this->scene->change(this->toTopItemEvent());
	}
}

quint64 ArrangeableItem::getLevel() const
{
	return this->level;
}

void ArrangeableItem::setLevel(quint64 level)
{
	this->level = level;

	if (level == alwaysOnTop)
	{
		this->setZValue(std::numeric_limits<qreal>::max());
	}
	else
	{
		this->setZValue(static_cast<qreal>(level));
	}
}

bool ArrangeableItem::isAlwaysOnTop() const
{
	return level == alwaysOnTop;
}

void ArrangeableItem::contextMenuAdditions(QMenu& contextMenu)
{
	if (!this->isAlwaysOnTop())
	{
		if (this->scene)
		{
			QSharedPointer<ArrangeableItem> highestItem = this->scene->getHighestItem();

			QSharedPointer<ArrangeableItem> lowestItem = this->scene->getLowestItem();

			this->actionToBottom.setTriggeredFunction(std::function<void()>([&]() { this->toBottomItem(); }));

			this->actionLower.setTriggeredFunction(std::function<void()>([&]() { this->lowerItem(); }));

			this->actionRaise.setTriggeredFunction(std::function<void()>([&]() { this->raiseItem(); }));

			this->actionToTop.setTriggeredFunction(std::function<void()>([&]() { this->toTopItem(); }));

			this->actionToTop.setEnabled(highestItem.data() != this);

			this->actionRaise.setEnabled(highestItem.data() != this);

			this->actionLower.setEnabled(lowestItem.data() != this);

			this->actionToBottom.setEnabled(lowestItem.data() != this);
		}

		contextMenu.addMenu(&(this->orderMenu));

		contextMenu.addSeparator();
	}
}

void ArrangeableItem::contextMenuAdditionsGlobal(QMenu& contextMenu)
{
	if (!this->isAlwaysOnTop())
	{
		if (this->scene)
		{
			bool lowerable = this->scene->isLowerable();

			bool raisable = this->scene->isRaisable();

			this->actionToBottom.setTriggeredFunction(std::function<void()>([&]() { this->scene->toBottomAll(); }));

			this->actionLower.setTriggeredFunction(std::function<void()>([&]() { this->scene->lowerAll(); }));

			this->actionRaise.setTriggeredFunction(std::function<void()>([&]() { this->scene->raiseAll(); }));

			this->actionToTop.setTriggeredFunction(std::function<void()>([&]() { this->scene->toTopAll(); }));

			this->actionToTop.setEnabled(raisable);

			this->actionRaise.setEnabled(raisable);

			this->actionLower.setEnabled(lowerable);

			this->actionToBottom.setEnabled(lowerable);
		}

		contextMenu.addMenu(&(this->orderMenu));

		contextMenu.addSeparator();
	}
}

MovableItem::MovableItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position) : ArrangeableItem(scene, id, level), grabbed(false), moving(false), grid(true)
{
	this->setPosition(position);
}

QVector<UndoRedoEvent> MovableItem::deleteItemEvent()
{
	this->endMove();

	return ArrangeableItem::deleteItemEvent();
}

QVector<UndoRedoEvent> MovableItem::moveItemEvent(const QPointF& newPosition)
{
	if (this->scene)
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;
		QPointF cPosition = this->position;

		std::function<void()> undo = [cScene, cId, cPosition]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<MovableItem>(item))
			{
				item.dynamicCast<MovableItem>()->setPosition(cPosition);
			}
		};

		cPosition = newPosition;

		std::function<void()> redo = [cScene, cId, cPosition]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<MovableItem>(item))
			{
				item.dynamicCast<MovableItem>()->setPosition(cPosition);
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

void MovableItem::moveItem(const QPointF& newPosition)
{
	if (this->scene)
	{
		this->scene->change(this->moveItemEvent(newPosition));
	}
}

const QPointF& MovableItem::getPosition() const
{
	return this->position;
}

bool MovableItem::checkMoved(QVector<UndoRedoEvent>& events)
{
	if (this->pos() != this->position)
	{
		events.append(this->moveItemEvent(this->pos()));

		return true;
	}

	return false;
}

bool MovableItem::isGrabbed() const
{
	return this->grabbed;
}

bool MovableItem::isMoving() const
{
	return this->moving;
}

void MovableItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	ArrangeableItem::mousePressEvent(event);

	if (event->button() == Qt::MouseButton::LeftButton)
	{
		this->beginMove();
	}
	else
	{
		this->endMove();
	}
}

void MovableItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	this->move();

	ArrangeableItem::mouseMoveEvent(event);
}

void MovableItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	ArrangeableItem::mouseReleaseEvent(event);

	this->endMove();
}

void MovableItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	this->endMove();

	ArrangeableItem::mouseDoubleClickEvent(event);

	this->endMove();
}

void MovableItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	this->endMove();

	ArrangeableItem::contextMenuEvent(event);
}

QVariant MovableItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == ItemPositionChange && this->grid)
	{
		qreal steps = 1.0;

		if (QApplication::keyboardModifiers() & Qt::KeyboardModifier::ShiftModifier)
		{
			steps = 20.0;
		}
		else if (QApplication::keyboardModifiers() & Qt::KeyboardModifier::ControlModifier)
		{
			steps = 10.0;
		}

		if (steps != 1.0)
		{
			QPointF newPosition = value.toPointF();

			qreal x = std::floor(newPosition.x() / steps) * steps;
			qreal y = std::floor(newPosition.y() / steps) * steps;

			return QPointF(x, y);
		}
	}

	return ArrangeableItem::itemChange(change, value);
}

void MovableItem::setPosition(const QPointF& position)
{
	bool gridBuff = this->grid;

	this->grid = false;

	this->position = position;

	this->setPos(position);

	this->grid = gridBuff;
}

void MovableItem::beginMove()
{
	if (this->scene && !this->grabbed)
	{
		this->setSelected(true);

		this->cursorPosition = this->scene->getCursorPosition();

		this->grabbed = true;

		this->moving = true;
	}
}

void MovableItem::move()
{
	if (this->scene && this->grabbed)
	{
		QPointF distance = this->scene->getCursorPosition() - this->cursorPosition;

		QList<QSharedPointer<MovableItem>> items = this->scene->getItems<MovableItem>();

		for (auto& item : items)
		{
			if (item)
			{
				item->setMoving(item->isSelected());
			}
		}

		for (auto& item : items)
		{
			if (item)
			{
				if (item->isSelected())
				{
					item->setPos(item->getPosition() + distance);
				}
			}
		}

		this->scene->setCursor(Qt::CursorShape::SizeAllCursor);
	}
}

void MovableItem::endMove()
{
	if (this->scene)
	{
		if (this->grabbed)
		{
			this->grabbed = false;

			QVector<UndoRedoEvent> events;

			for (auto& item : this->scene->getItems<MovableItem>())
			{
				if (item)
				{
					item->setMoving(false);

					item->checkMoved(events);
				}
			}

			this->scene->change(events);

			this->moving = false;
		}

		this->scene->setCursor();
	}
}

void MovableItem::setMoving(bool moving)
{
	this->moving = moving;
}
