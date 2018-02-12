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

#include "visibility.hpp"

Visibility::Visibility() : visibility(EnumVisibility::Undefined) { }

Visibility::Visibility(EnumVisibility visibility) : visibility(visibility) { }

Visibility::Visibility(int visibility) : visibility(static_cast<Visibility::EnumVisibility>(visibility)) { }

Visibility::Visibility(const QString& visibility) : visibility(EnumVisibility::Undefined)
{
	if (visibility == "public")
	{
		this->visibility = Visibility::Public;
	}
	else if (visibility == "protected")
	{
		this->visibility = Visibility::Protected;
	}
	else if (visibility == "private")
	{
		this->visibility = Visibility::Private;
	}
	else if (visibility == "package")
	{
		this->visibility = Visibility::Package;
	}
}

Visibility::operator EnumVisibility() const
{
	return this->visibility;
}

QString Visibility::toString() const
{
	switch (this->visibility)
	{
	case EnumVisibility::Undefined:
	{
		return "";
	}
	case EnumVisibility::Public:
	{
		return "public";
	}
	case EnumVisibility::Protected:
	{
		return "protected";
	}
	case EnumVisibility::Private:
	{
		return "private";
	}
	case EnumVisibility::Package:
	{
		return "package";
	}
	}

	return "";
}

QString Visibility::toSymbol() const
{
	switch (this->visibility)
	{
	case EnumVisibility::Undefined:
	{
		return "";
	}
	case EnumVisibility::Public:
	{
		return "+";
	}
	case EnumVisibility::Protected:
	{
		return "#";
	}
	case EnumVisibility::Private:
	{
		return "-";
	}
	case EnumVisibility::Package:
	{
		return "~";
	}
	}

	return "";
}
