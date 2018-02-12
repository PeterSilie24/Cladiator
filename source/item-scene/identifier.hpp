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

#include <QVector>
#include <QSharedPointer>

class Identifier
{
public:
	Identifier();

	Identifier(const QSharedPointer<quint64>& identifier);

	operator bool() const;

	bool operator==(const Identifier& other) const;

	bool operator!=(const Identifier& other) const;

	quint64 getValue() const;

	QString toString() const;

private:
	QSharedPointer<quint64> identifier;
};

class IdentifierManager
{
public:
	IdentifierManager();

	~IdentifierManager();

	Identifier newIdentifier();

	Identifier newIdentifier(quint64 requestedIdentifier);

	Identifier newIdentifier(const QString& requestedIdentifier);

private:
	quint64 last;

	QVector<quint64*> identifiers;

	Identifier createIdentifier(quint64 identifier);

	void deleteIdentifier(quint64* identifier);

	bool checkIdentifier(quint64 identifier);

	quint64 queryIdentifier();
};
