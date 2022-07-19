#ifndef TEXTMERGETOOL_H
#define TEXTMERGETOOL_H

#include "ttexteditor_global.h"
#include <QCoroTask>
#include <QWidget>

namespace Ui {
    class TextMergeTool;
}

struct TextMergeToolPrivate;
class TTEXTEDITOR_EXPORT TextMergeTool : public QWidget {
        Q_OBJECT

    public:
        explicit TextMergeTool(QWidget* parent = nullptr);
        ~TextMergeTool();

        QCoro::Task<> loadDiff(QString file1, QString file2);

        bool isConflictResolutionCompleted();
        QString conflictResolution();

    private:
        Ui::TextMergeTool* ui;
        TextMergeToolPrivate* d;

        void loadResolutionZones();

        QList<QPolygon> renderResolutionZones();

    signals:
        void conflictResolutionCompletedChanged();

        // QObject interface
    public:
        bool eventFilter(QObject* watched, QEvent* event);
};

#endif // TEXTMERGETOOL_H
