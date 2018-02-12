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

#include "content/connection.hpp"

#include "content/visibility.hpp"

#include "content/navigability.hpp"

class Association : public Connection
{
public:
	class Type
	{
	public:
		enum EnumType
		{
			Default = 0,
			Aggregation = 1,
			Composition = 2
		};

		Type();

		Type(EnumType type);

		Type(int type);

		Type(const QString& type);

		operator EnumType() const;

		QString toString() const;

	private:
		EnumType type;
	};

	Association();

	Association(const QString& name, bool showDirection, const Type& type, const QString& roleSideA, const Visibility& visibilitySideA, const QString& multiplicitySideA, const Navigability& avigabilitySideA, const QString& roleSideB, const Visibility& visibilitySideB, const QString& multiplicitySideB, const Navigability& avigabilitySideB);

	Association(QDomElement element);

	void appendXml(QDomElement element, const Identifier& id, quint64 level, bool perpendicular, EndNode& beginNode, EndNode& endNode, const QPolygonF& nodes);

	bool operator==(const Association& other) const;

	bool operator!=(const Association& other) const;

	static QString getTagName();

	const QString& getName() const;

	void setName(const QString& name);

	bool isShowDirection() const;

	void setShowDirection(bool showDirection);

	const Type& getType() const;

	void setType(const Type& type);

	const QString& getRoleSideA() const;

	void setRoleSideA(const QString& roleSideA);

	const Visibility& getVisibilitySideA() const;

	void setVisibilitySideA(const Visibility& visibilitySideA);

	const QString& getMultiplicitySideA() const;

	void setMultiplicitySideA(const QString& multiplicitySideA);

	const Navigability& getNavigabilitySideA() const;

	void setNavigabilitySideA(const Navigability& navigabilitySideA);

	const QString& getRoleSideB() const;

	void setRoleSideB(const QString& roleSideB);

	const Visibility& getVisibilitySideB() const;

	void setVisibilitySideB(const Visibility& visibilitySideB);

	const QString& getMultiplicitySideB() const;

	void setMultiplicitySideB(const QString& multiplicitySideB);

	const Navigability& getNavigabilitySideB() const;

	void setNavigabilitySideB(const Navigability& navigabilitySideB);

private:
	QString name;
	bool showDirection;
	Type type;

	QString roleSideA;
	Visibility visibilitySideA;
	QString multiplicitySideA;
	Navigability navigabilitySideA;

	QString roleSideB;
	Visibility visibilitySideB;
	QString multiplicitySideB;
	Navigability navigabilitySideB;
};
