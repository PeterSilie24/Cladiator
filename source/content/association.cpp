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

#include "association.hpp"

Association::Type::Type() : type(EnumType::Default) { }

Association::Type::Type(EnumType type) : type(type) { }

Association::Type::Type(int type) : type(static_cast<Type::EnumType>(type)) { }

Association::Type::Type(const QString& type) : type(EnumType::Default)
{
	if (type == "default")
	{
		this->type = EnumType::Default;
	}
	else if (type == "aggregation")
	{
		this->type = EnumType::Aggregation;
	}
	else if (type == "composition")
	{
		this->type = EnumType::Composition;
	}
}

Association::Type::operator EnumType() const
{
	return this->type;
}

QString Association::Type::toString() const
{
	switch (this->type)
	{
	case EnumType::Default:
	{
		return "default";
	}
	case EnumType::Aggregation:
	{
		return "aggregation";
	}
	case EnumType::Composition:
	{
		return "composition";
	}
	}

	return "";
}

Association::Association()
{
	this->setShowDirection(false);
}

Association::Association(const QString& name, bool showDirection, const Type& type, const QString& roleSideA, const Visibility& visibilitySideA, const QString& multiplicitySideA, const Navigability& navigabilitySideA, const QString& roleSideB, const Visibility& visibilitySideB, const QString& multiplicitySideB, const Navigability& navigabilitySideB)
{
	this->setName(name);
	this->setShowDirection(showDirection);
	this->setType(type);

	this->setRoleSideA(roleSideA);
	this->setVisibilitySideA(visibilitySideA);
	this->setMultiplicitySideA(multiplicitySideA);
	this->setNavigabilitySideA(navigabilitySideA);

	this->setRoleSideB(roleSideB);
	this->setVisibilitySideB(visibilitySideB);
	this->setMultiplicitySideB(multiplicitySideB);
	this->setNavigabilitySideB(navigabilitySideB);
}

Association::Association(QDomElement element) : Connection(element)
{
	this->setName(element.attribute("name"));
	this->setShowDirection((element.attribute("show-direction") == "true") ? true : false);
	this->setType(element.attribute("type"));

	QDomNodeList list = element.elementsByTagName("side-a");

	if (list.count())
	{
		QDomNode node = list.item(0);

		if (node.isElement())
		{
			QDomElement element = node.toElement();

			this->setRoleSideA(element.attribute("role"));
			this->setVisibilitySideA(element.attribute("visibility"));
			this->setMultiplicitySideA(element.attribute("multiplicity"));
			this->setNavigabilitySideA(element.attribute("navigability"));
		}
	}

	list = element.elementsByTagName("side-b");

	if (list.count())
	{
		QDomNode node = list.item(0);

		if (node.isElement())
		{
			QDomElement element = node.toElement();

			this->setRoleSideB(element.attribute("role"));
			this->setVisibilitySideB(element.attribute("visibility"));
			this->setMultiplicitySideB(element.attribute("multiplicity"));
			this->setNavigabilitySideB(element.attribute("navigability"));
		}
	}
}

void Association::appendXml(QDomElement element, const Identifier& id, quint64 level, bool perpendicular, EndNode& beginNode, EndNode& endNode, const QPolygonF& nodes)
{
	element.setAttribute("name", this->name);
	element.setAttribute("show-direction", this->showDirection ? "true" : "false");
	element.setAttribute("type", this->type.toString());

	QDomElement childElement = element.ownerDocument().createElement("side-a");

	childElement.setAttribute("role", this->roleSideA);
	childElement.setAttribute("visibility", this->visibilitySideA.toString());
	childElement.setAttribute("multiplicity", this->multiplicitySideA);
	childElement.setAttribute("navigability", this->navigabilitySideA.toString());

	element.appendChild(childElement);

	Connection::appendXml(element, id, level, perpendicular, beginNode, endNode, nodes);

	childElement = element.ownerDocument().createElement("side-b");

	childElement.setAttribute("role", this->roleSideB);
	childElement.setAttribute("visibility", this->visibilitySideB.toString());
	childElement.setAttribute("multiplicity", this->multiplicitySideB);
	childElement.setAttribute("navigability", this->navigabilitySideB.toString());

	element.appendChild(childElement);
}

bool Association::operator==(const Association& other) const
{
	if (this->name != other.name) { return false; }

	if (this->showDirection != other.showDirection) { return false; }

	if (this->type != other.type) { return false; }

	if (this->roleSideA != other.roleSideA) { return false; }

	if (this->visibilitySideA != other.visibilitySideA) { return false; }

	if (this->multiplicitySideA != other.multiplicitySideA) { return false; }

	if (this->navigabilitySideA != other.navigabilitySideA) { return false; }

	if (this->roleSideB != other.roleSideB) { return false; }

	if (this->visibilitySideB != other.visibilitySideB) { return false; }

	if (this->multiplicitySideB != other.multiplicitySideB) { return false; }

	if (this->navigabilitySideB != other.navigabilitySideB) { return false; }

	return true;
}

bool Association::operator!=(const Association& other) const
{
	return !this->operator==(other);
}

QString Association::getTagName()
{
	return "association";
}

const QString& Association::getName() const
{
	return this->name;
}

void Association::setName(const QString& name)
{
	this->name = name;
}

bool Association::isShowDirection() const
{
	return this->showDirection;
}

void Association::setShowDirection(bool showDirection)
{
	this->showDirection = showDirection;
}

const Association::Type& Association::getType() const
{
	return this->type;
}

void Association::setType(const Type& type)
{
	this->type = type;
}

const QString& Association::getRoleSideA() const
{
	return this->roleSideA;
}

void Association::setRoleSideA(const QString& roleSideA)
{
	this->roleSideA = RegExp::validate(roleSideA);
}

const Visibility& Association::getVisibilitySideA() const
{
	return this->visibilitySideA;
}

void Association::setVisibilitySideA(const Visibility& visibilitySideA)
{
	this->visibilitySideA = visibilitySideA;
}

const QString& Association::getMultiplicitySideA() const
{
	return this->multiplicitySideA;
}

void Association::setMultiplicitySideA(const QString& multiplicitySideA)
{
	this->multiplicitySideA = RegExp::validate(multiplicitySideA, RegExp::Multiplicity);
}

const Navigability& Association::getNavigabilitySideA() const
{
	return this->navigabilitySideA;
}

void Association::setNavigabilitySideA(const Navigability& navigabilitySideA)
{
	this->navigabilitySideA = navigabilitySideA;
}

const QString& Association::getRoleSideB() const
{
	return this->roleSideB;
}

void Association::setRoleSideB(const QString& roleSideB)
{
	this->roleSideB = RegExp::validate(roleSideB);
}

const Visibility& Association::getVisibilitySideB() const
{
	return this->visibilitySideB;
}

void Association::setVisibilitySideB(const Visibility& visibilitySideB)
{
	this->visibilitySideB = visibilitySideB;
}

const QString& Association::getMultiplicitySideB() const
{
	return this->multiplicitySideB;
}

void Association::setMultiplicitySideB(const QString& multiplicitySideB)
{
	this->multiplicitySideB = RegExp::validate(multiplicitySideB, RegExp::Multiplicity);
}

const Navigability& Association::getNavigabilitySideB() const
{
	return this->navigabilitySideB;
}

void Association::setNavigabilitySideB(const Navigability& navigabilitySideB)
{
	this->navigabilitySideB = navigabilitySideB;
}
