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

#include "class-item.hpp"

ClassItem::ClassItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position, const Class& c) : ConnectableItem(scene, id, level, position)
{
	this->setClass(c);
}

QVector<UndoRedoEvent> ClassItem::loadItems(ItemScene* scene, QDomElement element)
{
	QVector<UndoRedoEvent> events;

	if (scene)
	{
		QVector<Class> classes = Xml::loadObjects<Class>(element);

		for (auto& c : classes)
		{
			ItemScene* cScene = scene;
			Identifier cId = scene->newIdentifier(c.getRequestedId());
			Class cClass = c;

			if (cId)
			{
				std::function<void()> undo = [cScene, cId]()
				{
					cScene->removeItemById(cId);
				};

				std::function<void()> redo = [cScene, cId, cClass]()
				{
					cScene->addItem(QSharedPointer<Item>(new ClassItem(cScene, cId, cClass.getRequestedLevel(), cClass.getRequestedPosition(), cClass)));
				};

				events.append(UndoRedoEvent(undo, redo));
			}
		}
	}

	return events;
}

QVector<UndoRedoEvent> ClassItem::createItemEvent(ItemScene* scene, const QPointF& position)
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

			Class cClass = Class(tr("Class"), "", false, {}, {});
			QPointF cPosition = position - QPointF(10.0, 10.0);

			std::function<void()> redo = [cScene, cId, cPosition, cClass]()
			{
				cScene->addItem(QSharedPointer<Item>(new ClassItem(cScene, cId, cScene->queryLevel(), cPosition, cClass)));
			};

			return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
		}
	}

	return QVector<UndoRedoEvent>();
}

void ClassItem::createItem(ItemScene* scene, const QPointF& position)
{
	if (scene)
	{
		scene->change(createItemEvent(scene, position));
	}
}

QVector<UndoRedoEvent> ClassItem::changeClassEvent(const Class& newClass)
{
	if (this->scene)
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;
		Class cClass = this->c;

		std::function<void()> undo = [cScene, cId, cClass]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ClassItem>(item))
			{
				item.dynamicCast<ClassItem>()->setClass(cClass);
			}
		};

		cClass = newClass;

		std::function<void()> redo = [cScene, cId, cClass]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ClassItem>(item))
			{
				item.dynamicCast<ClassItem>()->setClass(cClass);
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

void ClassItem::changeClass(const Class& newClass)
{
	if (this->scene)
	{
		this->scene->change(this->changeClassEvent(newClass));
	}
}

const Class& ClassItem::getClass() const
{
	return this->c;
}

void ClassItem::edit()
{
	if (this->scene)
	{
		ClassEditor editor(this->scene->getWidget(), this->c);

		if (editor.exec())
		{
			Class newClass = editor.getClass();

			if (this->c != newClass)
			{
				this->changeClass(newClass);
			}
		}
	}
}

QPolygonF ClassItem::outline() const
{
	QPolygonF polygon;
	polygon.append(QPointF(this->rect.left(), this->rect.top()));
	polygon.append(QPointF(this->rect.right(), this->rect.top()));
	polygon.append(QPointF(this->rect.right(), this->rect.bottom()));
	polygon.append(QPointF(this->rect.left(), this->rect.bottom()));
	polygon.append(QPointF(this->rect.left(), this->rect.top()));

	return polygon;
}

void ClassItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	qreal padding = Settings::getBorderWidth() * 2.5;

	QColor color = this->getBackgroundColor();

	QPen pen;
	pen.setWidth(Settings::getBorderWidth());
	pen.setColor(Settings::getColor());
	pen.setJoinStyle(Qt::PenJoinStyle::SvgMiterJoin);

	painter->setPen(pen);

	painter->fillRect(this->rectHead, color);
	painter->drawRect(this->rectHead);
	
	painter->translate(padding, padding);

	this->documentHead.setTextWidth(this->rectHead.width() - padding * 2.0);
	this->documentHead.drawContents(painter);

	painter->translate(-padding, this->rectHead.height() - padding);

	painter->fillRect(this->rectAttributes, color);
	painter->drawRect(this->rectAttributes);

	painter->translate(padding, padding);

	this->documentAttributes.drawContents(painter);

	painter->translate(-padding, this->rectAttributes.height() - padding);

	painter->fillRect(this->rectMethods, color);
	painter->drawRect(this->rectMethods);

	painter->translate(padding, padding);
	
	this->documentMethods.drawContents(painter);
}

void ClassItem::appendXml(QDomElement parentElement)
{
	this->c.appendXml(Xml::appendObject<Class>(parentElement), this->id, this->getLevel(), this->getPosition());
}

void ClassItem::updateGeometry()
{
	qreal padding = Settings::getBorderWidth() * 2.5;

	QString html = "<font " + Settings::getColorHtmlStyle() + "><center>";

	if (this->c.getKeyword() != "")
	{
		html += "<font " + Settings::getFontHtmlStyle() + " " + Settings::getColorHtmlStyle(Colors::Value) + ">&lt;&lt;" + this->c.getKeyword().toHtmlEscaped() + "&gt;&gt;</font><br>";
	}

	if (this->c.isAbstract())
	{
		html += "<i><font " + Settings::getFontHtmlStyle(true) + ">" + this->c.getName().toHtmlEscaped() + "</font></i>";

		html += "<br><font " + Settings::getFontHtmlStyle() + " " + Settings::getColorHtmlStyle(Colors::Type) + ">{abstract}</font>";
	}
	else
	{
		html += "<font " + Settings::getFontHtmlStyle(true) + ">" + this->c.getName().toHtmlEscaped() + "</font>";
	}

	html += "</center></font>";

	this->documentHead.setHtml(html);

	QTextDocument documentBuff;

	documentBuff.setHtml(html);

	QSizeF sizeHead = documentBuff.documentLayout()->documentSize();

	html = "<font " + Settings::getFontHtmlStyle() + " " + Settings::getColorHtmlStyle() + "><table>";

	for (int i = 0; i < this->c.getAttributeCount(); i++)
	{
		html += "<tr><td><center>";
		html += this->c.getAttributeAt(i).getVisibility().toSymbol().toHtmlEscaped();
		html += "</center></td><td>&nbsp;";

		if (this->c.getAttributeAt(i).isClassScope())
		{
			html += "<u>";
		}

		switch (this->c.getAttributeAt(i).getVisibility())
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

		html += this->c.getAttributeAt(i).getName().toHtmlEscaped() + "</font>";

		if (this->c.getAttributeAt(i).getType() != "")
		{
			html += " : ";
			html += "<font " + Settings::getColorHtmlStyle(Colors::Type) + ">" + this->c.getAttributeAt(i).getType().toHtmlEscaped() + "</font>";
		}

		if (this->c.getAttributeAt(i).getMultiplicity() != "")
		{
			html += " <font " + Settings::getColorHtmlStyle(Colors::Type) + ">[" + this->c.getAttributeAt(i).getMultiplicity().toHtmlEscaped() + "]</font>";
		}

		if (this->c.getAttributeAt(i).getDefaultValue() != "")
		{
			html += " = ";
			html += "<font " + Settings::getColorHtmlStyle(Colors::Value) + ">" + this->c.getAttributeAt(i).getDefaultValue().toHtmlEscaped() + "</font>";
		}

		if (this->c.getAttributeAt(i).isClassScope())
		{
			html += "</u>";
		}

		html += "</td></tr>";
	}

	html += "</table></font>";

	this->documentAttributes.setHtml(html);

	QSizeF sizeAttributes = this->documentAttributes.documentLayout()->documentSize();

	html = "<font " + Settings::getFontHtmlStyle() + " " + Settings::getColorHtmlStyle() + "><table>";

	for (int i = 0; i < this->c.getMethodCount(); i++)
	{
		html += "<tr><td><center>";
		html += this->c.getMethodAt(i).getVisibility().toSymbol().toHtmlEscaped();
		html += "</center></td><td>&nbsp;";

		if (this->c.getMethodAt(i).isAbstract())
		{
			html += "<i>";
		}

		if (this->c.getMethodAt(i).isClassScope())
		{
			html += "<u>";
		}

		switch (this->c.getMethodAt(i).getVisibility())
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

		html += this->c.getMethodAt(i).getName().toHtmlEscaped() + "</font>";

		html += "(";

		for (int j = 0; j < this->c.getMethodAt(i).getParameterCount(); j++)
		{
			html += this->c.getMethodAt(i).getParameterAt(j).getName().toHtmlEscaped();

			if (this->c.getMethodAt(i).getParameterAt(j).getType() != "")
			{
				html += " : ";
				html += "<font " + Settings::getColorHtmlStyle(Colors::Type) + ">" + this->c.getMethodAt(i).getParameterAt(j).getType().toHtmlEscaped() + "</font>";
			}

			if (this->c.getMethodAt(i).getParameterAt(j).getMultiplicity() != "")
			{
				html += " <font " + Settings::getColorHtmlStyle(Colors::Type) + ">[" + this->c.getMethodAt(i).getParameterAt(j).getMultiplicity().toHtmlEscaped() + "]</font>";
			}

			if (this->c.getMethodAt(i).getParameterAt(j).getDefaultValue() != "")
			{
				html += " = ";
				html += "<font " + Settings::getColorHtmlStyle(Colors::Value) + ">" + this->c.getMethodAt(i).getParameterAt(j).getDefaultValue().toHtmlEscaped() + "</font>";
			}

			html += ((j < this->c.getMethodAt(i).getParameterCount() - 1) ? ", " : "");
		}

		html += ")";

		if (this->c.getMethodAt(i).getReturnType() != "")
		{
			html += " : ";
			html += "<font " + Settings::getColorHtmlStyle(Colors::Type) + ">" + this->c.getMethodAt(i).getReturnType().toHtmlEscaped() + "</font>";
		}

		if (this->c.getMethodAt(i).isClassScope())
		{
			html += "</u>";
		}

		if (this->c.getMethodAt(i).isAbstract())
		{
			html += "</i>";
		}

		html += "</td></tr>";
	}

	html += "</table></font>";

	this->documentMethods.setHtml(html);

	QSizeF sizeMethods = this->documentMethods.documentLayout()->documentSize();

	qreal width = qMax(sizeHead.width(), qMax(sizeAttributes.width(), sizeMethods.width())) + padding * 2.0;

	this->rectHead.setRect(0.0, 0.0, width, sizeHead.height() + padding * 2.0);

	this->rectAttributes.setRect(0.0, 0.0, width, sizeAttributes.height() + padding * 2.0);

	this->rectMethods.setRect(0.0, 0.0, width, sizeMethods.height() + padding * 2.0);

	this->rect.setRect(0.0, 0.0, width, this->rectHead.height() + this->rectAttributes.height() + this->rectMethods.height());

	ConnectableItem::updateGeometry();
}

void ClassItem::setClass(const Class& c)
{
	this->c = c;

	this->updateItem();
}
