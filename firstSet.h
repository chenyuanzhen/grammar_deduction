#ifndef FIRST_H
#define FIRST_H
#include <QSet>
#include <QChar>
#include <QString>

#include "varargs.h"
class First
{
private:
    QSet<QChar> terminators_set;

public:
    bool has_epsilon=false;
    First()=default;
    void insert(const QChar& terminator);
    void combine(const First& first);
    bool isEmpty();
    bool contains(const QChar& terminator);
    void clear();
    bool is_has_intersection(const First& first);

    QString print();
};

#endif // FIRST_H
