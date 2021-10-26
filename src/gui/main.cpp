/*
 * BabbleSynth
 * Copyright (C) 2021  Clo Yun-Hee Dufour
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <QtWidgets>

#include "app_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int id = QFontDatabase::addApplicationFont(":/fonts/Roboto-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    app.setFont(QFont(family));

    QFile stylesheetFile(":/qdarkstyle/dark/style.qss");
    stylesheetFile.open(QFile::ReadOnly | QFile::Text);

    QTextStream stylesheetStream(&stylesheetFile);
    app.setStyleSheet(stylesheetStream.readAll());

    babblesynth::gui::AppWindow appWindow;
    appWindow.setVisible(true);

    return app.exec();
}
