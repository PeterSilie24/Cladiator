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

#include <QApplication>

#include "cladiator/cladiator.hpp"

#include "config/info.hpp"

#include "utils/icons.hpp"

#include "utils/reg-exp.hpp"

#include "utils/translator.hpp"

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);

	Settings settings;

	Translator translator;

	translator.loadTranslation(application, QLocale(settings.getLanguage()));

	Icons icons;

	application.setWindowIcon(icons.loadIcon(Info::getApplicationIconName()));

	RegExp regExp;

	Cladiator cladiator;

	cladiator.show();

	return application.exec();
}
