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

#include "method-editor.hpp"

MethodEditor::MethodEditor(QWidget* parent, const Method& method)
	: QDialog(parent), actionNewParameter(this), actionEditParameter(this), actionMoveUpParameter(this), actionMoveDownParameter(this), actionDeleteParameter(this), method(method)
{
	this->ui.setupUi(this);

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowType::WindowContextHelpButtonHint);

	this->ui.lineEditName->setValidator(RegExp::getValidator());

	this->ui.lineEditName->setText(method.getName());

	this->ui.checkBoxAbstract->setChecked(method.isAbstract());

	this->ui.checkBoxClassScope->setChecked(method.isClassScope());

	this->ui.lineEditReturnType->setValidator(RegExp::getValidator(RegExp::Type));

	this->ui.lineEditReturnType->setText(method.getReturnType());

	this->ui.comboBoxVisibility->setCurrentIndex(method.getVisibility());

	for (auto& parameter : method.getParameters())
	{
		this->ui.listWidgetParameters->addItem(parameter.toString());
	}

	this->actionNewParameter.setText(tr("New Parameter"));
	this->actionNewParameter.setIcon(Icons::loadIcon("list-add"));

	this->connect(&(this->actionNewParameter), SIGNAL(triggered()), this, SLOT(actionNewParameterTriggered()));

	this->actionEditParameter.setText(tr("Edit"));
	this->actionEditParameter.setIcon(Icons::loadIcon("document-edit"));

	this->connect(&(this->actionEditParameter), SIGNAL(triggered()), this, SLOT(actionEditParameterTriggered()));

	this->actionMoveUpParameter.setText(tr("Move Up"));
	this->actionMoveUpParameter.setIcon(Icons::loadIcon("arrow-up"));

	this->connect(&(this->actionMoveUpParameter), SIGNAL(triggered()), this, SLOT(actionMoveUpParameterTriggered()));

	this->actionMoveDownParameter.setText(tr("Move Down"));
	this->actionMoveDownParameter.setIcon(Icons::loadIcon("arrow-down"));

	this->connect(&(this->actionMoveDownParameter), SIGNAL(triggered()), this, SLOT(actionMoveDownParameterTriggered()));

	this->actionDeleteParameter.setText(tr("Delete"));
	this->actionDeleteParameter.setIcon(Icons::loadIcon("edit-delete"));

	this->connect(&(this->actionDeleteParameter), SIGNAL(triggered()), this, SLOT(actionDeleteParameterTriggered()));

	this->ui.listWidgetParameters->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

	this->connect(this->ui.listWidgetParameters, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuRequestedParameters(const QPoint&)));
}

Method& MethodEditor::getMethod()
{
	this->method.setName(this->ui.lineEditName->text());

	this->method.setAbstract(this->ui.checkBoxAbstract->isChecked());

	this->method.setClassScope(this->ui.checkBoxClassScope->isChecked());

	this->method.setReturnType(this->ui.lineEditReturnType->text());

	this->method.setVisibility(this->ui.comboBoxVisibility->currentIndex());

	return this->method;
}

void MethodEditor::itemDoubleClicked(QListWidgetItem* item)
{
	if (item)
	{
		int index = qMax(item->listWidget()->row(item), 0);

		if (index < this->method.getParameterCount())
		{
			ParameterEditor editor(this, this->method.getParameterAt(index));

			if (editor.exec())
			{
				this->method.getParameterAt(index) = editor.getParameter();

				item->setText(this->method.getParameterAt(index).toString());
			}
		}
	}
}

void MethodEditor::contextMenuRequestedParameters(const QPoint& pos)
{
	Q_UNUSED(pos);

	QMenu contextMenu(this);

	contextMenu.addAction(&(this->actionNewParameter));

	if (this->ui.listWidgetParameters->currentItem())
	{
		contextMenu.addSeparator();

		contextMenu.addAction(&(this->actionEditParameter));

		contextMenu.addSeparator();

		this->actionMoveUpParameter.setEnabled(false);

		this->actionMoveDownParameter.setEnabled(false);

		if (this->ui.listWidgetParameters->currentRow() > 0)
		{
			this->actionMoveUpParameter.setEnabled(true);
		}

		if (this->ui.listWidgetParameters->currentRow() < this->ui.listWidgetParameters->count() - 1)
		{
			this->actionMoveDownParameter.setEnabled(true);
		}

		contextMenu.addActions({ &(this->actionMoveUpParameter), &(this->actionMoveDownParameter) });

		contextMenu.addSeparator();

		contextMenu.addAction(&(this->actionDeleteParameter));
	}

	contextMenu.exec(QCursor::pos());
}

void MethodEditor::actionNewParameterTriggered()
{
	ParameterEditor editor(this);

	if (editor.exec())
	{
		Parameter parameter = editor.getParameter();

		this->method.addParameter(parameter);

		this->ui.listWidgetParameters->addItem(parameter.toString());
	}
}

void MethodEditor::actionEditParameterTriggered()
{
	QListWidgetItem* item = this->ui.listWidgetParameters->currentItem();

	if (item)
	{
		this->ui.listWidgetParameters->itemDoubleClicked(item);
	}
}

void MethodEditor::actionMoveUpParameterTriggered()
{
	int index = this->ui.listWidgetParameters->currentRow();

	if (index > 0)
	{
		QListWidgetItem* item = this->ui.listWidgetParameters->takeItem(index);

		if (item)
		{
			this->ui.listWidgetParameters->insertItem(index - 1, item);

			this->ui.listWidgetParameters->setCurrentRow(index - 1);

			this->method.swapParameters(index, index - 1);
		}
	}
}

void MethodEditor::actionMoveDownParameterTriggered()
{
	int index = this->ui.listWidgetParameters->currentRow();

	if (index >= 0 && index < this->ui.listWidgetParameters->count() - 1)
	{
		QListWidgetItem* item = this->ui.listWidgetParameters->takeItem(index);

		if (item)
		{
			this->ui.listWidgetParameters->insertItem(index + 1, item);

			this->ui.listWidgetParameters->setCurrentRow(index + 1);

			this->method.swapParameters(index, index + 1);
		}
	}
}

void MethodEditor::actionDeleteParameterTriggered()
{
	QListWidgetItem* item = this->ui.listWidgetParameters->currentItem();

	if (item)
	{
		int index = qMax(item->listWidget()->row(item), 0);

		if (index < this->method.getParameterCount())
		{
			this->method.removeParameterAt(index);

			delete item;
		}
	}
}
