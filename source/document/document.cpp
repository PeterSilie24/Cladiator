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

#include "document.hpp"

Document::Document(QTabWidget* parent, const QString& path)
	: QWidget(parent), parent(parent), diagram(this)
{
	this->layout.addWidget(&(this->diagram));
	this->layout.setMargin(10);

	this->setLayout(&(this->layout));

	if (path != "")
	{
		QFileInfo info(path);

		if (info.exists() && info.isFile())
		{
			if (diagram.open(path))
			{
				QFileInfo fileInfo(path);

				this->title = fileInfo.fileName();

				parent->addTab(this, this->title);
			}
			else
			{
				throw QString(tr("The file \"") + path + tr("\" does not contain a class diagram."));
			}
		}
		else
		{
			throw QString(tr("Failed to open the file \"") + path + tr("\"."));
		}
	}
	else
	{
		this->title = tr("Untitled");

		parent->addTab(this, this->title);
	}
}

void Document::tick()
{
	this->changeTitle(this->getTitle(), this->diagram.hasChanged());
}

bool Document::save(const QString& path)
{
	if (this->diagram.save(path))
	{
		QFileInfo fileInfo(path);

		this->title = fileInfo.fileName();

		this->diagram.resetChanged();

		return true;
	}

	return false;
}

bool Document::exportAsImage()
{
	return this->diagram.exportAsImage();
}

bool Document::hasChanged() const
{
	return this->diagram.hasChanged();
}

void Document::updateScene()
{
	this->diagram.updateScene();
}

int Document::getZoomIndex() const
{
	return this->diagram.getZoomIndex();
}

void Document::setZoomIndex(int zoomIndex)
{
	this->diagram.setZoomIndex(zoomIndex);
}

int Document::getZoomPercent() const
{
	return this->diagram.getZoomPercent();
}

int Document::getMaxZoomIndex()
{
	return ClassDiagram::getMaxZoomIndex();
}

int Document::getDefaultZoomIndex()
{
	return ClassDiagram::getDefaultZoomIndex();
}

QList<QAction*> Document::getActionsCreate()
{
	return this->diagram.getActionsCreate();
}

const QString& Document::getPath() const
{
	return this->diagram.getPath();
}

const QString& Document::getTitle() const
{
	return this->title;
}

void Document::changeTitle(const QString& title, bool changed)
{
	int index = this->parent->indexOf(this);

	if (index >= 0)
	{
		this->title = title;

		this->parent->setTabText(index, changed ? (title + "*") : title);
	}
}
