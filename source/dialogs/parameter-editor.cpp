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

#include "parameter-editor.hpp"

ParameterEditor::ParameterEditor(QWidget* parent, const Parameter& parameter)
	: QDialog(parent), parameter(parameter)
{
	this->ui.setupUi(this);

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowType::WindowContextHelpButtonHint);

	this->ui.lineEditName->setValidator(RegExp::getValidator());

	this->ui.lineEditName->setText(parameter.getName());

	this->ui.lineEditType->setValidator(RegExp::getValidator(RegExp::Type));

	this->ui.lineEditType->setText(parameter.getType());

	this->ui.lineEditMultiplicity->setValidator(RegExp::getValidator(RegExp::Multiplicity));

	this->ui.lineEditMultiplicity->setText(parameter.getMultiplicity());

	this->ui.lineEditValue->setValidator(RegExp::getValidator(RegExp::Value));

	this->ui.lineEditValue->setText(parameter.getDefaultValue());
}

Parameter& ParameterEditor::getParameter()
{
	this->parameter.setName(this->ui.lineEditName->text());

	this->parameter.setType(this->ui.lineEditType->text());

	this->parameter.setMultiplicity(this->ui.lineEditMultiplicity->text());

	this->parameter.setDefaultValue(this->ui.lineEditValue->text());

	return this->parameter;
}
