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

#include "association-editor.hpp"

AssociationEditor::AssociationEditor(QWidget* parent, const Association& association)
	: QDialog(parent), association(association)
{
	this->ui.setupUi(this);

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowType::WindowContextHelpButtonHint);

	this->ui.lineEditName->setText(association.getName());

	this->ui.checkBoxShowDirection->setChecked(association.isShowDirection());

	this->ui.comboBoxType->setCurrentIndex(association.getType());

	this->ui.lineEditRoleSideA->setValidator(RegExp::getValidator());

	this->ui.lineEditRoleSideA->setText(association.getRoleSideA());

	this->ui.lineEditMultiplicitySideA->setValidator(RegExp::getValidator(RegExp::Multiplicity));

	this->ui.lineEditMultiplicitySideA->setText(association.getMultiplicitySideA());

	this->ui.comboBoxVisibilitySideA->setCurrentIndex(association.getVisibilitySideA());

	this->ui.comboBoxNavigabilitySideA->setCurrentIndex(association.getNavigabilitySideA());

	this->ui.lineEditRoleSideB->setValidator(RegExp::getValidator());

	this->ui.lineEditRoleSideB->setText(association.getRoleSideB());

	this->ui.lineEditMultiplicitySideB->setValidator(RegExp::getValidator(RegExp::Multiplicity));

	this->ui.lineEditMultiplicitySideB->setText(association.getMultiplicitySideB());

	this->ui.comboBoxVisibilitySideB->setCurrentIndex(association.getVisibilitySideB());

	this->ui.comboBoxNavigabilitySideB->setCurrentIndex(association.getNavigabilitySideB());

	this->connect(this->ui.pushButtonSwitch, SIGNAL(clicked()), this, SLOT(pushButtonSwitchClicked()));
}

Association& AssociationEditor::getAssociation()
{
	this->association.setName(this->ui.lineEditName->text());

	this->association.setShowDirection(this->ui.checkBoxShowDirection->isChecked());

	this->association.setType(this->ui.comboBoxType->currentIndex());

	this->association.setRoleSideA(this->ui.lineEditRoleSideA->text());

	this->association.setMultiplicitySideA(this->ui.lineEditMultiplicitySideA->text());

	this->association.setVisibilitySideA(this->ui.comboBoxVisibilitySideA->currentIndex());

	this->association.setNavigabilitySideA(this->ui.comboBoxNavigabilitySideA->currentIndex());

	this->association.setRoleSideB(this->ui.lineEditRoleSideB->text());

	this->association.setMultiplicitySideB(this->ui.lineEditMultiplicitySideB->text());

	this->association.setVisibilitySideB(this->ui.comboBoxVisibilitySideB->currentIndex());

	this->association.setNavigabilitySideB(this->ui.comboBoxNavigabilitySideB->currentIndex());

	return this->association;
}

void AssociationEditor::pushButtonSwitchClicked()
{
	QString tempString = this->ui.lineEditRoleSideA->text();

	this->ui.lineEditRoleSideA->setText(this->ui.lineEditRoleSideB->text());

	this->ui.lineEditRoleSideB->setText(tempString);

	tempString = this->ui.lineEditMultiplicitySideA->text();

	this->ui.lineEditMultiplicitySideA->setText(this->ui.lineEditMultiplicitySideB->text());

	this->ui.lineEditMultiplicitySideB->setText(tempString);

	int tempInt = this->ui.comboBoxVisibilitySideA->currentIndex();

	this->ui.comboBoxVisibilitySideA->setCurrentIndex(this->ui.comboBoxVisibilitySideB->currentIndex());

	this->ui.comboBoxVisibilitySideB->setCurrentIndex(tempInt);

	tempInt = this->ui.comboBoxNavigabilitySideA->currentIndex();

	this->ui.comboBoxNavigabilitySideA->setCurrentIndex(this->ui.comboBoxNavigabilitySideB->currentIndex());

	this->ui.comboBoxNavigabilitySideB->setCurrentIndex(tempInt);
}
