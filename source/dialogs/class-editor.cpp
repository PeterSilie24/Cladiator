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

#include "class-editor.hpp"

ClassEditor::ClassEditor(QWidget* parent, const Class& c)
	: QDialog(parent), actionNewAttribute(this), actionEditAttribute(this), actionMoveUpAttribute(this), actionMoveDownAttribute(this), actionDeleteAttribute(this), actionNewMethod(this), actionEditMethod(this), actionMoveUpMethod(this), actionMoveDownMethod(this), actionDeleteMethod(this), c(c)
{
	this->ui.setupUi(this);

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowType::WindowContextHelpButtonHint);

	this->ui.lineEditName->setValidator(RegExp::getValidator());

	this->ui.lineEditName->setText(c.getName());

	this->ui.lineEditKeyword->setValidator(RegExp::getValidator());

	this->ui.lineEditKeyword->setText(c.getKeyword());

	this->connect(this->ui.lineEditKeyword, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged(const QString&)));

	this->ui.checkBoxInterface->setChecked(c.getKeyword() == "interface");

	this->connect(this->ui.checkBoxInterface, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));

	this->oldKeyword = c.getKeyword();

	this->ui.checkBoxAbstract->setChecked(c.isAbstract());

	for (auto& attribute : c.getAttributes())
	{
		this->ui.listWidgetAttributes->addItem(attribute.toString());
	}

	for (auto& method : c.getMethods())
	{
		this->ui.listWidgetMethods->addItem(method.toString());
	}

	this->actionNewAttribute.setText(tr("New Attribute"));
	this->actionNewAttribute.setIcon(Icons::loadIcon("list-add"));

	this->connect(&(this->actionNewAttribute), SIGNAL(triggered()), this, SLOT(actionNewAttributeTriggered()));

	this->actionEditAttribute.setText(tr("Edit"));
	this->actionEditAttribute.setIcon(Icons::loadIcon("document-edit"));

	this->connect(&(this->actionEditAttribute), SIGNAL(triggered()), this, SLOT(actionEditAttributeTriggered()));

	this->actionMoveUpAttribute.setText(tr("Move Up"));
	this->actionMoveUpAttribute.setIcon(Icons::loadIcon("arrow-up"));

	this->connect(&(this->actionMoveUpAttribute), SIGNAL(triggered()), this, SLOT(actionMoveUpAttributeTriggered()));

	this->actionMoveDownAttribute.setText(tr("Move Down"));
	this->actionMoveDownAttribute.setIcon(Icons::loadIcon("arrow-down"));

	this->connect(&(this->actionMoveDownAttribute), SIGNAL(triggered()), this, SLOT(actionMoveDownAttributeTriggered()));

	this->actionDeleteAttribute.setText(tr("Delete"));
	this->actionDeleteAttribute.setIcon(Icons::loadIcon("edit-delete"));

	this->connect(&(this->actionDeleteAttribute), SIGNAL(triggered()), this, SLOT(actionDeleteAttributeTriggered()));

	this->ui.listWidgetAttributes->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

	this->connect(this->ui.listWidgetAttributes, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuRequestedAttributes(const QPoint&)));

	this->actionNewMethod.setText(tr("New Method"));
	this->actionNewMethod.setIcon(Icons::loadIcon("list-add"));

	this->connect(&(this->actionNewMethod), SIGNAL(triggered()), this, SLOT(actionNewMethodTriggered()));

	this->actionEditMethod.setText(tr("Edit"));
	this->actionEditMethod.setIcon(Icons::loadIcon("document-edit"));

	this->connect(&(this->actionEditMethod), SIGNAL(triggered()), this, SLOT(actionEditMethodTriggered()));

	this->actionMoveUpMethod.setText(tr("Move Up"));
	this->actionMoveUpMethod.setIcon(Icons::loadIcon("arrow-up"));

	this->connect(&(this->actionMoveUpMethod), SIGNAL(triggered()), this, SLOT(actionMoveUpMethodTriggered()));

	this->actionMoveDownMethod.setText(tr("Move Down"));
	this->actionMoveDownMethod.setIcon(Icons::loadIcon("arrow-down"));

	this->connect(&(this->actionMoveDownMethod), SIGNAL(triggered()), this, SLOT(actionMoveDownMethodTriggered()));

	this->actionDeleteMethod.setText(tr("Delete"));
	this->actionDeleteMethod.setIcon(Icons::loadIcon("edit-delete"));

	this->connect(&(this->actionDeleteMethod), SIGNAL(triggered()), this, SLOT(actionDeleteMethodTriggered()));

	this->ui.listWidgetMethods->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

	this->connect(this->ui.listWidgetMethods, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuRequestedMethods(const QPoint&)));
}

Class& ClassEditor::getClass()
{
	this->c.setName(this->ui.lineEditName->text());

	this->c.setKeyword(this->ui.lineEditKeyword->text());

	this->c.setAbstract(this->ui.checkBoxAbstract->isChecked());

	return this->c;
}

void ClassEditor::textChanged(const QString& text)
{
	this->ui.checkBoxInterface->setChecked(text == "interface");
}

void ClassEditor::stateChanged(int state)
{
	if (state)
	{
		this->oldKeyword = this->ui.lineEditKeyword->text();

		this->ui.lineEditKeyword->setText("interface");
	}
	else
	{
		if (this->ui.lineEditKeyword->text() == "interface")
		{
			if (this->oldKeyword != "interface")
			{
				this->ui.lineEditKeyword->setText(this->oldKeyword);
			}
			else
			{
				this->ui.lineEditKeyword->setText("");
			}
		}
	}
}

void ClassEditor::itemDoubleClickedAttributes(QListWidgetItem* item)
{
	if (item)
	{
		int index = qMax(item->listWidget()->row(item), 0);

		if (index < this->c.getAttributeCount())
		{
			AttributeEditor editor(this, this->c.getAttributeAt(index));

			if (editor.exec())
			{
				this->c.getAttributeAt(index) = editor.getAttribute();

				item->setText(this->c.getAttributeAt(index).toString());
			}
		}
	}
}

void ClassEditor::itemDoubleClickedMethods(QListWidgetItem* item)
{
	if (item)
	{
		int index = qMax(item->listWidget()->row(item), 0);

		if (index < this->c.getMethodCount())
		{
			MethodEditor editor(this, this->c.getMethodAt(index));

			if (editor.exec())
			{
				this->c.getMethodAt(index) = editor.getMethod();

				item->setText(this->c.getMethodAt(index).toString());
			}
		}
	}
}

void ClassEditor::contextMenuRequestedAttributes(const QPoint& pos)
{
	Q_UNUSED(pos);

	QMenu contextMenu(this);

	contextMenu.addAction(&(this->actionNewAttribute));

	if (this->ui.listWidgetAttributes->currentItem())
	{
		contextMenu.addSeparator();

		contextMenu.addAction(&(this->actionEditAttribute));

		contextMenu.addSeparator();

		this->actionMoveUpAttribute.setEnabled(false);

		this->actionMoveDownAttribute.setEnabled(false);

		if (this->ui.listWidgetAttributes->currentRow() > 0)
		{
			this->actionMoveUpAttribute.setEnabled(true);
		}

		if (this->ui.listWidgetAttributes->currentRow() < this->ui.listWidgetAttributes->count() - 1)
		{
			this->actionMoveDownAttribute.setEnabled(true);
		}

		contextMenu.addActions({ &(this->actionMoveUpAttribute), &(this->actionMoveDownAttribute) });

		contextMenu.addSeparator();

		contextMenu.addAction(&(this->actionDeleteAttribute));
	}

	contextMenu.exec(QCursor::pos());
}

void ClassEditor::actionNewAttributeTriggered()
{
	AttributeEditor editor(this);

	if (editor.exec())
	{
		Attribute attribute = editor.getAttribute();

		this->c.addAttribute(attribute);

		this->ui.listWidgetAttributes->addItem(attribute.toString());
	}
}

void ClassEditor::actionEditAttributeTriggered()
{
	QListWidgetItem* item = this->ui.listWidgetAttributes->currentItem();

	if (item)
	{
		this->ui.listWidgetAttributes->itemDoubleClicked(item);
	}
}

void ClassEditor::actionMoveUpAttributeTriggered()
{
	int index = this->ui.listWidgetAttributes->currentRow();

	if (index > 0)
	{
		QListWidgetItem* item = this->ui.listWidgetAttributes->takeItem(index);

		if (item)
		{
			this->ui.listWidgetAttributes->insertItem(index - 1, item);

			this->ui.listWidgetAttributes->setCurrentRow(index - 1);

			this->c.swapAttributes(index, index - 1);
		}
	}
}

void ClassEditor::actionMoveDownAttributeTriggered()
{
	int index = this->ui.listWidgetAttributes->currentRow();

	if (index >= 0 && index < this->ui.listWidgetAttributes->count() - 1)
	{
		QListWidgetItem* item = this->ui.listWidgetAttributes->takeItem(index);

		if (item)
		{
			this->ui.listWidgetAttributes->insertItem(index + 1, item);

			this->ui.listWidgetAttributes->setCurrentRow(index + 1);

			this->c.swapAttributes(index, index + 1);
		}
	}
}

void ClassEditor::actionDeleteAttributeTriggered()
{
	QListWidgetItem* item = this->ui.listWidgetAttributes->currentItem();

	if (item)
	{
		int index = qMax(item->listWidget()->row(item), 0);

		if (index < this->c.getAttributeCount())
		{
			this->c.removeAttributeAt(index);

			delete item;
		}
	}
}

void ClassEditor::contextMenuRequestedMethods(const QPoint& pos)
{
	Q_UNUSED(pos);

	QMenu contextMenu(this);

	contextMenu.addAction(&(this->actionNewMethod));

	if (this->ui.listWidgetMethods->currentItem())
	{
		contextMenu.addSeparator();

		contextMenu.addAction(&(this->actionEditMethod));

		contextMenu.addSeparator();

		this->actionMoveUpMethod.setEnabled(false);

		this->actionMoveDownMethod.setEnabled(false);

		if (this->ui.listWidgetMethods->currentRow() > 0)
		{
			this->actionMoveUpMethod.setEnabled(true);
		}

		if (this->ui.listWidgetMethods->currentRow() < this->ui.listWidgetMethods->count() - 1)
		{
			this->actionMoveDownMethod.setEnabled(true);
		}

		contextMenu.addActions({ &(this->actionMoveUpMethod), &(this->actionMoveDownMethod) });

		contextMenu.addSeparator();

		contextMenu.addAction(&(this->actionDeleteMethod));
	}

	contextMenu.exec(QCursor::pos());
}

void ClassEditor::actionNewMethodTriggered()
{
	MethodEditor editor(this);

	if (editor.exec())
	{
		Method method = editor.getMethod();

		this->c.addMethod(method);

		this->ui.listWidgetMethods->addItem(method.toString());
	}
}

void ClassEditor::actionEditMethodTriggered()
{
	QListWidgetItem* item = this->ui.listWidgetMethods->currentItem();

	if (item)
	{
		this->ui.listWidgetMethods->itemDoubleClicked(item);
	}
}

void ClassEditor::actionMoveUpMethodTriggered()
{
	int index = this->ui.listWidgetMethods->currentRow();

	if (index > 0)
	{
		QListWidgetItem* item = this->ui.listWidgetMethods->takeItem(index);

		if (item)
		{
			this->ui.listWidgetMethods->insertItem(index - 1, item);

			this->ui.listWidgetMethods->setCurrentRow(index - 1);

			this->c.swapMethods(index, index - 1);
		}
	}
}

void ClassEditor::actionMoveDownMethodTriggered()
{
	int index = this->ui.listWidgetMethods->currentRow();

	if (index >= 0 && index < this->ui.listWidgetMethods->count() - 1)
	{
		QListWidgetItem* item = this->ui.listWidgetMethods->takeItem(index);

		if (item)
		{
			this->ui.listWidgetMethods->insertItem(index + 1, item);

			this->ui.listWidgetMethods->setCurrentRow(index + 1);

			this->c.swapMethods(index, index + 1);
		}
	}
}

void ClassEditor::actionDeleteMethodTriggered()
{
	QListWidgetItem* item = this->ui.listWidgetMethods->currentItem();

	if (item)
	{
		int index = qMax(item->listWidget()->row(item), 0);

		if (index < this->c.getMethodCount())
		{
			this->c.removeMethodAt(index);

			delete item;
		}
	}
}
