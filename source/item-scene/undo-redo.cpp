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

#include "undo-redo.hpp"

UndoRedoEvent::UndoRedoEvent() { }

UndoRedoEvent::UndoRedoEvent(const std::function<void()>& undoFunction, const std::function<void()>& redoFunction) : undoFunction(undoFunction), redoFunction(redoFunction) { }

void UndoRedoEvent::undo() const
{
	if (this->undoFunction)
	{
		this->undoFunction();
	}
}

void UndoRedoEvent::redo() const
{
	if (this->redoFunction)
	{
		this->redoFunction();
	}
}

UndoRedo::UndoRedo() : changed(false) { }

bool UndoRedo::isUndoable() const
{
	return !!(this->stackUndo.size());
}

bool UndoRedo::isRedoable() const
{
	return !!(this->stackRedo.size());
}

void UndoRedo::undo()
{
	if (this->isUndoable())
	{
		QVector<UndoRedoEvent> events = this->stackUndo.pop();

		if (events.size())
		{
			for (int i = events.size() - 1; i >= 0; i--)
			{
				events[i].undo();
			}

			this->stackRedo.push(events);

			this->changed = true;
		}
	}
}

void UndoRedo::redo()
{
	if (this->isRedoable())
	{
		QVector<UndoRedoEvent> events = this->stackRedo.pop();

		if (events.size())
		{
			for (int i = 0; i < events.size(); i++)
			{
				events[i].redo();
			}

			this->stackUndo.push(events);

			this->changed = true;
		}
	}
}

void UndoRedo::change(const UndoRedoEvent& newEvent)
{
	this->change(QVector<UndoRedoEvent>({ newEvent }));
}

void UndoRedo::change(const QVector<UndoRedoEvent>& newEvents)
{
	if (newEvents.size())
	{
		this->stackRedo.clear();

		this->execute(newEvents);

		this->stackUndo.push(newEvents);

		this->changed = true;
	}
}

void UndoRedo::execute(const QVector<UndoRedoEvent>& newEvents)
{
	for (int i = 0; i < newEvents.size(); i++)
	{
		newEvents[i].redo();
	}
}

bool UndoRedo::hasChanged() const
{
	return this->changed;
}

void UndoRedo::resetChanged()
{
	this->changed = false;
}
