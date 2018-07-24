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
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include "ui_about.h"

#include "config/info.hpp"

#include "utils/icons.hpp"

#include "license.hpp"

class About : public QDialog
{
	Q_OBJECT

public:
	About(QWidget* parent);

private slots:
	void pushButtonLicenseClicked();

private:
	Ui::AboutDialog ui;
};
