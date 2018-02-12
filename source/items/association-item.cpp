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

#include "association-item.hpp"

AssociationItem::AssociationItem(ItemScene* scene, const Identifier& id, quint64 level, const Association& association, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes, bool perpendicular) : ConnectionItem(scene, id, level, association, beginNode, endNode, nodes, perpendicular), association(association), offsetXSideA(0.0), offsetXSideB(0.0)
{
	this->setEditable(true);
}

AssociationItem::AssociationItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position, const Association::Type& type) : ConnectionItem(scene, id, level, position, true), offsetXSideA(0.0), offsetXSideB(0.0)
{
	this->setEditable(true);

	this->association.setType(type);
}

QVector<UndoRedoEvent> AssociationItem::loadItems(ItemScene* scene, QDomElement element)
{
	QVector<UndoRedoEvent> events;

	if (scene)
	{
		QVector<Association> associations = Xml::loadObjects<Association>(element);

		for (auto& association : associations)
		{
			ItemScene* cScene = scene;
			Identifier cId = scene->newIdentifier(association.getRequestedId());
			Association cAssociation = association;

			if (cId)
			{
				std::function<void()> undo = [cScene, cId]()
				{
					cScene->removeItemById(cId);
				};

				std::function<void()> redo = [cScene, cId, cAssociation]()
				{
					cScene->addItem(QSharedPointer<Item>(new AssociationItem(cScene, cId, cAssociation.getRequestedLevel(), cAssociation, cAssociation.getRequestedBeginNode(), cAssociation.getRequestedEndNode(), cAssociation.getRequestedNodes(), cAssociation.isRequestedPerpendicular())));
				};

				events.append(UndoRedoEvent(undo, redo));
			}
		}
	}

	return events;
}

void AssociationItem::createItem(ItemScene* scene, const QPointF& position)
{
	if (scene)
	{
		Identifier id = scene->newIdentifier();

		if (id)
		{
			scene->addItem(QSharedPointer<Item>(new AssociationItem(scene, id, scene->queryLevel(), position)));
		}
	}
}

QVector<UndoRedoEvent> AssociationItem::changeAssociationEvent(const Association& newAssociation)
{
	if (this->scene)
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;
		Association cAssociation = this->association;

		std::function<void()> undo = [cScene, cId, cAssociation]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<AssociationItem>(item))
			{
				item.dynamicCast<AssociationItem>()->setAssociation(cAssociation);
			}
		};

		cAssociation = newAssociation;

		std::function<void()> redo = [cScene, cId, cAssociation]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<AssociationItem>(item))
			{
				item.dynamicCast<AssociationItem>()->setAssociation(cAssociation);
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

void AssociationItem::changeAssociation(const Association& newAssociation)
{
	if (this->scene)
	{
		this->scene->change(this->changeAssociationEvent(newAssociation));
	}
}

void AssociationItem::edit()
{
	if (this->scene)
	{
		AssociationEditor editor(this->scene->getWidget(), this->association);

		if (editor.exec())
		{
			Association newAssociation = editor.getAssociation();

			if (this->association != newAssociation)
			{
				this->changeAssociation(newAssociation);
			}
		}
	}
}

QPainterPath AssociationItem::shape() const
{
	QPainterPath path;

	QPainterPath pathRectName;
	pathRectName.addRect(this->rectName);

	path = path.united(pathRectName);

	QPainterPath pathDirectionArrow;
	pathDirectionArrow.addPolygon(this->directionArrow);

	path = path.united(pathDirectionArrow);

	QPainterPath pathArrowSideAFirst;
	pathArrowSideAFirst.addPolygon(this->arrowSideA.first);

	path = path.united(pathArrowSideAFirst);

	QPainterPath pathArrowSideASecond;
	pathArrowSideASecond.addPolygon(this->arrowSideA.second);

	path = path.united(pathArrowSideASecond);

	QPainterPath pathArrowSideBFirst;
	pathArrowSideBFirst.addPolygon(this->arrowSideB.first);

	path = path.united(pathArrowSideBFirst);

	QPainterPath pathArrowSideBSecond;
	pathArrowSideBSecond.addPolygon(this->arrowSideB.second);

	path = path.united(pathArrowSideBSecond);

	QPainterPath pathRectSideA;
	pathRectSideA.addRect(this->rectSideA);

	path = path.united(pathRectSideA);

	QPainterPath pathRectSideB;
	pathRectSideB.addRect(this->rectSideB);

	path = path.united(pathRectSideB);

	QPainterPath pathRectMultiplicitySideA;
	pathRectMultiplicitySideA.addRect(this->rectMultiplicitySideA);

	path = path.united(pathRectMultiplicitySideA);

	QPainterPath pathRectMultiplicitySideB;
	pathRectMultiplicitySideB.addRect(this->rectMultiplicitySideB);

	path = path.united(pathRectMultiplicitySideB);

	path = path.united(ConnectionItem::shape());

	return path;
}

void AssociationItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QPen pen;
	pen.setWidth(Settings::getBorderWidth());
	pen.setColor(Settings::getColor());

	painter->setPen(pen);
	painter->setRenderHints(QPainter::RenderHint::Antialiasing | QPainter::RenderHint::SmoothPixmapTransform | QPainter::RenderHint::HighQualityAntialiasing);

	QPolygonF nodes = this->getNodes();

	QPainterPath path;
	path.addPolygon(nodes);

	painter->drawPath(path);

	pen = QPen();
	pen.setWidth(Settings::getBorderWidth());
	pen.setColor(Settings::getColor());
	pen.setJoinStyle(Qt::PenJoinStyle::SvgMiterJoin);

	painter->setPen(pen);

	if (this->association.isShowDirection())
	{
		path = QPainterPath();
		path.addPolygon(this->directionArrow);

		painter->fillPath(path, Settings::getColor());

		painter->drawPath(path);
	}

	if (this->association.getType() != Association::Type::Default || this->association.getNavigabilitySideA() != Navigability::Undefined)
	{
		path = QPainterPath();
		path.addPolygon(this->arrowSideA.first);
		path.addPolygon(this->arrowSideA.second);

		if (this->association.getType() == Association::Type::Aggregation)
		{
			painter->fillPath(path, this->getBackgroundColor());
		}
		else if (this->association.getType() == Association::Type::Composition)
		{
			painter->fillPath(path, Settings::getColor());
		}

		painter->drawPath(path);
	}

	if (this->association.getNavigabilitySideB() != Navigability::Undefined)
	{
		path = QPainterPath();
		path.addPolygon(this->arrowSideB.first);
		path.addPolygon(this->arrowSideB.second);

		painter->drawPath(path);
	}

	painter->translate(this->rectName.left(), this->rectName.top());

	this->documentName.drawContents(painter);

	painter->translate(-this->rectName.left(), -this->rectName.top());

	painter->translate(this->rectSideA.left(), this->rectSideA.top());

	this->documentSideA.drawContents(painter);

	painter->translate(-this->rectSideA.left(), -this->rectSideA.top());

	painter->translate(this->rectSideB.left(), this->rectSideB.top());

	this->documentSideB.drawContents(painter);

	painter->translate(-this->rectSideB.left(), -this->rectSideB.top());

	painter->translate(this->rectMultiplicitySideA.left(), this->rectMultiplicitySideA.top());

	this->documentMultiplicitySideA.drawContents(painter);

	painter->translate(-this->rectMultiplicitySideA.left(), -this->rectMultiplicitySideA.top());

	painter->translate(this->rectMultiplicitySideB.left(), this->rectMultiplicitySideB.top());

	this->documentMultiplicitySideB.drawContents(painter);
}

void AssociationItem::appendXml(QDomElement parentElement, EndNode& beginNode, EndNode& endNode, const QPolygonF& nodes)
{
	this->association.appendXml(Xml::appendObject<Association>(parentElement), this->id, this->getLevel(), this->isPerpendicular(), beginNode, endNode, nodes);
}

UndoRedoEvent AssociationItem::anchor(ItemScene* scene, const Identifier& id, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes)
{
	ItemScene* cScene = scene;
	Identifier cId = id;
	Association cAssociation = Association();
	cAssociation.setType(this->association.getType());
	EndNode cBeginNode = beginNode;
	EndNode cEndNode = endNode;
	QPolygonF cNodes = nodes;

	std::function<void()> undo = [cScene, cId]()
	{
		cScene->removeItemById(cId);
	};

	std::function<void()> redo = [cScene, cId, cAssociation, cBeginNode, cEndNode, cNodes]()
	{
		cScene->addItem(QSharedPointer<Item>(new AssociationItem(cScene, cId, cScene->queryLevel(), cAssociation, cBeginNode, cEndNode, cNodes, true)));
	};

	return UndoRedoEvent(undo, redo);
}

void AssociationItem::updateGeometry()
{
	ConnectionItem::updateGeometry();

	this->calculateDirectionArrow();

	this->calculateArrows();

	this->calculateSides();
}

void AssociationItem::calculateDirectionArrow()
{
	qreal padding = Settings::getBorderWidth();

	this->directionArrow.clear();

	QPolygonF nodes = this->getNodes();

	if (nodes.size() >= 2)
	{
		QPointF first = nodes[nodes.size() / 2 - 1];
		QPointF second = nodes[nodes.size() / 2];

		QPointF position;

		QPointF direction = second - first;

		this->documentName.setHtml("<font " + Settings::getFontHtmlStyle() + " " + Settings::getColorHtmlStyle() + ">" + this->association.getName().toHtmlEscaped() + "</font>");

		QSizeF size = this->documentName.documentLayout()->documentSize();

		if (first.y() == second.y())
		{
			position = (first + second) / 2.0;

			position.setX(position.x() - size.width() / 2.0);
			position.setY(position.y() - size.height());

			position.setY(position.y() - padding);

			this->rectName = QRectF(position.x() + (this->association.isShowDirection() ? padding * 4.0 : 0.0), position.y(), size.width(), size.height());
		}
		else if ((direction.x() < 0.0 && direction.y() > 0.0) || (direction.x() > 0.0 && direction.y() < 0.0))
		{
			position = (first + second) / 2.0;

			position = position + qSqrt(size.height() * size.height()) * QPointF(-qAbs(direction.y()), -qAbs(direction.x())) / Math::length(direction);

			position.setY(position.y() - padding);

			this->rectName = QRectF(position.x() - (this->association.isShowDirection() ? padding * 4.0 : -padding * 4.0) - size.width(), position.y(), size.width(), size.height());
		}
		else
		{
			position = (first + second) / 2.0;

			position = position + qSqrt(size.height() * size.height()) * QPointF(qAbs(direction.y()), -qAbs(direction.x())) / Math::length(direction);

			position.setY(position.y() - padding);

			this->rectName = QRectF(position.x() + (this->association.isShowDirection() ? padding * 4.0 : -padding * 4.0), position.y(), size.width(), size.height());
		}

		if (direction.x() == 0.0)
		{
			direction = nodes.last() - nodes.first();
		}

		if (direction.x() == 0.0)
		{
			direction = QPointF(-1.0, 0.0);
		}

		direction.setY(0.0);

		position.setY(position.y() + size.height() / 2.0);

		if (Math::length(direction) > 0.0)
		{
			direction = Math::normalize(direction);

			QPointF orthogonal = Math::orthogonal(direction);

			position = position - padding * 2.0 * direction;

			this->directionArrow << position << (position + direction * padding * 4.0 + orthogonal * padding * 2.0) << (position + direction * padding * 4.0 - orthogonal * padding * 2.0) << position;
		}
	}
}

QPointF AssociationItem::calculateTextPosition(const QPointF& first, const QPointF& second, const QPointF& offset, const QSizeF& size, bool under)
{
	QPointF direction = second - first;

	if (Math::length(direction) > 0.0)
	{
		QPointF textOffset;

		if (qAbs(direction.x()) > qAbs(direction.y()))
		{
			if (direction.x() < 0.0)
			{
				textOffset.setX(-offset.x() - size.width());
			}
			else
			{
				textOffset.setX(offset.x());
			}

			if (under)
			{
				textOffset.setY(offset.y());
			}
			else
			{
				textOffset.setY(-offset.y() - size.height());
			}
		}
		else
		{
			if (under)
			{
				textOffset.setX(-offset.y() - size.width());
			}
			else
			{
				textOffset.setX(offset.y());
			}

			if (direction.y() < 0.0)
			{
				textOffset.setY(-offset.x() - size.height());
			}
			else
			{
				textOffset.setY(offset.x());
			}
		}

		return first + textOffset;
	}

	return QPointF();
}

void AssociationItem::calculateArrows()
{
	qreal size = Settings::getBorderWidth() * 5.0;

	this->arrowSideA.first.clear();
	this->arrowSideA.second.clear();

	this->offsetXSideA = 0.0;

	this->arrowSideB.first.clear();
	this->arrowSideB.second.clear();

	this->offsetXSideB = 0.0;

	QPolygonF nodes = this->getNodes();

	if (nodes.size() >= 2)
	{
		QPointF first = nodes[nodes.size() - 1];
		QPointF second = nodes[nodes.size() - 2];

		QPointF direction = second - first;

		if (Math::length(direction) > 0.0)
		{
			direction = Math::normalize(direction);

			QPointF orthogonal = Math::orthogonal(direction);

			if (this->association.getType() == Association::Type::Aggregation || this->association.getType() == Association::Type::Composition)
			{
				this->arrowSideA.first << first << (first + direction * size * 2.0 + orthogonal * size) << (first + direction * size * 4.0) << (first + direction * size * 2.0 - orthogonal * size) << first;

				this->offsetXSideA = size * 4.0;
			}
			else
			{
				if (this->association.getNavigabilitySideA() == Navigability::Navigable)
				{
					this->arrowSideA.first << (first + direction * size * 2.0 + orthogonal * size) << first << (first + direction * size * 2.0 - orthogonal * size);

					this->offsetXSideA = size * 2.0;
				}
				else if (this->association.getNavigabilitySideA() == Navigability::NotNavigable)
				{
					this->arrowSideA.first << (first + direction * size + orthogonal * size) << (first + direction * size * 2.0) << (first + direction * size - orthogonal * size);
					this->arrowSideA.second << (first + direction * size * 3.0 + orthogonal * size) << (first + direction * size * 2.0) << (first + direction * size * 3.0 - orthogonal * size);

					this->offsetXSideA = size * 3.0;
				}
			}
		}
	}

	if (nodes.size() >= 2)
	{
		QPointF first = nodes[0];
		QPointF second = nodes[1];

		QPointF direction = second - first;

		if (Math::length(direction) > 0.0)
		{
			direction = Math::normalize(direction);

			QPointF orthogonal = Math::orthogonal(direction);

			if (this->association.getNavigabilitySideB() == Navigability::Navigable)
			{
				this->arrowSideB.first << (first + direction * size * 2.0 + orthogonal * size) << first << (first + direction * size * 2.0 - orthogonal * size);

				this->offsetXSideB = size * 2.0;
			}
			else if (this->association.getNavigabilitySideB() == Navigability::NotNavigable)
			{
				this->arrowSideB.first << (first + direction * size + orthogonal * size) << (first + direction * size * 2.0) << (first + direction * size - orthogonal * size);
				this->arrowSideB.second << (first + direction * size * 3.0 + orthogonal * size) << (first + direction * size * 2.0) << (first + direction * size * 3.0 - orthogonal * size);

				this->offsetXSideB = size * 3.0;
			}
		}
	}
}

void AssociationItem::calculateSides()
{
	qreal padding = Settings::getBorderWidth();

	QPolygonF nodes = this->getNodes();

	if (nodes.size() >= 2)
	{
		QString html = "<font " + Settings::getFontHtmlStyle() + " " + Settings::getColorHtmlStyle() + ">";

		html += this->association.getVisibilitySideA().toSymbol().toHtmlEscaped();

		switch (this->association.getVisibilitySideA())
		{
		case Visibility::Public:
		{
			html += "<font " + Settings::getColorHtmlStyle(Colors::VisibilityPublic) + ">";

			break;
		}
		case Visibility::Protected:
		{
			html += "<font " + Settings::getColorHtmlStyle(Colors::VisibilityProtected) + ">";

			break;
		}
		case Visibility::Private:
		{
			html += "<font " + Settings::getColorHtmlStyle(Colors::VisibilityPrivate) + ">";

			break;
		}
		case Visibility::Package:
		{
			html += "<font " + Settings::getColorHtmlStyle(Colors::VisibilityPackage) + ">";

			break;
		}
		default:
		{
			html += "<font " + Settings::getColorHtmlStyle() + ">";

			break;
		}
		}

		html += " " + this->association.getRoleSideA().toHtmlEscaped();

		html += "</font></font>";

		this->documentSideA.setHtml(html);

		QSizeF sizeSideA = this->documentSideA.documentLayout()->documentSize();

		QPointF positionSideA = this->calculateTextPosition(nodes[nodes.size() - 1], nodes[nodes.size() - 2], QPointF(padding * 2.0 + this->offsetXSideA, padding), sizeSideA);

		this->rectSideA.setRect(positionSideA.x(), positionSideA.y(), sizeSideA.width(), sizeSideA.height());

		this->documentMultiplicitySideA.setHtml("<font " + Settings::getFontHtmlStyle() + " " + Settings::getColorHtmlStyle(Colors::Type) + ">" + this->association.getMultiplicitySideA().toHtmlEscaped() + "</font>");

		QSizeF sizeMultiplicitySideA = this->documentMultiplicitySideA.documentLayout()->documentSize();

		QPointF positionMultiplicitySideA = this->calculateTextPosition(nodes[nodes.size() - 1], nodes[nodes.size() - 2], QPointF(padding * 2.0 + this->offsetXSideA, padding), sizeMultiplicitySideA, true);

		this->rectMultiplicitySideA.setRect(positionMultiplicitySideA.x(), positionMultiplicitySideA.y(), sizeMultiplicitySideA.width(), sizeMultiplicitySideA.height());

		html = "<font " + Settings::getFontHtmlStyle() + " " + Settings::getColorHtmlStyle() + ">";

		html += this->association.getVisibilitySideB().toSymbol().toHtmlEscaped();

		switch (this->association.getVisibilitySideB())
		{
		case Visibility::Public:
		{
			html += "<font " + Settings::getColorHtmlStyle(Colors::VisibilityPublic) + ">";

			break;
		}
		case Visibility::Protected:
		{
			html += "<font " + Settings::getColorHtmlStyle(Colors::VisibilityProtected) + ">";

			break;
		}
		case Visibility::Private:
		{
			html += "<font " + Settings::getColorHtmlStyle(Colors::VisibilityPrivate) + ">";

			break;
		}
		case Visibility::Package:
		{
			html += "<font " + Settings::getColorHtmlStyle(Colors::VisibilityPackage) + ">";

			break;
		}
		default:
		{
			html += "<font " + Settings::getColorHtmlStyle() + ">";

			break;
		}
		}

		html += " " + this->association.getRoleSideB().toHtmlEscaped();

		html += "</font></font>";

		this->documentSideB.setHtml(html);

		QSizeF sizeSideB = this->documentSideB.documentLayout()->documentSize();

		QPointF positionSideB = this->calculateTextPosition(nodes[0], nodes[1], QPointF(padding * 2.0 + this->offsetXSideB, padding), sizeSideB);

		this->rectSideB.setRect(positionSideB.x(), positionSideB.y(), sizeSideB.width(), sizeSideB.height());

		this->documentMultiplicitySideB.setHtml("<font " + Settings::getFontHtmlStyle() + " " + Settings::getColorHtmlStyle(Colors::Type) + ">" + this->association.getMultiplicitySideB().toHtmlEscaped() + "</font>");

		QSizeF sizeMultiplicitySideB = this->documentMultiplicitySideB.documentLayout()->documentSize();

		QPointF positionMultiplicitySideB = this->calculateTextPosition(nodes[0], nodes[1], QPointF(padding * 2.0 + this->offsetXSideB, padding), sizeMultiplicitySideB, true);

		this->rectMultiplicitySideB.setRect(positionMultiplicitySideB.x(), positionMultiplicitySideB.y(), sizeMultiplicitySideB.width(), sizeMultiplicitySideB.height());
	}
}

void AssociationItem::setAssociation(const Association& association)
{
	this->association = association;

	this->updateItem();
}
