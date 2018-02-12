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

#include <QAction>
#include <QList>
#include <QSharedPointer>
#include <QString>

#include <functional>

class Action : public QAction
{
	Q_OBJECT

public:
	Action(QObject* parent = nullptr, const QString& text = "", const QIcon& icon = QIcon(), const QKeySequence& shortcut = QKeySequence());

	void setChangedFunction(const std::function<void()>& function);
	void setHoveredFunction(const std::function<void()>& function);
	void setToggledFunction(const std::function<void(bool)>& function);
	void setTriggeredFunction(const std::function<void()>& function);
	void setTriggeredFunction(const std::function<void(bool)>& function);

	private slots:
	void actionChanged();
	void actionHovered();
	void actionToggled(bool checked);
	void actionTriggered();
	void actionTriggered(bool checked);

private:
	std::function<void()> actionChangedFunction;
	std::function<void()> actionHoveredFunction;
	std::function<void(bool)> actionToggledFunction;
	std::function<void()> actionTriggeredFunction;
	std::function<void(bool)> actionTriggeredBoolFunction;
};

class Separator : public Action
{
	Q_OBJECT

public:
	Separator(QObject* parent = nullptr);
};

class ActionGroup
{
public:
	void appendAction(const QSharedPointer<Action>& action);

	void removeAction(const QSharedPointer<Action>& action);

	QVector<QSharedPointer<Action>>& getActions();

	const QVector<QSharedPointer<Action>>& getActions() const;

	void setEnableFunction(const std::function<bool()>& function);

	void setEnabled(bool enable);

	bool shouldBeEnabled() const;

	static void autoEnabled(const QVector<QSharedPointer<ActionGroup>>& actionGroups);

private:
	std::function<bool()> enableFunction;

	QVector<QSharedPointer<Action>> actions;
};
