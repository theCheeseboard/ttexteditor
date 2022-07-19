#include "textmergetool.h"
#include "ui_textmergetool.h"

#include "conflictresolutiontexteditorrenderstep.h"
#include "differ.h"
#include <QActionGroup>
#include <QFile>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <libcontemporary_global.h>
#include <tlogger.h>

struct TextMergeToolPrivate {
        struct ConflictResolutionZone {
                enum ResolveDirection {
                    NoResolution,
                    ResolveLeft,
                    ResolveRight,
                    ResolveLeftThenRight,
                    ResolveRightThenLeft
                };

                QString leftContent;
                QString rightContent;
                ResolveDirection resolveDirection = NoResolution;

                QString effectiveLeftContent() const;
        };

        QList<ConflictResolutionZone> resolutionZones;
        bool diffCalculated = false;
};

TextMergeTool::TextMergeTool(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::TextMergeTool) {
    ui->setupUi(this);
    d = new TextMergeToolPrivate();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->spinner->setFixedSize(SC_DPI_WT(QSize(32, 32), QSize, this));
    ui->resolutionArea->setFixedWidth(SC_DPI_W(50, this));

    ui->leftTextEditor->setReadOnly(true);
    ui->rightTextEditor->setReadOnly(true);
    ui->leftTextEditor->pushRenderStep(new ConflictResolutionTextEditorRenderStep(ui->leftTextEditor));
    ui->rightTextEditor->pushRenderStep(new ConflictResolutionTextEditorRenderStep(ui->rightTextEditor));

    connect(ui->leftTextEditor->verticalScrollBar(), &QScrollBar::valueChanged, this, [this] {
        ui->resolutionArea->update();
        ui->rightTextEditor->verticalScrollBar()->setValue(ui->leftTextEditor->verticalScrollBar()->value());
    });
    connect(ui->rightTextEditor->verticalScrollBar(), &QScrollBar::valueChanged, this, [this] {
        ui->resolutionArea->update();

        // TODO: Something smarter with the scrolling
        ui->leftTextEditor->verticalScrollBar()->setValue(ui->rightTextEditor->verticalScrollBar()->value());
    });

    ui->resolutionArea->installEventFilter(this);
}

TextMergeTool::~TextMergeTool() {
    delete ui;
    delete d;
}

QCoro::Task<> TextMergeTool::loadDiff(QString file1, QString file2) {
    ui->stackedWidget->setCurrentWidget(ui->progressPage);
    d->diffCalculated = false;
    emit conflictResolutionCompletedChanged();
    bool equating = false;

    auto differ = Differ(file1.split("\n"), file2.split("\n"));
    auto results = co_await differ.diff();
    if (results.isEmpty()) {
        TextMergeToolPrivate::ConflictResolutionZone zone;
        zone.leftContent = file1;
        zone.rightContent = file2;
        d->resolutionZones.append(zone);
    } else {
        Differ<QString>::DiffResult::Type currentType = results.first().type;

        TextMergeToolPrivate::ConflictResolutionZone currentZone;

        for (const auto& result : results) {
            if (result.type == Differ<QString>::DiffResult::Type::Equal) {
                if (currentType != Differ<QString>::DiffResult::Type::Equal) {
                    // End of resolution zone
                    d->resolutionZones.append(currentZone);
                    currentZone = TextMergeToolPrivate::ConflictResolutionZone();
                }
                currentZone.leftContent += result.item + "\n";
                currentZone.rightContent += result.item + "\n";
            } else {
                if (currentType == Differ<QString>::DiffResult::Type::Equal) {
                    // End of resolution zone
                    d->resolutionZones.append(currentZone);
                    currentZone = TextMergeToolPrivate::ConflictResolutionZone();
                }

                if (result.type == Differ<QString>::DiffResult::Type::Remove) {
                    currentZone.leftContent += result.item + "\n";
                } else {
                    currentZone.rightContent += result.item + "\n";
                }
            }

            currentType = result.type;
        }

        d->resolutionZones.append(currentZone);
    }

    d->diffCalculated = true;
    emit conflictResolutionCompletedChanged();
    this->loadResolutionZones();
}

void TextMergeTool::loadResolutionZones() {
    int leftScroll = ui->leftTextEditor->verticalScrollBar()->value();
    int rightScroll = ui->rightTextEditor->verticalScrollBar()->value();
    ui->leftTextEditor->clearLineProperties("conflictResolution");
    ui->rightTextEditor->clearLineProperties("conflictResolution");

    QString leftContent;
    QString rightContent;
    for (const auto& zone : d->resolutionZones) {
        leftContent.append(zone.effectiveLeftContent());
        rightContent.append(zone.rightContent);
    }
    ui->leftTextEditor->setText(leftContent);
    ui->rightTextEditor->setText(rightContent);

    int leftLine = 0;
    int rightLine = 0;
    for (const auto& zone : d->resolutionZones) {
        int leftLength = zone.effectiveLeftContent().split("\n").length() - 1;
        int rightLength = zone.rightContent.split("\n").length() - 1;

        // Check if this is a conflict resolution zone
        if (zone.leftContent != zone.rightContent) {
            int base = zone.resolveDirection == TextMergeToolPrivate::ConflictResolutionZone::NoResolution ? 0 : 5;

            if (leftLength == 0) {
                ui->leftTextEditor->setLineProperty(leftLine, "conflictResolution", 4 + base);
            } else {
                for (auto i = 0; i < leftLength; i++) {
                    if (i == 0 && i == leftLength - 1) {
                        ui->leftTextEditor->setLineProperty(leftLine + i, "conflictResolution", 3 + base);
                    } else if (i == 0) {
                        ui->leftTextEditor->setLineProperty(leftLine + i, "conflictResolution", 0 + base);
                    } else if (i == leftLength - 1) {
                        ui->leftTextEditor->setLineProperty(leftLine + i, "conflictResolution", 1 + base);
                    } else {
                        ui->leftTextEditor->setLineProperty(leftLine + i, "conflictResolution", 2 + base);
                    }
                }
            }

            if (rightLength == 0) {
                ui->rightTextEditor->setLineProperty(rightLine, "conflictResolution", 4 + base);
            } else {
                for (auto i = 0; i < rightLength; i++) {
                    if (i == 0 && i == rightLength - 1) {
                        ui->rightTextEditor->setLineProperty(rightLine + i, "conflictResolution", 3 + base);
                    } else if (i == 0) {
                        ui->rightTextEditor->setLineProperty(rightLine + i, "conflictResolution", 0 + base);
                    } else if (i == rightLength - 1) {
                        ui->rightTextEditor->setLineProperty(rightLine + i, "conflictResolution", 1 + base);
                    } else {
                        ui->rightTextEditor->setLineProperty(rightLine + i, "conflictResolution", 2 + base);
                    }
                }
            }
        }

        leftLine += leftLength;
        rightLine += rightLength;
    }

    ui->leftTextEditor->verticalScrollBar()->setValue(leftScroll);
    ui->rightTextEditor->verticalScrollBar()->setValue(rightScroll);
    ui->resolutionArea->update();
    ui->stackedWidget->setCurrentWidget(ui->diffPage);
}

QList<QPolygon> TextMergeTool::renderResolutionZones() {
    QList<QPolygon> polygons;

    int leftLine = 0;
    int rightLine = 0;
    for (const auto& zone : d->resolutionZones) {
        int leftLength = zone.effectiveLeftContent().split("\n").length() - 1;
        int rightLength = zone.rightContent.split("\n").length() - 1;

        // Check if this is a conflict resolution zone
        if (zone.leftContent == zone.rightContent) {
            polygons.append(QPolygon());
        } else {
            QPolygon poly;

            poly.append({QPoint(0, ui->leftTextEditor->lineTop(leftLine) - ui->leftTextEditor->verticalScrollBar()->value()),
                QPoint(ui->resolutionArea->width(), ui->rightTextEditor->lineTop(rightLine) - ui->rightTextEditor->verticalScrollBar()->value()),
                QPoint(ui->resolutionArea->width(), ui->rightTextEditor->lineTop(rightLine + rightLength) - ui->rightTextEditor->verticalScrollBar()->value()),
                QPoint(0, ui->leftTextEditor->lineTop(leftLine + leftLength) - ui->leftTextEditor->verticalScrollBar()->value())});

            polygons.append(poly);
        }

        leftLine += leftLength;
        rightLine += rightLength;
    }
    return polygons;
}

bool TextMergeTool::isConflictResolutionCompleted() {
    if (!d->diffCalculated) return false;
    for (auto zone : d->resolutionZones) {
        if (zone.leftContent != zone.rightContent && zone.resolveDirection == TextMergeToolPrivate::ConflictResolutionZone::NoResolution) return false;
    }
    return true;
}

QString TextMergeTool::conflictResolution() {
    QString content;
    for (auto zone : d->resolutionZones) {
        content.append(zone.effectiveLeftContent());
    }
    return content;
}

QString TextMergeToolPrivate::ConflictResolutionZone::effectiveLeftContent() const {
    switch (resolveDirection) {
        case TextMergeToolPrivate::ConflictResolutionZone::NoResolution:
            return leftContent;
            break;
        case TextMergeToolPrivate::ConflictResolutionZone::ResolveLeft:
            return leftContent;
            break;
        case TextMergeToolPrivate::ConflictResolutionZone::ResolveRight:
            return rightContent;
            break;
        case TextMergeToolPrivate::ConflictResolutionZone::ResolveLeftThenRight:
            return leftContent + rightContent;
            break;
        case TextMergeToolPrivate::ConflictResolutionZone::ResolveRightThenLeft:
            return rightContent + leftContent;
            break;
    };
    return "";
}

bool TextMergeTool::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->resolutionArea) {
        if (event->type() == QEvent::Paint) {
            QPainter painter(ui->resolutionArea);

            auto zones = this->renderResolutionZones();
            for (int i = 0; i < zones.length(); i++) {
                auto poly = zones.at(i);
                auto zone = d->resolutionZones.at(i);

                if (poly.length() == 0) continue;

                QColor background;
                QColor border;

                if (zone.resolveDirection == TextMergeToolPrivate::ConflictResolutionZone::NoResolution) {
                    background = QColor(200, 0, 0, 100);
                    border = Qt::red;
                } else {
                    background = QColor(127, 127, 127, 100);
                    border = this->palette().color(QPalette::WindowText);
                }

                painter.setPen(Qt::transparent);
                painter.setBrush(background);
                painter.drawPolygon(poly);

                painter.setPen(border);
                painter.drawLine(poly.at(0), poly.at(1));
                painter.drawLine(poly.at(2) - QPoint(0, 1), poly.at(3) - QPoint(0, 1));

                QString icon;
                switch (zone.resolveDirection) {
                    case TextMergeToolPrivate::ConflictResolutionZone::NoResolution:
                        icon = "vcs-resolve-none";
                        break;
                    case TextMergeToolPrivate::ConflictResolutionZone::ResolveLeft:
                        icon = "vcs-resolve-left";
                        break;
                    case TextMergeToolPrivate::ConflictResolutionZone::ResolveRight:
                        icon = "vcs-resolve-right";
                        break;
                    case TextMergeToolPrivate::ConflictResolutionZone::ResolveLeftThenRight:
                        icon = "vcs-resolve-left-right";
                        break;
                    case TextMergeToolPrivate::ConflictResolutionZone::ResolveRightThenLeft:
                        icon = "vcs-resolve-right-left";
                        break;
                }

                QRect iconRect;
                iconRect.setSize(SC_DPI_WT(QSize(24, 24), QSize, this));
                iconRect.moveCenter(poly.boundingRect().center());

                QImage iconImage = QIcon::fromTheme(icon).pixmap(iconRect.size()).toImage();
                libContemporaryCommon::tintImage(iconImage, this->palette().color(QPalette::WindowText));
                painter.drawImage(iconRect, iconImage);
            }
        } else if (event->type() == QEvent::MouseButtonPress) {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() != Qt::LeftButton) return false;

            auto zones = this->renderResolutionZones();
            for (int i = 0; i < zones.length(); i++) {
                auto poly = zones.at(i);
                if (poly.containsPoint(mouseEvent->pos(), Qt::OddEvenFill)) {
                    auto* menu = new QMenu();
                    auto* leftAction = menu->addAction(tr("Take Left"), this, [=] {
                        d->resolutionZones[i].resolveDirection = TextMergeToolPrivate::ConflictResolutionZone::ResolveLeft;
                        this->loadResolutionZones();

                        emit conflictResolutionCompletedChanged();
                    });
                    auto* rightAction = menu->addAction(tr("Take Right"), this, [=] {
                        d->resolutionZones[i].resolveDirection = TextMergeToolPrivate::ConflictResolutionZone::ResolveRight;
                        this->loadResolutionZones();

                        emit conflictResolutionCompletedChanged();
                    });
                    menu->addSeparator();
                    auto* leftRightAction = menu->addAction(tr("Take Left and then Right"), this, [=] {
                        d->resolutionZones[i].resolveDirection = TextMergeToolPrivate::ConflictResolutionZone::ResolveLeftThenRight;
                        this->loadResolutionZones();

                        emit conflictResolutionCompletedChanged();
                    });
                    auto* rightLeftAction = menu->addAction(tr("Take Right and then Left"), this, [=] {
                        d->resolutionZones[i].resolveDirection = TextMergeToolPrivate::ConflictResolutionZone::ResolveRightThenLeft;
                        this->loadResolutionZones();

                        emit conflictResolutionCompletedChanged();
                    });

                    leftAction->setCheckable(true);
                    rightAction->setCheckable(true);
                    leftRightAction->setCheckable(true);
                    rightLeftAction->setCheckable(true);

                    auto actionGroup = new QActionGroup(menu);
                    actionGroup->addAction(leftAction);
                    actionGroup->addAction(rightAction);
                    actionGroup->addAction(leftRightAction);
                    actionGroup->addAction(rightLeftAction);

                    switch (d->resolutionZones.at(i).resolveDirection) {
                        case TextMergeToolPrivate::ConflictResolutionZone::NoResolution:
                            break;
                        case TextMergeToolPrivate::ConflictResolutionZone::ResolveLeft:
                            leftAction->setChecked(true);
                            break;
                        case TextMergeToolPrivate::ConflictResolutionZone::ResolveRight:
                            rightAction->setChecked(true);
                            break;
                        case TextMergeToolPrivate::ConflictResolutionZone::ResolveLeftThenRight:
                            leftRightAction->setChecked(true);
                            break;
                        case TextMergeToolPrivate::ConflictResolutionZone::ResolveRightThenLeft:
                            rightLeftAction->setChecked(true);
                            break;
                    }

                    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
                    menu->popup(mouseEvent->globalPosition().toPoint());
                    return true;
                }
            }
        }
    }
    return false;
}
