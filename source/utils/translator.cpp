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

#include "translator.hpp"

Translator::Translator()
{
	if (!this->weakTranslatorApp)
	{
		this->strongTranslatorApp = QSharedPointer<QTranslator>(new QTranslator());

		this->weakTranslatorApp = this->strongTranslatorApp.toWeakRef();
	}

	if (!this->weakTranslatorBase)
	{
		this->strongTranslatorBase = QSharedPointer<QTranslator>(new QTranslator());

		this->weakTranslatorBase = this->strongTranslatorBase.toWeakRef();
	}
}

QString Translator::getTranslationsDir()
{
	if (weakTranslatorApp && weakTranslatorBase)
	{
		return QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/translations/";
	}

	return "";
}

QStringList Translator::getBcp47Translations()
{
	QStringList bcp47Translations;

	if (weakTranslatorApp && weakTranslatorBase)
	{
		QDir dir(getTranslationsDir());

		dir.setNameFilters({ Info::getFileName() + "_*.qm" });

		dir.setFilter(QDir::Files);

		QStringList filesApp = dir.entryList();

		for (auto& file : filesApp)
		{
			file = file.replace(Info::getFileName() + "_", "");

			file = file.replace(".qm", "");
		}

		dir.setNameFilters({ "qtbase_*.qm" });

		QStringList filesBase = dir.entryList();

		for (auto& file : filesBase)
		{
			file = file.replace("qtbase_", "");

			file = file.replace(".qm", "");

			if (filesApp.contains(file))
			{
				bcp47Translations.append(file);
			}
		}
	}

	return bcp47Translations;
}

bool Translator::isBcp47TranslationSupported(const QString& bcp47Translation)
{
	return getBcp47Translations().contains(bcp47Translation);
}

void Translator::loadTranslation(QApplication& application, const QLocale& locale)
{
	Q_UNUSED(application);

	QSharedPointer<QTranslator> translatorApp = weakTranslatorApp.toStrongRef();

	QSharedPointer<QTranslator> translatorBase = weakTranslatorBase.toStrongRef();

	if (translatorApp && translatorBase)
	{
		translatorApp->load(locale, Info::getFileName(), "_", getTranslationsDir());

		application.installTranslator(translatorApp.data());

		translatorBase->load(locale, "qtbase", "_", getTranslationsDir());

		application.installTranslator(translatorBase.data());
	}
}

QWeakPointer<QTranslator> Translator::weakTranslatorApp;

QWeakPointer<QTranslator> Translator::weakTranslatorBase;
