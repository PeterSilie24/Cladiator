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

#include "attribute-editor.hpp"

AttributeEditor::AttributeEditor(QWidget* parent, const Attribute& attribute)
	: QDialog(parent), attribute(attribute)
{
	this->ui.setupUi(this);

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowType::WindowContextHelpButtonHint);

	this->ui.lineEditName->setValidator(RegExp::getValidator());

	this->ui.lineEditName->setText(attribute.getName());

	this->ui.checkBoxClassScope->setChecked(attribute.isClassScope());

	this->ui.lineEditType->setValidator(RegExp::getValidator(RegExp::Type));

	this->ui.lineEditType->setText(attribute.getType());

	this->ui.lineEditMultiplicity->setValidator(RegExp::getValidator(RegExp::Multiplicity));

	this->ui.lineEditMultiplicity->setText(attribute.getMultiplicity());

	this->ui.lineEditValue->setValidator(RegExp::getValidator(RegExp::Value));

	this->ui.lineEditValue->setText(attribute.getDefaultValue());

	this->ui.comboBoxVisibility->setCurrentIndex(attribute.getVisibility());
}

Attribute& AttributeEditor::getAttribute()
{
	this->attribute.setName(this->ui.lineEditName->text());

	this->attribute.setClassScope(this->ui.checkBoxClassScope->isChecked());

	this->attribute.setType(this->ui.lineEditType->text());

	this->attribute.setMultiplicity(this->ui.lineEditMultiplicity->text());

	this->attribute.setDefaultValue(this->ui.lineEditValue->text());

	this->attribute.setVisibility(this->ui.comboBoxVisibility->currentIndex());

	return this->attribute;
}
