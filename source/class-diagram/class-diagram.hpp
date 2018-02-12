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

#include <QMenu>
#include <QString>
#include <QTextStream>
#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QSharedPointer>

#include "item-scene/item-scene.hpp"

#include "dialogs/export-as-image.hpp"

class ItemCreator;

class ClassDiagram : public QGraphicsView, public ItemScene
{
	Q_OBJECT

public:
	ClassDiagram(QWidget* parent);

	bool save(const QString& path);
	bool open(const QString& path);

	bool exportAsImage();

	bool isInsertable() const;

	void updateScene();

	void zoomIn();
	void zoomOut();
	void zoomReset();

	void zoomToFit();

	int getZoomIndex() const;
	void setZoomIndex(int zoomIndex);

	int getZoomPercent() const;

	static int getMaxZoomIndex();
	static int getDefaultZoomIndex();

	const QString& getPath() const;

protected:
	void mousePressEvent(QMouseEvent* event) override;

	void mouseMoveEvent(QMouseEvent* event) override;

	void mouseReleaseEvent(QMouseEvent* event) override;

	void dragMoveEvent(QDragMoveEvent* event) override;
	void dropEvent(QDropEvent* event) override;

	void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
	void dataChanged();

private:
	QWidget* parent;

	Action actionPaste;

	int zoomIndex;

	QString path;

	bool insertable;
};
