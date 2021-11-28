#include "firstSet.h"
#include "QDebug"
void First::insert(const QChar& terminator){
//    qDebug() << "insert: " << terminator << ' ';
    if(terminator == EPSILON)
        this->has_epsilon = true;
    else
        terminators_set.insert(terminator);
}

void First::combine(const First& first){
    for(auto&item:first.terminators_set){
        terminators_set.insert(item);
    }
}



QString First::print(){
    QString cache;
    for(auto&item: terminators_set){
        cache += item;
        cache += ", ";
    }
    if(has_epsilon){
        cache += EPSILON;
    }
    else{
       cache.chop(2);
    }
    return cache;
}

bool First::contains(const QChar& terminator){
    return terminators_set.contains(terminator);
}

bool First::isEmpty(){
    return terminators_set.isEmpty();
}

void First::clear(){
    has_epsilon = false;
    terminators_set.clear();
}

bool First::is_has_intersection(const First& first){
    for(auto& item : first.terminators_set){
        if(this->terminators_set.contains(item))
            return true;
    }
    return false;
}
