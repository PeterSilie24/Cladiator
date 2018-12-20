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

#include <QApplication>
#include <QTranslator>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QDir>

#include "config/info.hpp"

class Translator
{
public:
	Translator();

	static QString getTranslationsDir();

	static QStringList getBcp47Translations();

	static bool isBcp47TranslationSupported(const QString& bcp47Translation);

	static void loadTranslation(QApplication& application, const QLocale& locale = QLocale::system());

private:
	QSharedPointer<QTranslator> strongTranslatorApp;

	static QWeakPointer<QTranslator> weakTranslatorApp;

	QSharedPointer<QTranslator> strongTranslatorQt;

	static QWeakPointer<QTranslator> weakTranslatorQt;
};
