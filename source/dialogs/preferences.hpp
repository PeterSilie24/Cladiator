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
#include <QFontDialog>
#include <QColorDialog>
#include <QVector>
#include <QTextStream>

#include "ui_preferences.h"

#include "config/settings.hpp"

#include "utils/translator.hpp"

class Preferences : public QDialog
{
	Q_OBJECT

public:
	Preferences(const Settings::StructSettings& settings, QWidget* parent);

	void loadSettings(Settings::StructSettings settings = Settings::StructSettings());

	const Settings::StructSettings& getSettings();

private slots:
	void pushButtonResetClicked();

	void pushButtonFontChooseClicked();

	void listWidgetColorsItemDoubleClicked(QListWidgetItem* item);

private:
	Ui::PreferencesDialog ui;

	Settings::StructSettings settings;

	QStringList locales;
};
