/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtMath>

#include "coloraxis_p.h"

QT_CHARTS_BEGIN_NAMESPACE

QStringList ColorAxis::createColorLabels(qreal min, qreal max, int ticks,
                                         ChartPresenter *presenter) const {
    QStringList labels;

    if (max <= min || ticks < 1) return labels;

    int n = qMax(int(-qFloor(std::log10((max - min) / (ticks - 1)))), 0) + 1;
    for (int i = 0; i < ticks; ++i) {
        qreal value = min + (i * (max - min) / (ticks - 1));
        labels << presenter->numberToString(value, 'f', n);
    }

    return labels;
}

QT_CHARTS_END_NAMESPACE