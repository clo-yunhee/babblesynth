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

#ifndef QCOLORXYSERIES_H
#define QCOLORXYSERIES_H

#include <qchartglobal.h>

#include <QXYSeries>

QT_CHARTS_BEGIN_NAMESPACE

class QColorXYSeriesPrivate;

class QColorXYSeries : public QXYSeries {
    Q_OBJECT

   protected:
    explicit QColorXYSeries(QColorXYSeriesPrivate &d,
                            QObject *parent = nullptr);

   public:
    static constexpr SeriesType SeriesTypeColorLine =
        SeriesType(SeriesTypeCandlestick + 10);

    enum class PointConfiguration {
        Color = 0,
        Size,
        Visibility,
        LabelVisibility
    };
    Q_ENUM(PointConfiguration)

    void clearPointConfiguration(const int index);
    void clearPointConfiguration(const int index, const PointConfiguration key);
    void clearPointsConfiguration();
    void clearPointsConfiguration(const PointConfiguration key);
    void setPointConfiguration(
        const int index,
        const QHash<PointConfiguration, QVariant> &configuration);
    void setPointConfiguration(const int index, const PointConfiguration key,
                               const QVariant &value);
    void setPointsConfiguration(
        const QHash<int, QHash<PointConfiguration, QVariant>>
            &pointsConfiguration);
    QHash<PointConfiguration, QVariant> pointConfiguration(
        const int index) const;
    QHash<int, QHash<PointConfiguration, QVariant>> pointsConfiguration() const;

    void sizeBy(const QVector<qreal> &sourceData, const qreal minSize,
                const qreal maxSize);
    void colorBy(const QVector<qreal> &sourceData,
                 const QLinearGradient &gradient = QLinearGradient());

   Q_SIGNALS:
    void pointsConfigurationChanged(
        const QHash<int, QHash<PointConfiguration, QVariant>> &configuration);

   private:
    Q_DECLARE_PRIVATE(QColorXYSeries)
    friend class QColorAxisPrivate;
    friend class ColorXYChart;
};

inline uint qHash(QColorXYSeries::PointConfiguration key, uint seed) {
    return ::qHash(static_cast<uint>(key), seed);
}

QT_CHARTS_END_NAMESPACE

#endif  // QCOLORXYSERIES_H