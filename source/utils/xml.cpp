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

#include "xml.hpp"

QVector<QDomElement> Xml::getChildElements(QDomElement element)
{
	QVector<QDomElement> childElements;

	QDomNodeList list = element.childNodes();

	for (int i = 0; i < list.count(); i++)
	{
		QDomNode node = list.item(i);

		if (node.isElement())
		{
			childElements.append(node.toElement());
		}
	}

	return childElements;
}

QVector<QDomElement> Xml::getAllElements(QDomElement element)
{
	QVector<QDomElement> elements;

	elements.append(element);

	QVector<QDomElement> childElements = getChildElements(element);

	for (auto& childElement : childElements)
	{
		elements.append(getAllElements(childElement));
	}

	return elements;
}
