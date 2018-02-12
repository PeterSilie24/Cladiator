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

#include "icons.hpp"

Icons::Icons()
{
	if (!this->weakIcons)
	{
		this->strongIcons = QSharedPointer<QMap<QString, QIcon>>(new QMap<QString, QIcon>());

		this->weakIcons = this->strongIcons.toWeakRef();
	}
}

QString Icons::getIconsDir()
{
	if (weakIcons)
	{
		return QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/icons/";
	}

	return "";
}

QString Icons::getResourceIconsDir()
{
	if (weakIcons)
	{
		return ":/cladiator/icons/";
	}

	return "";
}

QIcon Icons::loadIcon(const QString& name)
{
	QSharedPointer<QMap<QString, QIcon>> icons = weakIcons.toStrongRef();

	if (icons)
	{
		if (icons->contains(name))
		{
			return (*icons)[name];
		}

		QIcon icon;

		if (QIcon::hasThemeIcon(name))
		{
			icon = QIcon::fromTheme(name);
		}
		else
		{
			icon = QIcon::fromTheme(name, QIcon(getIconsDir() + name + ".svg"));
		}

		icons->insert(name, icon);

		return icon;
	}

	return QIcon();
}

QWeakPointer<QMap<QString, QIcon>> Icons::weakIcons;
