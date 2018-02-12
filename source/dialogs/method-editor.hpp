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

#include <QDialog>
#include <QMenu>

#include "ui_method-editor.h"

#include "utils/icons.hpp"

#include "content/class.hpp"

#include "parameter-editor.hpp"

class MethodEditor : public QDialog
{
	Q_OBJECT

public:
	MethodEditor(QWidget* parent, const Method& method = Method());

	Method& getMethod();

private slots:
	void itemDoubleClicked(QListWidgetItem* item);

	void contextMenuRequestedParameters(const QPoint& pos);

	void actionNewParameterTriggered();
	void actionEditParameterTriggered();
	void actionMoveUpParameterTriggered();
	void actionMoveDownParameterTriggered();
	void actionDeleteParameterTriggered();

private:
	Ui::MethodEditorDialog ui;

	QAction actionNewParameter;
	QAction actionEditParameter;
	QAction actionMoveUpParameter;
	QAction actionMoveDownParameter;
	QAction actionDeleteParameter;

	Method method;
};
