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

#include "info.hpp"

Version::Version(quint32 majorVersion, quint32 minorVersion, quint32 patchVersion)
	: majorVersion(majorVersion), minorVersion(minorVersion), patchVersion(patchVersion)
{

}

QString Version::toString() const
{
	return QString::number(this->majorVersion) + "." + QString::number(this->minorVersion) + "." + QString::number(this->patchVersion);
}

QString Info::getFileName()
{
	return QString(FILE_NAME);
}

QString Info::getApplicationName()
{
	return QString(APPLICATION_NAME);
}

QString Info::getApplicationDescription()
{
	return QObject::tr(APPLICATION_DESCRIPTION);
}

QString Info::getApplicationIconName()
{
	return QString(APPLICATION_ICON_NAME);
}

QString Info::getCopyright()
{
	return QString(COPYRIGHT);
}

Version Info::getVersion()
{
	return Version(MAJOR, MINOR, PATCH);
}
