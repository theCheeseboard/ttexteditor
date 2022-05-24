#include "statusbarbutton.h"

#include <QPainter>
#include <tvariantanimation.h>

struct StatusBarButtonPrivate {
        tVariantAnimation* anim;
        QColor idle;
        QColor raised;
        QColor pressed;
};

StatusBarButton::StatusBarButton(QWidget* parent) :
    QPushButton{parent} {
    d = new StatusBarButtonPrivate();

    d->idle = this->palette().color(QPalette::Window);
    d->idle.setAlpha(0);

    d->raised = d->idle.lighter();
    d->raised.setAlpha(200);

    d->pressed = d->idle.darker();
    d->pressed.setAlpha(200);

    d->anim = new tVariantAnimation(this);
    d->anim->setStartValue(d->idle);
    d->anim->setEndValue(d->idle);
    d->anim->setDuration(250);
    d->anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(d->anim, &tVariantAnimation::valueChanged, this, [=] {
        this->update();
    });

    QFont font = this->font();
    font.setPointSizeF(font.pointSizeF() * 0.8);
    this->setFont(font);

    this->setCursor(QCursor(Qt::PointingHandCursor));
}

StatusBarButton::~StatusBarButton() {
    delete d;
}

tPaintCalculator StatusBarButton::paintCalculator(QPainter* painter) const {
    tPaintCalculator calculator;
    calculator.setPainter(painter);
    calculator.setLayoutDirection(this->layoutDirection());
    calculator.setDrawBounds(this->size());

    QRect backgroundRect;
    backgroundRect.setSize(this->size());
    backgroundRect.moveTopLeft(QPoint(0, 0));
    calculator.addRect(backgroundRect, [=](QRectF bounds) {
        painter->fillRect(bounds, d->anim->currentValue().value<QColor>());
    });

    QRect textRect;
    textRect.moveTopLeft(SC_DPI_WT(QPoint(3, 3), QPoint, this));
    textRect.setHeight(this->fontMetrics().height());
    textRect.setWidth(this->fontMetrics().horizontalAdvance(this->text()));
    calculator.addRect("text", textRect, [=](QRectF bounds) {
        painter->drawText(bounds, Qt::AlignCenter, this->text());
    });

    return calculator;
}

void StatusBarButton::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    paintCalculator(&painter).performPaint();
}

QSize StatusBarButton::sizeHint() const {
    QRect textRect = paintCalculator(nullptr).boundsOf("text").toRect();
    textRect.adjust(-textRect.left(), -textRect.top(), textRect.left(), textRect.top());
    return textRect.size();
}

void StatusBarButton::enterEvent(QEnterEvent* event) {
    d->anim->stop();
    d->anim->setStartValue(d->anim->currentValue());
    d->anim->setEndValue(d->raised);
    d->anim->start();
}

void StatusBarButton::leaveEvent(QEvent* event) {
    d->anim->stop();
    d->anim->setStartValue(d->anim->currentValue());
    d->anim->setEndValue(d->idle);
    d->anim->start();
}

void StatusBarButton::mousePressEvent(QMouseEvent* event) {
    d->anim->stop();
    d->anim->setStartValue(d->anim->currentValue());
    d->anim->setEndValue(d->pressed);
    d->anim->start();
}

void StatusBarButton::mouseReleaseEvent(QMouseEvent* event) {
    d->anim->stop();
    d->anim->setStartValue(d->anim->currentValue());
    d->anim->setEndValue(d->raised);
    d->anim->start();
}
