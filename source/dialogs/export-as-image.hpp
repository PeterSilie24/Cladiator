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

#include <QDialog>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_export-as-image.h"

#include "item-scene/item-scene.hpp"

class ExportAsImage : public QDialog
{
	Q_OBJECT

public:
	ExportAsImage(ItemScene* scene);

private slots:
	void changed(int i);

	void pushButtonExportClicked();

private:
	void updateImage();

	Ui::ExportAsImageDialog ui;

	ItemScene* const scene;

	QGraphicsScene graphicsScene;

	QGraphicsPixmapItem pixmapItem;
};
