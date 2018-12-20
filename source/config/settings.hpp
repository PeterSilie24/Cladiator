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

#include <QSharedPointer>
#include <QWeakPointer>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QString>
#include <QStandardPaths>
#include <QApplication>
#include <QFont>
#include <QLocale>
#include <QColor>
#include <QVector>

enum class Colors
{
	Default,
	Background,
	VisibilityPublic,
	VisibilityProtected,
	VisibilityPrivate,
	VisibilityPackage,
	Type,
	Value
};

class Settings
{
public:
	struct ColorOption
	{
		ColorOption();

		ColorOption(QColor color, QString name, QString description);

		QColor color;
		QString name;
		QString description;
	};

	struct StructSettings
	{
		StructSettings();

		bool saveLocally;
		QString language;
		QFont font;
		QVector<ColorOption> colors;
	};

	Settings();

	static QString getSettingsDir();

	static bool change(QWidget* parent);

	static void save();

	static void restore();

	static void clear();

	static bool isSaveLocally();

	static bool isLocallySavable();

	static QString getLanguage();

	static QFont getFont(bool headline = false);

	static QString getFontHtmlStyle(bool headline = false);

	static qreal getBorderWidth();

	static QColor getColor(const Colors& color = Colors::Default);

	static QString getColorHtmlStyle(const Colors& color = Colors::Default);

private:
	static QString getLocalDir();

	static QString getLocalSettingsDir();

	static QString getUserDir();

	static QString getUserSettingsDir();

	QSharedPointer<StructSettings> strongSettings;

	static QWeakPointer<StructSettings> weakSettings;
};
