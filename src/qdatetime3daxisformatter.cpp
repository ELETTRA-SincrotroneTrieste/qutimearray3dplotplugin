#include "qdatetime3daxisformatter.h"
#include <QDateTime>
#include <QValue3DAxis>
#include <cumacros.h>
#include <QtDebug>

Q_DECLARE_METATYPE(QtDataVisualization::QValue3DAxisFormatter *)

using namespace  QtDataVisualization;

static const qreal oneDayMs = 60.0 * 60.0 * 24.0 * 1000.0;

class QDateTime3DAxisFormatterPrivate {
public:
    QDateTime m_originDateTime;
    QString m_selectionFormat;
};

QDateTime3DAxisFormatter::QDateTime3DAxisFormatter(QObject *parent) : QValue3DAxisFormatter(parent) {
    d = new QDateTime3DAxisFormatterPrivate;
    d->m_selectionFormat = "MM.dd hh:mm:ss";
    qRegisterMetaType<QValue3DAxisFormatter *>();
    printf("QDateTime3DAxisFormatter::QDateTime3DAxisFormatter \n");
}

QDateTime3DAxisFormatter::~QDateTime3DAxisFormatter() {
    delete d;
}

void QDateTime3DAxisFormatter::setDateTimeFormat(const QString &fmt) {
    d->m_selectionFormat = fmt;
}

QString QDateTime3DAxisFormatter::dateTimeFormat() const {
    return d->m_selectionFormat;
}

QValue3DAxisFormatter *QDateTime3DAxisFormatter::createNewInstance() const
{
    return new QDateTime3DAxisFormatter();
}

void QDateTime3DAxisFormatter::populateCopy(QValue3DAxisFormatter &copy) const
{
    QValue3DAxisFormatter::populateCopy(copy);

    QDateTime3DAxisFormatter *dateTimeFormatter = static_cast<QDateTime3DAxisFormatter *>(&copy);
    dateTimeFormatter->d->m_originDateTime = d->m_originDateTime;
    dateTimeFormatter->d->m_selectionFormat = d->m_selectionFormat;
}

void QDateTime3DAxisFormatter::recalculate()
{
    // We want our axis to always have gridlines at date breaks

    QValue3DAxisFormatter::recalculate();

    return;

    // Convert range into QDateTimes
    QDateTime minTime = valueToDateTime(qreal(axis()->min()));
    QDateTime maxTime = valueToDateTime(qreal(axis()->max()));

    qDebug() << __PRETTY_FUNCTION__ << minTime << maxTime;

    // Find out the grid counts
    QTime midnight(0, 0);
    QDateTime minFullDate(minTime.date(), midnight);
    int gridCount = 0;
    if (minFullDate != minTime)
        minFullDate = minFullDate.addDays(1);
    QDateTime maxFullDate(maxTime.date(), midnight);

    gridCount += minFullDate.daysTo(maxFullDate) + 1;
    int subGridCount = axis()->subSegmentCount() - 1;

    // Reserve space for position arrays and label strings
    gridPositions().resize(gridCount);
    subGridPositions().resize((gridCount + 1) * subGridCount);
    labelPositions().resize(gridCount);
    labelStrings().reserve(gridCount);

    // Calculate positions and format labels
    qint64 startMs = minTime.toMSecsSinceEpoch();
    qint64 endMs = maxTime.toMSecsSinceEpoch();
    qreal dateNormalizer = endMs - startMs;
    qreal firstLineOffset = (minFullDate.toMSecsSinceEpoch() - startMs) / dateNormalizer;
    qreal segmentStep = oneDayMs / dateNormalizer;
    qreal subSegmentStep = 0;
    if (subGridCount > 0)
        subSegmentStep = segmentStep / qreal(subGridCount + 1);

    for (int i = 0; i < gridCount; i++) {
        qreal gridValue = firstLineOffset + (segmentStep * qreal(i));
        gridPositions()[i] = float(gridValue);
        labelPositions()[i] = float(gridValue);
        labelStrings() << minFullDate.addDays(i).toString(axis()->labelFormat());
    }

    for (int i = 0; i <= gridCount; i++) {
        if (subGridPositions().size()) {
            for (int j = 0; j < subGridCount; j++) {
                float position;
                if (i)
                    position =  gridPositions().at(i - 1) + subSegmentStep * (j + 1);
                else
                    position =  gridPositions().at(0) - segmentStep + subSegmentStep * (j + 1);
                if (position > 1.0f || position < 0.0f)
                    position = gridPositions().at(0);
                subGridPositions()[i * subGridCount + j] = position;
            }
        }
    }
}

QString QDateTime3DAxisFormatter::stringForValue(qreal value, const QString &format) const
{
    Q_UNUSED(format)
    return valueToDateTime(value).toString(d->m_selectionFormat);
}

QDateTime QDateTime3DAxisFormatter::originDateTime() const
{
    return d->m_originDateTime;
}

QString QDateTime3DAxisFormatter::selectionFormat() const
{
    return d->m_selectionFormat;
}

void QDateTime3DAxisFormatter::setOriginDateTime(const QDateTime &datet)
{
    if (d->m_originDateTime != datet) {
        d->m_originDateTime = datet;
        markDirty(true);
        emit originDateTimeChanged(datet);
    }
}

void QDateTime3DAxisFormatter::setSelectionFormat(const QString &format)
{
    if (d->m_selectionFormat != format) {
        d->m_selectionFormat = format;
        markDirty(true); // Necessary to regenerate already visible selection label
        emit selectionFormatChanged(format);
    }
}

QDateTime QDateTime3DAxisFormatter::valueToDateTime(qreal msecs) const
{
    return d->m_originDateTime.addMSecs(msecs);
}

