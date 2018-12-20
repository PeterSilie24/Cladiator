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

#include "settings.hpp"

#include "dialogs/preferences.hpp"

Settings::ColorOption::ColorOption()
{

}

Settings::ColorOption::ColorOption(QColor color, QString name, QString description) : color(color), name(name), description(description)
{

}

Settings::StructSettings::StructSettings() : saveLocally(true), language(""), font(QApplication::font())
{
	this->colors.append(ColorOption(QColor("#000000"), "Default", QT_TRANSLATE_NOOP("QObject", "Default")));
	this->colors.append(ColorOption(QColor("#FFFFFF"), "Background", QT_TRANSLATE_NOOP("QObject", "Background")));
	this->colors.append(ColorOption(QColor("#5FB404"), "VisibilityPublic", QT_TRANSLATE_NOOP("QObject", "Public visibility")));
	this->colors.append(ColorOption(QColor("#B40404"), "VisibilityProtected", QT_TRANSLATE_NOOP("QObject", "Protected visibility")));
	this->colors.append(ColorOption(QColor("#B40404"), "VisibilityPrivate", QT_TRANSLATE_NOOP("QObject", "Private visibility")));
	this->colors.append(ColorOption(QColor("#B40404"), "VisibilityPackage", QT_TRANSLATE_NOOP("QObject", "Package visibility")));
	this->colors.append(ColorOption(QColor("#045FB4"), "Type", QT_TRANSLATE_NOOP("QObject", "Type")));
	this->colors.append(ColorOption(QColor("#DF7401"), "Value", QT_TRANSLATE_NOOP("QObject", "Value")));
}

Settings::Settings()
{
	if (!this->weakSettings)
	{
		this->strongSettings = QSharedPointer<StructSettings>(new StructSettings());

		this->weakSettings = this->strongSettings.toWeakRef();

		restore();
	}
}

QString Settings::getSettingsDir()
{
	QSharedPointer<StructSettings> settings = weakSettings.toStrongRef();

	if (settings)
	{
		if (!settings->saveLocally)
		{
			return getUserSettingsDir();
		}

		return getLocalSettingsDir();
	}

	return "";
}

bool Settings::change(QWidget* parent)
{
	QSharedPointer<StructSettings> settings = weakSettings.toStrongRef();

	if (settings)
	{
		Preferences preferences(*settings, parent);

		if (preferences.exec())
		{
			*settings = preferences.getSettings();

			settings->saveLocally = settings->saveLocally && isLocallySavable();

			save();

			return true;
		}
	}

	return false;
}

void Settings::save()
{
	clear();

	QSharedPointer<StructSettings> strongSettings = weakSettings.toStrongRef();

	if (strongSettings)
	{
		QSettings settings(getSettingsDir() + "settings.ini", QSettings::Format::IniFormat);

		settings.setValue("SaveLocally", strongSettings->saveLocally);

		settings.setValue("Language", strongSettings->language);

		settings.beginGroup("Schema");

		settings.setValue("FontFamily", strongSettings->font.family());

		for (auto& color : strongSettings->colors)
		{
			settings.setValue("Color" + color.name, color.color.name());
		}

		settings.endGroup();
	}
}

void Settings::restore()
{
	QSharedPointer<StructSettings> strongSettings = weakSettings.toStrongRef();

	if (strongSettings)
	{
		QString path = getUserSettingsDir() + "settings.ini";

		QFile file(path);

		if (!file.exists())
		{
			path = getLocalSettingsDir() + "settings.ini";
		}

		QSettings settings(path, QSettings::Format::IniFormat);

		strongSettings->saveLocally = settings.value("SaveLocally", strongSettings->saveLocally).toBool() && isLocallySavable();

		strongSettings->language = settings.value("Language", strongSettings->language).toString();

		settings.beginGroup("Schema");

		strongSettings->font.setFamily(settings.value("FontFamily", strongSettings->font.family()).toString());

		for (auto& color : strongSettings->colors)
		{
			color.color = QColor(settings.value("Color" + color.name, color.color.name()).toString());
		}

		settings.endGroup();
	}
}

void Settings::clear()
{
	QSharedPointer<StructSettings> settings = weakSettings.toStrongRef();

	if (settings)
	{
		if (settings->saveLocally)
		{
			QDir dir(getUserDir());

			if (dir.exists())
			{
				dir.removeRecursively();
			}
		}
	}
}

bool Settings::isSaveLocally()
{
	QSharedPointer<StructSettings> settings = weakSettings.toStrongRef();

	if (settings)
	{
		return settings->saveLocally;
	}

	return false;
}

bool Settings::isLocallySavable()
{
	QFileInfo fileInfo(getLocalDir());

	if (!fileInfo.isDir() || !fileInfo.isWritable())
	{
		return false;
	}

	fileInfo = QFileInfo(getLocalSettingsDir());

	if (fileInfo.exists() && fileInfo.isDir())
	{
		if (!fileInfo.isWritable())
		{
			return false;
		}
	}

	return true;
}

QString Settings::getLanguage()
{
	QSharedPointer<StructSettings> settings = weakSettings.toStrongRef();

	if (settings)
	{
		QString language = settings->language;

		if (language == "")
		{
			language = QLocale::system().bcp47Name();
		}

		return language;
	}

	return "";
}

QFont Settings::getFont(bool headline)
{
	QSharedPointer<StructSettings> settings = weakSettings.toStrongRef();

	if (settings)
	{
		QFont fontReturn = settings->font;

		fontReturn.setPointSizeF(10.0);

		if (headline)
		{
			fontReturn.setPointSizeF(fontReturn.pointSizeF() * 2.0);
		}

		return fontReturn;
	}

	return QFont();
}

QString Settings::getFontHtmlStyle(bool headline)
{
	QFont font = getFont(headline);

	return "style=\"font-family : " + font.family() + " ; font-size: " + QString::number(font.pointSize()) + "pt ;\"";
}

qreal Settings::getBorderWidth()
{
	return 2.0;
}

QColor Settings::getColor(const Colors& color)
{
	QSharedPointer<StructSettings> settings = weakSettings.toStrongRef();

	if (settings)
	{
		if (static_cast<int>(color) >= 0 && static_cast<int>(color) < settings->colors.size())
		{
			return settings->colors[static_cast<int>(color)].color;
		}
	}

	return QColor();
}

QString Settings::getColorHtmlStyle(const Colors& color)
{
	return "color=\"" + getColor(color).name() + "\"";
}

QString Settings::getLocalDir()
{
	if (weakSettings)
	{
		return QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
	}

	return "";
}

QString Settings::getLocalSettingsDir()
{
	if (weakSettings)
	{
		return getLocalDir() + "/config/";
	}

	return "";
}

QString Settings::getUserDir()
{
	if (weakSettings)
	{
		QStringList appConfigLocations = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);

		if (appConfigLocations.size())
		{
			return appConfigLocations.first();
		}
	}

	return "";
}

QString Settings::getUserSettingsDir()
{
	if (weakSettings)
	{
		return getUserDir() + "/config/";;
	}

	return "";
}

QWeakPointer<Settings::StructSettings> Settings::weakSettings;
