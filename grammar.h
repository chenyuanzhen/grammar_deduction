#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <QVector>
#include <QList>
#include <QHash>
#include <QMap>

#include "varargs.h"
#include "regular.h"
#include "firstSet.h"


class Grammar
{
private:
    typedef First Follow;
    QChar start{EPSILON};
    // 目前用数字代替, 用于消除左递归中, 生成新的非终结符
    int special_char= 48;
    QHash<QChar, Regular> regular_hash;
    // int 0为不可推为终结符, 1为可推为终结符, 2为未访问
    bool find_can_be_end(QChar input, QHash<QChar, int>& can_end, QHash<QChar, QVector<bool>>& is_visit, QHash<QChar, bool>& can_reach);
    QHash<QChar, First> first_hash;
    QHash<QChar, Follow> follow_hash;
    First _find_first(const QChar& non_terminal);
public:
    Grammar();
//    void init();
    void simplify(QVector<QChar>& can_not_end, QVector<QChar>& can_not_reach);
    void set_start(QChar _start);
    void insert_grammar(QChar non_terminal, const QString& deduction);
    QString print_all_grammer();
    void turn_left_recursion_to_right();
    void turn_left_common_factor();

    void find_first();
    QString print_first_set();

    void find_follow();
    Follow add_follow(const QChar& c,  QHash<QChar, QVector<QChar>>& add_follow_hash);
    QString print_follow_set();

    QPair<QVector<QString>, int> deduce(const QString& str);



};



#endif // GRAMMAR_H
