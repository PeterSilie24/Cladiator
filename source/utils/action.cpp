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

#include "action.hpp"

Action::Action(QObject* parent, const QString& text, const QIcon& icon, const QKeySequence& shortcut) : QAction(parent)
{
	this->setText(text);

	this->setIcon(icon);

	if (!shortcut.isEmpty())
	{
		this->setShortcut(shortcut);

		#if QT_VERSION >= 0x050A00
		this->setShortcutVisibleInContextMenu(true);
		#endif
	}

	this->connect(this, SIGNAL(changed()), this, SLOT(actionChanged()));

	this->connect(this, SIGNAL(hovered()), this, SLOT(actionHovered()));

	this->connect(this, SIGNAL(toggled(bool)), this, SLOT(actionToggled(bool)));

	this->connect(this, SIGNAL(triggered()), this, SLOT(actionTriggered()));

	this->connect(this, SIGNAL(triggered(bool)), this, SLOT(actionTriggered(bool)));
}

void Action::setChangedFunction(const std::function<void()>& function)
{
	this->actionChangedFunction = function;
}

void Action::setHoveredFunction(const std::function<void()>& function)
{
	this->actionHoveredFunction = function;
}

void Action::setToggledFunction(const std::function<void(bool)>& function)
{
	this->actionToggledFunction = function;
}

void Action::setTriggeredFunction(const std::function<void()>& function)
{
	this->actionTriggeredFunction = function;
}

void Action::setTriggeredFunction(const std::function<void(bool)>& function)
{
	this->actionTriggeredBoolFunction = function;
}

void Action::actionChanged()
{
	if (this->actionChangedFunction)
	{
		this->actionChangedFunction();
	}
}

void Action::actionHovered()
{
	if (this->actionHoveredFunction)
	{
		this->actionHoveredFunction();
	}
}

void Action::actionToggled(bool checked)
{
	if (this->actionToggledFunction)
	{
		this->actionToggledFunction(checked);
	}
}

void Action::actionTriggered()
{
	if (this->actionTriggeredFunction)
	{
		this->actionTriggeredFunction();
	}
}

void Action::actionTriggered(bool checked)
{
	if (this->actionTriggeredBoolFunction)
	{
		this->actionTriggeredBoolFunction(checked);
	}
}

Separator::Separator(QObject* parent) : Action(parent)
{
	this->setSeparator(true);
}

void ActionGroup::appendAction(const QSharedPointer<Action>& action)
{
	this->actions.append(action);
}

void ActionGroup::removeAction(const QSharedPointer<Action>& action)
{
	for (int i = 0; i < this->actions.size(); i++)
	{
		if (this->actions[i] == action)
		{
			this->actions.removeAt(i);

			break;
		}
	}
}

QVector<QSharedPointer<Action>>& ActionGroup::getActions()
{
	return this->actions;
}

const QVector<QSharedPointer<Action>>& ActionGroup::getActions() const
{
	return this->actions;
}

void ActionGroup::setEnableFunction(const std::function<bool()>& function)
{
	this->enableFunction = function;
}

void ActionGroup::setEnabled(bool enable)
{
	for (auto& action : this->actions)
	{
		if (action)
		{
			action->setEnabled(enable);
		}
	}
}

bool ActionGroup::shouldBeEnabled() const
{
	if (this->enableFunction)
	{
		return this->enableFunction();
	}

	return false;
}

void ActionGroup::autoEnabled(const QVector<QSharedPointer<ActionGroup>>& actionGroups)
{
	QVector<bool> results;
	QVector<QSharedPointer<Action>> actions;

	for (auto& actionGroup : actionGroups)
	{	
		if (actionGroup)
		{
			for (auto& newAction : actionGroup->getActions())
			{
				bool unique = true;

				for (auto& action : actions)
				{
					if (newAction == action)
					{
						unique = false;

						break;
					}
				}

				if (unique)
				{
					actions.append(newAction);
				}
			}

			results.append(actionGroup->shouldBeEnabled());
		}
		else
		{
			results.append(false);
		}
	}

	for (auto& action : actions)
	{
		if (action)
		{
			bool enable = true;

			for (int i = 0; i < qMin(actionGroups.size(), results.size()); i++)
			{
				bool ingroup = false;

				if (actionGroups[i])
				{
					for (auto& otherAction : actionGroups[i]->getActions())
					{
						if (action == otherAction)
						{
							ingroup = true;

							break;
						}
					}
				}

				if (ingroup)
				{
					enable &= results[i];
				}
			}

			action->setEnabled(enable);
		}
	}
}
