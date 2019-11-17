////////////////////////////////////////////////////////////////////////////////
// Module Name:  skip_list.h/hpp
// Authors:      Leonid Dworzanski, Sergey Shershakov
// Version:      2.0.0
// Date:         28.10.2018
//
// This is a part of the course "Algorithms and Data Structures"
// provided by  the School of Software Engineering of the Faculty
// of Computer Science at the Higher School of Economics.
////////////////////////////////////////////////////////////////////////////////

// !!! DO NOT include skip_list.h here, 'cause it leads to circular refs. !!!

#include <cstdlib>
#include "skip_list.h"

//#include "skip_list.h"


//==============================================================================
// helpful methods
//==============================================================================

//double randDouble()
//{
//    srand(time(nullptr));
//    return (double)rand()/RAND_MAX;
//}

//==============================================================================

//==============================================================================
// class NodeSkipList
//==============================================================================

template<class Value, class Key, int numLevels>
void NodeSkipList<Value, Key, numLevels>::clear(void)
{
    for (int i = 0; i < numLevels; ++i)
        Base::nextJump[i] = 0;

    Base::levelHighest = -1;
}

//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(void)
{
    clear();
}

//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(const Key& tkey)
{
    clear();

    Base::Base::key = tkey;
}

//------------------------------------------------------------------------------

template<class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(const Key& tkey, const Value& val)
{
    clear();

    Base::Base::key = tkey;
    Base::Base::value = val;
}


//==============================================================================
// class SkipList
//==============================================================================

template<class Value, class Key, int numLevels>
SkipList<Value, Key, numLevels>::SkipList(double probability)
{
    _probability = probability;

    // Lets use m_pPreHead as a final sentinel element
    for (int i = 0; i < numLevels; ++i)
        Base::_preHead->nextJump[i] = Base::_preHead;

    Base::_preHead->levelHighest = numLevels - 1;
}

template<class Value, class Key, int numLevels>
SkipList<Value, Key, numLevels>::~SkipList()
{
    // Base destructor will be called automatically
}

template<class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::insert(const Value& val, const Key& key)
{
    Node* update[numLevels]; // массив ссылок на предыдущие элементы на всех уровнях
    Node* x = Base::_preHead->next;
    srand(time(nullptr));

    // нашел все предыдущие элементы
    for (size_t i = numLevels - 1; i >= 0; --i)
    {
        while(x->next != Base::_preHead && x->nextJump[i]->key <= key)
            x = x->nextJump[i];

        update[i] = x;
    }

    // х - элемент, после которого надо сгенерировать новый элемент
    while (x->next != Base::_preHead && x->next->key <= key)
        x = x->next;

    // генерация и перепревязка указателей базового уровня
    Node* newNode = new Node(key, val);
    newNode->next = x->next;
    x->next = newNode;

    // поиск числа генерируемых уровней
    while(newNode->levelHighest < numLevels && (double)rand()/RAND_MAX <= _probability)
        ++newNode->levelHighest;

    // обновление всех ссылок
    for (size_t i = 0; i <= newNode->levelHighest; ++i)
    {
        newNode->nextJump[i] = update[i]->next;
        update[i]->next = newNode;
    }
}

template<class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::removeNext(SkipList::Node* nodeBefore)
{
    if (nodeBefore == nullptr ||
        nodeBefore->next == nullptr ||
        nodeBefore->next == Base::_preHead)
    {
        return; // это однострочник, но в реализации выше кодстайл такой же, не снижай плз
    }

    Node* nodeToRemove = nodeBefore->next;

    for (size_t i = 0; i <= nodeToRemove->levelHighest; ++i)
        nodeBefore->nextJump[i] = nodeToRemove->nextJump[i];

    nodeBefore->next = nodeToRemove->next;

    delete nodeToRemove;
}
// TODO: !!! One need to implement all declared methods !!!
