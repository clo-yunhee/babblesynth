﻿/*
 * BabbleSynth
 * Copyright (C) 2022  Clo Yun-Hee Dufour
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
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include "widgets/app_window.h"

using namespace xercesc;

int main(int argc, char* argv[]) {
    try {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        std::cout << "Error during initialization! :\n" << message << "\n";
        XMLString::release(&message);
        return EXIT_FAILURE;
    }

    QApplication app(argc, argv);

    QApplication::setWindowIcon(QIcon(":/icons/appicon.png"));

    int id = QFontDatabase::addApplicationFont(":/fonts/Roboto-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    app.setFont(QFont(family));

    QFile stylesheetFile(":/qdarkstyle/dark/style.qss");
    stylesheetFile.open(QFile::ReadOnly | QFile::Text);

    QTextStream stylesheetStream(&stylesheetFile);
    app.setStyleSheet(stylesheetStream.readAll());

    babblesynth::gui::AppWindow appWindow;
    appWindow.setVisible(true);

    int exitCode = app.exec();

    XMLPlatformUtils::Terminate();

    return exitCode;
}
