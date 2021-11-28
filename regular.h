#ifndef REGUALR_H
#define REGUALR_H
#include <QVector>
#include <QStack>
#include <QList>
#include <QHash>
#include <list>
#include <QPair>


#include "varargs.h"
typedef struct Node{
    QString str;
    std::list<Node *> child;
    // 不维护father
    std::list<Node *> father;
    Node()=default;
    Node(const QString& s):str(s){};
}Node;

class Regular{

private:

//     friend class CDriver;  //声明 CDriver 为友元类
    Node * root, *end;
    QChar non_terminal=EPSILON;
    QList<QString> deductions;

    void _integrat(QString cache, QList<QString>& output, Node* node);

    QPair<Node*, Node*> connect(QPair<Node*, Node*> start, QPair<Node*, Node*> end);

    QPair<Node*, Node*> select(QPair<Node*, Node*> start, QPair<Node*, Node*> end);

    friend class Grammar;

public:

    void integrat();

    Regular(QChar _non_terminal=EPSILON);

    void insert_deduction(const QString& deduction);

    QList<QString> get_deductions();

    QChar get_non_terminal();

    bool is_opeartor(QChar c);

    void erase(int index);

    void erase(QVector<int> input);

    bool bring_in(Regular& regular);

    Regular clean_left_cursion(QChar regular_name);

    void _clean_Left_common_factor(int& regular_name, QVector<Regular>& vector);
    QVector<Regular> clean_Left_common_factor(int& regular_name);


};

#endif // REGUALR_H
