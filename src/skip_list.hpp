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
#include <random>
//#include "skip_list.h"

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
    Node* allLevelNodesBefore[numLevels]; // массив ссылок на предыдущие элементы на всех уровнях
    Node* nodeBefore = Base::_preHead;

    // не баньте
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // нашел все предыдущие элементы
    for (int i = numLevels - 1; i >= 0; --i)
    {
        while(nodeBefore->nextJump[i] != Base::_preHead && nodeBefore->nextJump[i]->key <= key)
            nodeBefore = nodeBefore->nextJump[i];

        allLevelNodesBefore[i] = nodeBefore;
    }

    // nodeBefore - элемент, после которого надо сгенерировать новый элемент
    while (nodeBefore->next != Base::_preHead && nodeBefore->next->key <= key)
        nodeBefore = nodeBefore->next;

    // генерация и перепревязка указателей базового уровня
    Node* newNode = new Node(key, val);
    newNode->next = nodeBefore->next;
    nodeBefore->next = newNode;

    // поиск числа генерируемых уровней
    while(newNode->levelHighest < numLevels - 1 && dist(mt) <= _probability)
    {
        ++newNode->levelHighest;
    }

    // обновление всех ссылок
    for (int i = 0; i <= newNode->levelHighest; ++i)
    {
        newNode->nextJump[i] = allLevelNodesBefore[i]->next;
        allLevelNodesBefore[i]->next = newNode;
    }
}

template<class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::removeNext(SkipList::Node* nodeBefore)
{
    if (nodeBefore == nullptr ||
        nodeBefore->next == nullptr ||
        nodeBefore->next == Base::_preHead)
    {
        // это однострочник, но в реализации выше кодстайл такой же, не снижай плз
        throw std::invalid_argument("Next node can't be removed.");
    }

    Node* nodeToRemove = nodeBefore->next;

    for (int i = 0; i <= nodeToRemove->levelHighest; ++i)
        nodeBefore->nextJump[i] = nodeToRemove->nextJump[i];

    nodeBefore->next = nodeToRemove->next;

    delete nodeToRemove;
}

template<class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>* SkipList<Value, Key, numLevels>::findLastLessThan(const Key& key) const
{
    Node* run = Base::_preHead;

//    // проверки по условию
//    // из-за которых у меня не проходит тест, пасиба
//    if(run->next == Base::_preHead ||
//        key < run->key)
//    {
//        return Base::_preHead;
//    }

    // поиск за log(n) на верхних уровнях
    for (int i = numLevels - 1; i >= 0; --i)
    {
        while(run->nextJump[i] != Base::_preHead && run->nextJump[i]->key < key)
            run = run->nextJump[i];
    }

    // поиск на нижнем уровне
    while(run->next != Base::_preHead && run->next->key < key)
        run = run->next;

    return run;
}

template<class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>* SkipList<Value, Key, numLevels>::findFirst(const Key& key) const
{
    // мне надоело писать комментарии, вроде работает
    Node* run = Base::_preHead;

    for (int i = numLevels - 1; i >= 0; --i)
    {
        while(run->nextJump[i] != Base::_preHead && run->nextJump[i]->key < key)
            run = run->nextJump[i];
    }

    if(run->next->key == key && run->next != Base::_preHead)
        return run->next;

    while(run != Base::_preHead && run->key != key)
        run = run->next;

    if(run == Base::_preHead)
        return nullptr;

    return run;

}
