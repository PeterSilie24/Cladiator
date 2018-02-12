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

#include <QObject>
#include <QWidget>
#include <QApplication>
#include <QGraphicsScene>
#include <QImage>
#include <QPrinter>
#include <QPrintDialog>
#include <QGraphicsView>
#include <QFile>
#include <QMap>
#include <QMapIterator>
#include <QClipboard>

#include "items/item.hpp"

#include "item-dragger.hpp"

class ItemScene : public IdentifierManager, public UndoRedo, public ItemDragger
{
public:
	ItemScene(QGraphicsView* view, const QString& tagName);

	~ItemScene();

	QWidget* getWidget();

	QRectF getBoundingRect();

	void recalculateRect(const QRectF& additionalRect);

	void centerScene();

	void setCursor(const QCursor& cursor = Qt::CursorShape::ArrowCursor);

	bool isExportable();

	QImage renderToImage(const QRectF& sceneRect, const QSize& imageSize, bool transparent = false);

	QImage renderToImage(qreal scale, bool transparent = false);

	bool exportImage(const QString& path, qreal scale);

	void print();

	QPointF getCursorPosition() const;

	QSharedPointer<Item> getItemById(const Identifier& id);

	template <typename T = Item>
	QList<QSharedPointer<T>> getItems()
	{
		return this->getVisibleItems<T>();
	}

	template <typename T = SelectableItem>
	QList<QSharedPointer<T>> getSelectedItems()
	{
		QList<QSharedPointer<T>> items = this->getItems<T>();

		QList<QSharedPointer<T>> selectedItems;

		for (auto& item : items)
		{
			if (item)
			{
				if (item->isSelected())
				{
					selectedItems.append(item);
				}
			}
		}

		return selectedItems;
	}

	template <typename T = Item>
	QList<QSharedPointer<T>> getVisibleItems()
	{
		QList<QSharedPointer<T>> visibleItems;

		for (auto& item : this->items)
		{
			if (instanceof<T>(item))
			{
				if (item->isVisible())
				{
					visibleItems.append(item.dynamicCast<T>());
				}
			}
		}

		return visibleItems;
	}

	template <typename T = Item>
	QList<QSharedPointer<T>> getItemsAtPosition(const QPointF& position)
	{
		QList<QSharedPointer<T>> items = this->getItems<T>();

		QList<QSharedPointer<T>> itemsAtPosition;

		for (auto& item : items)
		{
			if (instanceof<T>(item))
			{
				if (item->shape().contains(position - item->pos()))
				{
					itemsAtPosition.push_front(item);
				}
			}
		}

		return itemsAtPosition;
	}

	template <typename T = Item>
	QSharedPointer<T> getItemAtPosition(const QPointF& position)
	{
		QList<QSharedPointer<T>> itemsAtPosition = this->getItemsAtPosition<T>(position);

		if (itemsAtPosition.size())
		{
			return itemsAtPosition.first();
		}

		return QSharedPointer<T>();
	}

	bool isItemAtPosition(const QPointF& position);

	Identifier queryId(quint64 id);

	void addItem(const QSharedPointer<Item>& item);

	void removeItem(Item* item);

	void removeItemById(const Identifier& id);

	void removeAllItems();

	void updateItems();

	bool lowerItem(ArrangeableItem* arrangeableItem);

	bool raiseItem(ArrangeableItem* arrangeableItem);

	QSharedPointer<ArrangeableItem> getLowestItem();

	QSharedPointer<ArrangeableItem> getHighestItem();

	bool isSelectable() const;

	bool isSelected() const;

	void selectAll();

	void deselectAll();

	void deleteAll();

	void defragOrder();

	quint64 queryLevel();

	bool isLowerable() const;

	bool isRaisable() const;

	void toBottomAll();

	void lowerAll();

	void raiseAll();

	void toTopAll();

	void importItems(QDomElement root, bool reset = false);

	void importItems(const QString& content, bool reset = false);

	void importItems(QFile& file, bool reset = false);

	QString exportItems();

	QString exportSelectedItems();

	bool isCopyable() const;

	bool isInsertable() const;

	void cut();

	void copy();

	void paste();

private:
	void integrateIds(QVector<QDomElement>& elements);

	void integrateLevels(QVector<QDomElement>& elements);

	void integratePositions(QVector<QDomElement>& elements, const QPointF& offsetPosition);

	void integrate(QDomElement root);

	void integrate(QDomElement root, const QPointF& offsetPosition);

	QGraphicsView* view;

	QGraphicsScene scene;

	static qreal minimumSize;

	QRectF minimumRect;

	QList<QSharedPointer<Item>> items;

	QRectF rectItemsCache;

	QString tagName;
};
