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

#ifndef BABBLESYNTH_CHART_VIEW_H
#define BABBLESYNTH_CHART_VIEW_H

#include <babblesynth.h>

#include <QtCharts>
#include <QtWidgets>
#include <optional>

#include "colorplot/colorplot.h"

namespace babblesynth {
namespace gui {

class ChartView : public QChartView {
    Q_OBJECT

   public:
    ChartView(QChart* chart, QWidget* parent = nullptr);

    QPointF pointFromPos(const QPointF& pos) const;
    QPointF posFromPoint(const QPointF& point) const;

   signals:
    void mouseHovered(const QString& series, const QPointF& point, int index);
    void mouseLeft(const QString& series);

    void mouseDoubleClicked(const QString& series, const QPointF& point,
                            int index);
    void mouseRightClicked(const QString& series, const QPointF& point,
                           int index);

    void mousePressed(const QString& series, const QPointF& point, int index);
    void mouseReleased(const QString& series);
    void mouseDragging(const QString& series, const QPointF& point);

    void wheelScrolled(const QString& series, const QPointF& point, int index,
                       double dy);

   protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

   private:
    bool checkThrottle();

    QHash<QString, QVariant> findMatchingPoints(const QPointF& mousePos) const;

    QHash<QString, bool> m_seriesDragging;
    QHash<QString, bool> m_seriesRightClicking;

    QElapsedTimer m_eventThrottle;
};

}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_CHART_VIEW_H