#include "grammar.h"

#include <QDebug>
Grammar::Grammar()
{

}

void Grammar::insert_grammar(QChar non_terminal, const QString& deduction){

    if(!regular_hash.contains(non_terminal)){
        regular_hash.insert(non_terminal, Regular(non_terminal));
    }
    regular_hash[non_terminal].insert_deduction(deduction);

}


void Grammar::set_start(QChar _start){
    if(start == EPSILON)
        start = _start;
}
// true为非终结符
bool is_non_terminal(QChar _char){
     return !('a' <= _char && _char <= 'z') && _char != EPSILON;
}

bool Grammar::find_can_be_end(QChar non_terminal, QHash<QChar, int>& can_end, QHash<QChar, QVector<bool>>& is_visit, QHash<QChar, bool>& can_reach){
    can_reach[non_terminal] = true;
    if(can_end[non_terminal] != 2)
        return can_end[non_terminal];

    int index = 0 ;
    can_end[non_terminal] = 0;
    for(auto&deduction: regular_hash[non_terminal].get_deductions()){
        // 所有推导只访问一次, 防止递归死循环
        if(is_visit[non_terminal][index])
            continue;
        bool is_to_end = true;
        is_visit[non_terminal][index] = true;
        // 遍历推导规则
        for(auto& _char : deduction){
            // 遇见非终结符, 将其进行推导
            if(is_non_terminal(_char)){
                if(!find_can_be_end(_char, can_end, is_visit, can_reach)){
                    is_to_end = false;
                }
            }
        }
        // 说明存在一个推导, 可以推出全是终结符串
        if(is_to_end){
            can_end[non_terminal] = true;
        }
        ++index;
    }
    return can_end[non_terminal];
}

void Grammar::simplify(QVector<QChar>& can_not_end, QVector<QChar>& can_not_reach){
        // int 0为不可推为终结符, 1为可推为终结符, 2为未访问
    QHash<QChar, int> can_end;
    QHash<QChar, bool> can_reach;
    QHash<QChar, QVector<bool>> is_visit;
    for(auto&regular: regular_hash){
        can_reach.insert(regular.get_non_terminal(), false);
        is_visit.insert(regular.get_non_terminal(), QVector<bool>(regular.get_deductions().size(), false));
        can_end.insert(regular.get_non_terminal(), 2);
    }

    find_can_be_end(start, can_end, is_visit, can_reach);

    for(auto&c : can_end.keys()){
       if(!can_end[c]){
           can_not_end.push_back(c);
           qDebug() << c << " 不可终止"<< endl;
       }
    }
    for(auto&c : can_reach.keys()){
       if(!can_reach[c]){
           can_not_reach.push_back(c);
           qDebug() << c << " 不可到达"<< endl;
       }
    }
    // 删除含不可终止和不可到达的非终结符规则
    for(auto& _char : regular_hash.keys()){
        if(!can_end[_char] || !can_reach[_char]){
            regular_hash.erase(regular_hash.find(_char));
            continue;
        }
        int index = 0;
        QVector<int> wait_for_erase;
        auto deductions =  regular_hash[_char].get_deductions();
        for(auto iter =  deductions.begin(); iter != deductions.end(); ++iter, ++index){
             bool can_delete = false;

             for(auto& _char : *iter){
                 if(is_non_terminal(_char) && (!can_end[_char] || !can_reach[_char])){
                     can_delete = true;
                     break;
                 }
             }

             if(can_delete){
                 wait_for_erase.push_back(index);
             }
        }
         regular_hash[_char].erase(wait_for_erase);
    }


}

void Grammar::turn_left_common_factor(){
    for(int i = 0; i < regular_hash.size(); ++i){
        auto key = regular_hash.keys()[i];
        regular_hash[key].integrat();


        // 进行左公因子提取
       auto output = regular_hash[key].clean_Left_common_factor(special_char);
       // 插入新的规则
       for(auto& regular:output){
           if(regular.non_terminal == EPSILON)
               continue;
           regular_hash.insert(regular.get_non_terminal(), regular);
       }
//      qDebug() << static_cast<QChar> (regular_hash[key].non_terminal) << ' ' << regular_hash[key].get_deductions();
    }
}

/**
 * @brief Grammar::turn_left_recursion_to_right
 * 直接递归
 * 间接递归
 * 提取公因式
 */
void Grammar::turn_left_recursion_to_right(){

    auto key_list = regular_hash.keys();
    for(int i = 0; i < key_list.size(); ++i){
        auto key = key_list[i];
        regular_hash[key].integrat();
        // 带入操作
        for(int j = 0; j < i; ++j){
            auto last = key_list[j];
            for(auto&node : regular_hash[key].root->child){
                if(node->str.front() == last)
                    regular_hash[key].bring_in(regular_hash[last]);
            }
        }
        bool has_left_cursion{false};
        // 检查存在左递归
        for(auto&node : regular_hash[key].root->child){
            if(node->str.front() == key){
                has_left_cursion = true;
                break;
            }
        }
        // 清除左递归
        if(has_left_cursion){
            regular_hash[key].clean_Left_common_factor(special_char);
            QChar regular_name = static_cast<QChar>(special_char);
            ++special_char;
            Regular regular(regular_name);
            regular.insert_deduction(EPSILON);
            int len = regular_hash[key].root->child.size();
            int index = 0;
            for(auto iter = regular_hash[key].root->child.begin(); index < len; ++iter, ++index){
                if((*iter)->str.front() == key){
                    regular.insert_deduction((*iter)->str.mid(1) + regular_name);
                    iter = regular_hash[key].root->child.erase(iter);
                    --iter;
                }
                else{
                    if(!(*iter)->str.isEmpty()){
                        if((*iter)->str[0] == EPSILON)
                            regular_hash[key].root->child.push_back(new Node(regular_name));
                        else
                            (*iter)->str.push_back(regular_name);
                    }
                }
            }
            regular_hash.insert(regular_name, regular);
        }
    }
}

QString Grammar::print_all_grammer(){
    QString cache;
    for(auto&item : regular_hash){
        cache += item.get_non_terminal() + ' ';
        for(auto& deduction : item.get_deductions()){
             cache += deduction + '|';
        }
        cache.chop(1);
        cache += '\n';
    }
    return cache;
}

First Grammar::_find_first(const QChar& non_terminal){
//    qDebug() << non_terminal << "+ "<< regular_hash[non_terminal].get_deductions() << endl;
    if(first_hash.contains(non_terminal) && !first_hash[non_terminal].isEmpty()){
        return first_hash[non_terminal];
    }

    first_hash.insert(non_terminal,First());
    QVector<Node*> cache;
    for(auto&node : regular_hash[non_terminal].root->child){
        // 是终结符, 直接插入first集合
        if(!is_non_terminal(node->str[0])){
            first_hash[non_terminal].insert(node->str[0]);
        }
        // EPSILON 标记直接弹出
        else if(node->str[0] == EPSILON){
            first_hash[non_terminal].has_epsilon = true;
        }
        // 非终结符
        else{
            cache.push_back(node);
        }
    }

    // state0表示 无非终结符, 1表示有非终结符且first(非)全含epsilon, 2表示有非终结但不全含epsilon
    for(auto& node : cache){
        int i = 0;
        for(; i < node->str.size(); ++i){
            if(!is_non_terminal(node->str[i])){
                first_hash[non_terminal].insert(node->str[i]);
                break;
            }
            auto B_first = _find_first(node->str[i]);
            if(!first_hash[non_terminal].is_has_intersection(B_first)){
                first_hash[non_terminal].combine(B_first);
                if(!B_first.has_epsilon){
                    break;
                }
                // 最后一个
                if(i == node->str.size() - 1)
                    first_hash[non_terminal].has_epsilon = true;


            }
            else{
                regular_hash[non_terminal].bring_in(regular_hash[node->str[i]]);
                // 进行左公因子提取
                auto output = regular_hash[non_terminal].clean_Left_common_factor(special_char);
                // 插入新的规则
                for(auto& r:output){
                    regular_hash.insert(r.get_non_terminal(), r);
                    // 对新的规则也要求first元素
                    _find_first(r.non_terminal);
                }

                first_hash[non_terminal].clear();
                return _find_first(non_terminal);
            }

        }

    }

    return first_hash[non_terminal];
}

void Grammar::find_first(){
    // 调入
    for(int i = 0;  i < regular_hash.keys().size(); ++i)
        _find_first(regular_hash.keys()[i]);
}

QString Grammar::print_first_set(){
    QString cache;
    for(auto&non_terminal: first_hash.keys()){
        if(regular_hash.contains(non_terminal)){
            cache.push_back(non_terminal);
            cache += ": " + first_hash[non_terminal].print() + "\n";
        }
    }
    return cache;
}

void Grammar::find_follow(){
    QHash<QChar, QVector<QChar>> add_to_follow_hash;
    for(auto&key : regular_hash.keys()){
        follow_hash.insert(key, Follow());
    }
    // 初始化开头符号的Follow集合
    follow_hash[start].insert(TERMINATOR);
    // 初始化
    for(auto&regular : regular_hash.values()){
        for(auto&node:regular.root->child){
            for(int i = 0; i < node->str.size(); ++i){
                if(!is_non_terminal(node->str[i]))
                    continue;

                int j = 1;
                for(; j + i < node->str.size(); ++j){
                    if(!is_non_terminal(node->str[i + j])){
                        follow_hash[node->str[i]].insert(node->str[i + j]);
                        break;
                    }
                    else{
                        follow_hash[node->str[i]].combine(first_hash[node->str[i + j]]);
                        if(!first_hash[node->str[i + j]].has_epsilon){
                            break;
                        }
                    }
                }

                if(j + i == node->str.size()){
                   add_to_follow_hash[node->str[i]].push_back(regular.get_non_terminal());
                }
            }
        }
    }
//    qDebug() << add_to_follow_hash;
    // 可能会出现regular的Follow还没求. 用递归解决
    for(auto&key: regular_hash.keys())
        add_follow(key, add_to_follow_hash);

}
Grammar::Follow Grammar::add_follow(const QChar& c, QHash<QChar, QVector<QChar>>& add_follow_hash){
    if(!add_follow_hash.contains(c) || add_follow_hash[c].empty()){
        return follow_hash[c];
    }

    for(auto& item : add_follow_hash[c]){
        if(item == c)
            continue;
        follow_hash[c].combine(add_follow(item, add_follow_hash));
    }
    add_follow_hash[c].clear();
    return follow_hash[c];
}

QString Grammar::print_follow_set(){
    QString cache;
    for(auto&non_terminal: follow_hash.keys())
    {
        if(regular_hash.contains(non_terminal)){
            cache.push_back(non_terminal);
            cache += ": "  +  follow_hash[non_terminal].print() + "\n";
        }
    }
    return cache;
}

QPair<QVector<QString>, int> Grammar::deduce(const QString& str){
    QVector<QString> output;
    QVector<QChar> cache{start};
    QString prefix;
    int index = 0;
    auto find_deduction = [&](const QChar& c){
        // 寻找可以推导的规则
        for(auto&tmp : regular_hash[c].get_deductions()){
            // 观察当前规则首字符 终结符直接匹配, 非终结符看其first集合
            if((!is_non_terminal(tmp[0]) && tmp[0] == str[index]) || first_hash[tmp[0]].contains(str[index]) || (first_hash[tmp[0]].has_epsilon && follow_hash[tmp[0]].contains(str[index]))){
                // 将该规则压入栈中
                for(int i = tmp.size() - 1; i >= 0; --i){
                    if(tmp[i] == EPSILON)
                        continue;
                    cache.push_front(tmp[i]);
                }
            }
        }
    };
    // 0表示推导失败, 1表示可以推导, 2表示存在二义性
    int state = 0;
    while(!cache.empty()){
        // 为终结符直接匹配
        if(!is_non_terminal(cache.front())){
            if(str[index] == cache.front()){
                ++index;
                prefix += cache.front();
                cache.pop_front();
            }
            // 终结符不匹配,推导失败
            else{
                state = 0;
                return qMakePair(output, state);
            }
        }
        // 为非终结符
        else{
            // first集合不含空
            if(!first_hash[cache.front()].has_epsilon){
                // first集合包含str[index], 说明可以继续推导
                if(first_hash[cache.front()].contains(str[index])){
                    // 将上一步推导步骤记录
                    QString tmp;
                    QChar front = cache.front();
                    for(int i= 0; i < cache.size(); ++i)
                        tmp += cache[i];
                    output.push_back(prefix + tmp);

                    cache.pop_front();
                     // 寻找可以推导的规则
                    find_deduction(front);
                }
                // first集合不包含, 说明不可继续推导
                else{
                    return qMakePair(output, 0);
                }
            }
            // first集合含空 需观察follow集合
            else{
                // follow集合包含
                if(follow_hash[cache.front()].contains(str[index])){
                    // 若first集合也包含, 说明存在二义性
                    if(first_hash[cache.front()].contains(str[index])){
                        state = 2;
                        return qMakePair(output, state);
                    }
                    // 非终结符变为@
                    QString tmp;
                    for(int i= 0; i < cache.size(); ++i)
                        tmp += cache[i];

                    output.push_back(prefix + tmp);
                    cache.pop_front();
                }
                // follow集合没有, 只能选择first集合的
                else{

                   // 将上一步推导步骤记录
                   QString tmp;
                   QChar back = cache.front();
                   for(int i= 0; i < cache.size(); ++i)
                       tmp += cache[i];
                   output.push_back(prefix + tmp);
                   cache.pop_front();
                    // 寻找可以推导的规则
                   find_deduction(back);
                }
            }
        }
    }
    output.push_back(str);
    return qMakePair(output, 1);
}


//void Grammar::init(){
//    qDebug() << print_all_grammer();

//    qDebug() << "简化: ";
//    simplify();


//    qDebug() << "提取左公因子: ";
//    turn_left_common_factor();
//    qDebug() << print_all_grammer();

//    qDebug() << "消除左递归: ";
//    turn_left_recursion_to_right();

//    // 简化
//    simplify();
//    qDebug() << print_all_grammer();

//    qDebug() << "提取左公因子: ";
//    turn_left_common_factor();
//    qDebug() << print_all_grammer();

//    qDebug() << "first: ";
//    find_first();
//     qDebug() << print_all_grammer();

//    // 简化
//    simplify();
//    qDebug() << print_all_grammer();
//    print_first_set();

//    qDebug() << "Follow: ";
//    find_follow();
//    print_follow_set();
//    // 推导 abd
//    QString str{"abd"};
//    qDebug() << "推导" << str;
//    auto output = deduce(str);
//}
