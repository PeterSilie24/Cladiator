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

#include <QRegExp>
#include <QValidator>
#include <QMap>
#include <QString>
#include <QSharedPointer>
#include <QWeakPointer>

class RegExp
{
public:
	static const QString Default;

	static const QString Type;

	static const QString Value;

	static const QString Multiplicity;

	RegExp();

	static QRegExpValidator* getValidator(const QString& regExp = Default);

	static QString validate(const QString& string, const QString& regExp = Default);

private:
	QSharedPointer<QMap<QString, QSharedPointer<QRegExpValidator>>> strongValidators;

	static QWeakPointer<QMap<QString, QSharedPointer<QRegExpValidator>>> weakValidators;
};
