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

#include "source_parameters.h"

#include <babblesynth.h>
#include <fftw3.h>
#include <private/qlogvalueaxis_p.h>
#include <qareaseries.h>
#include <qlogvalueaxis.h>

#include <cmath>
#include <new>
#include <stdexcept>

#include "app_state.h"
#include "clickable_label.h"

using namespace babblesynth::gui;

SourceParameters::SourceParameters(QWidget *parent) : QWidget(parent) {
    setObjectName("SourceParameters");
    setWindowTitle(tr("Source parameters"));
    setMinimumSize(QSize(540, 360));

    QComboBox *sourceType = new QComboBox(this);
    for (const auto &type : source::sources.values()) {
        sourceType->addItem(QString::fromStdString(type.name()));
    }

    m_sourceParams = new QFormLayout;

    m_sourceGraph = new QSplineSeries(this);

    QValueAxis *timeAxis = new QValueAxis(this);
    timeAxis->setRange(0, 1);

    QValueAxis *valueAxis = new QValueAxis(this);
    valueAxis->setRange(-1, 2);

    QChart *chart = new QChart;
    chart->legend()->hide();
    chart->addSeries(m_sourceGraph);
    chart->addAxis(timeAxis, Qt::AlignBottom);
    chart->addAxis(valueAxis, Qt::AlignLeft);
    chart->setTitle(tr("Glottal source waveform"));
    chart->setTheme(QChart::ChartThemeBlueIcy);

    m_sourceGraph->attachAxis(timeAxis);
    m_sourceGraph->attachAxis(valueAxis);

    QChartView *chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);

    m_spectrum = new QLineSeries(this);

    QAreaSeries *spectrumArea = new QAreaSeries(m_spectrum);

    QValueAxis *freqAxis = new QValueAxis(this);
    freqAxis->setRange(0, 10'000);

    QValueAxis *magAxis = new QValueAxis(this);
    magAxis->setRange(-90, 110);

    QChart *spectrum = new QChart;
    spectrum->legend()->hide();
    spectrum->addSeries(spectrumArea);
    spectrum->addAxis(freqAxis, Qt::AlignBottom);
    spectrum->addAxis(magAxis, Qt::AlignLeft);
    spectrum->setTitle(tr("Glottal source spectrum"));
    spectrum->setTheme(QChart::ChartThemeBlueIcy);

    spectrumArea->attachAxis(freqAxis);
    spectrumArea->attachAxis(magAxis);

    QChartView *spectrumView = new QChartView(spectrum, this);
    spectrumView->setRenderHint(QPainter::Antialiasing);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addLayout(m_sourceParams);
    bottomLayout->addWidget(chartView);
    bottomLayout->addWidget(spectrumView);

    QVBoxLayout *rootLayout = new QVBoxLayout;
    rootLayout->addWidget(sourceType);
    rootLayout->addLayout(bottomLayout);
    setLayout(rootLayout);

    updateFields();
    redrawGraph();

    QObject::connect(sourceType, &QComboBox::currentTextChanged, this,
                     &SourceParameters::onSourceTypeChanged);
}

void SourceParameters::onSourceTypeChanged(const QString &name) {
    appState->source()
        ->getParameter("Source type")
        .setValue(source::sources.valueOf(name.toStdString()));
    updateFields();
}

void SourceParameters::updateFields() {
    // Clear the QFormLayout
    while (m_sourceParams->count() > 0) {
        QLayoutItem *item = m_sourceParams->takeAt(0);
        QWidget *widget = item->widget();
        if (widget != nullptr) {
            widget->deleteLater();
        }
        delete item;
    }

    auto sourceGenerator = appState->source();

    for (const auto &name : sourceGenerator->getSource()->getParameterNames()) {
        addField(sourceGenerator->getSource()->getParameter(name));
    }

    layout()->update();
}

void SourceParameters::addField(parameter &param) {
    QString name = QString::fromStdString(param.name());
    const std::string type = param.type();

    QWidget *field;

    if (type == "int") {
        auto spin = new QSpinBox;
        if (param.isRanged()) {
            spin->setRange(param.min<int>(), param.max<int>());
        }
        spin->setValue(param.value<int>());
        QObject::connect(spin, qOverload<int>(&QSpinBox::valueChanged), this,
                         [this, spin, &param](int value) {
                             auto o = param.setValue(value);
                             if (!o.has_value()) {
                                 redrawGraph();
                             } else {
                                 spin->setValue(o.value());
                             }
                         });
        field = spin;
    } else if (type == "double") {
        auto spin = new QDoubleSpinBox;
        spin->setDecimals(2);
        spin->setSingleStep(0.01);
        if (param.isRanged()) {
            spin->setRange(param.min<double>(), param.max<double>());
        }
        spin->setValue(param.value<double>());
        QObject::connect(spin, qOverload<double>(&QDoubleSpinBox::valueChanged),
                         this, [this, spin, &param](double value) {
                             auto o = param.setValue(value);
                             if (!o.has_value()) {
                                 redrawGraph();
                             } else {
                                 spin->setValue(o.value());
                             }
                         });
        field = spin;
    } else if (type == "bool") {
        auto check = new QCheckBox;
        check->setChecked(param.value<bool>());
        QObject::connect(check, &QCheckBox::stateChanged, this,
                         [this, check, &param](const int state) {
                             auto o = param.setValue(state == Qt::Checked);
                             if (!o.has_value()) {
                                 redrawGraph();
                             } else {
                                 check->setChecked(o.value());
                             }
                         });
        field = check;
    } else {
        throw std::invalid_argument(
            "this param type can't be used in source params");
    }

    // Pre-defined parameter names and descriptions.
    QString helpText;

    if (name == "Oq") {
        name = "O<sub>q</sub>";
        helpText =
            tr("the open quotient, defined by the ratio between the open phase "
               "duration and the fundamental period. It translates spectrally "
               "to a lower or higher frequency peak in the glottal spectrum, "
               "also known as the glottal \"formant\".");
    } else if (name == "am") {
        name = utf8("αₘ");  // small alpha + subscript m
        helpText = tr("the asymmetry coefficient, defined by the ratio between "
                      "the opening phase and the open phase durations. It "
                      "translates spectrally to a wider or narrower bandwidth "
                      "of the peak in the glottal spectrum. It can also be "
                      "defined in terms of %2, the speed quotient, sometimes "
                      "used in scientific literature: %1 = %2 / (1 + %2)")
                       .arg(utf8("αₘ"))
                       .arg("S<sub>q</sub>");
    } else if (name == "Qa") {
        name = utf8("Qₐ");  // Q + subscript a
        helpText = tr("the return phase quotient, defined by the ratio between "
                      "the return phase time constant and the duration between "
                      "the glottal closing instant and the end of the period: "
                      "%1 = %2 / [(1 - %3) %4]. It translates to a flatter or "
                      "steeper spectrum, also called spectral tilt or rolloff.")
                       .arg(utf8("Qₐ"))
                       .arg(utf8("Tₐ"))
                       .arg("O<sub>q</sub>")
                       .arg(utf8("T₀"));
    }

    QPixmap pixmap = QIcon(":/icons/help.png").pixmap(QSize(24, 24));

    ClickableLabel *helpIcon = new ClickableLabel;
    helpIcon->setPixmap(pixmap);
    helpIcon->setCursor(QCursor(Qt::PointingHandCursor));

    QObject::connect(helpIcon, &ClickableLabel::clicked,
                     [this, name, helpText](QMouseEvent *event) {
                         QMessageBox dialog(
                             QMessageBox::NoIcon,
                             tr("Parameter info for %1").arg(name),
                             QString("%1: %2").arg(name).arg(helpText),
                             QMessageBox::Ok, this);
                         dialog.exec();
                     });

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(new QLabel(name));
    layout->addWidget(field);
    layout->addWidget(helpIcon);

    m_sourceParams->addRow(layout);
}

void SourceParameters::redrawGraph() {
    constexpr int nPoints = 1200;

    QVector<QPointF> points(nPoints);

    for (int i = 0; i < nPoints; ++i) {
        const double t = i / double(nPoints - 1);
        const double theta = 2 * M_PI * t;

        const double y =
            appState->source()->getSource()->evaluateAtPhase(theta);

        points[i].setX(t);
        points[i].setY(y);
    }

    m_sourceGraph->replace(points);

    const int fs = 20050;
    const int f0 = 150;
    const int nfft = 1024;

    m_spectrum->replace(calculateSpectrum(fs, f0, nfft));

    layout()->update();
}

QVector<QPointF> SourceParameters::calculateSpectrum(int fs, int f0, int nfft) {
    // Prepare FFT memory and plan.
    auto x = fftw_alloc_real(nfft);
    auto plan = fftw_plan_r2r_1d(nfft, x, x, FFTW_R2HC, FFTW_MEASURE);

    // Period duration in samples
    const int periodSamps = std::round(double(fs) / double(f0));

    // Factor to scale the spectrum to the same amplitude
    const int nPeriods = nfft / periodSamps;

    // Length of generated source.
    const int genSamps = nPeriods * periodSamps;

    // Offset in the array (to center it).
    const int tOffset = nfft / 2 - genSamps / 2;

    // Generate samples for one period.
    std::vector<double> onePeriod(periodSamps);

    double avg = 0;

    for (int i = 0; i < periodSamps; ++i) {
        const double t = i / (periodSamps - 1.0);
        const double theta = 2 * M_PI * t;

        avg += onePeriod[i] =
            appState->source()->getSource()->evaluateAtPhase(theta);
    }

    avg /= periodSamps;

    // Calculate the average in order to remove DC.
    for (int i = 0; i < periodSamps; ++i) {
        onePeriod[i] -= avg;
    }

    // Then replicate it nPeriods times.
    for (int i = 0; i < tOffset; ++i) {
        x[i] = 0.0;
    }
    for (int i = tOffset + genSamps; i < nfft; ++i) {
        x[i] = 0.0;
    }

    for (int k = 0, i = tOffset; k < nPeriods; ++k, i += periodSamps) {
        std::copy(onePeriod.cbegin(), onePeriod.cend(), &x[i]);
    }

    // Apply Blackman-Nutall window.
    constexpr double wa0 = 0.3635819;
    constexpr double wa1 = 0.4891775;
    constexpr double wa2 = 0.1365995;
    constexpr double wa3 = 0.0106411;

    for (int i = 0; i < genSamps; ++i) {
        x[tOffset + i] *= wa0 -
                          wa1 * std::cos((2 * M_PI * i) / (genSamps - 1)) +
                          wa2 * std::cos((4 * M_PI * i) / (genSamps - 1)) -
                          wa3 * std::cos((6 * M_PI * i) / (genSamps - 1));
    }

    // Execute FFT.
    fftw_execute(plan);

    // Create the series data.
    QVector<QPointF> data(nfft / 2);

    for (int i = 0; i < nfft / 2; ++i) {
        const double zr = x[i];
        const double zi = i > 0 ? x[nfft - 1 - i] : 0.0;

        const double f = i / (nfft / 2.0 + 1) * (fs / 2.0);
        const double a = zr * zr + zi * zi;
        const double w = 20.0 * std::log10(a > 0 ? a : 1e-100);

        data[i].setX(f);
        data[i].setY(w);
    }

    fftw_free(x);
    fftw_destroy_plan(plan);

    return data;
}