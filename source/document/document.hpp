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

#include <QApplication>
#include <QWidget>
#include <QTabWidget>
#include <QLayout>
#include <QList>
#include <QMessageBox>

#include "class-diagram/class-diagram.hpp"

class Document : public QWidget
{
	Q_OBJECT

public:
	Document(QTabWidget* parent, const QString& path = "");

	void tick();

	template <typename TReturn>
	TReturn diagramCall(TReturn(ClassDiagram::*member)())
	{
		return (this->diagram.*member)();
	}

	template <typename TReturn>
	TReturn diagramCall(TReturn(ClassDiagram::*member)() const) const
	{
		return (this->diagram.*member)();
	}

	bool save(const QString& path);

	bool exportAsImage();

	bool hasChanged() const;

	void updateScene();

	int getZoomIndex() const;
	void setZoomIndex(int zoomIndex);

	int getZoomPercent() const;

	static int getMaxZoomIndex();
	static int getDefaultZoomIndex();

	QList<QAction*> getActionsCreate();

	const QString& getPath() const;

	const QString& getTitle() const;

private:
	void changeTitle(const QString& title, bool changed = false);

	QTabWidget* parent;

	QVBoxLayout layout;

	ClassDiagram diagram;

	QString title;
};
