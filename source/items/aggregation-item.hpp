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

#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include "items/association-item.hpp"

class AggregationItem : public AssociationItem
{
	Q_OBJECT

public:
	AggregationItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position);

	static void createItem(ItemScene* scene, const QPointF& position);
};
