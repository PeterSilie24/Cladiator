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

#include "comment-item.hpp"

CommentItem::CommentItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position, const Comment& comment) : ConnectableItem(scene, id, level, position)
{
	this->setComment(comment);
}

QVector<UndoRedoEvent> CommentItem::loadItems(ItemScene* scene, QDomElement element)
{
	QVector<UndoRedoEvent> events;

	if (scene)
	{
		QVector<Comment> comments = Xml::loadObjects<Comment>(element);

		for (auto& comment : comments)
		{
			ItemScene* cScene = scene;
			Identifier cId = scene->newIdentifier(comment.getRequestedId());
			Comment cComment = comment;

			if (cId)
			{
				std::function<void()> undo = [cScene, cId]()
				{
					cScene->removeItemById(cId);
				};

				std::function<void()> redo = [cScene, cId, cComment]()
				{
					cScene->addItem(QSharedPointer<Item>(new CommentItem(cScene, cId, cComment.getRequestedLevel(), cComment.getRequestedPosition(), cComment)));
				};

				events.append(UndoRedoEvent(undo, redo));
			}
		}
	}

	return events;
}

QVector<UndoRedoEvent> CommentItem::createItemEvent(ItemScene* scene, const QPointF& position)
{
	if (scene)
	{
		Identifier id = scene->newIdentifier();

		if (id)
		{
			ItemScene* cScene = scene;
			Identifier cId = id;

			std::function<void()> undo = [cScene, cId]()
			{
				cScene->removeItemById(cId);
			};

			Comment cComment = Comment(tr("Comment"));
			QPointF cPosition = position - QPointF(10.0, 10.0);

			std::function<void()> redo = [cScene, cId, cPosition, cComment]()
			{
				cScene->addItem(QSharedPointer<Item>(new CommentItem(cScene, cId, cScene->queryLevel(), cPosition, cComment)));
			};

			return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
		}
	}

	return QVector<UndoRedoEvent>();
}

void CommentItem::createItem(ItemScene* scene, const QPointF& position)
{
	if (scene)
	{
		scene->change(createItemEvent(scene, position));
	}
}

QVector<UndoRedoEvent> CommentItem::changeCommentEvent(const Comment& newComment)
{
	if (this->scene)
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;
		Comment cComment = this->comment;

		std::function<void()> undo = [cScene, cId, cComment]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<CommentItem>(item))
			{
				item.dynamicCast<CommentItem>()->setComment(cComment);
			}
		};

		cComment = newComment;

		std::function<void()> redo = [cScene, cId, cComment]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<CommentItem>(item))
			{
				item.dynamicCast<CommentItem>()->setComment(cComment);
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

void CommentItem::changeComment(const Comment& newComment)
{
	if (this->scene)
	{
		this->scene->change(this->changeCommentEvent(newComment));
	}
}

const Comment& CommentItem::getComment() const
{
	return this->comment;
}

void CommentItem::edit()
{
	if (this->scene)
	{
		CommentEditor editor(this->scene->getWidget(), this->comment);

		if (editor.exec())
		{
			Comment newComment = editor.getComment();

			if (this->comment != newComment)
			{
				this->changeComment(newComment);
			}
		}
	}
}

QPolygonF CommentItem::outline() const
{
	qreal padding = Settings::getBorderWidth() * 2.5;

	QPolygonF polygon;
	polygon.append(QPointF(this->rect.left(), this->rect.top()));
	polygon.append(QPointF(this->rect.right() - padding * 2.0, this->rect.top()));
	polygon.append(QPointF(this->rect.right(), this->rect.top() + padding * 2.0));
	polygon.append(QPointF(this->rect.right(), this->rect.bottom()));
	polygon.append(QPointF(this->rect.left(), this->rect.bottom()));
	polygon.append(QPointF(this->rect.left(), this->rect.top()));

	return polygon;
}

void CommentItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	qreal padding = Settings::getBorderWidth() * 2.5;

	QPen pen;
	pen.setWidth(Settings::getBorderWidth());
	pen.setColor(Settings::getColor());
	pen.setJoinStyle(Qt::PenJoinStyle::SvgMiterJoin);

	painter->setPen(pen);

	QPolygonF polygon;
	polygon.append(QPointF(this->rect.left(), this->rect.top()));
	polygon.append(QPointF(this->rect.right() - padding * 2.0, this->rect.top()));
	polygon.append(QPointF(this->rect.right(), this->rect.top() + padding * 2.0));
	polygon.append(QPointF(this->rect.right(), this->rect.bottom()));
	polygon.append(QPointF(this->rect.left(), this->rect.bottom()));
	polygon.append(QPointF(this->rect.left(), this->rect.top()));

	QPainterPath path;
	path.addPolygon(polygon);

	painter->fillPath(path, this->getBackgroundColor());
	painter->drawPath(path);

	polygon.clear();

	polygon.append(QPointF(this->rect.right() - padding * 2.2, this->rect.top()));
	polygon.append(QPointF(this->rect.right() - padding * 2.2, this->rect.top() + padding * 2.2));
	polygon.append(QPointF(this->rect.right(), this->rect.top() + padding * 2.2));

	path = QPainterPath();
	path.addPolygon(polygon);

	painter->drawPath(path);

	painter->translate(0.0, padding);

	painter->translate(padding, padding);

	document.drawContents(painter);
}

void CommentItem::appendXml(QDomElement parentElement)
{
	this->comment.appendXml(Xml::appendObject<Comment>(parentElement), this->id, this->getLevel(), this->getPosition());
}

void CommentItem::updateGeometry()
{
	qreal padding = Settings::getBorderWidth() * 2.5;

	QString html = "<font " + Settings::getFontHtmlStyle() + " " + Settings::getColorHtmlStyle() + ">";

	html += this->comment.getContent().toHtmlEscaped().replace("\n", "<br>");

	html += "</font>";

	this->document.setHtml(html);

	QSizeF size = this->document.documentLayout()->documentSize();

	this->rect.setRect(0.0, 0.0, size.width() + padding * 3.0, size.height() + padding * 3.0);

	ConnectableItem::updateGeometry();
}

void CommentItem::setComment(const Comment& comment)
{
	this->comment = comment;

	this->updateItem();
}
