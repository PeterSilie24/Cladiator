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

#include "connection-item.hpp"

ConnectableItem::ConnectableItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position) : MovableItem(scene, id, level, position)
{
	
}

QVector<UndoRedoEvent> ConnectableItem::deleteItemEvent()
{
	QVector<UndoRedoEvent> events;

	if (this->scene)
	{
		for (auto& connectionItemId : this->connectionItemIds)
		{
			QSharedPointer<Item> item = this->scene->getItemById(connectionItemId);

			if (instanceof<ConnectionItem>(item))
			{
				events.append(item.dynamicCast<ConnectionItem>()->deleteItemEvent());
			}
		}
	}

	events.append(MovableItem::deleteItemEvent());

	return events;
}

void ConnectableItem::appendConnectionItem(const Identifier& connectionItemId)
{
	this->connectionItemIds.append(connectionItemId);
}

void ConnectableItem::removeConnectionItem(const Identifier& connectionItemId)
{
	for (int i = 0; i < this->connectionItemIds.size(); i++)
	{
		if (this->connectionItemIds[i] == connectionItemId)
		{
			this->connectionItemIds.removeAt(i);

			break;
		}
	}
}

void ConnectableItem::geometryChange()
{
	if (this->scene)
	{
		for (auto& connectionItemId : this->connectionItemIds)
		{
			QSharedPointer<Item> connectionItem = scene->getItemById(connectionItemId);

			if (instanceof<ConnectionItem>(connectionItem))
			{
				connectionItem.dynamicCast<ConnectionItem>()->updateItem();
			}
		}
	}

	MovableItem::geometryChange();
}

NodeItem::NodeItem(ItemScene* scene, const QPointF& position, ConnectionItem* connectionItem) : MovableItem(scene, Identifier(), ArrangeableItem::alwaysOnTop, position), connectionItem(connectionItem), paintable(false)
{
	this->setEditable(false);

	this->setCopyable(false);
}

QVector<UndoRedoEvent> NodeItem::deleteItemEvent()
{
	this->endMove();

	if (this->connectionItem)
	{
		return this->connectionItem->deleteNodeEvent(this->connectionItem->getNodeItemIndex(this));
	}

	return QVector<UndoRedoEvent>();
}

QVector<UndoRedoEvent> NodeItem::moveItemEvent(const QPointF& newPosition)
{
	if (this->scene && this->connectionItem)
	{
		ItemScene* cScene = this->scene;
		Identifier cItemId = this->connectionItem->getId();
		int cIndex = this->connectionItem->getNodeItemIndex(this);
		QPointF cPosition = this->getPosition();

		std::function<void()> undo = [cScene, cItemId, cIndex, cPosition]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cItemId);

			if (instanceof<ConnectionItem>(item))
			{
				QSharedPointer<NodeItem> nodeItem = item.dynamicCast<ConnectionItem>()->getNodeItemAt(cIndex);

				if (nodeItem)
				{
					nodeItem->setPosition(cPosition);
				}
			}
		};

		cPosition = newPosition;

		std::function<void()> redo = [cScene, cItemId, cIndex, cPosition]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cItemId);

			if (instanceof<ConnectionItem>(item))
			{
				QSharedPointer<NodeItem> nodeItem = item.dynamicCast<ConnectionItem>()->getNodeItemAt(cIndex);

				if (nodeItem)
				{
					nodeItem->setPosition(cPosition);
				}
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

QPainterPath NodeItem::shape() const
{
	qreal radius = Settings::getBorderWidth() * 2.5;

	QPainterPath path;
	path.addEllipse(QPointF(), radius, radius);

	return path;
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	qreal radius = Settings::getBorderWidth() * 2.5;

	if (this->paintable)
	{
		QPen pen;
		pen.setColor(Settings::getColor());
		pen.setWidth(Settings::getBorderWidth());

		painter->setPen(pen);
		painter->setRenderHints(QPainter::RenderHint::Antialiasing | QPainter::RenderHint::SmoothPixmapTransform | QPainter::RenderHint::HighQualityAntialiasing);

		QPainterPath path;

		if (this->isHovered() || this->isSelected())
		{
			path.addEllipse(QPointF(), radius, radius);
		}
		else
		{
			path.addEllipse(QPointF(), radius * 0.75, radius * 0.75);
		}

		painter->fillPath(path, this->getBackgroundColor());
		painter->drawPath(path);
	}
}

void NodeItem::setPaintable(bool paintable)
{
	this->paintable = paintable;

	this->update();
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == ItemPositionHasChanged)
	{
		if (this->connectionItem)
		{
			this->connectionItem->updateItem();
		}
	}
	else if (change == ItemSelectedHasChanged)
	{
		if (this->connectionItem)
		{
			this->connectionItem->setNodesPaintable(this->connectionItem->isAnyNodeSelected() ? true : value.toBool());
		}
	}

	return MovableItem::itemChange(change, value);
}

EndNodeItem::EndNodeItem(ItemScene* scene, const QPointF& position, ConnectionItem* connectionItem, const Identifier& connectedItemId) : NodeItem(scene, position, connectionItem), connectedItemId(connectedItemId), updatingPosition(false)
{
	if (scene && connectionItem && connectedItemId)
	{
		QSharedPointer<Item> connectedItem = scene->getItemById(connectedItemId);

		if (instanceof<ConnectableItem>(connectedItem))
		{
			connectedItem.dynamicCast<ConnectableItem>()->appendConnectionItem(connectionItem->getId());
		}
	}
}

EndNodeItem::~EndNodeItem()
{
	if (this->scene && this->connectionItem)
	{
		QSharedPointer<Item> connectedItem = this->scene->getItemById(this->connectedItemId);

		if (instanceof<ConnectableItem>(connectedItem))
		{
			connectedItem.dynamicCast<ConnectableItem>()->removeConnectionItem(this->connectionItem->getId());
		}
	}
}

QVector<UndoRedoEvent> EndNodeItem::deleteItemEvent()
{
	this->endMove();

	if (this->connectionItem)
	{
		this->setSelected(false);

		return this->connectionItem->deleteItemEvent();
	}

	return QVector<UndoRedoEvent>();
}

QVector<UndoRedoEvent> EndNodeItem::moveItemEvent(const QPointF& newPosition)
{
	QVector<UndoRedoEvent> events;

	if (this->connectionItem && this->scene)
	{
		QSharedPointer<Item> connectedItem = this->scene->getItemById(this->connectedItemId);

		if (instanceof<ConnectableItem>(connectedItem))
		{
			if (connectedItem.dynamicCast<ConnectableItem>()->isSelected())
			{
				return events;
			}
		}

		QSharedPointer<ConnectableItem> connectableItem = this->scene->getItemAtPosition<ConnectableItem>(newPosition);

		Identifier connectableItemId;

		if (connectableItem)
		{
			connectableItemId = connectableItem->getId();
		}

		if (this->connectionItem->isAnchored())
		{
			events.append(this->changeConnectedItemEvent(connectableItemId, newPosition));

			events.append(this->connectionItem->calculateNodesEvent());
		}
		else
		{
			EndNode beginNode;
			EndNode endNode;

			if (connectableItemId)
			{
				beginNode = EndNode(connectableItemId);
			}
			else
			{
				beginNode = EndNode(newPosition);
			}

			QSharedPointer<EndNodeItem> otherItem;

			if (this->connectionItem->isBeginNodeItem(this))
			{
				otherItem = this->connectionItem->getEndNodeItem();
			}
			else
			{
				otherItem = this->connectionItem->getBeginNodeItem();
			}

			if (otherItem)
			{
				endNode = otherItem->getEndNode();
			}

			this->setConnectedItem(connectableItemId, newPosition);

			events.append(this->connectionItem->anchorConnectionEvent(beginNode, endNode));

			events.append(this->connectionItem->calculateNodesEvent());
		}
	}

	return events;
}

QVector<UndoRedoEvent> EndNodeItem::changeConnectedItemEvent(const Identifier& newConnectedItemId, const QPointF& newPosition)
{
	if (this->scene && this->connectionItem)
	{
		ItemScene* cScene = this->scene;
		Identifier cItemId = this->connectionItem->getId();
		QPointF cPosition = this->getPosition();
		Identifier cConnectedItemId = this->connectedItemId;

		if (this->connectionItem->isBeginNodeItem(this))
		{
			std::function<void()> undo = [cScene, cItemId, cPosition, cConnectedItemId]()
			{
				QSharedPointer<Item> item = cScene->getItemById(cItemId);

				if (instanceof<ConnectionItem>(item))
				{
					QSharedPointer<EndNodeItem> beginNodeItem = item.dynamicCast<ConnectionItem>()->getBeginNodeItem();

					if (beginNodeItem)
					{
						beginNodeItem->setConnectedItem(cConnectedItemId, cPosition);
					}
				}
			};

			cPosition = newPosition;
			cConnectedItemId = newConnectedItemId;

			std::function<void()> redo = [cScene, cItemId, cPosition, cConnectedItemId]()
			{
				QSharedPointer<Item> item = cScene->getItemById(cItemId);

				if (instanceof<ConnectionItem>(item))
				{
					QSharedPointer<EndNodeItem> beginNodeItem = item.dynamicCast<ConnectionItem>()->getBeginNodeItem();

					if (beginNodeItem)
					{
						beginNodeItem->setConnectedItem(cConnectedItemId, cPosition);
					}
				}
			};

			return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
		}
		else if (this->connectionItem->isEndNodeItem(this))
		{
			std::function<void()> undo = [cScene, cItemId, cPosition, cConnectedItemId]()
			{
				QSharedPointer<Item> item = cScene->getItemById(cItemId);

				if (instanceof<ConnectionItem>(item))
				{
					QSharedPointer<EndNodeItem> endNodeItem = item.dynamicCast<ConnectionItem>()->getEndNodeItem();

					if (endNodeItem)
					{
						endNodeItem->setConnectedItem(cConnectedItemId, cPosition);
					}
				}
			};

			cPosition = newPosition;
			cConnectedItemId = newConnectedItemId;

			std::function<void()> redo = [cScene, cItemId, cPosition, cConnectedItemId]()
			{
				QSharedPointer<Item> item = cScene->getItemById(cItemId);

				if (instanceof<ConnectionItem>(item))
				{
					QSharedPointer<EndNodeItem> endNodeItem = item.dynamicCast<ConnectionItem>()->getEndNodeItem();

					if (endNodeItem)
					{
						endNodeItem->setConnectedItem(cConnectedItemId, cPosition);
					}
				}
			};

			return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
		}
	}

	return QVector<UndoRedoEvent>({});
}

void EndNodeItem::changeConnectedItem(const Identifier& newConnectedItemId, const QPointF& newPosition)
{
	if (this->scene)
	{
		this->scene->change(this->changeConnectedItemEvent(newConnectedItemId, newPosition));
	}
}

const Identifier& EndNodeItem::getConnectedItemId() const
{
	return this->connectedItemId;
}

EndNode EndNodeItem::getEndNode() const
{
	if (this->connectedItemId)
	{
		return EndNode(this->connectedItemId, this->getPosition());
	}
	else
	{
		return EndNode(this->getPosition());
	}
}

bool EndNodeItem::isConnectedItemSelected() const
{
	if (this->scene)
	{
		QSharedPointer<Item> connectedItem = this->scene->getItemById(this->connectedItemId);

		if (instanceof<SelectableItem>(connectedItem))
		{
			return connectedItem->isSelected();
		}
	}

	return false;
}

void EndNodeItem::updatePosition()
{
	if (!this->updatingPosition)
	{
		QPointF position;

		if (this->calculatePosition(position))
		{
			this->updatingPosition = true;

			this->setPosition(position);

			this->updatingPosition = false;
		}
	}
}

bool EndNodeItem::checkMoved(QVector<UndoRedoEvent>& events)
{
	bool result = NodeItem::checkMoved(events);

	if (this->connectionItem && !result)
	{
		if (this->connectionItem->isBeginNodeItem(this))
		{
			if (!this->connectionItem->isAnchored())
			{
				events.append(this->moveItemEvent(this->pos()));

				return true;
			}
		}
	}

	return result;
}

QVariant EndNodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == ItemPositionChange)
	{
		if (this->connectedItemId)
		{
			return value;
		}
	}
	else if (change == ItemPositionHasChanged)
	{
		if (this->connectionItem)
		{
			if (!this->updatingPosition)
			{
				this->connectionItem->updateItem();
			}

			MovableItem::itemChange(change, value);

			return value;
		}
	}

	return NodeItem::itemChange(change, value);
}

void EndNodeItem::setConnectedItem(const Identifier& newConnectedItemId, const QPointF& newPosition)
{
	if (this->scene && this->connectionItem)
	{
		QSharedPointer<Item> oldConnectedItem = this->scene->getItemById(this->connectedItemId);
		QSharedPointer<Item> newConnectedItem = this->scene->getItemById(newConnectedItemId);

		if (instanceof<ConnectableItem>(oldConnectedItem))
		{
			oldConnectedItem.dynamicCast<ConnectableItem>()->removeConnectionItem(this->connectionItem->getId());
		}

		if (instanceof<ConnectableItem>(newConnectedItem))
		{
			newConnectedItem.dynamicCast<ConnectableItem>()->appendConnectionItem(this->connectionItem->getId());
		}

		this->connectedItemId = newConnectedItemId;
	}

	this->setPosition(newPosition);

	if (this->connectionItem)
	{
		this->connectionItem->updateItem();
	}
}

bool EndNodeItem::calculatePosition(QPointF& position)
{
	if (this->scene && this->connectionItem && (!this->isMoving() || this->isConnectedItemSelected()))
	{
		QPointF otherPosition;
		QPointF thisPosition = this->pos();

		QSharedPointer<EndNodeItem> otherItem;

		QSharedPointer<NodeItem> nextNodeItem;

		if (this->connectionItem->isBeginNodeItem(this))
		{
			nextNodeItem = this->connectionItem->getFirstNodeItem();

			otherItem = this->connectionItem->getEndNodeItem();
		}
		else
		{
			nextNodeItem = this->connectionItem->getLastNodeItem();

			otherItem = this->connectionItem->getBeginNodeItem();
		}

		QSharedPointer<Item> otherConnectedItem;
		QSharedPointer<Item> thisConnectedItem = this->scene->getItemById(this->getConnectedItemId());

		bool otherMoving = false;
		bool otherConnectedItemSelected = false;

		if (otherItem)
		{
			otherConnectedItem = this->scene->getItemById(otherItem->getConnectedItemId());

			otherMoving = otherItem->isMoving();

			otherConnectedItemSelected = otherItem->isConnectedItemSelected();
		}

		if (nextNodeItem)
		{
			otherPosition = nextNodeItem->pos();
		}
		else if (otherConnectedItem && (!otherMoving || otherConnectedItemSelected))
		{
			QRectF rect = otherConnectedItem->outline().boundingRect();

			rect.moveTo(otherConnectedItem->pos());

			otherPosition.setX((rect.left() + rect.right()) / 2.0);
			otherPosition.setY((rect.top() + rect.bottom()) / 2.0);
		}
		else if (otherItem)
		{
			otherPosition = otherItem->pos();
		}

		if (thisConnectedItem)
		{
			QPolygonF outline = thisConnectedItem->outline();

			outline.translate(thisConnectedItem->pos());

			QRectF rect = outline.boundingRect();

			thisPosition.setX((rect.left() + rect.right()) / 2.0);
			thisPosition.setY((rect.top() + rect.bottom()) / 2.0);

			QPolygonF line;
			line << thisPosition << otherPosition;

			if (this->connectionItem->isPerpendicular())
			{
				if (nextNodeItem)
				{
					if (otherPosition.y() >= rect.top() && otherPosition.y() <= rect.bottom())
					{
						thisPosition.setY(otherPosition.y());
					}
					if (otherPosition.x() >= rect.left() && otherPosition.x() <= rect.right())
					{
						thisPosition.setX(otherPosition.x());
					}
				}
				else if (otherConnectedItem && (!otherMoving || otherConnectedItemSelected))
				{
					QRectF otherRect = otherConnectedItem->outline().boundingRect();

					otherRect.moveTo(otherConnectedItem->pos());

					QRectF thisRect = thisConnectedItem->outline().boundingRect();

					thisRect.moveTo(thisConnectedItem->pos());

					qreal left = qMax(otherRect.left(), thisRect.left());
					qreal top = qMax(otherRect.top(), thisRect.top());
					qreal right = qMin(otherRect.right(), thisRect.right());
					qreal bottom = qMin(otherRect.bottom(), thisRect.bottom());

					if (top <= bottom)
					{
						thisPosition.setY((top + bottom) / 2.0);
						otherPosition.setY((top + bottom) / 2.0);
					}
					if (left <= right)
					{
						thisPosition.setX((left + right) / 2.0);
						otherPosition.setX((left + right) / 2.0);
					}
				}

				if (outline.containsPoint(thisPosition, Qt::FillRule::OddEvenFill))
				{
					line.clear();
					line << thisPosition << otherPosition;
				}
			}

			QPolygonF intersections = Math::intersectionOfPolygons(outline, line);

			if (intersections.size())
			{
				position = intersections.first();
			}
			else
			{
				position = otherPosition;
			}

			return true;
		}
	}
	
	return false;
}

ConnectionItem::ConnectionItem(ItemScene* scene, const Identifier& id, quint64 level, const Connection& connection, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes, bool perpendicular) : ArrangeableItem(scene, id, level), actionNewNode(this), actionSwitchDirection(this), actionPerpendicularConnecting(this), connection(connection), anchored(true), perpendicular(perpendicular)
{
	this->setEditable(false);

	if (scene)
	{
		this->endNodeItem = QSharedPointer<EndNodeItem>(new EndNodeItem(scene, endNode.getPosition(), this, scene->queryId(endNode.getConnectedId())));

		scene->addItem(this->endNodeItem.dynamicCast<Item>());

		this->beginNodeItem = QSharedPointer<EndNodeItem>(new EndNodeItem(scene, beginNode.getPosition(), this, scene->queryId(beginNode.getConnectedId())));

		scene->addItem(this->beginNodeItem.dynamicCast<Item>());
	}

	for (auto& node : nodes)
	{
		this->newNodeItemAt(node, this->nodeItems.size());
	}

	this->actionNewNode.setText(tr("New Node"));
	this->actionNewNode.setIcon(Icons::loadIcon("list-add"));

	this->connect(&(this->actionNewNode), SIGNAL(triggered()), this, SLOT(actionNewNodeTriggered()));

	this->actionSwitchDirection.setText(tr("Switch Direction"));
	this->actionSwitchDirection.setIcon(Icons::loadIcon("circular-arrow-shape"));

	this->connect(&(this->actionSwitchDirection), SIGNAL(triggered()), this, SLOT(actionSwitchDirectionTriggered()));

	this->actionPerpendicularConnecting.setText(tr("Perpendicular Connecting"));
	this->actionPerpendicularConnecting.setCheckable(true);
	this->actionPerpendicularConnecting.setChecked(this->perpendicular);

	this->connect(&(this->actionPerpendicularConnecting), SIGNAL(toggled(bool)), this, SLOT(actionPerpendicularConnectingToggled(bool)));
}

ConnectionItem::ConnectionItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position, bool perpendicular) : ArrangeableItem(scene, id, level), actionNewNode(this), actionSwitchDirection(this), actionPerpendicularConnecting(this), anchored(false), perpendicular(perpendicular)
{
	this->setEditable(false);

	if (scene)
	{
		QSharedPointer<ConnectableItem> item = scene->getItemAtPosition<ConnectableItem>(position);

		Identifier connectedItemId;

		if (item)
		{
			connectedItemId = item->getId();
		}

		this->endNodeItem = QSharedPointer<EndNodeItem>(new EndNodeItem(scene, position, this, connectedItemId));

		scene->addItem(this->endNodeItem.dynamicCast<Item>());

		this->beginNodeItem = QSharedPointer<EndNodeItem>(new EndNodeItem(scene, position, this, connectedItemId));

		scene->addItem(this->beginNodeItem.dynamicCast<Item>());
	}

	this->actionNewNode.setText(tr("New Node"));
	this->actionNewNode.setIcon(Icons::loadIcon("list-add"));

	this->connect(&(this->actionNewNode), SIGNAL(triggered()), this, SLOT(actionNewNodeTriggered()));

	this->actionSwitchDirection.setText(tr("Switch Direction"));
	this->actionSwitchDirection.setIcon(Icons::loadIcon("circular-arrow-shape"));

	this->connect(&(this->actionSwitchDirection), SIGNAL(triggered()), this, SLOT(actionSwitchDirectionTriggered()));

	this->actionPerpendicularConnecting.setText(tr("Perpendicular Connecting"));
	this->actionPerpendicularConnecting.setCheckable(true);
	this->actionPerpendicularConnecting.setChecked(this->perpendicular);

	this->connect(&(this->actionPerpendicularConnecting), SIGNAL(toggled(bool)), this, SLOT(actionPerpendicularConnectingToggled(bool)));
}

ConnectionItem::~ConnectionItem()
{
	if (this->scene)
	{
		for (auto& nodeItem : this->nodeItems)
		{
			this->scene->removeItem(nodeItem.data());
		}

		this->scene->removeItem(this->beginNodeItem.data());

		this->scene->removeItem(this->endNodeItem.data());
	}
}

void ConnectionItem::postConstruction()
{
	if (this->anchored)
	{
		this->updateItem();
	}

	ArrangeableItem::postConstruction();
}

QVector<UndoRedoEvent> ConnectionItem::loadItems(ItemScene* scene, QDomElement element)
{
	QVector<UndoRedoEvent> events;

	if (scene)
	{
		QVector<Connection> connections = Xml::loadObjects<Connection>(element);

		for (auto& connection : connections)
		{
			ItemScene* cScene = scene;
			Identifier cId = scene->newIdentifier(connection.getRequestedId());
			Connection cConnection = connection;

			if (cId)
			{
				std::function<void()> undo = [cScene, cId]()
				{
					cScene->removeItemById(cId);
				};

				std::function<void()> redo = [cScene, cId, cConnection]()
				{
					cScene->addItem(QSharedPointer<Item>(new ConnectionItem(cScene, cId, cConnection.getRequestedLevel(), cConnection, cConnection.getRequestedBeginNode(), cConnection.getRequestedEndNode(), cConnection.getRequestedNodes(), cConnection.isRequestedPerpendicular())));
				};

				events.append(UndoRedoEvent(undo, redo));
			}
		}
	}

	return events;
}

void ConnectionItem::createItem(ItemScene* scene, const QPointF& position)
{
	if (scene)
	{
		Identifier id = scene->newIdentifier();

		if (id)
		{
			scene->addItem(QSharedPointer<Item>(new ConnectionItem(scene, id, scene->queryLevel(), position)));
		}
	}
}

QVector<UndoRedoEvent> ConnectionItem::anchorConnectionEvent(const EndNode& beginNode, const EndNode& endNode)
{
	if (this->scene && !this->anchored)
	{
		Identifier cId = this->id;
		ItemScene* cScene = this->scene;
		UndoRedoEvent cEvent = anchor(this->scene, this->id, beginNode, endNode, QPolygonF());

		std::function<void()> undo = [cEvent]()
		{
			cEvent.undo();
		};

		std::function<void()> redo = [cId, cScene, cEvent]()
		{
			if (!cScene->getItemById(cId))
			{
				cEvent.redo();
			}
		};

		this->anchored = true;

		//this->setSelected(true);

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

void ConnectionItem::anchorConnection(const EndNode& beginNode, const EndNode& endNode)
{
	if (this->scene)
	{
		this->scene->change(this->anchorConnectionEvent(beginNode, endNode));
	}
}

QVector<UndoRedoEvent> ConnectionItem::deleteItemEvent()
{
	QVector<UndoRedoEvent> events;

	if (this->anchored)
	{
		events = ArrangeableItem::deleteItemEvent();

		if (this->scene)
		{
			ItemScene* cScene = this->scene;
			Identifier cId = this->id;

			std::function<void()> undo = [cScene, cId]()
			{
				QSharedPointer<Item> item = cScene->getItemById(cId);

				if (instanceof<ConnectionItem>(item))
				{
					item.dynamicCast<ConnectionItem>()->showNodes();
				}
			};

			std::function<void()> redo = [cScene, cId]()
			{
				QSharedPointer<Item> item = cScene->getItemById(cId);

				if (instanceof<ConnectionItem>(item))
				{
					item.dynamicCast<ConnectionItem>()->hideNodes();
				}
			};

			events.append(UndoRedoEvent(undo, redo));
		}
	}

	return events;
}

QVector<UndoRedoEvent> ConnectionItem::calculateNodesEvent()
{
	if (this->scene)
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;
		QSharedPointer<int> cNodeCounter = QSharedPointer<int>(new int(0));

		std::function<void()> undo = [cScene, cId, cNodeCounter]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ConnectionItem>(item) && cNodeCounter)
			{
				for (int i = *cNodeCounter - 1; i >= 0; i--)
				{
					item.dynamicCast<ConnectionItem>()->deleteNodeItemAt(0);
				}
			}
		};

		std::function<void()> redo = [cScene, cId, cNodeCounter]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ConnectionItem>(item) && cNodeCounter)
			{
				QPolygonF nodes = item.dynamicCast<ConnectionItem>()->calculateNodePositions();

				*cNodeCounter = nodes.size();

				for (int i = *cNodeCounter - 1; i >= 0; i--)
				{
					item.dynamicCast<ConnectionItem>()->newNodeItemAt(nodes[i], 0);
				}
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

void ConnectionItem::calculateNodes()
{
	if (this->scene)
	{
		this->scene->change(this->calculateNodesEvent());
	}
}

QVector<UndoRedoEvent> ConnectionItem::newNodeEvent(const QPointF& position, int index)
{
	if (this->scene && this->anchored)
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;
		QPointF cPosition = position;
		int cIndex = index;

		std::function<void()> undo = [cScene, cId, cIndex]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ConnectionItem>(item))
			{
				item.dynamicCast<ConnectionItem>()->deleteNodeItemAt(cIndex);
			}
		};

		std::function<void()> redo = [cScene, cId, cPosition, cIndex]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ConnectionItem>(item))
			{
				item.dynamicCast<ConnectionItem>()->newNodeItemAt(cPosition, cIndex);
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>();
}

QVector<UndoRedoEvent> ConnectionItem::newNodeEvent(const QPointF& position)
{
	int index = 0;

	if (this->nodeItems.size())
	{
		qreal distance = std::numeric_limits<qreal>::max();

		if (this->beginNodeItem && this->nodeItems.first())
		{
			qreal length = Math::distanceToLine(this->beginNodeItem->pos(), this->nodeItems.first()->pos(), position);

			if (length < distance)
			{
				index = 0;

				distance = length;
			}
		}

		if (this->endNodeItem && this->nodeItems.last())
		{
			qreal length = Math::distanceToLine(this->endNodeItem->pos(), this->nodeItems.last()->pos(), position);

			if (length < distance)
			{
				index = this->nodeItems.size();

				distance = length;
			}
		}

		for (int i = 1; i < this->nodeItems.size(); i++)
		{
			if (this->nodeItems[i - 1] && this->nodeItems[i])
			{
				qreal length = Math::distanceToLine(this->nodeItems[i - 1]->pos(), this->nodeItems[i]->pos(), position);

				if (length < distance)
				{
					index = i;

					distance = length;
				}
			}
		}
	}

	return this->newNodeEvent(position, index);
}

void ConnectionItem::newNode(const QPointF& position)
{
	if (this->scene)
	{
		this->scene->change(this->newNodeEvent(position));
	}
}

QVector<UndoRedoEvent> ConnectionItem::deleteNodeEvent(int index)
{
	if (this->scene && this->anchored && index >= 0 && index < this->nodeItems.size() && this->nodeItems[index])
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;
		QPointF cPosition = this->nodeItems[index]->pos();
		NodeItem* cNodeItem = this->nodeItems[index].data();
		QSharedPointer<QSharedPointer<int>> cIndex(new QSharedPointer<int>());

		std::function<void()> undo = [cScene, cId, cPosition, cIndex]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ConnectionItem>(item) && cIndex)
			{
				if (*cIndex)
				{
					item.dynamicCast<ConnectionItem>()->newNodeItemAt(cPosition, **cIndex);
				}
			}
		};

		std::function<void()> redo = [cScene, cId, cNodeItem, cIndex]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ConnectionItem>(item) && cIndex)
			{
				if (!*cIndex)
				{
					*cIndex = QSharedPointer<int>(new int(item.dynamicCast<ConnectionItem>()->getNodeItemIndex(cNodeItem)));
				}

				if (*cIndex)
				{
					item.dynamicCast<ConnectionItem>()->deleteNodeItemAt(**cIndex);
				}
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>({ });
}

void ConnectionItem::deleteNode(int index)
{
	if (this->scene)
	{
		this->scene->change(this->deleteNodeEvent(index));
	}
}

QVector<UndoRedoEvent> ConnectionItem::changePerpendicularEvent(bool perpendicular)
{
	if (this->scene)
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;
		bool cPerpendicular = this->perpendicular;

		std::function<void()> undo = [cScene, cId, cPerpendicular]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ConnectionItem>(item))
			{
				item.dynamicCast<ConnectionItem>()->setPerpendicular(cPerpendicular);
			}
		};

		cPerpendicular = perpendicular;

		std::function<void()> redo = [cScene, cId, cPerpendicular]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ConnectionItem>(item))
			{
				item.dynamicCast<ConnectionItem>()->setPerpendicular(cPerpendicular);
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>({});
}

void ConnectionItem::changePerpendicular(bool perpendicular)
{
	if (this->scene)
	{
		this->scene->change(this->changePerpendicularEvent(perpendicular));
	}
}

QVector<UndoRedoEvent> ConnectionItem::switchDirectionEvent()
{
	if (this->scene)
	{
		ItemScene* cScene = this->scene;
		Identifier cId = this->id;

		std::function<void()> undo = [cScene, cId]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ConnectionItem>(item))
			{
				item.dynamicCast<ConnectionItem>()->changeDirection();
			}
		};

		std::function<void()> redo = [cScene, cId]()
		{
			QSharedPointer<Item> item = cScene->getItemById(cId);

			if (instanceof<ConnectionItem>(item))
			{
				item.dynamicCast<ConnectionItem>()->changeDirection();
			}
		};

		return QVector<UndoRedoEvent>({ UndoRedoEvent(undo, redo) });
	}

	return QVector<UndoRedoEvent>({});
}

void ConnectionItem::switchDirection()
{
	if (this->scene)
	{
		this->scene->change(this->switchDirectionEvent());
	}
}

int ConnectionItem::getNodeItemIndex(NodeItem* nodeItem)
{
	for (int i = 0; i < this->nodeItems.size(); i++)
	{
		if (this->nodeItems[i].data() == nodeItem)
		{
			return i;
		}
	}

	return -1;
}

QSharedPointer<NodeItem> ConnectionItem::getNodeItemAt(int index)
{
	if (index >= 0 && index < this->nodeItems.size())
	{
		return this->nodeItems[index];
	}

	return QSharedPointer<NodeItem>();
}

QSharedPointer<NodeItem> ConnectionItem::getFirstNodeItem()
{
	if (this->nodeItems.size())
	{
		return this->nodeItems.first();
	}

	return QSharedPointer<NodeItem>();
}

QSharedPointer<NodeItem> ConnectionItem::getLastNodeItem()
{
	if (this->nodeItems.size())
	{
		return this->nodeItems.last();
	}

	return QSharedPointer<NodeItem>();
}

bool ConnectionItem::isBeginNodeItem(const EndNodeItem* beginNodeItem)
{
	return this->beginNodeItem.data() == beginNodeItem;
}

QSharedPointer<EndNodeItem> ConnectionItem::getBeginNodeItem()
{
	return this->beginNodeItem;
}

bool ConnectionItem::isEndNodeItem(const EndNodeItem* endNodeItem)
{
	return this->endNodeItem.data() == endNodeItem;
}

QSharedPointer<EndNodeItem> ConnectionItem::getEndNodeItem()
{
	return this->endNodeItem;
}

bool ConnectionItem::isAnyNodeSelected() const
{
	if (this->beginNodeItem)
	{
		if (this->beginNodeItem->isSelected())
		{
			return true;
		}
	}

	if (this->endNodeItem)
	{
		if (this->endNodeItem->isSelected())
		{
			return true;
		}
	}

	for (auto& nodeItem : this->nodeItems)
	{
		if (nodeItem)
		{
			if (nodeItem->isSelected())
			{
				return true;
			}
		}
	}

	return false;
}

void ConnectionItem::setNodesPaintable(bool paintable)
{
	if (this->beginNodeItem)
	{
		this->beginNodeItem->setPaintable(paintable);
	}

	if (this->endNodeItem)
	{
		this->endNodeItem->setPaintable(paintable);
	}

	for (auto& nodeItem : this->nodeItems)
	{
		if (nodeItem)
		{
			nodeItem->setPaintable(paintable);
		}
	}
}

QPainterPath ConnectionItem::shape() const
{
	QPainterPath path;

	qreal radius = Settings::getBorderWidth() * 5.0;

	for (int i = 1; i < this->polygon.size(); i++)
	{
		QPointF first = this->polygon[i - 1];

		QPointF second = this->polygon[i];

		QPointF direction = first - second;

		qreal length = Math::length(direction);

		if (length > 0.0)
		{
			direction /= length;

			QPointF orthogonal = Math::orthogonal(direction);

			QPolygonF rect;

			rect.append(first + (orthogonal + direction) * radius);
			rect.append(second + (orthogonal - direction) * radius);
			rect.append(second - (orthogonal + direction) * radius);
			rect.append(first - (orthogonal - direction) * radius);
			rect.append(first + (orthogonal + direction) * radius);

			QPainterPath pathBuff;
			pathBuff.addPolygon(rect);

			path = path.united(pathBuff);
		}
	}

	return path;
}

void ConnectionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	QPen pen;
	pen.setWidth(Settings::getBorderWidth());
	pen.setColor(Settings::getColor());
	pen.setStyle(Qt::PenStyle::DashLine);
	pen.setDashPattern({ Settings::getBorderWidth() * 2.0, Settings::getBorderWidth() * 2.0 });

	painter->setPen(pen);
	painter->setRenderHints(QPainter::RenderHint::Antialiasing | QPainter::RenderHint::SmoothPixmapTransform | QPainter::RenderHint::HighQualityAntialiasing);

	QPainterPath path;
	path.addPolygon(this->polygon);

	painter->drawPath(path);
}

void ConnectionItem::appendXml(QDomElement parentElement)
{
	EndNode beginNode;
	EndNode endNode;

	if (this->beginNodeItem)
	{
		beginNode = this->beginNodeItem->getEndNode();
	}

	if (this->endNodeItem)
	{
		endNode = this->endNodeItem->getEndNode();
	}

	QPolygonF nodes;

	for (int i = 1; i < this->polygon.size() - 1; i++)
	{
		nodes.append(this->polygon[i]);
	}

	this->appendXml(parentElement, beginNode, endNode, nodes);
}

void ConnectionItem::appendXml(QDomElement parentElement, EndNode& beginNode, EndNode& endNode, const QPolygonF& nodes)
{
	this->connection.appendXml(Xml::appendObject<Connection>(parentElement), this->id, this->getLevel(), this->isPerpendicular(), beginNode, endNode, nodes);
}

QPolygonF ConnectionItem::getNodes() const
{
	return polygon;
}

bool ConnectionItem::isAnchored() const
{
	return this->anchored;
}

bool ConnectionItem::isPerpendicular() const
{
	return this->perpendicular;
}

void ConnectionItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
	if (this->anchored)
	{
		if (this->scene)
		{
			cursorPosition = this->scene->getCursorPosition();
		}

		ArrangeableItem::contextMenuEvent(event);
	}
}

QVariant ConnectionItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == GraphicsItemChange::ItemSelectedHasChanged)
	{
		if (value.toBool())
		{
			this->setNodesPaintable(true);
		}
		else
		{
			this->setNodesPaintable(this->isAnyNodeSelected());
		}
	}

	return ArrangeableItem::itemChange(change, value);
}

void ConnectionItem::contextMenuAdditions(QMenu& contextMenu)
{
	contextMenu.addActions({ &(this->actionNewNode) });

	contextMenu.addSeparator();

	contextMenu.addActions({ &(this->actionSwitchDirection), &(this->actionPerpendicularConnecting) });

	contextMenu.addSeparator();

	ArrangeableItem::contextMenuAdditions(contextMenu);
}

UndoRedoEvent ConnectionItem::anchor(ItemScene* scene, const Identifier& id, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes)
{
	ItemScene* cScene = scene;
	Identifier cId = id;
	Connection cConnection = Connection();
	EndNode cBeginNode = beginNode;
	EndNode cEndNode = endNode;
	QPolygonF cNodes = nodes;

	std::function<void()> undo = [cScene, cId]()
	{
		cScene->removeItemById(cId);
	};

	std::function<void()> redo = [cScene, cId, cConnection, cBeginNode, cEndNode, cNodes]()
	{
		cScene->addItem(QSharedPointer<Item>(new ConnectionItem(cScene, cId, cScene->queryLevel(), cConnection, cBeginNode, cEndNode, cNodes, false)));
	};

	return UndoRedoEvent(undo, redo);
}

void ConnectionItem::setPerpendicular(bool perpendicular)
{
	this->perpendicular = perpendicular;

	this->actionPerpendicularConnecting.setChecked(perpendicular);

	this->updateItem();
}

void ConnectionItem::updateGeometry()
{
	if (this->beginNodeItem)
	{
		this->beginNodeItem->updatePosition();
	}

	if (this->endNodeItem)
	{
		this->endNodeItem->updatePosition();
	}

	this->polygon.clear();

	if (this->beginNodeItem)
	{
		this->polygon.append(this->beginNodeItem->pos());
	}

	for (auto& nodeItem : this->nodeItems)
	{
		if (nodeItem)
		{
			this->polygon.append(nodeItem->pos());
		}
	}

	if (this->endNodeItem)
	{
		this->polygon.append(this->endNodeItem->pos());
	}
}

void ConnectionItem::actionNewNodeTriggered()
{
	this->newNode(cursorPosition);
}

void ConnectionItem::actionSwitchDirectionTriggered()
{
	this->switchDirection();
}

void ConnectionItem::actionPerpendicularConnectingToggled(bool checked)
{
	if (checked != this->perpendicular)
	{
		this->changePerpendicular(checked);
	}
}

QPolygonF ConnectionItem::calculateNodePositions()
{
	QPolygonF nodes;

	if (this->scene && !this->nodeItems.size() && this->anchored && this->getBeginNodeItem() && this->getEndNodeItem())
	{
		if (this->getBeginNodeItem()->getConnectedItemId() && this->getEndNodeItem()->getConnectedItemId())
		{
			QSharedPointer<Item> beginConnectedItem = this->scene->getItemById(this->getBeginNodeItem()->getConnectedItemId());

			QSharedPointer<Item> endConnectedItem = this->scene->getItemById(this->getEndNodeItem()->getConnectedItemId());

			if (instanceof<ConnectableItem>(beginConnectedItem) && instanceof<ConnectableItem>(endConnectedItem))
			{
				if (beginConnectedItem == endConnectedItem)
				{
					QRectF rect = beginConnectedItem->shape().boundingRect();

					rect.moveTo(beginConnectedItem->pos());

					nodes << QPointF(rect.left() - 100.0, (rect.top() + rect.bottom()) / 2.0);

					nodes << QPointF(rect.left() - 100.0, rect.bottom() + 100.0);

					nodes << QPointF((rect.left() + rect.right()) / 2.0, rect.bottom() + 100.0);

					return nodes;
				}
				else if (this->perpendicular)
				{
					QPolygonF beginOutline = beginConnectedItem->outline();

					beginOutline.translate(beginConnectedItem->pos());

					QRectF beginRect = beginConnectedItem->shape().boundingRect();

					beginRect.moveTo(beginConnectedItem->pos());

					QPointF beginCenter((beginRect.left() + beginRect.right()) / 2.0, (beginRect.top() + beginRect.bottom()) / 2.0);

					QPolygonF endOutline = endConnectedItem->outline();

					endOutline.translate(endConnectedItem->pos());

					QRectF endRect = endConnectedItem->shape().boundingRect();

					endRect.moveTo(endConnectedItem->pos());

					QPointF endCenter((endRect.left() + endRect.right()) / 2.0, (endRect.top() + endRect.bottom()) / 2.0);

					QPolygonF line;

					line << beginCenter << endCenter;

					QPolygonF beginIntersection = Math::intersectionOfPolygons(beginOutline, line);

					QPolygonF endIntersection = Math::intersectionOfPolygons(endOutline, line);
					
					if (beginIntersection.size() && endIntersection.size())
					{
						QPointF beginPosition = beginIntersection.first();

						QPointF endPosition = endIntersection.first();

						QPointF direction = endPosition - beginPosition;

						if (qAbs(direction.y()) > qAbs(direction.x()))
						{
							nodes << QPointF((beginRect.right() + beginRect.left()) / 2.0, (beginPosition.y() + endPosition.y()) / 2.0);

							nodes << QPointF((endRect.right() + endRect.left()) / 2.0, (beginPosition.y() + endPosition.y()) / 2.0);
						}
						else
						{
							nodes << QPointF((beginPosition.x() + endPosition.x()) / 2.0, (beginRect.top() + beginRect.bottom()) / 2.0);

							nodes << QPointF((beginPosition.x() + endPosition.x()) / 2.0, (endRect.top() + endRect.bottom()) / 2.0);
						}
					}
				}
			}
		}
	}

	return nodes;
}

void ConnectionItem::newNodeItemAt(const QPointF& position, int index)
{
	if (this->scene && index >= 0 && index <= this->nodeItems.size())
	{
		QSharedPointer<NodeItem> nodeItem = QSharedPointer<NodeItem>(new NodeItem(this->scene, position, this));

		this->nodeItems.insert(this->nodeItems.begin() + index, nodeItem);

		this->scene->addItem(nodeItem);

		this->setNodesPaintable(this->isSelected() || this->isAnyNodeSelected());

		this->updateItem();
	}
}

void ConnectionItem::deleteNodeItemAt(int index)
{
	if (this->scene && index >= 0 && index < this->nodeItems.size())
	{
		bool wasPaintable = this->isSelected() || this->isAnyNodeSelected();

		this->scene->removeItem(this->nodeItems[index].data());

		this->nodeItems.removeAt(index);

		if (!this->isAnyNodeSelected() && wasPaintable)
		{
			this->setSelected(true);
		}

		this->updateItem();
	}
}

void ConnectionItem::showNodes()
{
	if (this->beginNodeItem)
	{
		this->beginNodeItem->show();
	}

	if (this->endNodeItem)
	{
		this->endNodeItem->show();
	}

	for (auto& nodeItem : this->nodeItems)
	{
		if (nodeItem)
		{
			nodeItem->show();
		}
	}
}

void ConnectionItem::hideNodes()
{
	if (this->beginNodeItem)
	{
		this->beginNodeItem->setSelected(false);

		this->beginNodeItem->hide();
	}

	if (this->endNodeItem)
	{
		this->endNodeItem->setSelected(false);

		this->endNodeItem->hide();
	}

	for (auto& nodeItem : this->nodeItems)
	{
		if (nodeItem)
		{
			nodeItem->setSelected(false);

			nodeItem->hide();
		}
	}
}

void ConnectionItem::changeDirection()
{
	QSharedPointer<EndNodeItem> buff = this->beginNodeItem;

	this->beginNodeItem = this->endNodeItem;

	this->endNodeItem = buff;

	for (int i = 0; i < this->nodeItems.size() / 2; i++)
	{
		this->nodeItems.swap(i, this->nodeItems.size() - i - 1);
	}
	
	this->updateItem();
}
