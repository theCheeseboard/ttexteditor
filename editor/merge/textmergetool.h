#ifndef TEXTMERGETOOL_H
#define TEXTMERGETOOL_H

#include <QWidget>

namespace Ui {
    class TextConflictResolution;
}

struct TextMergeToolPrivate;
class TextMergeTool : public QWidget {
        Q_OBJECT

    public:
        explicit TextMergeTool(QWidget* parent = nullptr);
        ~TextMergeTool();

        void loadGitDiff(QString diff);

        bool isConflictResolutionCompleted();
        QString conflictResolution();

    private:
        Ui::TextConflictResolution* ui;
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
