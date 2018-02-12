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

#include "item-scene.hpp"

ItemScene::ItemScene(QGraphicsView* view, const QString& tagName) : ItemDragger(view, this), view(view), minimumRect(-minimumSize, -minimumSize, minimumSize * 2.0, minimumSize * 2.0), tagName(tagName)
{
	if (view)
	{
		view->setScene(&(this->scene));
	}

	this->scene.setSceneRect(minimumRect);

	this->centerScene();
}

ItemScene::~ItemScene()
{
	this->removeAllItems();
}

QWidget* ItemScene::getWidget()
{
	return view;
}

QRectF ItemScene::getBoundingRect()
{
	QList<QSharedPointer<Item>> items = this->getVisibleItems();

	if (items.size())
	{
		bool set = false;

		qreal left = 0.0;
		qreal top = 0.0;
		qreal right = 0.0;
		qreal bottom = 0.0;

		for (auto& item : items)
		{
			if (item)
			{
				QRectF rect = item->boundingRect();

				QPointF position = item->pos();

				rect.setLeft(rect.left() + position.x());
				rect.setTop(rect.top() + position.y());
				rect.setRight(rect.right() + position.x());
				rect.setBottom(rect.bottom() + position.y());

				if (!set)
				{
					left = rect.left();
					top = rect.top();
					right = rect.right();
					bottom = rect.bottom();

					set = true;
				}
				else
				{
					left = qMin(left, rect.left());
					top = qMin(top, rect.top());
					right = qMax(right, rect.right());
					bottom = qMax(bottom, rect.bottom());
				}
			}
		}

		if (set)
		{
			return QRectF(left, top, right - left, bottom - top);
		}
	}

	return QRectF();
}

void ItemScene::recalculateRect(const QRectF& additionalRect)
{
	if (!this->rectItemsCache.contains(additionalRect))
	{
		QRectF rectItems;

		rectItems.setLeft(qMin(this->rectItemsCache.left(), additionalRect.left()));
		rectItems.setTop(qMin(this->rectItemsCache.top(), additionalRect.top()));
		rectItems.setRight(qMax(this->rectItemsCache.right(), additionalRect.right()));
		rectItems.setBottom(qMax(this->rectItemsCache.bottom(), additionalRect.bottom()));

		this->rectItemsCache = rectItems;

		rectItems.setRect(rectItems.left() - rectItems.width() / 2.0, rectItems.top() - rectItems.height() / 2.0, rectItems.width() * 2.0, rectItems.height() * 2.0);

		rectItems.setLeft(qMin(rectItems.left(), minimumRect.left()));
		rectItems.setTop(qMin(rectItems.top(), minimumRect.top()));
		rectItems.setRight(qMax(rectItems.right(), minimumRect.right()));
		rectItems.setBottom(qMax(rectItems.bottom(), minimumRect.bottom()));

		QRectF rectScene = this->scene.sceneRect();

		if (!rectScene.contains(rectItems))
		{
			qreal left = qMin(rectItems.left(), rectScene.left());
			qreal top = qMin(rectItems.top(), rectScene.top());
			qreal right = qMax(rectItems.right(), rectScene.right());
			qreal bottom = qMax(rectItems.bottom(), rectScene.bottom());

			this->scene.setSceneRect(left, top, right - left, bottom - top);
		}
	}
}

void ItemScene::centerScene()
{
	if (this->view)
	{
		view->centerOn(0, 0);
	}
}

void ItemScene::setCursor(const QCursor& cursor)
{
	if (this->view)
	{
		this->view->viewport()->setCursor(cursor);
	}
}

bool ItemScene::isExportable()
{
	for (auto& item : this->items)
	{
		if (item)
		{
			if (item->isVisible())
			{
				return true;
			}
		}
	}

	return false;
}

QImage ItemScene::renderToImage(const QRectF& sceneRect, const QSize& imageSize, bool transparent)
{
	this->scene.clearSelection();

	QImage image(imageSize, QImage::Format::Format_ARGB32);

	if (!image.isNull())
	{
		image.fill(transparent ? QColor(255, 255, 255, 0) : Settings::getColor(Colors::Background));

		QPainter painter(&image);

		this->scene.render(&painter, QRectF(), sceneRect);
	}

	return image;
}

QImage ItemScene::renderToImage(qreal scale, bool transparent)
{
	QRectF rect = this->getBoundingRect();

	QSize size = rect.size().toSize();
	size.setWidth(size.width() * scale);
	size.setHeight(size.height() * scale);

	return this->renderToImage(rect, size, transparent);
}

bool ItemScene::exportImage(const QString& path, qreal scale)
{
	return this->renderToImage(scale).save(path);
}

void ItemScene::print()
{
	if (this->isExportable())
	{
		QPrintDialog dialog(getWidget());

		if (dialog.exec() == QDialog::DialogCode::Accepted)
		{
			QPrinter* printer = dialog.printer();

			if (printer)
			{
				QRect pageRect = printer->pageRect();

				QSizeF sizePage = pageRect.size();

				QRectF rectScene = this->getBoundingRect();

				QSizeF sizeScene = rectScene.size();

				qreal aspectRatioPage = sizePage.width() / sizePage.height();
				qreal aspectRatioScene = sizeScene.width() / sizeScene.height();

				if (aspectRatioPage >= aspectRatioScene)
				{
					rectScene.setWidth(aspectRatioPage * rectScene.height());
				}
				else
				{
					rectScene.setHeight(rectScene.width() / aspectRatioPage);
				}

				QImage image = this->renderToImage(rectScene, sizePage.toSize());

				QPainter painterPrinter(printer);

				painterPrinter.drawImage(pageRect, image);

				painterPrinter.end();
			}
		}
	}
}

QPointF ItemScene::getCursorPosition() const
{
	QPointF position = QCursor::pos();

	position = this->view->mapFromGlobal(position.toPoint());

	return this->view->mapToScene(position.toPoint());
}

QSharedPointer<Item> ItemScene::getItemById(const Identifier& id)
{
	if (id)
	{
		for (auto& item : this->items)
		{
			if (item)
			{
				if (item->getId() == id)
				{
					return item;
				}
			}
		}
	}

	return QSharedPointer<Item>();
}

bool ItemScene::isItemAtPosition(const QPointF& position)
{
	return this->scene.itemAt(position, QTransform()) ? true : false;
}

Identifier ItemScene::queryId(quint64 id)
{
	for (auto& item : this->items)
	{
		if (item)
		{
			if (item->getId().getValue() == id)
			{
				return item->getId();
			}
		}
	}

	return Identifier();
}

void ItemScene::addItem(const QSharedPointer<Item>& item)
{
	if (item)
	{
		if (instanceof<ArrangeableItem>(item))
		{
			if (!item.dynamicCast<ArrangeableItem>()->isAlwaysOnTop())
			{
				for (int i = 0; i < this->items.size(); i++)
				{
					if (instanceof<ArrangeableItem>(this->items[i]))
					{
						if (!this->items[i].dynamicCast<ArrangeableItem>()->isAlwaysOnTop() && this->items[i].dynamicCast<ArrangeableItem>()->getLevel() > item.dynamicCast<ArrangeableItem>()->getLevel())
						{
							this->items.insert(this->items.begin() + i, item);

							this->scene.addItem(item.data());

							item->postConstruction();

							return;
						}
					}
				}
			}
		}

		this->items.append(item);

		this->scene.addItem(item.data());

		item->postConstruction();
	}
}

void ItemScene::removeItem(Item* item)
{
	for (int i = 0; i < this->items.size(); i++)
	{
		if (this->items[i].data() == item)
		{
			QSharedPointer<Item> itemRAIIDeleteLater = this->items[i];

			this->items.removeAt(i);

			break;
		}
	}
}

void ItemScene::removeItemById(const Identifier& id)
{
	for (int i = 0; i < this->items.size(); i++)
	{
		if (this->items[i])
		{
			if (this->items[i]->getId() == id)
			{
				QSharedPointer<Item> itemRAIIDeleteLater = this->items[i];

				this->items.removeAt(i);

				break;
			}
		}
	}
}

void ItemScene::removeAllItems()
{
	while (this->items.size())
	{
		QSharedPointer<Item> itemRAIIDeleteLater = this->items.last();

		this->items.removeLast();
	}
}

void ItemScene::updateItems()
{
	for (auto& item : this->items)
	{
		if (item)
		{
			item->updateItem();
		}
	}
}

bool ItemScene::lowerItem(ArrangeableItem* arrangeableItem)
{
	if (instanceof<ArrangeableItem>(arrangeableItem))
	{
		int thisIndex = -1;
		int otherIndex = -1;

		for (int i = this->items.size() - 1; i >= 0; i--)
		{
			if (instanceof<ArrangeableItem>(this->items[i]))
			{
				if (!this->items[i].dynamicCast<ArrangeableItem>()->isAlwaysOnTop())
				{
					if (this->items[i].data() == arrangeableItem)
					{
						thisIndex = i;
					}
					else if (this->items[i].dynamicCast<ArrangeableItem>()->getLevel() < arrangeableItem->getLevel())
					{
						otherIndex = i;

						if (this->items[i].dynamicCast<ArrangeableItem>()->isVisible())
						{
							break;
						}
					}
				}
			}
		}

		if (thisIndex >= 0 && otherIndex >= 0)
		{
			quint64 thisLevel = arrangeableItem->getLevel();

			arrangeableItem->setLevel(this->items[otherIndex].dynamicCast<ArrangeableItem>()->getLevel());
			this->items[otherIndex].dynamicCast<ArrangeableItem>()->setLevel(thisLevel);

			this->items.swap(thisIndex, otherIndex);

			return true;
		}
	}

	return false;
}

bool ItemScene::raiseItem(ArrangeableItem* arrangeableItem)
{
	if (instanceof<ArrangeableItem>(arrangeableItem))
	{
		int thisIndex = -1;
		int otherIndex = -1;

		for (int i = 0; i < this->items.size(); i++)
		{
			if (instanceof<ArrangeableItem>(this->items[i]))
			{
				if (!this->items[i].dynamicCast<ArrangeableItem>()->isAlwaysOnTop())
				{
					if (this->items[i].data() == arrangeableItem)
					{
						thisIndex = i;
					}
					else if (this->items[i].dynamicCast<ArrangeableItem>()->getLevel() > arrangeableItem->getLevel())
					{
						otherIndex = i;

						if (this->items[i].dynamicCast<ArrangeableItem>()->isVisible())
						{
							break;
						}
					}
				}
			}
		}

		if (thisIndex >= 0 && otherIndex >= 0)
		{
			quint64 thisLevel = arrangeableItem->getLevel();

			arrangeableItem->setLevel(this->items[otherIndex].dynamicCast<ArrangeableItem>()->getLevel());
			this->items[otherIndex].dynamicCast<ArrangeableItem>()->setLevel(thisLevel);

			this->items.swap(thisIndex, otherIndex);

			return true;
		}
	}

	return false;
}

QSharedPointer<ArrangeableItem> ItemScene::getLowestItem()
{
	QList<QSharedPointer<ArrangeableItem>> items = this->getItems<ArrangeableItem>();

	for (int i = 0; i < items.size(); i++)
	{
		if (items[i])
		{
			if (!items[i]->isAlwaysOnTop())
			{
				return items[i];
			}
		}
	}

	return QSharedPointer<ArrangeableItem>();
}

QSharedPointer<ArrangeableItem> ItemScene::getHighestItem()
{
	QList<QSharedPointer<ArrangeableItem>> items = this->getItems<ArrangeableItem>();

	for (int i = items.size() - 1; i >= 0; i--)
	{
		if (items[i])
		{
			if (!items[i]->isAlwaysOnTop())
			{
				return items[i];
			}
		}
	}

	return QSharedPointer<ArrangeableItem>();
}

bool ItemScene::isSelectable() const
{
	for (auto& item : this->items)
	{
		if (instanceof<SelectableItem>(item))
		{
			if (item->isVisible())
			{
				return true;
			}
		}
	}

	return false;
}

bool ItemScene::isSelected() const
{
	QList<QGraphicsItem*> selectedItems = this->scene.selectedItems();

	if (selectedItems.size())
	{
		for (auto& selectedItem : selectedItems)
		{
			if (instanceof<SelectableItem>(selectedItem))
			{
				return true;
			}
		}
	}

	return false;
}

void ItemScene::selectAll()
{
	for (auto& item : this->items)
	{
		if (instanceof<SelectableItem>(item))
		{
			if (item->isVisible())
			{
				item->setSelected(true);
			}
		}
	}
}

void ItemScene::deselectAll()
{
	this->scene.clearSelection();
}

void ItemScene::deleteAll()
{
	QVector<UndoRedoEvent> events;

	for (auto& item : this->items)
	{
		if (instanceof<EditableItem>(item))
		{
			if (item->isSelected() && item->isVisible())
			{
				events.append(item.dynamicCast<EditableItem>()->deleteItemEvent());
			}
		}
	}

	this->change(events);
}

void ItemScene::defragOrder()
{
	QList<QSharedPointer<ArrangeableItem>> arrangeableItems;

	for (auto& item : this->items)
	{
		if (instanceof<ArrangeableItem>(item))
		{
			if (!item.dynamicCast<ArrangeableItem>()->isAlwaysOnTop())
			{
				arrangeableItems.append(item.dynamicCast<ArrangeableItem>());
			}
		}
	}

	for (int i = 0; i < arrangeableItems.size(); i++)
	{
		arrangeableItems[i]->setLevel(i);
	}
}

quint64 ItemScene::queryLevel()
{
	quint64 level = 0;

	for (auto& item : this->items)
	{
		if (instanceof<ArrangeableItem>(item))
		{
			if (!item.dynamicCast<ArrangeableItem>()->isAlwaysOnTop() && item.dynamicCast<ArrangeableItem>()->getLevel() > level)
			{
				level = item.dynamicCast<ArrangeableItem>()->getLevel();
			}
		}
	}

	if (level < ArrangeableItem::alwaysOnTop - 1)
	{
		level++;
	}

	if (level == ArrangeableItem::alwaysOnTop)
	{
		level--;
	}

	return level;
}

bool ItemScene::isLowerable() const
{
	bool found = false;

	for (int i = 0; i < this->items.size(); i++)
	{
		if (instanceof<ArrangeableItem>(this->items[i]))
		{
			if (!this->items[i].dynamicCast<ArrangeableItem>()->isAlwaysOnTop())
			{
				if (!this->items[i]->isSelected())
				{
					if (!found)
					{
						found = true;
					}
				}
				else
				{
					if (found)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool ItemScene::isRaisable() const
{
	bool found = false;

	for (int i = this->items.size() - 1; i >= 0; i--)
	{
		if (instanceof<ArrangeableItem>(this->items[i]))
		{
			if (!this->items[i].dynamicCast<ArrangeableItem>()->isAlwaysOnTop())
			{
				if (!this->items[i]->isSelected())
				{
					if (!found)
					{
						found = true;
					}
				}
				else
				{
					if (found)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

void ItemScene::toBottomAll()
{
	if (this->isLowerable())
	{
		QVector<UndoRedoEvent> events;

		QList<QSharedPointer<SelectableItem>> selectedItems = this->getSelectedItems();

		for (int i = selectedItems.size() - 1; i >= 0; i--)
		{
			if (instanceof<ArrangeableItem>(selectedItems[i]))
			{
				if (!selectedItems[i].dynamicCast<ArrangeableItem>()->isAlwaysOnTop())
				{
					events.append(selectedItems[i].dynamicCast<ArrangeableItem>()->toBottomItemEvent());
				}
			}
		}

		this->change(events);
	}
}

void ItemScene::lowerAll()
{
	QVector<UndoRedoEvent> events;

	QList<QSharedPointer<ArrangeableItem>> arrangeableItems;

	for (auto& item : this->items)
	{
		if (instanceof<ArrangeableItem>(item))
		{
			if (!item.dynamicCast<ArrangeableItem>()->isAlwaysOnTop())
			{
				arrangeableItems.append(item.dynamicCast<ArrangeableItem>());
			}
		}
	}

	if (arrangeableItems.size())
	{
		if (arrangeableItems.first() != this->getLowestItem())
		{
			events.append(arrangeableItems.first()->lowerItemEvent());
		}
	}

	for (int i = 1; i < arrangeableItems.size(); i++)
	{
		if (arrangeableItems[i]->isSelected() && !arrangeableItems[i - 1]->isSelected())
		{
			events.append(arrangeableItems[i]->lowerItemEvent());
		}
	}

	this->change(events);
}

void ItemScene::raiseAll()
{
	QVector<UndoRedoEvent> events;

	QList<QSharedPointer<ArrangeableItem>> arrangeableItems;

	for (auto& item : this->items)
	{
		if (instanceof<ArrangeableItem>(item))
		{
			if (!item.dynamicCast<ArrangeableItem>()->isAlwaysOnTop())
			{
				arrangeableItems.append(item.dynamicCast<ArrangeableItem>());
			}
		}
	}

	if (arrangeableItems.size())
	{
		if (arrangeableItems.last() != this->getHighestItem())
		{
			events.append(arrangeableItems.last()->raiseItemEvent());
		}
	}

	for (int i = arrangeableItems.size() - 2; i >= 0; i--)
	{
		if (arrangeableItems[i]->isSelected() && !arrangeableItems[i + 1]->isSelected())
		{
			events.append(arrangeableItems[i]->raiseItemEvent());
		}
	}

	this->change(events);
}

void ItemScene::toTopAll()
{
	if (this->isRaisable())
	{
		QVector<UndoRedoEvent> events;

		QList<QSharedPointer<SelectableItem>> selectedItems = this->getSelectedItems();

		for (int i = 0; i < selectedItems.size(); i++)
		{
			if (instanceof<ArrangeableItem>(selectedItems[i]))
			{
				if (!selectedItems[i].dynamicCast<ArrangeableItem>()->isAlwaysOnTop())
				{
					events.append(selectedItems[i].dynamicCast<ArrangeableItem>()->toTopItemEvent());
				}
			}
		}

		this->change(events);
	}
}

void ItemScene::importItems(QDomElement root, bool reset)
{
	if (root.tagName() == this->tagName)
	{
		if (reset)
		{
			this->removeAllItems();

			this->integrate(root);
		}
		else
		{
			this->integrate(root, this->getCursorPosition());
		}

		QVector<UndoRedoEvent> events;

		for (auto& itemCreator : ItemDragger::getItemCreators())
		{
			events.append(itemCreator.loadItems(this, root));
		}

		if (reset)
		{
			this->execute(events);

			this->centerScene();
		}
		else
		{
			this->change(events);
		}

		this->defragOrder();
	}
}

void ItemScene::importItems(const QString& content, bool reset)
{
	QDomDocument document;

	if (document.setContent(content))
	{
		this->importItems(document.firstChildElement(), reset);
	}
}

void ItemScene::importItems(QFile& file, bool reset)
{
	QDomDocument document;

	if (document.setContent(&file))
	{
		this->importItems(document.firstChildElement(), reset);
	}
}

QString ItemScene::exportItems()
{
	QDomDocument document;

	QDomProcessingInstruction header = document.createProcessingInstruction("xml", "version=\"1.0\"");

	document.appendChild(header);

	QDomElement root = document.createElement(this->tagName);

	QList<QSharedPointer<Item>> items = this->getVisibleItems();

	for (auto& item : items)
	{
		if (item)
		{
			item->appendXml(root);
		}
	}

	document.appendChild(root);

	return document.toString();
}

QString ItemScene::exportSelectedItems()
{
	QDomDocument document;

	QDomProcessingInstruction header = document.createProcessingInstruction("xml", "version=\"1.0\"");

	document.appendChild(header);

	QDomElement root = document.createElement(this->tagName);

	QList<QSharedPointer<SelectableItem>> items = this->getSelectedItems();

	for (auto& item : items)
	{
		if (item)
		{
			item->appendXml(root);
		}
	}

	document.appendChild(root);

	return document.toString();
}

bool ItemScene::isCopyable() const
{
	if (this->isSelected())
	{
		for (auto& item : this->items)
		{
			if (instanceof<EditableItem>(item))
			{
				if (item.dynamicCast<EditableItem>()->isVisible() && item.dynamicCast<EditableItem>()->isSelected() && item.dynamicCast<EditableItem>()->isCopyable())
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool ItemScene::isInsertable() const
{
	const QClipboard* clipboard = QApplication::clipboard();

	if (clipboard)
	{
		if (clipboard->text() != "")
		{
			const QMimeData *mimeData = clipboard->mimeData();

			if (mimeData)
			{
				if (mimeData->hasText())
				{
					if (mimeData->text() != "")
					{
						QDomDocument document;

						if (document.setContent(mimeData->text()))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

void ItemScene::cut()
{
	if (this->isCopyable())
	{
		this->copy();

		this->deleteAll();
	}
}

void ItemScene::copy()
{
	if (this->isCopyable())
	{
		QClipboard* clipboard = QApplication::clipboard();

		if (clipboard)
		{
			clipboard->setText(this->exportSelectedItems());
		}
	}
}

void ItemScene::paste()
{
	const QClipboard* clipboard = QApplication::clipboard();

	if (clipboard)
	{
		const QMimeData *mimeData = clipboard->mimeData();

		if (mimeData)
		{
			if (mimeData->hasText())
			{
				this->importItems(mimeData->text());
			}
		}
	}
}

void ItemScene::integrateIds(QVector<QDomElement>& elements)
{
	QMap<quint64, Identifier> ids;

	for (auto& element : elements)
	{
		if (element.hasAttribute("id"))
		{
			quint64 id = element.attribute("id").toULongLong(nullptr, 16);

			if (!ids.contains(id))
			{
				ids.insert(id, this->newIdentifier());
			}
		}
	}

	for (auto& element : elements)
	{
		if (element.hasAttribute("id"))
		{
			quint64 id = element.attribute("id").toULongLong(nullptr, 16);

			if (ids.contains(id))
			{
				element.setAttribute("id", ids[id].toString());
			}
			else
			{
				element.setAttribute("id", "0");
			}
		}
	}
}

void ItemScene::integrateLevels(QVector<QDomElement>& elements)
{
	QMap<quint64, quint64> levels;

	for (auto& element : elements)
	{
		if (element.hasAttribute("level"))
		{
			quint64 level = element.attribute("level").toULongLong();

			if (!levels.contains(level))
			{
				levels.insert(level, 0);
			}
		}
	}

	quint64 levelCounter = this->queryLevel();

	QMapIterator<quint64, quint64> i(levels);

	while (i.hasNext())
	{
		i.next();

		if (levels.contains(i.key()))
		{
			levels[i.key()] = levelCounter;

			if (levelCounter < ArrangeableItem::alwaysOnTop - 1)
			{
				levelCounter++;
			}
		}
	}

	for (auto& element : elements)
	{
		if (element.hasAttribute("level"))
		{
			quint64 level = element.attribute("level").toULongLong();

			if (levels.contains(level))
			{
				element.setAttribute("level", QString::number(levels[level]));
			}
			else if (levels.size())
			{
				element.setAttribute("level", QString::number(levels.last()));
			}
			else
			{
				element.setAttribute("level", QString::number(levelCounter));
			}
		}
	}
}

void ItemScene::integratePositions(QVector<QDomElement>& elements, const QPointF& offsetPosition)
{
	QPoint minPosition(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());

	for (auto& element : elements)
	{
		if (element.hasAttribute("position-x"))
		{
			minPosition.setX(qMin(minPosition.x(), element.attribute("position-x").toInt()));
		}

		if (element.hasAttribute("position-y"))
		{
			minPosition.setY(qMin(minPosition.y(), element.attribute("position-y").toInt()));
		}
	}

	QPointF offset = offsetPosition - minPosition;

	for (auto& element : elements)
	{
		if (element.hasAttribute("position-x"))
		{
			element.setAttribute("position-x", QString::number(element.attribute("position-x").toInt() + offset.x()));
		}

		if (element.hasAttribute("position-y"))
		{
			element.setAttribute("position-y", QString::number(element.attribute("position-y").toInt() + offset.y()));
		}
	}
}

void ItemScene::integrate(QDomElement root)
{
	QVector<QDomElement> elements = Xml::getAllElements(root);

	this->integrateIds(elements);

	this->integrateLevels(elements);
}

void ItemScene::integrate(QDomElement root, const QPointF& offsetPosition)
{
	QVector<QDomElement> elements = Xml::getAllElements(root);

	this->integrateIds(elements);

	this->integrateLevels(elements);

	this->integratePositions(elements, offsetPosition);
}

qreal ItemScene::minimumSize = 2048;
