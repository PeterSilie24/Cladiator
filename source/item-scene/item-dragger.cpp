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

#include "item-dragger.hpp"

#include "item-scene.hpp"

ItemCreator::ItemCreator()
{

}

ItemCreator::ItemCreator(std::function<QVector<UndoRedoEvent>(ItemScene*, QDomElement element)> loadItems, std::function<void(ItemScene*, const QPointF&)> createItem, const QString& text, const QString& iconName) : loadItemsFunction(loadItems), createItemFunction(createItem), text(text), iconName(iconName)
{

}

QVector<UndoRedoEvent> ItemCreator::loadItems(ItemScene* scene, QDomElement element) const
{
	if (this->loadItemsFunction)
	{
		return this->loadItemsFunction(scene, element);
	}

	return QVector<UndoRedoEvent>();
}

void ItemCreator::createItem(ItemScene* scene, const QPointF& position) const
{
	if (this->createItemFunction)
	{
		this->createItemFunction(scene, position);
	}
}

const QString& ItemCreator::getText() const
{
	return this->text;
}

const QString& ItemCreator::getIconName() const
{
	return this->iconName;
}

QVector<ItemCreator> ItemDragger::itemCreators;

const QVector<ItemCreator> ItemDragger::getItemCreators()
{
	return ItemDragger::itemCreators;
}

ItemDragger::ItemDragger(QObject* parent, ItemScene* scene) : scene(scene), newItemCreator(nullptr)
{
	for (int i = 0; i < ItemDragger::itemCreators.size(); i++)
	{
		this->actionsCreate.append(QSharedPointer<Action>(new Action(parent, ItemDragger::itemCreators[i].getText(), Icons::loadIcon(ItemDragger::itemCreators[i].getIconName()))));

		if (this->actionsCreate.last())
		{
			int cIndex = i;

			this->actionsCreate.last()->setTriggeredFunction(std::function<void()>([&, cIndex]() { this->dragNewStart(cIndex); }));
		}
	}
}

void ItemDragger::dragNewTrigger()
{
	if (this->isDragNewItem())
	{
		const ItemCreator* newItemCreator = this->newItemCreator;

		this->dragNewEnd();

		if (this->scene && newItemCreator)
		{
			newItemCreator->createItem(this->scene, this->scene->getCursorPosition());
		}
	}
}

void ItemDragger::dragNewEnd()
{
	this->dragNewStart(nullptr);

	if (this->scene)
	{
		this->scene->setCursor();
	}
}

bool ItemDragger::isDragNewItem()
{
	return !!this->newItemCreator;
}

QList<QAction*> ItemDragger::getActionsCreate()
{
	QList<QAction*> actionsCreate;

	for (auto& actionCreate : this->actionsCreate)
	{
		actionsCreate.append(actionCreate.data());
	}

	return actionsCreate;
}

void ItemDragger::dragNewStart(int index)
{
	if (index >= 0 && index < ItemDragger::itemCreators.size())
	{
		this->dragNewStart(&ItemDragger::itemCreators[index]);
	}
}

void ItemDragger::dragNewStart(const ItemCreator* newItemCreator)
{
	this->newItemCreator = newItemCreator;

	if (this->scene && newItemCreator)
	{
		this->scene->setCursor(Qt::CursorShape::CrossCursor);
	}
}
