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

#include "preferences.hpp"

Preferences::Preferences(const Settings::StructSettings& settings, QWidget* parent)
	: QDialog(parent), settings(settings)
{
	this->ui.setupUi(this);

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowType::WindowContextHelpButtonHint);

	this->loadSettings(settings);

	this->connect(this->ui.pushButtonReset, SIGNAL(clicked()), this, SLOT(pushButtonResetClicked()));

	this->connect(this->ui.pushButtonFontChoose, SIGNAL(clicked()), this, SLOT(pushButtonFontChooseClicked()));

	this->connect(this->ui.listWidgetColors, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listWidgetColorsItemDoubleClicked(QListWidgetItem*)));
}

void Preferences::loadSettings(Settings::StructSettings settings)
{
	this->settings = settings;

	this->ui.checkBoxSavePerUser->setChecked(settings.savePerUser);

	settings.font.setPointSizeF(this->ui.lineEdit->font().pointSizeF());

	this->ui.lineEdit->setText(settings.font.family());

	this->ui.lineEdit->setFont(settings.font);

	bool systemLanguageSupported = Translator::isBcp47TranslationSupported(QLocale::system().bcp47Name());

	if (systemLanguageSupported)
	{
		this->ui.comboBoxLanguage->addItem(tr("System default - ") + QLocale::system().nativeLanguageName() + " (" + QLocale::system().bcp47Name() + ")");

		this->locales.append("");
	}

	QStringList bcp47Translations = Translator::getBcp47Translations();

	for (auto& bcp47Translation : bcp47Translations)
	{
		QLocale locale(bcp47Translation);

		this->ui.comboBoxLanguage->addItem(locale.nativeLanguageName() + " (" + locale.bcp47Name() + ")");

		this->locales.append(bcp47Translation);
	}

	this->ui.comboBoxLanguage->setCurrentIndex(0);

	if (this->locales.contains(settings.language))
	{
		this->ui.comboBoxLanguage->setCurrentIndex(this->locales.indexOf(settings.language));
	}

	ui.listWidgetColors->clear();

	for (int i = 0; i < settings.colors.size(); i++)
	{
		this->ui.listWidgetColors->addItem(QObject::tr(qPrintable(settings.colors[i].description)));

		QListWidgetItem* item = this->ui.listWidgetColors->item(i);

		if (item)
		{
			QColor color = settings.colors[i].color;

			int avarage = (color.red() + color.green() + color.blue()) / 3;

			if (avarage > 127)
			{
				item->setTextColor(Qt::black);
			}
			else
			{
				item->setTextColor(Qt::white);
			}

			item->setBackgroundColor(color);
		}
	}
}

const Settings::StructSettings& Preferences::getSettings()
{
	this->settings.savePerUser = this->ui.checkBoxSavePerUser->isChecked();

	int index = this->ui.comboBoxLanguage->currentIndex();

	if (index >= 0 && index < this->locales.size())
	{
		this->settings.language = this->locales[index];
	}

	return this->settings;
}

void Preferences::pushButtonResetClicked()
{
	this->loadSettings();
}

void Preferences::pushButtonFontChooseClicked()
{
	QFont oldFont = this->ui.lineEdit->font();

	QFont font = QFontDialog::getFont(nullptr, this->settings.font, this);

	this->settings.font = font;

	font.setPointSizeF(oldFont.pointSizeF());

	oldFont.setFamily(font.family());

	this->ui.lineEdit->setText(font.family());

	this->ui.lineEdit->setFont(oldFont);
}

void Preferences::listWidgetColorsItemDoubleClicked(QListWidgetItem* item)
{
	if (item)
	{
		int index = qMax(item->listWidget()->row(item), 0);

		if (index >= 0 && index < settings.colors.size())
		{
			QColor color = QColorDialog::getColor(this->settings.colors[index].color, this);

			this->settings.colors[index].color = color;

			int avarage = (color.red() + color.green() + color.blue()) / 3;

			if (avarage > 127)
			{
				item->setTextColor(Qt::black);
			}
			else
			{
				item->setTextColor(Qt::white);
			}

			item->setBackgroundColor(color);
		}
	}
}
