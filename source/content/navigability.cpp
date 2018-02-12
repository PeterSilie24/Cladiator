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

#include "navigability.hpp"

Navigability::Navigability() : navigability(EnumNavigability::Undefined) { }

Navigability::Navigability(EnumNavigability navigability) : navigability(navigability) { }

Navigability::Navigability(int navigability) : navigability(static_cast<Navigability::EnumNavigability>(navigability)) { }

Navigability::Navigability(const QString& navigability) : navigability(EnumNavigability::Undefined)
{
	if (navigability == "undefined")
	{
		this->navigability = EnumNavigability::Undefined;
	}
	else if (navigability == "navigable")
	{
		this->navigability = EnumNavigability::Navigable;
	}
	else if (navigability == "not-navigable")
	{
		this->navigability = EnumNavigability::NotNavigable;
	}
}

Navigability::operator EnumNavigability() const
{
	return this->navigability;
}

QString Navigability::toString() const
{
	switch (this->navigability)
	{
	case EnumNavigability::Undefined:
	{
		return "undefined";
	}
	case EnumNavigability::Navigable:
	{
		return "navigable";
	}
	case EnumNavigability::NotNavigable:
	{
		return "not-navigable";
	}
	}

	return "";
}
