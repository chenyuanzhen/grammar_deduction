#include "regular.h"
#include <QDebug>

// Regular类
Regular::Regular(QChar _non_terminal):root(new Node()), end(new Node()), non_terminal(_non_terminal), deductions(){

};

void Regular::insert_deduction(const QString& deduction){
    QString cache = deduction + ')';
    QStack<QPair<Node*, Node*>> node_stack;
    QStack<QChar> operator_stack;
    operator_stack.push('(');
    Node* node = new Node();

    for(int i = 0; i < cache.size(); ++i){
        if(cache[i] == '|'){
            operator_stack.push(cache[i]);
            node_stack.push(qMakePair(node, node));
            node = new Node();
        }
        else if(cache[i] == '('){
            operator_stack.push(cache[i]);
            node_stack.push(qMakePair(node, node));
            node = new Node();
        }
        else if(cache[i] == ')'){
            bool is_select = false;
            if(!node->str.isEmpty()){
                node_stack.push(qMakePair(node, node));
                node = new Node();
            }

            while(operator_stack.top() != '('){
                if(operator_stack.top() == '|'){
                   is_select = true;
                   auto pair1 = node_stack.top();
                   node_stack.pop();

                   auto pair2 = node_stack.top();
                   node_stack.pop();

                   node_stack.push(select(pair1, pair2));
                }
                operator_stack.pop();
            }

            while(node_stack.size() >= 2){
                auto pair1 = node_stack.top();
                node_stack.pop();
                auto pair2 = node_stack.top();
                node_stack.pop();
                node_stack.push(connect(pair2, pair1));
            }

        }
        else{
            node->str += cache[i];
        }
    }
    root->child.push_back(node_stack.top().first);
    end = node_stack.top().second;
}

bool Regular::is_opeartor(QChar c){
    return c == '|';

}

QPair<Node*, Node*> Regular::connect(QPair<Node*, Node*> start, QPair<Node*, Node*> end){
    start.second->child.push_back(end.first);
    end.first->father.push_back(start.second);
    return qMakePair(start.first, end.second);
}

QPair<Node*, Node*> Regular::select(QPair<Node*, Node*> pair1, QPair<Node*, Node*> pair2){
    Node * start = new Node();
    Node * end = new Node();

    start->child.push_back(pair1.first);
    start->child.push_back(pair2.first);
//    end->father.push_back(pair1.second);
//    end->father.push_back(pair2.second);

//    pair1.first->father.push_back(start);
    pair1.second->child.push_back(end);

//    pair2.first->father.push_back(start);
    pair2.second->child.push_back(end);
    return qMakePair(start, end);
}

void Regular::_integrat(QString cache, QList<QString>& output, Node *node){
    if(node == nullptr)
        return;

    if(node->str != "" && !node->str.isEmpty() )
        cache += node->str;

    if(node->child.empty()){
        output.push_back(cache);
        return;
    }
    for(auto& item: node->child){
        _integrat(cache, output, item);
    }
}


void Regular::integrat(){
    end = new Node();
    QList<QString> output;
    QString cache;
    _integrat(cache, output, root);

    root->child.clear();
    for(auto&item : output){
        Node* node = new Node(item);
        node->child.push_back(end);
        root->child.push_back(node);
    }
}

QList<QString> Regular::get_deductions(){
   QList<QString> output;
   QString cache;
   integrat();
    for(auto&item : root->child){
        if(!item->str.isEmpty())
            output.push_back(item->str);
    }
   return output;
}

void Regular::erase(int index){
    auto iter = root->child.begin();
    for(int i = 0; i < index; ++i){
        iter++;
    }
    delete(*iter);
    root->child.erase(iter);
}

void Regular::erase(QVector<int> input){

    auto iter = root->child.end();
    int index = input.size() - 1;
    int i = root->child.size();
    while(index >= 0 && !root->child.empty()){
        if(i == input[index]){
            iter = root->child.erase(iter);
            --index;
        }
         iter--;
         i--;
    }
}


QChar Regular::get_non_terminal(){
    return non_terminal;
}

//Regular Regular
void Regular::_clean_Left_common_factor(int& regular_name, QVector<Regular>& output){
    QHash<QChar, QVector<Node*>> node_vector_hash;
    for(auto iter = root->child.begin(); iter != root->child.end(); ++iter){
        node_vector_hash[(*iter)->str[0]].push_back(*iter);
    }
    root->child.clear();
    for(auto&key: node_vector_hash.keys()){
        if(node_vector_hash[key].size() > 1){
            Regular regular(static_cast<QChar>(regular_name));
            regular.non_terminal = static_cast<QChar>(regular_name);
            for(auto&node : node_vector_hash[key]){
                // 截断首字符
                if(node->str.size() > 1)
                    regular.root->child.push_back(new Node(node->str.mid(1)));
                else
                    regular.root->child.push_back(new Node(EPSILON));
            }
            Node* node = new Node();
            node->str.push_back(key);
            node->str.push_back(static_cast<QChar>(regular_name));
            root->child.push_back(node);
            output.push_back(regular);

            ++regular_name;
        }
        else{
            root->child.push_back(node_vector_hash[key][0]);
        }
    }

}


//Regular Regular
QVector<Regular> Regular::clean_Left_common_factor(int& regular_name){
//    qDebug() << static_cast<QChar> (regular_name);
    QVector<Regular> output;
    _clean_Left_common_factor(regular_name, output);

    for(int i = 0; i < output.size(); ++i){
        output[i]._clean_Left_common_factor(regular_name, output);
    }
    return output;
}






// 输入生成另一个regular的符号
// clean_left_cursion需要合并
Regular Regular::clean_left_cursion(QChar regular_name){
//    // 进行合并 由grammar负责
//    integrat();
    QVector<Node*> has_left_cursion;
    Node* node = new Node(regular_name);
    QVector<int> wait_for_delete;
    int index = 0 ;

    // 检索存在左递归的规则, 进行提取左递归的非终结符, 去除节点
    for(auto iter = root->child.begin(); iter != root->child.end(); ++iter, ++index){
        if((*iter)->str[0] == non_terminal){
            has_left_cursion.push_back(*iter);
            // 去除公因子
            (*iter)->str.remove(0, 1);
            (*iter)->child.clear();
            (*iter)->str.push_back(regular_name);
//            qDebug() << non_terminal <<" clean_left_cursion: " << (*iter)->str;
            // 去除首指针
            iter = root->child.erase(iter);
            --iter;
        }
    }

    if(has_left_cursion.empty())
        return Regular();


    // 末尾添加node指针
    for(auto& node: root->child){
        node->str.push_back(regular_name);
    }
//    end->child.push_back(node);
    end = new Node();
    node->child.push_back(end);

    // 新建 regular
    Regular regular(regular_name);

    for(auto&item: has_left_cursion){
        item->child.push_back(regular.end);
        regular.root->child.push_back(item);
    }
    Node* epsilon = new Node(EPSILON);
    regular.root->child.push_back(epsilon);
    epsilon->child.push_back(regular.end);
    return regular;

}


bool Regular::bring_in(Regular& regular){
    bool is_add = false;
    int len = root->child.size();
    // 检查带入左递归
    for(auto iter = root->child.begin(); iter != root->child.end() && len >= 0; ++iter, --len){
        // 带入的值没有更新到 hash中
        for(int i = 0; i < (*iter)->str.size(); ++i){
            // 表达式中左递归需带入
            if((*iter)->str[i] == regular.non_terminal){
                is_add = true;
                for(auto&item: regular.get_deductions()){
                    if(item == EPSILON){
                        root->child.push_back(new Node((*iter)->str.left(i) + (*iter)->str.mid(i+1)));
                    }
                    else{
                        // 改动上面的iter中的值,
                       root->child.push_back(new Node((*iter)->str.left(i) + item + (*iter)->str.mid(i+1)));
                    }
                }
                // 删掉当前这条规则
                iter = root->child.erase(iter);
                --iter;
                break;
            }
        }
    }
    return is_add;
}


//bool Regular::substitution(const Regular& regular, Node* node){
//    Regular copy = regular;
//    for(int i = 0; i < node->str; ++i){
//       if(node->str[i] == regular.non_terminal){
//           regular.root->str = node->str.left(i);
//           regular.end->str = node->str.mid(i+1);

//       }
//    }
//    return false;
//}
