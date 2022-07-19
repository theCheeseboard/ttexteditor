#ifndef DIFFER_H
#define DIFFER_H

#include <QCoroFuture>
#include <QCoroTask>
#include <QList>
#include <QPoint>
#include <QtConcurrent>

template<typename T> class Differ {
    public:
        struct DiffResult {
                enum class Type {
                    Equal,
                    Add,
                    Remove
                };

                Type type;
                T item;
        };

        Differ(QList<T> from, QList<T> to) {
            this->from = from;
            this->to = to;
            this->table.fill(0, (from.size() + 1) * (to.size() + 1));
        }

        QCoro::Task<QList<DiffResult>> diff() {
            // Populate the table

            co_await QtConcurrent::map(this->table, [this](int& value) {
                int i = &value - this->table.data();
                auto coords = toCoords(i);
                if (coords.x() == 0 || coords.y() == 0) {
                    // First row and column is always 0
                    value = 0;
                } else if (fromVal(coords) == toVal(coords)) {
                    // Take the value from the top left cell and add one
                    value = tableAtCoords(coords + QPoint(-1, -1)) + 1;
                } else {
                    // Take the max value from the adjacent cells
                    value = qMax(tableAtCoords(coords + QPoint(-1, 0)), tableAtCoords(coords + QPoint(0, -1)));
                }
            });

            //            for (auto i = 0; i < table.length(); i++) {
            //                auto coords = toCoords(i);
            //                if (coords.x() == 0 || coords.y() == 0) {
            //                    // First row and column is always 0
            //                    table[i] = 0;
            //                } else if (fromVal(coords) == toVal(coords)) {
            //                    // Take the value from the top left cell and add one
            //                    table[i] = tableAtCoords(coords + QPoint(-1, -1)) + 1;
            //                } else {
            //                    // Take the max value from the adjacent cells
            //                    table[i] = qMax(tableAtCoords(coords + QPoint(-1, 0)), tableAtCoords(coords + QPoint(0, -1)));
            //                }
            //            }

            QList<DiffResult> results;
            QPoint searchPoint(from.size(), to.size());
            while (searchPoint.x() != 0 && searchPoint.y() != 0) {
                DiffResult result;
                if (from.at(searchPoint.x() - 1) == to.at(searchPoint.y() - 1)) {
                    // Found an item
                    result.type = DiffResult::Type::Equal;
                    result.item = from.at(searchPoint.x() - 1);
                    searchPoint += QPoint(-1, -1);
                } else if (tableAtCoords(searchPoint + QPoint(-1, 0)) > tableAtCoords(searchPoint + QPoint(0, -1))) {
                    result.type = DiffResult::Type::Remove;
                    result.item = from.at(searchPoint.x() - 1);
                    searchPoint += QPoint(-1, 0);
                } else {
                    result.type = DiffResult::Type::Add;
                    result.item = to.at(searchPoint.y() - 1);
                    searchPoint += QPoint(0, -1);
                }
                results.prepend(result);
            }

            //            for (auto result : results) co_yield result;
            co_return results;
        }

    private:
        int tableAtCoords(QPoint coords) {
            if (coords.x() < 0 || coords.y() < 0) return 0;
            return table.at(toIndex(coords));
        }

        int toIndex(QPoint coords) {
            return coords.y() * (from.length() + 1) + coords.x();
        }

        QPoint toCoords(int index) {
            return {static_cast<int>(index % (from.length() + 1)), static_cast<int>(index / (from.length() + 1))};
        }

        T fromVal(QPoint coords) {
            return from.at(coords.x() - 1);
        }

        T toVal(QPoint coords) {
            return to.at(coords.y() - 1);
        }

        QList<int> table;
        QList<T> from, to;
};

#endif // DIFFER_H
