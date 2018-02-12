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

#include "about.hpp"

About::About(QWidget* parent)
	: QDialog(parent)
{
	this->ui.setupUi(this);

	this->setWindowFlags(this->windowFlags() | Qt::WindowType::MSWindowsFixedSizeDialogHint);

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowType::WindowContextHelpButtonHint);

	this->disconnect(this->ui.pushButtonLicense, SIGNAL(clicked()), this, SLOT(accept()));

	this->disconnect(this->ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(reject()));

	this->connect(this->ui.pushButtonLicense, SIGNAL(clicked()), this, SLOT(pushButtonLicenseClicked()));

	this->connect(this->ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(accept()));

	QString html = "<center>";

	html += "<br><img src=\"" + Icons::getResourceIconsDir().toHtmlEscaped() + Info::getApplicationIconName().toHtmlEscaped() + ".svg" + "\" width=\"96\" height=\"96\">";

	html += "<h1>" + Info::getApplicationName().toHtmlEscaped() + "</h1>";

	html += "<p><h3>" + Info::getApplicationDescription().toHtmlEscaped() + " " + Info::getVersion().toString().toHtmlEscaped() + "</h3></p>";

	html += "<p><font style=\"font-size: 10px;\">Copyright &copy; " + Info::getCopyright().toHtmlEscaped() + "</font></p>";

	html += "</center>";

	this->document.setHtml(html);

	QSizeF size = this->document.documentLayout()->documentSize();

	size.setWidth(qMax(size.width(), static_cast<qreal>(this->width())) + 40.0);

	size.setHeight(qMax(size.height(), static_cast<qreal>(this->height())) + 75.0);

	this->setFixedSize(size.toSize());

	this->document.setTextWidth(size.width());
}

void About::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	QPainter painter(this);

	painter.setRenderHints(QPainter::RenderHint::Antialiasing | QPainter::RenderHint::SmoothPixmapTransform | QPainter::RenderHint::HighQualityAntialiasing | QPainter::RenderHint::TextAntialiasing);

	QPen pen;

	painter.setPen(pen);

	document.drawContents(&painter);
}

void About::pushButtonLicenseClicked()
{
	License license(this);

	license.exec();
}
