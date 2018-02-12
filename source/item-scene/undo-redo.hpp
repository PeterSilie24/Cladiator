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

#include <QStack>
#include <QVector>
#include <QSharedPointer>
#include <QGraphicsItem>

#include <functional>

class UndoRedo;

class UndoRedoEvent
{
public:
	UndoRedoEvent();

	UndoRedoEvent(const std::function<void()>& undoFunction, const std::function<void()>& redoFunction);

	void undo() const;

	void redo() const;

private:
	std::function<void()> undoFunction;

	std::function<void()> redoFunction;
};

class UndoRedo
{
public:
	UndoRedo();

	bool isUndoable() const;

	bool isRedoable() const;

	void undo();

	void redo();

	void change(const UndoRedoEvent& newEvent);

	void change(const QVector<UndoRedoEvent>& newEvents);

	void execute(const QVector<UndoRedoEvent>& newEvents);

	bool hasChanged() const;

	void resetChanged();

private:
	bool changed;

	QStack<QVector<UndoRedoEvent>> stackUndo;
	QStack<QVector<UndoRedoEvent>> stackRedo;
};