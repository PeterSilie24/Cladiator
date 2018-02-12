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

#include "ui_class-editor.h"

#include "utils/icons.hpp"

#include "content/class.hpp"

#include "attribute-editor.hpp"
#include "method-editor.hpp"

class ClassEditor : public QDialog
{
	Q_OBJECT

public:
	ClassEditor(QWidget* parent, const Class& c = Class());

	Class& getClass();

private slots:
	void textChanged(const QString& text);

	void stateChanged(int state);

	void itemDoubleClickedAttributes(QListWidgetItem* item);
	void itemDoubleClickedMethods(QListWidgetItem* item);

	void contextMenuRequestedAttributes(const QPoint& pos);
	void contextMenuRequestedMethods(const QPoint& pos);

	void actionNewAttributeTriggered();
	void actionEditAttributeTriggered();
	void actionMoveUpAttributeTriggered();
	void actionMoveDownAttributeTriggered();
	void actionDeleteAttributeTriggered();

	void actionNewMethodTriggered();
	void actionEditMethodTriggered();
	void actionMoveUpMethodTriggered();
	void actionMoveDownMethodTriggered();
	void actionDeleteMethodTriggered();

private:
	Ui::ClassEditorDialog ui;

	QAction actionNewAttribute;
	QAction actionEditAttribute;
	QAction actionMoveUpAttribute;
	QAction actionMoveDownAttribute;
	QAction actionDeleteAttribute;

	QAction actionNewMethod;
	QAction actionEditMethod;
	QAction actionMoveUpMethod;
	QAction actionMoveDownMethod;
	QAction actionDeleteMethod;

	Class c;

	QString oldStereotype;
};
