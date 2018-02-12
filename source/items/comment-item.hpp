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

#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include "content/comment.hpp"

#include "dialogs/comment-editor.hpp"

#include "item-scene/item-scene.hpp"

#include "items/connection-item.hpp"

class CommentItem : public ConnectableItem
{
	Q_OBJECT

public:
	CommentItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position, const Comment& comment);

	static QVector<UndoRedoEvent> loadItems(ItemScene* scene, QDomElement element);

	static QVector<UndoRedoEvent> createItemEvent(ItemScene* scene, const QPointF& position);
	static void createItem(ItemScene* scene, const QPointF& position);

	QVector<UndoRedoEvent> changeCommentEvent(const Comment& newComment);
	void changeComment(const Comment& newComment);

	const Comment& getComment() const;

	void edit() override;

	QPolygonF outline() const override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	void appendXml(QDomElement parentElement) override;

protected:
	virtual void updateGeometry() override;

private:
	void setComment(const Comment& comment);

	Comment comment;

	QTextDocument document;

	QRectF rect;
};
