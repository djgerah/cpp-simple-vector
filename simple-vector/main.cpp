#include "simple_vector.h"

#include <cassert>
#include <iostream>
#include <numeric>
#include <utility>

using namespace std;

class X 
{
public:

    X() : X(5) {}

    X(size_t num) : x_(num) {}

    X(const X& other) = delete;
    
    X& operator=(const X& other) = delete;

    X(X&& other) 
        {
            x_ = exchange(other.x_, 0);
        }

    X& operator=(X&& other) 
    {
        x_ = exchange(other.x_, 0);
        return *this;
    }

    size_t GetX() const 
    {
        return x_;
    }

private:

    size_t x_;
};

SimpleVector<int> GenerateVector(size_t size) 
{
    SimpleVector<int> vec(size);
    iota(vec.begin(), vec.end(), 1);
    
    return vec;
}

void TestTemporaryObjConstructor() 
{
    const size_t size = 1000000;
    cout << "Test with temporary object, copy elision" << endl;

    SimpleVector<int> moved_vector(GenerateVector(size));
    assert(moved_vector.GetSize() == size);
    cout << "Done!" << endl << endl;
}

void TestTemporaryObjOperator() 
{
    const size_t size = 1000000;
    cout << "Test with temporary object, operator=" << endl;

    SimpleVector<int> moved_vector;
    assert(moved_vector.GetSize() == 0);
    moved_vector = GenerateVector(size);
    assert(moved_vector.GetSize() == size);

    cout << "Done!" << endl << endl;
}

void TestNamedMoveConstructor() 
{
    const size_t size = 1000000;
    cout << "Test with named object, move constructor" << endl;

    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector(move(vector_to_move));
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);

    cout << "Done!" << endl << endl;
}

void TestNamedMoveOperator() 
{
    const size_t size = 1000000;
    cout << "Test with named object, operator=" << endl;

    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector = move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);

    cout << "Done!" << endl << endl;
}

void TestNoncopiableMoveConstructor() 
{
    const size_t size = 5;
    cout << "Test noncopiable object, move constructor" << endl;
    SimpleVector<X> vector_to_move;

    for (size_t i = 0; i < size; ++i) 
    {
        vector_to_move.PushBack(X(i));
    }

    SimpleVector<X> moved_vector = move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);

    for (size_t i = 0; i < size; ++i) 
    {
        assert(moved_vector[i].GetX() == i);
    }

    cout << "Done!" << endl << endl;
}

void TestNoncopiablePushBack() 
{
    const size_t size = 5;
    cout << "Test noncopiable push back" << endl;

    SimpleVector<X> vec;

    for (size_t i = 0; i < size; ++i) 
    {
        vec.PushBack(X(i));
    }

    assert(vec.GetSize() == size);

    for (size_t i = 0; i < size; ++i) 
    {
        assert(vec[i].GetX() == i);
    }

    cout << "Done!" << endl << endl;
}

void TestNoncopiableInsert() 
{
    const size_t size = 8;
    cout << "Test noncopiable insert" << endl;
    SimpleVector<X> vec;

    for (size_t i = 0; i < size; ++i) 
    {
        vec.PushBack(X(i));
    }

    cout << "vec { ";

    for (size_t i = 0; i < vec.GetSize(); ++i) 
    {
        cout << vec[i].GetX() << " ";
    }
    
    cout << "}";
    cout << ", size = " << vec.GetSize() << ", capacity = " << vec.GetCapacity() << endl;

    // в начало
    vec.Insert(vec.begin(), X(size + 1));
    assert(vec.GetSize() == size + 1);
    assert(vec.begin()->GetX() == size + 1);

    // в конец
    vec.Insert(vec.end(), X(size + 2));
    assert(vec.GetSize() == size + 2);
    assert((vec.end() - 1)->GetX() == size + 2);

    // в середину
    vec.Insert(vec.begin() + 3, X(size + 3));
    assert(vec.GetSize() == size + 3);
    assert((vec.begin() + 3)->GetX() == size + 3);

    cout << "vec { ";

    for (size_t i = 0; i < vec.GetSize(); ++i) 
    {
        cout << vec[i].GetX() << " ";
    }
    
    cout << "}";
    cout << ", size = " << vec.GetSize() << ", capacity = " << vec.GetCapacity() << endl;

    cout << "Done!" << endl << endl;
}

void TestNoncopiableErase() 
{
    const size_t size = 10;
    cout << "Test noncopiable erase" << endl;

    SimpleVector<X> vec;

    for (size_t i = 0; i < size; ++i) 
    {
        vec.PushBack(X(i));
    }

    cout << "vec { ";

    for (size_t i = 0; i < vec.GetSize(); ++i) 
    {
        cout << vec[i].GetX() << " ";
    }
    
    cout << "}";
    cout << ", size = " << vec.GetSize() << endl;

    auto it = vec.Erase(vec.begin());
    assert(it->GetX() == 1);

    cout << "vec { ";

    for (size_t i = 0; i < vec.GetSize(); ++i) 
    {
        cout << vec[i].GetX() << " ";
    }
    
    cout << "}";
    cout << ", size = " << vec.GetSize() << endl;

    cout << "Done!" << endl << endl;
}

void TestNoncopiableResize() 
{
    size_t size = 10;
    SimpleVector<X> vec;

    for (size_t i = 0; i < size; ++i) 
    {
        vec.PushBack(X(i));
    }

    size_t old_capacity = vec.GetCapacity();
    cout << "Test noncopiable resize" << endl;
    cout << "vec size = " << vec.GetSize() << ", capacity = " << vec.GetCapacity() << endl;

    // new_size > capacity
    vec.Resize(20);
    assert(vec.GetSize() == 20);
    cout << "new vec size " << vec.GetSize() << " > capacity " << old_capacity << ", new capacity = " << vec.GetCapacity() << endl;

    // new_size < capacity
    vec.Resize(10);
    assert(vec.GetSize() == 10);
    cout << "new vec size " << vec.GetSize() << " < capacity " << vec.GetCapacity() << ", new capacity = " << vec.GetCapacity() << endl;

    // new_size == capacity
    vec.Resize(32);
    assert(vec.GetSize() == 32);
    cout << "new vec size " << vec.GetSize() << " = capacity " << vec.GetCapacity() << ", new capacity = " << vec.GetCapacity() << endl;

    cout << "Done!" << endl << endl;
}

void TestNoncopiableReserve() 
{
    const size_t size = 10;
    SimpleVector<X> vec;

    for (size_t i = 0; i < size; ++i) 
    {
        vec.PushBack(X(i));
    }

    size_t old_capacity = vec.GetCapacity();
    cout << "Test noncopiable reserve" << endl;
    cout << "vec size = " << vec.GetSize() << ", capacity = " << vec.GetCapacity() << endl;

    // new capacity > capacity
    vec.Reserve(20);
    assert(vec.GetCapacity() == 20);
    cout << "reserved size " << vec.GetCapacity() << " > capacity " << old_capacity << ", new capacity = " << vec.GetCapacity() << endl;

    // new capacity >= capacity
    size_t capacity = vec.GetCapacity();
    vec.Reserve(10);
    assert(vec.GetCapacity() == capacity);
    cout << "reserved size " << vec.GetCapacity() << " = capacity " << capacity << ", capacity still = " << vec.GetCapacity() << endl;

    cout << "Done!" << endl << endl;
}

void TestPopBack() 
{
    const size_t size = 10;
    SimpleVector<X> vec;

    for (size_t i = 0; i < size; ++i) 
    {
        vec.PushBack(X(i));
    }

    cout << "Test noncopiable pop back" << endl;
    cout << "vec { ";

    for (size_t i = 0; i < vec.GetSize(); ++i) 
    {
        cout << vec[i].GetX() << " ";
    }
    
    cout << "}";
    cout << ", size = " << vec.GetSize() << endl;

    size_t org_size = vec.GetSize();
    vec.PopBack();

    assert(vec.GetSize() == (org_size - 1));
    cout << "vec size after pop back" << endl;
    cout << "vec { ";

    for (size_t i = 0; i < vec.GetSize(); ++i) 
    {
        cout << vec[i].GetX() << " ";
    }
    
    cout << "}";
    cout << ", size = " << vec.GetSize() << endl;
    auto it = vec.cend();
    assert(it->GetX() == 9);
    cout << "Done!" << endl;   
}

int main()
{
    TestTemporaryObjConstructor();
    TestTemporaryObjOperator();
    TestNamedMoveConstructor();
    TestNamedMoveOperator();
    TestNoncopiableMoveConstructor();
    TestNoncopiablePushBack();
    TestNoncopiableInsert();
    TestNoncopiableErase();
    TestNoncopiableResize();
    TestNoncopiableReserve();
    TestPopBack();

    return 0;
}