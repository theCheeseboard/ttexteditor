#ifndef STATUSBARBUTTON_H
#define STATUSBARBUTTON_H

#include <QPushButton>
#include <tpaintcalculator.h>

struct StatusBarButtonPrivate;
class StatusBarButton : public QPushButton {
        Q_OBJECT
    public:
        explicit StatusBarButton(QWidget* parent = nullptr);
        ~StatusBarButton();

    signals:

    private:
        StatusBarButtonPrivate* d;
        tPaintCalculator paintCalculator(QPainter* painter) const;

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
        QSize sizeHint() const;

        // QWidget interface
    protected:
        void enterEvent(QEnterEvent* event);
        void leaveEvent(QEvent* event);

        // QWidget interface
    protected:
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
};

#endif // STATUSBARBUTTON_H
