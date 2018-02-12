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

#define STR(s) #s

#define FILE_NAME "cladiator"

#define APPLICATION_NAME "Cladiator"

#define APPLICATION_ICON_NAME "cladiator"

#define EXECUTABLE_ICON_NAME "cladiator.ico"

#define COPYRIGHT "2017 - 2018 Phil Badura"

#define MAJOR 0
#define MINOR 9
#define PATCH 1

#define MAKE_VERSION(major, minor, patch) STR(major) "." STR(minor) "." STR(patch) ".0"

#define VERSION MAKE_VERSION(MAJOR, MINOR, PATCH)

#ifdef __cplusplus

#include <QObject>

#else

#define QT_TRANSLATE_NOOP(x, s) s

#endif

#define APPLICATION_DESCRIPTION QT_TRANSLATE_NOOP("QObject", "Class Diagram Editor")

#ifdef __cplusplus

#include <QString>

#include <QSharedPointer>
#include <QWeakPointer>

struct Version
{
public:
	Version(quint32 majorVersion = 1, quint32 minorVersion = 0, quint32 patchVersion = 0);

	QString toString() const;

	quint32 majorVersion;
	quint32 minorVersion;
	quint32 patchVersion;
};

class Info
{
public:
	static QString getFileName();

	static QString getApplicationName();

	static QString getApplicationDescription();

	static QString getApplicationIconName();

	static QString getCopyright();

	static Version getVersion();
};

#endif
