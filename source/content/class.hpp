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

#include "content/content.hpp"

#include "content/visibility.hpp"

class Attribute
{
public:
	Attribute();

	Attribute(const QString& name, bool classScope, const QString& type, const QString& multiplicity, const QString& defaultValue, Visibility visibility);

	Attribute(QDomElement element);

	static QString getTagName();

	void appendXml(QDomElement element);

	bool operator==(const Attribute& other) const;

	bool operator!=(const Attribute& other) const;

	const QString& getName() const;

	void setName(const QString& name);

	bool isClassScope() const;

	void setClassScope(bool classScope);

	const QString& getType() const;

	void setType(const QString& type);

	const QString& getMultiplicity() const;

	void setMultiplicity(const QString& multiplicity);

	const QString& getDefaultValue() const;

	void setDefaultValue(const QString& defaultValue);

	const Visibility& getVisibility() const;

	void setVisibility(const Visibility& visibility);

	QString toString() const;

private:
	QString name;
	bool classScope;
	QString type;
	QString multiplicity;
	QString defaultValue;
	Visibility visibility;
};

class Parameter
{
public:
	Parameter();

	Parameter(const QString& name, const QString& type, const QString& multiplicity, const QString& defaultValue);

	Parameter(QDomElement element);

	static QString getTagName();

	void appendXml(QDomElement element);

	bool operator==(const Parameter& other) const;

	bool operator!=(const Parameter& other) const;

	const QString& getName() const;

	void setName(const QString& name);

	const QString& getType() const;

	void setType(const QString& type);

	const QString& getMultiplicity() const;

	void setMultiplicity(const QString& multiplicity);

	const QString& getDefaultValue() const;

	void setDefaultValue(const QString& defaultValue);

	QString toString() const;

private:
	QString name;
	QString type;
	QString multiplicity;
	QString defaultValue;
};

class Method
{
public:
	Method();

	Method(const QString& name, bool abstract, bool classScope, const QString& returnType, Visibility visibility, const QVector<Parameter>& parameters);

	Method(QDomElement element);

	static QString getTagName();

	void appendXml(QDomElement element);

	bool operator==(const Method& other) const;

	bool operator!=(const Method& other) const;

	const QString& getName() const;

	void setName(const QString& name);

	bool isAbstract() const;

	void setAbstract(bool abstract);

	bool isClassScope() const;

	void setClassScope(bool classScope);

	const QString& getReturnType() const;

	void setReturnType(const QString& returnType);

	const Visibility& getVisibility() const;

	void setVisibility(const Visibility& visibility);

	const QVector<Parameter>& getParameters() const;

	QVector<Parameter>& getParameters();

	int getParameterCount() const;

	void addParameter(const Parameter& parameter);

	void removeParameterAt(int index);

	void swapParameters(int index1, int index2);

	const Parameter& getParameterAt(int index) const;

	Parameter& getParameterAt(int index);

	void setParameters(const QVector<Parameter>& parameters);

	QString toString() const;

private:
	QString name;
	bool abstract;
	bool classScope;
	QString returnType;

	Visibility visibility;

	QVector<Parameter> parameters;
};

class Class : public PositionableContent
{
public:
	Class();

	Class(const QString& name, const QString& stereotype, bool abstract, const QVector<Attribute>& attributes, const QVector<Method>& methods);

	Class(QDomElement element);

	static QString getTagName();

	void appendXml(QDomElement element, const Identifier& id, quint64 level, const QPointF& position);

	bool operator==(const Class& other) const;

	bool operator!=(const Class& other) const;

	const QString& getName() const;

	void setName(const QString& name);

	const QString& getStereotype() const;

	void setStereotype(const QString& stereotype);

	bool isAbstract() const;

	void setAbstract(bool abstract);

	const QVector<Attribute>& getAttributes() const;

	QVector<Attribute>& getAttributes();

	int getAttributeCount() const;

	void addAttribute(const Attribute& attribute);

	void removeAttributeAt(int index);

	void swapAttributes(int index1, int index2);

	const Attribute& getAttributeAt(int index) const;

	Attribute& getAttributeAt(int index);

	void setAttributes(const QVector<Attribute>& attributes);

	const QVector<Method>& getMethods() const;

	QVector<Method>& getMethods();

	int getMethodCount() const;

	void addMethod(const Method& method);

	void removeMethodAt(int index);

	void swapMethods(int index1, int index2);

	const Method& getMethodAt(int index) const;

	Method& getMethodAt(int index);

	void setMethods(const QVector<Method>& methods);

private:
	QString name;
	QString stereotype;
	bool abstract;

	QVector<Attribute> attributes;
	QVector<Method> methods;
};
