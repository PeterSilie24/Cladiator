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

#include "class.hpp"

Attribute::Attribute()
{
	this->setClassScope(false);
}

Attribute::Attribute(const QString& name, bool classScope, const QString& type, const QString& multiplicity, const QString& defaultValue, Visibility visibility)
{
	this->setName(name);

	this->setClassScope(classScope);

	this->setType(type);

	this->setMultiplicity(multiplicity);

	this->setDefaultValue(defaultValue);

	this->setVisibility(visibility);
}

Attribute::Attribute(QDomElement element)
{
	this->setName(element.attribute("name"));

	this->setClassScope((element.attribute("class-scope") == "true") ? true : false);

	this->setType(element.attribute("type"));

	this->setMultiplicity(element.attribute("multiplicity"));

	this->setDefaultValue(element.attribute("default-value"));

	this->setVisibility(element.attribute("visibility"));
}

QString Attribute::getTagName()
{
	return "attribute";
}

void Attribute::appendXml(QDomElement element)
{
	element.setAttribute("name", this->name);
	element.setAttribute("class-scope", this->classScope ? "true" : "false");
	element.setAttribute("type", this->type);
	element.setAttribute("multiplicity", this->multiplicity);
	element.setAttribute("default-value", this->defaultValue);

	element.setAttribute("visibility", this->visibility.toString());
}

bool Attribute::operator==(const Attribute& other) const
{
	if (this->name != other.name) { return false; }

	if (this->classScope != other.classScope) { return false; }

	if (this->type != other.type) { return false; }

	if (this->multiplicity != other.multiplicity) { return false; }

	if (this->defaultValue != other.defaultValue) { return false; }

	if (this->visibility != other.visibility) { return false; }

	return true;
}

bool Attribute::operator!=(const Attribute& other) const
{
	return !this->operator==(other);
}

const QString& Attribute::getName() const
{
	return this->name;
}

void Attribute::setName(const QString& name)
{
	this->name = RegExp::validate(name);
}

bool Attribute::isClassScope() const
{
	return this->classScope;
}

void Attribute::setClassScope(bool classScope)
{
	this->classScope = classScope;
}

const QString& Attribute::getType() const
{
	return this->type;
}

void Attribute::setType(const QString& type)
{
	this->type = RegExp::validate(type, RegExp::Type);
}

const QString& Attribute::getMultiplicity() const
{
	return this->multiplicity;
}

void Attribute::setMultiplicity(const QString& multiplicity)
{
	this->multiplicity = RegExp::validate(multiplicity, RegExp::Multiplicity);
}

const QString& Attribute::getDefaultValue() const
{
	return this->defaultValue;
}

void Attribute::setDefaultValue(const QString& defaultValue)
{
	this->defaultValue = RegExp::validate(defaultValue, RegExp::Value);
}

const Visibility& Attribute::getVisibility() const
{
	return this->visibility;
}

void Attribute::setVisibility(const Visibility& visibility)
{
	this->visibility = visibility;
}

QString Attribute::toString() const
{
	QString string = this->visibility.toSymbol() + " " + this->name;

	if (this->type != "")
	{
		string += " : " + this->type;
	}

	if (this->multiplicity != "")
	{
		string += " [" + this->multiplicity + "]";
	}

	if (this->defaultValue != "")
	{
		string += " = " + this->defaultValue;
	}

	return string;
}

Parameter::Parameter() { }

Parameter::Parameter(const QString& name, const QString& type, const QString& multiplicity, const QString& defaultValue)
{
	this->setName(name);

	this->setType(type);

	this->setMultiplicity(multiplicity);

	this->setDefaultValue(defaultValue);
}

Parameter::Parameter(QDomElement element)
{
	this->setName(element.attribute("name"));

	this->setType(element.attribute("type"));

	this->setMultiplicity(element.attribute("multiplicity"));

	this->setDefaultValue(element.attribute("default-value"));
}

QString Parameter::getTagName()
{
	return "parameter";
}

void Parameter::appendXml(QDomElement element)
{
	element.setAttribute("name", this->name);
	element.setAttribute("type", this->type);
	element.setAttribute("multiplicity", this->multiplicity);
	element.setAttribute("default-value", this->defaultValue);
}

bool Parameter::operator==(const Parameter& other) const
{
	if (this->name != other.name) { return false; }

	if (this->type != other.type) { return false; }

	if (this->multiplicity != other.multiplicity) { return false; }

	if (this->defaultValue != other.defaultValue) { return false; }

	return true;
}

bool Parameter::operator!=(const Parameter& other) const
{
	return !this->operator==(other);
}

const QString& Parameter::getName() const
{
	return this->name;
}

void Parameter::setName(const QString& name)
{
	this->name = RegExp::validate(name);
}

const QString& Parameter::getType() const
{
	return this->type;
}

void Parameter::setType(const QString& type)
{
	this->type = RegExp::validate(type, RegExp::Type);
}

const QString& Parameter::getMultiplicity() const
{
	return this->multiplicity;
}

void Parameter::setMultiplicity(const QString& multiplicity)
{
	this->multiplicity = RegExp::validate(multiplicity, RegExp::Multiplicity);
}

const QString& Parameter::getDefaultValue() const
{
	return this->defaultValue;
}

void Parameter::setDefaultValue(const QString& defaultValue)
{
	this->defaultValue = RegExp::validate(defaultValue, RegExp::Value);
}

QString Parameter::toString() const
{
	QString string = this->name;

	if (this->type != "")
	{
		string += " : " + this->type;
	}

	if (this->multiplicity != "")
	{
		string += " [" + this->multiplicity + "]";
	}

	if (this->defaultValue != "")
	{
		string += " = " + this->defaultValue;
	}

	return string;
}

Method::Method()
{
	this->setAbstract(false);

	this->setClassScope(false);
}

Method::Method(const QString& name, bool abstract, bool classScope, const QString& returnType, Visibility visibility, const QVector<Parameter>& parameters)
{
	this->setName(name);

	this->setAbstract(abstract);

	this->setClassScope(classScope);

	this->setReturnType(returnType);

	this->setVisibility(visibility);

	this->setParameters(parameters);
}

Method::Method(QDomElement element)
{
	this->setName(element.attribute("name"));

	this->setAbstract((element.attribute("abstract") == "true") ? true : false);

	this->setClassScope((element.attribute("class-scope") == "true") ? true : false);

	this->setReturnType(element.attribute("return-type"));

	this->setVisibility(element.attribute("visibility"));

	this->parameters = Xml::loadObjects<Parameter>(element);
}

QString Method::getTagName()
{
	return "method";
}

void Method::appendXml(QDomElement element)
{
	element.setAttribute("name", this->name);
	element.setAttribute("abstract", this->abstract ? "true" : "false");
	element.setAttribute("class-scope", this->classScope ? "true" : "false");
	element.setAttribute("return-type", this->returnType);

	element.setAttribute("visibility", this->visibility.toString());

	for (auto& parameter : this->parameters)
	{
		parameter.appendXml(Xml::appendObject<Parameter>(element));
	}
}

bool Method::operator==(const Method& other) const
{
	if (this->name != other.name) { return false; }

	if (this->abstract != other.abstract) { return false; }

	if (this->classScope != other.classScope) { return false; }

	if (this->returnType != other.returnType) { return false; }

	if (this->visibility != other.visibility) { return false; }

	if (this->parameters.size() != other.parameters.size()) { return false; }

	for (int i = 0; i < this->parameters.size(); i++)
	{
		if (this->parameters[i] != other.parameters[i]) { return false; }
	}

	return true;
}

bool Method::operator!=(const Method& other) const
{
	return !this->operator==(other);
}

const QString& Method::getName() const
{
	return this->name;
}

void Method::setName(const QString& name)
{
	this->name = RegExp::validate(name);
}

bool Method::isAbstract() const
{
	return this->abstract;
}

void Method::setAbstract(bool abstract)
{
	this->abstract = abstract;
}

bool Method::isClassScope() const
{
	return this->classScope;
}

void Method::setClassScope(bool classScope)
{
	this->classScope = classScope;
}

const QString& Method::getReturnType() const
{
	return this->returnType;
}

void Method::setReturnType(const QString& returnType)
{
	this->returnType = RegExp::validate(returnType, RegExp::Type);
}

const Visibility& Method::getVisibility() const
{
	return this->visibility;
}

void Method::setVisibility(const Visibility& visibility)
{
	this->visibility = visibility;
}

const QVector<Parameter>& Method::getParameters() const
{
	return this->parameters;
}

QVector<Parameter>& Method::getParameters()
{
	return this->parameters;
}

int Method::getParameterCount() const
{
	return this->parameters.size();
}

void Method::addParameter(const Parameter& parameter)
{
	this->parameters.append(parameter);
}

void Method::removeParameterAt(int index)
{
	this->parameters.removeAt(index);
}

void Method::swapParameters(int index1, int index2)
{
	if (index1 != index2 && qMin(index1, index2) >= 0 && qMax(index1, index2) < this->parameters.size())
	{
		Parameter temp = this->parameters[index1];

		this->parameters[index1] = this->parameters[index2];

		this->parameters[index2] = temp;
	}
}

const Parameter& Method::getParameterAt(int index) const
{
	return this->parameters[index];
}

Parameter& Method::getParameterAt(int index)
{
	return this->parameters[index];
}

void Method::setParameters(const QVector<Parameter>& parameters)
{
	this->parameters = parameters;
}

QString Method::toString() const
{
	QString string = this->visibility.toSymbol() + " " + this->name + "(";

	for (int i = 0; i < this->parameters.size(); i++)
	{
		string += this->parameters[i].toString();

		string += ((i < this->parameters.size() - 1) ? ", " : "");
	}

	string += ")";

	if (this->returnType != "")
	{
		string += " : " + this->returnType;
	}

	return string;
}

Class::Class()
{
	this->setAbstract(false);
}

Class::Class(const QString& name, const QString& keyword, bool abstract, const QVector<Attribute>& attributes, const QVector<Method>& methods)
{
	this->setName(name);

	this->setKeyword(keyword);

	this->setAbstract(abstract);

	this->setAttributes(attributes);

	this->setMethods(methods);
}

Class::Class(QDomElement element) : PositionableContent(element)
{
	this->setName(element.attribute("name"));

	this->setKeyword(element.attribute("keyword"));

	this->setAbstract((element.attribute("abstract") == "true") ? true : false);

	this->attributes = Xml::loadObjects<Attribute>(element);

	this->methods = Xml::loadObjects<Method>(element);
}

QString Class::getTagName()
{
	return "class";
}

void Class::appendXml(QDomElement element, const Identifier& id, quint64 level, const QPointF& position)
{
	PositionableContent::appendXml(element, id, level, position);

	element.setAttribute("name", this->name);
	element.setAttribute("keyword", this->keyword);
	element.setAttribute("abstract", this->abstract ? "true" : "false");

	for (auto& attribute : this->attributes)
	{
		attribute.appendXml(Xml::appendObject<Attribute>(element));
	}

	for (auto& method : this->methods)
	{
		method.appendXml(Xml::appendObject<Method>(element));
	}
}

bool Class::operator==(const Class& other) const
{
	if (this->name != other.name) { return false; }

	if (this->keyword != other.keyword) { return false; }

	if (this->abstract != other.abstract) { return false; }

	if (this->attributes.size() != other.attributes.size()) { return false; }

	if (this->methods.size() != other.methods.size()) { return false; }

	for (int i = 0; i < this->attributes.size(); i++)
	{
		if (this->attributes[i] != other.attributes[i]) { return false; }
	}

	for (int i = 0; i < this->methods.size(); i++)
	{
		if (this->methods[i] != other.methods[i]) { return false; }
	}

	return true;
}

bool Class::operator!=(const Class& other) const
{
	return !this->operator==(other);
}

const QString& Class::getName() const
{
	return this->name;
}

void Class::setName(const QString& name)
{
	this->name = RegExp::validate(name);
}

const QString& Class::getKeyword() const
{
	return this->keyword;
}

void Class::setKeyword(const QString& keyword)
{
	this->keyword = RegExp::validate(keyword);
}

bool Class::isAbstract() const
{
	return this->abstract;
}

void Class::setAbstract(bool abstract)
{
	this->abstract = abstract;
}

const QVector<Attribute>& Class::getAttributes() const
{
	return this->attributes;
}

QVector<Attribute>& Class::getAttributes()
{
	return this->attributes;
}

int Class::getAttributeCount() const
{
	return this->attributes.size();
}

void Class::addAttribute(const Attribute& attribute)
{
	this->attributes.append(attribute);
}

void Class::removeAttributeAt(int index)
{
	this->attributes.removeAt(index);
}

void Class::swapAttributes(int index1, int index2)
{
	if (index1 != index2 && qMin(index1, index2) >= 0 && qMax(index1, index2) < this->attributes.size())
	{
		Attribute temp = this->attributes[index1];

		this->attributes[index1] = this->attributes[index2];

		this->attributes[index2] = temp;
	}
}

const Attribute& Class::getAttributeAt(int index) const
{
	return this->attributes[index];
}

Attribute& Class::getAttributeAt(int index)
{
	return this->attributes[index];
}

void Class::setAttributes(const QVector<Attribute>& attributes)
{
	this->attributes = attributes;
}

const QVector<Method>& Class::getMethods() const
{
	return this->methods;
}

QVector<Method>& Class::getMethods()
{
	return this->methods;
}

int Class::getMethodCount() const
{
	return this->methods.size();
}

void Class::addMethod(const Method& method)
{
	this->methods.append(method);
}

void Class::removeMethodAt(int index)
{
	this->methods.removeAt(index);
}

void Class::swapMethods(int index1, int index2)
{
	if (index1 != index2 && qMin(index1, index2) >= 0 && qMax(index1, index2) < this->methods.size())
	{
		Method temp = this->methods[index1];

		this->methods[index1] = this->methods[index2];

		this->methods[index2] = temp;
	}
}

const Method& Class::getMethodAt(int index) const
{
	return this->methods[index];
}

Method& Class::getMethodAt(int index)
{
	return this->methods[index];
}

void Class::setMethods(const QVector<Method>& methods)
{
	this->methods = methods;
}
