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

#include "comment.hpp"

Comment::Comment() { }

Comment::Comment(const QString& content) : content(content) { }

Comment::Comment(QDomElement element) : PositionableContent(element), content(element.attribute("content")) { }

QString Comment::getTagName()
{
	return "comment";
}

void Comment::appendXml(QDomElement element, const Identifier& id, quint64 level, const QPointF& position)
{
	PositionableContent::appendXml(element, id, level, position);
	
	element.setAttribute("content", this->content);
}

bool Comment::operator==(const Comment& other) const
{
	return this->content == other.content;
}

bool Comment::operator!=(const Comment& other) const
{
	return !this->operator==(other);
}

const QString& Comment::getContent() const
{
	return this->content;
}

void Comment::setContent(const QString& content)
{
	this->content = content;
}
