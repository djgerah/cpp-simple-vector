#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>

using namespace std;

template <typename Type>
class ArrayPtr 
{
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size) 
    {
        // Реализуйте конструктор самостоятельно
        if (size == 0) { raw_ptr_ = nullptr; }
        else { raw_ptr_ = new Type[size]; }
    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    // Реализуйте конструктор самостоятельно
    explicit ArrayPtr(Type* raw_ptr) noexcept 
        : raw_ptr_(raw_ptr)
        {}

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr&) = delete;

    ~ArrayPtr() 
    {
        // Напишите деструктор самостоятельно
        delete[] raw_ptr_;
    }

    // Запрещаем присваивание
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept 
    {
        // Заглушка. Реализуйте метод самостоятельно
        Type* temp = raw_ptr_;
        raw_ptr_ = nullptr;
        return temp;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept 
    {
        // Реализуйте операцию самостоятельно
        return raw_ptr_[index];
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept 
    {
        // Реализуйте операцию самостоятельно
        return raw_ptr_[index];
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const 
    {
        // Заглушка. Реализуйте операцию самостоятельно
        if (raw_ptr_ != nullptr) { return true; }
        return false;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept 
    {
        // Заглушка. Реализуйте метод самостоятельно
        return &raw_ptr_[0];
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept 
    {
        // Реализуйте метод самостоятельно
        Type* temp = other.raw_ptr_;
        other.raw_ptr_ = raw_ptr_;
        raw_ptr_ = temp;
    }

private:

    Type* raw_ptr_ = nullptr;
};