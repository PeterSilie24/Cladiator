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

#include "class-diagram.hpp"

#include "cladiator/cladiator.hpp"

QList<qreal> zoomConstants = { 0.1, 1.0 / 6.0, 0.25, 1.0 / 3.0, 0.4, 0.5, 2.0 / 3.0, 0.75, 0.8, 0.9, 1.0, 1.25, 1.5, 1.75, 2.0, 2.5, 3.0, 4.0, 5.0, 7.5, 10.0 };

ClassDiagram::ClassDiagram(QWidget* parent)
	: QGraphicsView(parent), ItemScene(this, "class-diagram"), parent(parent),
	actionPaste(this, tr("Paste"), Icons::loadIcon("edit-paste"), QKeySequence("Ctrl+V")),
	zoomIndex(0), insertable(false)
{
	this->setAcceptDrops(true);

	this->zoomReset();

	this->setDragMode(QGraphicsView::DragMode::RubberBandDrag);

	this->setBackgroundBrush(QBrush(Settings::getColor(Colors::Background), Qt::SolidPattern));

	this->actionPaste.setTriggeredFunction(std::function<void()>([&]() { this->paste(); }));

	this->connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(dataChanged()));
}

bool ClassDiagram::save(const QString& path)
{
	QFile file(path);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		return false;
	}

	QTextStream stream(&file);
	stream << this->exportItems();

	this->path = path;

	return true;
}

bool ClassDiagram::open(const QString& path)
{
	QFile file(path);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}

	this->importItems(file, true);

	this->path = path;

	return true;
}

bool ClassDiagram::exportAsImage()
{
	if (this->isExportable())
	{
		ExportAsImage exportAsImage(this);

		if (exportAsImage.exec())
		{
			return true;
		}
	}

	return false;
}

bool ClassDiagram::isInsertable() const
{
	return this->insertable;
}

void ClassDiagram::updateScene()
{
	this->setBackgroundBrush(QBrush(Settings::getColor(Colors::Background), Qt::SolidPattern));

	this->updateItems();
}

void ClassDiagram::zoomIn()
{
	this->setZoomIndex(this->zoomIndex + 1);
}

void ClassDiagram::zoomOut()
{
	this->setZoomIndex(this->zoomIndex - 1);
}

void ClassDiagram::zoomReset()
{
	this->setZoomIndex(ClassDiagram::getDefaultZoomIndex());
}

void ClassDiagram::zoomToFit()
{
	QRectF rect = this->getBoundingRect();

	this->centerOn((rect.left() + rect.right()) / 2.0, (rect.top() + rect.bottom()) / 2.0);

	qreal aspectWidth = static_cast<qreal>(this->width()) / rect.width();
	qreal aspectHeight = static_cast<qreal>(this->height()) / rect.height();

	qreal aspect = qMin(aspectWidth, aspectHeight);

	int zoomIndex = 0;
	
	for (int i = zoomConstants.size() - 1; i >= 0; i--)
	{
		if (zoomConstants[i] < aspect)
		{
			zoomIndex = i;

			break;
		}
	}

	zoomIndex = qMin(zoomIndex, this->getDefaultZoomIndex());

	this->setZoomIndex(zoomIndex);
}

int ClassDiagram::getZoomIndex() const
{
	return this->zoomIndex;
}

void ClassDiagram::setZoomIndex(int zoomIndex)
{
	if (zoomIndex >= 0 && zoomIndex < zoomConstants.size())
	{
		this->zoomIndex = zoomIndex;

		this->resetMatrix();

		this->scale(zoomConstants[this->zoomIndex], zoomConstants[this->zoomIndex]);
	}
}

int ClassDiagram::getZoomPercent() const 
{
	return static_cast<int>(zoomConstants[this->zoomIndex] * 100.0);
}

int ClassDiagram::getMaxZoomIndex()
{
	return qMax(zoomConstants.size() - 1, 0);
}

int ClassDiagram::getDefaultZoomIndex()
{
	for (int i = 0; i < zoomConstants.size(); i++)
	{
		if (zoomConstants[i] == 1.0)
		{
			return i;
		}
	}

	return 0;
}

const QString& ClassDiagram::getPath() const
{
	return this->path;
}

void ClassDiagram::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		this->dragNewTrigger();
	}
	else
	{
		this->dragNewEnd();
	}
	
	if (event->button() != Qt::MouseButton::RightButton)
	{
		QGraphicsView::mousePressEvent(event);
	}
	else
	{
		if (!this->isItemAtPosition(this->getCursorPosition()))
		{
			this->deselectAll();
		}
	}
}

void ClassDiagram::mouseMoveEvent(QMouseEvent* event)
{
	QGraphicsView::mouseMoveEvent(event);
}

void ClassDiagram::mouseReleaseEvent(QMouseEvent* event)
{
	this->viewport()->setCursor(Qt::CursorShape::ArrowCursor);

	if (event->button() != Qt::MouseButton::RightButton)
	{
		QGraphicsView::mouseReleaseEvent(event);
	}
}

void ClassDiagram::dragMoveEvent(QDragMoveEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		event->acceptProposedAction();
	}
}

void ClassDiagram::dropEvent(QDropEvent* event)
{
	if (event->mimeData()->hasUrls())
	{
		QList<QUrl> list = event->mimeData()->urls();

		for (auto& url : list)
		{
			Cladiator::enqueueFile(url.toLocalFile());
		}

		event->acceptProposedAction();
	}
}

void ClassDiagram::contextMenuEvent(QContextMenuEvent* event)
{
	QGraphicsView::contextMenuEvent(event);

	if (!event->isAccepted())
	{
		QMenu contextMenu(this);

		this->actionPaste.setEnabled(this->isInsertable());

		contextMenu.addAction(&this->actionPaste);

		contextMenu.addSeparator();

		QMenu contextMenuNew(tr("New"), this);

		contextMenuNew.addActions(this->getActionsCreate());

		contextMenu.addMenu(&contextMenuNew);

		contextMenu.exec(event->globalPos());
	}
}

void ClassDiagram::dataChanged()
{
	this->insertable = ItemScene::isInsertable();
}
