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

#include "items/item.hpp"

#include "item-scene/item-scene.hpp"

#include "content/connection.hpp"

class ConnectionItem;

class ConnectableItem : public MovableItem
{
	Q_OBJECT

public:
	ConnectableItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position);

	virtual QVector<UndoRedoEvent> deleteItemEvent() override;

	void appendConnectionItem(const Identifier& connectionItemId);

	void removeConnectionItem(const Identifier& connectionItemId);

	virtual void geometryChange() override;

private:
	QList<Identifier> connectionItemIds;
};

class NodeItem : public MovableItem
{
	Q_OBJECT

public:
	NodeItem(ItemScene* scene, const QPointF& position, ConnectionItem* connectionItem);

	virtual QVector<UndoRedoEvent> deleteItemEvent() override;

	virtual QVector<UndoRedoEvent> moveItemEvent(const QPointF& newPosition) override;

	virtual QPainterPath shape() const override;

	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	void setPaintable(bool paintable);

protected:
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

	ConnectionItem* const connectionItem;

private:
	bool paintable;
};

class EndNodeItem : public NodeItem
{
public:
	EndNodeItem(ItemScene* scene, const QPointF& position, ConnectionItem* connectionItem, const Identifier& connectedItemId);

	~EndNodeItem();

	virtual QVector<UndoRedoEvent> deleteItemEvent() override;

	virtual QVector<UndoRedoEvent> moveItemEvent(const QPointF& newPosition) override;

	QVector<UndoRedoEvent> changeConnectedItemEvent(const Identifier& newConnectedItemId, const QPointF& newPosition);

	void changeConnectedItem(const Identifier& newConnectedItemId, const QPointF& newPosition);

	const Identifier& getConnectedItemId() const;

	EndNode getEndNode() const;

	bool isConnectedItemSelected() const;

	void updatePosition();

	virtual bool checkMoved(QVector<UndoRedoEvent>& events) override;

protected:
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
	void setConnectedItem(const Identifier& newConnectedItemId, const QPointF& newPosition);

	bool calculatePosition(QPointF& position);

	Identifier connectedItemId;

	bool updatingPosition;
};

class ConnectionItem : public ArrangeableItem
{
	Q_OBJECT

public:
	ConnectionItem(ItemScene* scene, const Identifier& id, quint64 level, const Connection& connection, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes, bool perpendicular);

	ConnectionItem(ItemScene* scene, const Identifier& id, quint64 level, const QPointF& position, bool perpendicular = false);

	~ConnectionItem();

	virtual void postConstruction() override;

	static QVector<UndoRedoEvent> loadItems(ItemScene* scene, QDomElement element);

	static void createItem(ItemScene* scene, const QPointF& position);

	QVector<UndoRedoEvent> anchorConnectionEvent(const EndNode& beginNode, const EndNode& endNode);
	void anchorConnection(const EndNode& beginNode, const EndNode& endNode);

	virtual QVector<UndoRedoEvent> deleteItemEvent() override;

	QVector<UndoRedoEvent> calculateNodesEvent();
	void calculateNodes();

	QVector<UndoRedoEvent> newNodeEvent(const QPointF& position, int index);
	QVector<UndoRedoEvent> newNodeEvent(const QPointF& position);
	void newNode(const QPointF& position);

	QVector<UndoRedoEvent> deleteNodeEvent(int index);
	void deleteNode(int index);

	QVector<UndoRedoEvent> changePerpendicularEvent(bool perpendicular);
	void changePerpendicular(bool perpendicular);

	QVector<UndoRedoEvent> switchDirectionEvent();
	void switchDirection();

	int getNodeItemIndex(NodeItem* nodeItem);

	QSharedPointer<NodeItem> getNodeItemAt(int index);

	QSharedPointer<NodeItem> getFirstNodeItem();

	QSharedPointer<NodeItem> getLastNodeItem();

	bool isBeginNodeItem(const EndNodeItem* beginNodeItem);

	QSharedPointer<EndNodeItem> getBeginNodeItem();

	bool isEndNodeItem(const EndNodeItem* endNodeItem);

	QSharedPointer<EndNodeItem> getEndNodeItem();

	bool isAnyNodeSelected() const;

	void setNodesPaintable(bool paintable);

	virtual QPainterPath shape() const override;

	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

	virtual void appendXml(QDomElement parentElement) override;

	virtual void appendXml(QDomElement parentElement, EndNode& beginNode, EndNode& endNode, const QPolygonF& nodes);

	QPolygonF getNodes() const;

	bool isAnchored() const;

	bool isPerpendicular() const;

protected:
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

	virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

	virtual void contextMenuAdditions(QMenu& contextMenu) override;

	virtual UndoRedoEvent anchor(ItemScene* scene, const Identifier& id, const EndNode& beginNode, const EndNode& endNode, const QPolygonF& nodes);

	void setPerpendicular(bool perpendicular);

	virtual void updateGeometry() override;

private slots:
	void actionNewNodeTriggered();

	void actionSwitchDirectionTriggered();

	void actionPerpendicularConnectingToggled(bool checked);

private:
	QPolygonF calculateNodePositions();

	void newNodeItemAt(const QPointF& position, int index);
	void deleteNodeItemAt(int index);

	void showNodes();
	void hideNodes();

	void changeDirection();

	QAction actionNewNode;
	QAction actionSwitchDirection;
	QAction actionPerpendicularConnecting;

	Connection connection;

	QSharedPointer<EndNodeItem> beginNodeItem;
	QSharedPointer<EndNodeItem> endNodeItem;

	QList<QSharedPointer<NodeItem>> nodeItems;

	QPolygonF polygon;

	QPointF cursorPosition;

	bool anchored;

	bool perpendicular;
};
