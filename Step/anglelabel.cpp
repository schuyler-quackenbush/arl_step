#include "anglelabel.h"

#include <QPainter>

AngleLabel::AngleLabel(QWidget *parent)
    : QLabel(parent)
{
}

AngleLabel::AngleLabel(const QString &text, int angle, QWidget *parent)
: QLabel(text, parent)
{
    this->angle = angle;
}

void AngleLabel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::Dense1Pattern);

    painter.rotate(angle);

    painter.drawText(0,0, text());
}

QSize AngleLabel::minimumSizeHint() const
{
    QSize s = QLabel::minimumSizeHint();
    return QSize(s.height(), s.width());
}

QSize AngleLabel::sizeHint() const
{
    QSize s = QLabel::sizeHint();
    return QSize(s.height(), s.width());
}
