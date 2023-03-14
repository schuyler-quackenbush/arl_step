#ifndef ANGLELABEL_H
#define ANGLELABEL_H

#include <QLabel>

class AngleLabel : public QLabel
{
    Q_OBJECT

public:
    explicit AngleLabel(QWidget *parent=0);
    explicit AngleLabel(const QString &text, int angle, QWidget *parent=0);
    int angle;

protected:
    void paintEvent(QPaintEvent*);
    QSize sizeHint() const ;
    QSize minimumSizeHint() const;
};

#endif // ANGLELABEL_H
