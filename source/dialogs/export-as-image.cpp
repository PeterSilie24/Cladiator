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

#include "export-as-image.hpp"

ExportAsImage::ExportAsImage(ItemScene* scene)
	: QDialog(scene ? scene->getWidget() : nullptr), scene(scene)
{
	this->ui.setupUi(this);

	this->setWindowFlags(this->windowFlags() | Qt::WindowType::WindowMaximizeButtonHint);

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowType::WindowContextHelpButtonHint);

	this->ui.graphicsView->setScene(&(this->graphicsScene));

	this->ui.graphicsView->setBackgroundBrush(QBrush(QColor(239, 239, 239), Qt::SolidPattern));

	this->graphicsScene.addItem(&(this->pixmapItem));

	this->updateImage();

	this->connect(this->ui.checkBoxTransparentBackground, SIGNAL(stateChanged(int)), this, SLOT(changed(int)));

	this->connect(this->ui.comboBoxResolution, SIGNAL(currentIndexChanged(int)), this, SLOT(changed(int)));

	this->disconnect(this->ui.pushButtonExport, SIGNAL(clicked()), this, SLOT(accept()));

	this->connect(this->ui.pushButtonExport, SIGNAL(clicked()), this, SLOT(pushButtonExportClicked()));
}

void ExportAsImage::changed(int i)
{
	Q_UNUSED(i);

	this->updateImage();
}

void ExportAsImage::pushButtonExportClicked()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Export as Image"), "", "PNG (*.png);;JPG (*.jpg);;Bitmap (*.bmp)");

	if (path != "")
	{
		QFileInfo info(path);

		QString suffix = info.suffix().toLower();
		
		if (suffix != "png" && suffix != "jpg" && suffix != "bmp")
		{
			path = info.path() + "/" + info.baseName() + ".png";
		}

		if (this->pixmapItem.pixmap().toImage().save(path))
		{
			this->accept();
		}
		else
		{
			QMessageBox::critical(this, tr("Error"), tr("Failed to export \"") + path + tr("\" as image."));
		}
	}
}

void ExportAsImage::updateImage()
{
	if (this->scene)
	{
		qreal scale = qPow(2.0, static_cast<qreal>(this->ui.comboBoxResolution->currentIndex()));

		bool transparent = this->ui.checkBoxTransparentBackground->isChecked();

		QImage image = this->scene->renderToImage(scale, transparent);

		this->pixmapItem.setPixmap(QPixmap::fromImage(image));

		QSizeF size = image.size();

		QRectF rect(0, 0, size.width(), size.height());

		this->graphicsScene.setSceneRect(rect);
	}
}
