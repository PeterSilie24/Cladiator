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

#include <QString>

class Visibility
{
public:
	enum EnumVisibility
	{
		Undefined = 0,
		Public = 1,
		Protected = 2,
		Private = 3,
		Package = 4
	};

	Visibility();

	Visibility(EnumVisibility visibility);

	Visibility(int visibility);

	Visibility(const QString& visibility);

	operator EnumVisibility() const;

	QString toString() const;

	QString toSymbol() const;

private:
	EnumVisibility visibility;
};
