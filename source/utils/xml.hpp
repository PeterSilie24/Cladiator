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
#include <QtXml>

class Xml
{
public:
	template <typename T>
	static QVector<T> loadObjects(QDomElement element)
	{
		QVector<T> objects;

		QDomNodeList list = element.elementsByTagName(T::getTagName());

		for (int i = 0; i < list.count(); i++)
		{
			QDomNode node = list.item(i);

			if (node.isElement())
			{
				QDomElement element = node.toElement();

				objects.append(T(element));
			}
		}

		return objects;
	}

	template <typename T>
	static QDomElement appendObject(QDomElement element)
	{
		QDomElement childElement = element.ownerDocument().createElement(T::getTagName());

		element.appendChild(childElement);

		return childElement;
	}

	static QVector<QDomElement> getChildElements(QDomElement element);

	static QVector<QDomElement> getAllElements(QDomElement element);
};
