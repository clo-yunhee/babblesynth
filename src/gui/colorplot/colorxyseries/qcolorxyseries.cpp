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

#include "qcolorxyseries.h"

#include <qchartglobal.h>

#include <QAbstractAxis>
#include <QPainter>

#include "../coloraxis/qcoloraxis.h"
#include "qcolorxyseries_p.h"

QT_CHARTS_BEGIN_NAMESPACE

QColorXYSeries::QColorXYSeries(QColorXYSeriesPrivate &d, QObject *parent)
    : QXYSeries(d, parent) {}

/*!
    Removes the configuration of a point located at \a index
    and restores the default look derived from the series' settings.

    \note It doesn't affect the configuration of other points.
    \sa clearPointsConfiguration(), setPointConfiguration()
    \since 6.2
*/
void QColorXYSeries::clearPointConfiguration(const int index) {
    Q_D(QColorXYSeries);
    if (d->m_pointsConfiguration.contains(index)) {
        d->m_pointsConfiguration.remove(index);
        emit pointsConfigurationChanged(d->m_pointsConfiguration);
    }
}

/*!
    Removes the configuration property identified by \a key from the point at \a
   index and restores the default look derived from the series' settings.

    Removes the configuration type, such as color or size,
    specified by \a key from the point at \a index with configuration
   customizations, allowing that configuration property to be rendered as the
   default specified in the series' properties.

    \note It doesn't affect the configuration of other points.
    \sa clearPointsConfiguration(), setPointConfiguration()
    \since 6.2
*/
void QColorXYSeries::clearPointConfiguration(
    const int index, const QColorXYSeries::PointConfiguration key) {
    Q_D(QColorXYSeries);
    if (d->m_pointsConfiguration.contains(index)) {
        auto &conf = d->m_pointsConfiguration[index];
        if (conf.contains(key)) {
            conf.remove(key);
            d->m_pointsConfiguration[index] = conf;
            emit pointsConfigurationChanged(d->m_pointsConfiguration);
        }
    }
}

/*!
    Removes the configuration of all points in the series and restores
    the default look derived from the series' settings.

    \sa setPointConfiguration()
    \since 6.2
*/
void QColorXYSeries::clearPointsConfiguration() {
    Q_D(QColorXYSeries);
    d->m_pointsConfiguration.clear();
    emit pointsConfigurationChanged(d->m_pointsConfiguration);
}

/*!
    Removes the configuration property identified by \a key from all
    points and restores the default look derived from the series' settings.

    Removes the configuration type, such as color or size,
    specified by \a key from all points with configuration customizations,
    allowing that configuration property to be rendered as the default
    specified in the series properties.

    \sa clearPointsConfiguration(), setPointConfiguration()
    \since 6.2
*/
void QColorXYSeries::clearPointsConfiguration(
    const QColorXYSeries::PointConfiguration key) {
    Q_D(QColorXYSeries);
    bool needsUpdate = false;
    for (const int &index : d->m_pointsConfiguration.keys()) {
        auto &conf = d->m_pointsConfiguration[index];
        if (conf.contains(key)) {
            conf.remove(key);
            d->m_pointsConfiguration[index] = conf;
            needsUpdate = true;
        }
    }

    if (needsUpdate) emit pointsConfigurationChanged(d->m_pointsConfiguration);
}

/*!
    Enables customizing the appearance of a point located at \a index with
    desired \a configuration.

    With points configuration you can change various aspects of every point's
   appearance.

    A point's configuration is represented as a hash map with
   QColorXYSeries::pointConfiguration keys and QVariant values. For example:
    \code
    QLineSeries *series = new QLineSeries();
    series->setName("Customized serie");
    series->setPointsVisible(true);

    *series << QPointF(0, 6) << QPointF(2, 4) << QPointF(3, 6) << QPointF(7, 4)
   << QPointF(10, 5)
            << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18,
   3)
            << QPointF(20, 2);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();

    QHash<QColorXYSeries::PointConfiguration, QVariant> conf;
    conf[QColorXYSeries::PointConfiguration::Color] = QColor(Qt::red);
    conf[QColorXYSeries::PointConfiguration::Size] = 8;
    conf[QColorXYSeries::PointConfiguration::LabelVisibility] = true;

    series->setPointConfiguration(4, conf);

    conf.remove(QColorXYSeries::PointConfiguration::LabelVisibility);
    series->setPointConfiguration(6, conf);
    \endcode

    In this example, you can see a default QLineSeries with 10 points
    and with changed configuration of two points. Both changed points are
    red and visibly bigger than the others with a look derived from series.
    By default, points don't have labels, but the point at index 4 has
    the label thanks to the QColorXYSeries::PointConfiguration::LabelVisibility
    configuration value.
    Below is an example of a chart created in this way:
    \image xyseries_point_configuration.png

    \sa pointsConfiguration(), clearPointsConfiguration()
    \since 6.2
*/
void QColorXYSeries::setPointConfiguration(
    const int index,
    const QHash<QColorXYSeries::PointConfiguration, QVariant> &configuration) {
    Q_D(QColorXYSeries);
    if (d->m_pointsConfiguration[index] != configuration) {
        d->m_pointsConfiguration[index] = configuration;
        emit pointsConfigurationChanged(d->m_pointsConfiguration);
    }
}

/*!
    Enables customizing a particular aspect of a point's configuration.

    \note Points configuration concept provides a flexible way to configure
   various aspects of a point's appearance. Thus, values need to have an elastic
   type such as QVariant. See QColorXYSeries::PointConfiguration to see what \a
   value should be passed for certain \a key. \sa pointsConfiguration()
    \since 6.2
*/
void QColorXYSeries::setPointConfiguration(
    const int index, const QColorXYSeries::PointConfiguration key,
    const QVariant &value) {
    Q_D(QColorXYSeries);
    QHash<QColorXYSeries::PointConfiguration, QVariant> conf;
    if (d->m_pointsConfiguration.contains(index))
        conf = d->m_pointsConfiguration[index];

    bool callSignal = false;
    if (conf.contains(key)) {
        if (conf[key] != value) callSignal = true;
    } else {
        callSignal = true;
    }

    conf[key] = value;
    d->m_pointsConfiguration[index] = conf;

    if (callSignal) {
        emit pointsConfigurationChanged(d->m_pointsConfiguration);
    }
}

/*!
    Enables customizing the configuration of multiple points as specified
    by \a pointsConfiguration.

    \sa pointsConfiguration()
    \since 6.2
*/
void QColorXYSeries::setPointsConfiguration(
    const QHash<int, QHash<QColorXYSeries::PointConfiguration, QVariant>>
        &pointsConfiguration) {
    Q_D(QColorXYSeries);
    if (d->m_pointsConfiguration != pointsConfiguration) {
        d->m_pointsConfiguration = pointsConfiguration;
        emit pointsConfigurationChanged(d->m_pointsConfiguration);
    }
}

/*!
    Returns a map representing the configuration of a point at \a index.

    With points configuration you can change various aspects of each point's
   look.

    \sa setPointConfiguration()
    \since 6.2
*/
QHash<QColorXYSeries::PointConfiguration, QVariant>
QColorXYSeries::pointConfiguration(const int index) const {
    Q_D(const QColorXYSeries);
    return d->m_pointsConfiguration[index];
}

/*!
    Returns a map with points' indexes as keys and points' configuration as
   values.

    \sa setPointConfiguration(), pointConfiguration()
    \since 6.2
*/
QHash<int, QHash<QColorXYSeries::PointConfiguration, QVariant>>
QColorXYSeries::pointsConfiguration() const {
    Q_D(const QColorXYSeries);
    return d->m_pointsConfiguration;
}

/*!
    Sets the points' sizes according to a passed list of values. Values from
    \a sourceData are sorted and mapped to a point size which is between \a
   minSize and \a maxSize.

    \note If \a sourceData length is smaller than number of points in the
   series, then size of the points at the end of the series will stay the same.
    \sa setPointConfiguration(), pointConfiguration()
    \since 6.2
*/
void QColorXYSeries::sizeBy(const QVector<qreal> &sourceData,
                            const qreal minSize, const qreal maxSize) {
    Q_D(QColorXYSeries);

    Q_ASSERT(minSize <= maxSize);
    Q_ASSERT(minSize >= 0);

    qreal min = std::numeric_limits<qreal>::max();
    qreal max = -std::numeric_limits<qreal>::max();
    for (const auto &p : sourceData) {
        min = qMin(min, p);
        max = qMax(max, p);
    }

    const qreal range = max - min;

    for (int i = 0; i < sourceData.size() && i < d->m_points.size(); ++i) {
        qreal pointSize = minSize;
        if (range != 0) {
            const qreal startValue = sourceData.at(i) - min;
            const qreal percentage = startValue / range;
            pointSize = qMax(minSize, percentage * maxSize);
        }
        setPointConfiguration(i, QColorXYSeries::PointConfiguration::Size,
                              pointSize);
    }
}

/*!
    Sets the points' color according to a passed list of values. Values from
    \a sourceData are sorted and mapped to the \a gradient.

    If the series has a QColorAxis attached, then a gradient from the axis
    is going to be used.

    \sa setPointConfiguration(), pointConfiguration(), QColorAxis
    \since 6.2
*/
void QColorXYSeries::colorBy(const QVector<qreal> &sourceData,
                             const QLinearGradient &gradient) {
    Q_D(QColorXYSeries);

    d->m_colorByData = sourceData;
    if (d->m_colorByData.isEmpty()) return;

    const qreal imgSize = 100.0;

    qreal min = std::numeric_limits<qreal>::max();
    qreal max = std::numeric_limits<qreal>::min();
    for (const auto &p : sourceData) {
        min = qMin(min, p);
        max = qMax(max, p);
    }

    qreal range = max - min;

    QLinearGradient usedGradient = gradient;

    // Gradient will be taked from the first attached color axis.
    // If there are more color axis, they will have just changed range.
    bool axisFound = false;
    const auto axes = attachedAxes();
    for (const auto &axis : axes) {
        if (axis->type() == QColorAxis::AxisTypeColor) {
            QColorAxis *colorAxis = static_cast<QColorAxis *>(axis);
            if (!axisFound) {
                usedGradient =
                    QLinearGradient(QPointF(0, 0), QPointF(0, imgSize));
                const auto stops = colorAxis->gradient().stops();
                for (const auto &stop : stops)
                    usedGradient.setColorAt(stop.first, stop.second);

                if (!colorAxis->autoRange()) {
                    min = colorAxis->min();
                    max = colorAxis->max();
                    range = max - min;
                }

                axisFound = true;
            }

            if (colorAxis->autoRange()) colorAxis->setRange(min, max);
        }
    }

    QImage image(imgSize, imgSize, QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.fillRect(image.rect(), usedGradient);

    // To ensure that negative values will be well handled, distance from min to
    // 0 will be added to min and every single value. This will move entire
    // values list to positive only values.
    const qreal diff = min < 0 ? qAbs(min) : 0;
    min += diff;

    for (int i = 0; i < sourceData.size() && i < d->m_points.size(); ++i) {
        const qreal startValue = qMax(0.0, sourceData.at(i) + diff - min);
        const qreal percentage = startValue / range;
        QColor color =
            image.pixelColor(0, qMin(percentage * imgSize, imgSize - 1));
        setPointConfiguration(i, QColorXYSeries::PointConfiguration::Color,
                              color);
    }
}

QColorXYSeriesPrivate::QColorXYSeriesPrivate(QColorXYSeries *q)
    : QXYSeriesPrivate(q) {}

QVector<qreal> QColorXYSeriesPrivate::colorByData() const {
    return m_colorByData;
}

QT_CHARTS_END_NAMESPACE