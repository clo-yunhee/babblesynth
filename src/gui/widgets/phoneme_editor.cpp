/*
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

#include <QBoxLayout>
#include <QGroupBox>

#include "phoneme_editor.h"

using namespace babblesynth::gui;

PhonemeEditor::PhonemeEditor(QWidget *parent) : QWidget(parent) {
    setObjectName("PhonemeEditor");
    setWindowTitle(tr("Phoneme dictionary editor"));
    setMinimumSize(QSize(400, 800));

    QVBoxLayout *leftLayout = new QVBoxLayout;
    {
        m_mappingList = new QListWidget(this);

        leftLayout->addWidget(m_mappingList);
    }

    QVBoxLayout *rightLayout = new QVBoxLayout;
    {
        QGroupBox *recordBox = new QGroupBox(tr("Recording"));
        QHBoxLayout *recordLayout = new QHBoxLayout;
        recordBox->setLayout(recordLayout);
        {
            {
                m_recordButton = new QPushButton(tr("Record"));

                recordLayout->addStretch();
                recordLayout->addWidget(m_recordButton);
                recordLayout->addStretch();
            }
        }

        rightLayout->addWidget(recordBox);
        rightLayout->addStretch();
    }

    QHBoxLayout *rootLayout = new QHBoxLayout;

    rootLayout->addLayout(leftLayout);
    rootLayout->addLayout(rightLayout, 1);

    setLayout(rootLayout);
}
