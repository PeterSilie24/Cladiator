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

#include "identifier.hpp"

Identifier::Identifier() { }

Identifier::Identifier(const QSharedPointer<quint64>& identifier) : identifier(identifier) { }

Identifier::operator bool() const
{
	return !(this->identifier.isNull());
}

bool Identifier::operator==(const Identifier& other) const
{
	if (this->identifier)
	{
		return (this->getValue() == other.getValue());
	}

	return false;
}

bool Identifier::operator!=(const Identifier& other) const
{
	return !this->operator==(other);
}

quint64 Identifier::getValue() const
{
	if (this->identifier)
	{
		return *(this->identifier);
	}

	return 0;
}

QString Identifier::toString() const
{
	return QString::number(this->getValue(), 16).toUpper();
}

IdentifierManager::IdentifierManager() : last(0) { }

IdentifierManager::~IdentifierManager()
{
	for (quint64* identifier : this->identifiers)
	{
		delete identifier;
	}

	this->identifiers.clear();
}

Identifier IdentifierManager::newIdentifier()
{
	return this->createIdentifier(this->queryIdentifier());
}

Identifier IdentifierManager::newIdentifier(quint64 requestedIdentifier)
{
	if (this->checkIdentifier(requestedIdentifier))
	{
		return this->createIdentifier(requestedIdentifier);
	}

	return Identifier();
}

Identifier IdentifierManager::newIdentifier(const QString& requestedIdentifier)
{
	return this->newIdentifier(requestedIdentifier.toULongLong(nullptr, 16));
}

Identifier IdentifierManager::createIdentifier(quint64 identifier)
{
	if (!identifier)
	{
		return Identifier();
	}

	quint64* newIdentifier = new quint64(identifier);

	this->identifiers.append(newIdentifier);

	return Identifier(QSharedPointer<quint64>(newIdentifier, [&](quint64* identifier) { this->deleteIdentifier(identifier); }));
}

void IdentifierManager::deleteIdentifier(quint64* identifier)
{
	for (int i = 0; i < this->identifiers.size(); i++)
	{
		if (this->identifiers[i] == identifier)
		{
			delete this->identifiers[i];

			this->identifiers.removeAt(i);

			break;
		}
	}
}

bool IdentifierManager::checkIdentifier(quint64 identifier)
{
	if (!identifier)
	{
		return false;
	}

	for (int i = 0; i < this->identifiers.size(); i++)
	{
		if (*(this->identifiers[i]) == identifier)
		{
			return false;
		}
	}

	return true;
}

quint64 IdentifierManager::queryIdentifier()
{
	if (this->last < ~(quint64(0)) && this->checkIdentifier(this->last + 1))
	{
		return ++(this->last);
	}
	else
	{
		for (quint64 i = 1; i < ~(quint64(0)); i++)
		{
			if (this->checkIdentifier(i))
			{
				this->last = i;

				return this->last;
			}
		}
	}

	return 0;
}
