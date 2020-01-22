#ifndef QDATETIME3DAXISFORMATTER_H
#define QDATETIME3DAXISFORMATTER_H

#include <QValue3DAxisFormatter>
#include <QDateTime>
#include <QVector>

class QDateTime3DAxisFormatterPrivate;

class QDateTime3DAxisFormatter : public QtDataVisualization::QValue3DAxisFormatter
{
    Q_OBJECT

    //! [1]
    Q_PROPERTY(QDateTime originDateTime READ originDateTime WRITE setOriginDateTime NOTIFY originDateTimeChanged)
    Q_PROPERTY(QString selectionFormat READ selectionFormat WRITE setSelectionFormat NOTIFY selectionFormatChanged)

public:
    explicit QDateTime3DAxisFormatter(QObject *parent = 0);
    virtual ~QDateTime3DAxisFormatter();

    //! [0]
    virtual QValue3DAxisFormatter *createNewInstance() const;
    virtual void populateCopy(QValue3DAxisFormatter &copy) const;
    virtual void recalculate();
    virtual QString stringForValue(qreal value, const QString &format) const;
    //! [0]

    QDateTime originDateTime() const;
    QString selectionFormat() const;
    QString dateTimeFormat() const;

public Q_SLOTS:
    void setOriginDateTime(const QDateTime &datet);
    void setSelectionFormat(const QString &format);
    void setDateTimeFormat(const QString &fmt);

Q_SIGNALS:
    void originDateTimeChanged(const QDateTime &date);
    void selectionFormatChanged(const QString &format);

private:
    Q_DISABLE_COPY(QDateTime3DAxisFormatter)

    QDateTime valueToDateTime(qreal msecs) const;
    QDateTime3DAxisFormatterPrivate *d;

};

#endif // QDATETIME3DAXISFORMATTER_H
