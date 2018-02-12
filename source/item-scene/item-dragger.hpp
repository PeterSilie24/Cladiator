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

#include <QtXml>

#include "utils/action.hpp"

#include "undo-redo.hpp"

class ItemScene;

class ItemCreator
{
public:
	ItemCreator();

	ItemCreator(std::function<QVector<UndoRedoEvent>(ItemScene*, QDomElement element)> loadItems, std::function<void(ItemScene*, const QPointF&)> createItem, const QString& text, const QString& iconName);

	template <typename T>
	static ItemCreator create(const QString& text, const QString& iconName)
	{
		return ItemCreator(T::loadItems, T::createItem, text, iconName);
	}

	QVector<UndoRedoEvent> loadItems(ItemScene* scene, QDomElement element) const;

	void createItem(ItemScene* scene, const QPointF& position) const;

	const QString& getText() const;

	const QString& getIconName() const;

private:
	std::function<QVector<UndoRedoEvent>(ItemScene*, QDomElement element)> loadItemsFunction;
	std::function<void(ItemScene*, const QPointF&)> createItemFunction;

	QString text;
	QString iconName;
};

class ItemDragger
{
private:
	static QVector<ItemCreator> itemCreators;

public:
	template <typename T>
	static void appendItemCreator(const QString& text, const QString& iconName)
	{
		ItemDragger::itemCreators.append(ItemCreator::create<T>(text, iconName));
	}

	static const QVector<ItemCreator> getItemCreators();

	ItemDragger(QObject* parent, ItemScene* scene);

	void dragNewTrigger();

	void dragNewEnd();

	bool isDragNewItem();

	QList<QAction*> getActionsCreate();

private:
	void dragNewStart(int index);

	void dragNewStart(const ItemCreator* newItemCreator = nullptr);

	ItemScene* const scene;

	const ItemCreator* newItemCreator;

	QVector<QSharedPointer<Action>> actionsCreate;
};
