#pragma once

#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <utility>

#include "array_ptr.h"

// Дополнительный класс-обёртка для реализации конструктора с резервированием,
// чтобы компилятор мог вызвать правильный конструктор. 
// Этот конструктор принимает по значению объект этого класса-обёртки
class ReserveProxyObj
{
public:

    ReserveProxyObj(size_t capacity_to_reserve) 
        : capacity_(capacity_to_reserve)
        {}

    size_t GetCapacity() const noexcept 
    {
        return capacity_;
    }

private:

    size_t capacity_;
};

template <typename Type>
class SimpleVector 
{
public:

    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) 
        : SimpleVector(size, Type())
        {}

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) 
        : items_(size)
        , size_(size)
        , capacity_(size)
        {
            std::fill(begin(), (begin() + size), value);
        }   

    // Конструктор списков инициализаторов, cоздаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : items_(init.size())
        , size_(init.size())
        , capacity_(init.size())
        {
            std::copy(init.begin(), init.end(), begin());
        }

    // Конструктор копирования. Конструктор копирования создаёт новый объект. 
    // Поля инициализируются значениями из объекта-образца
    SimpleVector(const SimpleVector& other)
        : items_(other.capacity_), size_(other.size_)
    {
        std::copy(other.begin(), other.end(), begin());
    }

    // Конструктор перемещения позволяет переместить ресурсы, принадлежащие объекту rvalue, в lvalue без копирования.
    SimpleVector(SimpleVector&& other)
        : items_(other.capacity_)
    {
        swap(other);
    }

    // Конструктор, резервирующий сразу нужное количество памяти.
    // Принимает по значению объект класса-обёртки ReserveProxyObj
    SimpleVector(ReserveProxyObj object)
    {
        Reserve(object.GetCapacity());
    }
    
    // Оператор присваивания. Оператор присваивания модифицирует уже существующий объект.
    // Вызывается при присваивании нового значения уже существующему объекту
    SimpleVector& operator=(const SimpleVector& rhs) 
    {
        if (&items_ != &rhs.items_)
        {
            ArrayPtr<Type> temp(rhs.GetCapacity());
            std::copy(rhs.begin(), rhs.end(), temp.Get());
            items_.swap(temp);
            size_ = rhs.GetSize();
            capacity_ = rhs.GetCapacity();
        }
        return *this;
    }

    // Оператор присваивания временного объекта. Оператор присваивания модифицирует уже существующий объект.
    // Вызывается при присваивании нового значения уже существующему объекту
    SimpleVector& operator=(SimpleVector&& rhs) 
    {
        if (&items_ != &rhs.items_)
        {
            ArrayPtr<Type> temp(rhs.GetCapacity());
            std::move(rhs.begin(), rhs.end(), temp.Get());
            items_.swap(temp);
            size_ = rhs.GetSize();
            capacity_ = rhs.GetCapacity();
        }
        return *this;
    }
    
    // Деструктор. Вызывается при уничтожении объекта
    ~SimpleVector()
    {
        Clear();
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept 
    {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept 
    {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept 
    {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept 
    {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept 
    {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) 
    {
        if (index >= size_) { throw std::out_of_range ("index >= size"); }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const 
    {
        // Напишите тело самостоятельно
        if (index >= size_) { throw std::out_of_range ("index >= size"); }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept 
    {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) 
    {
        // Напишите тело самостоятельно
        // Самый простой случай — уменьшение размера массива. 
        // Для этого SimpleVector достаточно изменить значение поля size_
        if (new_size <= size_) { size_ = new_size; }
        
        // Увеличение размера в пределах текущей вместимости контейнера. 
        // Помимо увеличения значения поля size_ происходит заполнение новых элементов значением по умолчанию для типа Type
        if (new_size <= capacity_) { fill((begin() + size_), (begin() + size_ + new_size)); size_ = new_size; }
        
        // Когда новый размер превышает текущую вместимость, SimpleVector создаёт новый массив большего размера 
        // в динамической памяти, куда копирует элементы исходного массива и инициализирует остальные элементы 
        // значением по умолчанию.
        if (new_size > capacity_)
        {
            // Во многих реализациях стандартной библиотеки, новую вместимость SimpleVector можно выбрать 
            // как максимум из new_capacity (new_size) и capacity_ * 2
            size_t new_capacity = std::max(new_size, capacity_ * 2);

            ArrayPtr<Type> temp(new_capacity);

            fill(temp.Get(), (temp.Get() + new_capacity));
            std::move(begin(), (begin() + capacity_), temp.Get());
            items_.swap(temp);
            
            size_ = new_size;
            capacity_ = new_capacity;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept 
    {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept 
    {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept 
    {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept 
    {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept 
    {
        return begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept 
    {
        return end();
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
	Iterator Insert(ConstIterator pos, const Type& value) 
    {
	assert(pos >= begin() && pos <= end());
        size_t index = std::distance(cbegin(), pos);

        // Перелокация памяти не нужна
	if (capacity_ > size_) 
        {
            // Копируем элементы после заданной позиции в обратном порядке на одно место вправо. 
            // На обрзовавшееся место вставляем value
            std::copy_backward(const_cast<Iterator>(pos), end(), (end() + 1));
	    items_[index] = value;
	}

        // Перелокация памяти нужна
	else 
        {
	    size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            // Создаем новый массив повыщенной вместимости
	    ArrayPtr<Type> temp(new_capacity);
            // Копируем все элементы до заданной позиции,
	    std::copy(begin(), const_cast<Iterator>(pos), temp.Get());
            // вставляем value,
	    temp[index] = value;
            // копируем все элементы после позиции.
	    std::copy(const_cast<Iterator>(pos), end(), (temp.Get() + index + 1));
            
	    items_.swap(temp);
            // Обновляем вместимость вектора,
	    capacity_ = new_capacity;
	}
	// обновляем размер
        ++size_;
        // return (SimpleVector::Iterator)pos;
        return const_cast<Iterator>(pos);
	}

    // Некопирующий Insert
    // Перемещает значение value в позицию pos.
    // Возвращает итератор на перемещенное значение
    // Если перед перемещением значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, Type&& value)
    {
	assert(pos >= begin() && pos <= end());
        size_t index = std::distance(cbegin(), pos);

        // Перелокация памяти не нужна
	if (capacity_ > size_) 
        {
            // Перемещаем элементы после заданной позиции в обратном порядке на одно место вправо. 
            // На обрзовавшееся место перемещаем value
            std::move_backward(const_cast<Iterator>(pos), end(), (end() + 1));
	    items_[index] = std::move(value);
	}

        // Перелокация памяти нужна
	else 
        {
	    size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            // Создаем новый массив повыщенной вместимости
	    ArrayPtr<Type> temp(new_capacity);
            // Перемещаем все элементы до заданной позиции,
	    std::move(begin(), const_cast<Iterator>(pos), temp.Get());
            // перемещаем value,
	    temp[index] = std::move(value);
            // перемещаем все элементы после позиции.
	    std::move(const_cast<Iterator>(pos), end(), (temp.Get() + index + 1));
            
	    items_.swap(temp);
            // Обновляем вместимость вектора,
	    capacity_ = new_capacity;
	}
	// обновляем размер
	++size_;
        // return &items_[index];
        return begin() + index;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) 
    {
        if (size_ + 1 > capacity_) 
        {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            std::fill(temp.Get(), (temp.Get() + new_capacity), Type());
            std::copy(begin(), (begin() + size_), temp.Get());
            items_.swap(temp);
            capacity_ = new_capacity;
        }
        items_[size_] = item;
        ++size_;
    }

    // Некопирующий PushBack
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type&& item) 
    {
        if (size_ + 1 > capacity_) 
        {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            std::move(begin(), (begin() + size_), temp.Get());
            items_.swap(temp);
            capacity_ = new_capacity;
        }
        items_[size_] = std::move(item);
        ++size_;
    }

    // Удаляет последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept 
    {
        assert(items_);
        // Чтобы удалить последний элемент непустого контейнера SimpleVector 
        // нужно уменьшить значение поля size_. 
        // Элемент продолжит находиться в векторе, но будет недоступен для использования.
        
        Erase(end()); // Элемент не продолжит находиться в векторе и станет недоступен для использования.
        // --size_; 
    }
   
    // Некопирующий Erase
    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) 
    {
        size_t index = std::distance(cbegin(), pos);
        ArrayPtr<Type> temp(capacity_);

        std::move(begin(), (begin() + index), temp. Get());
        std::move((begin() + index + 1), (begin() + size_), (temp.Get() + index));
        items_.swap(temp);
        --size_;
        
        return &items_[index];
    }

    // Если new_capacity больше текущей capacity, память должна быть перевыделена, 
    // а элементы вектора перемещены в новый отрезок памяти.
    void Reserve(size_t new_capacity)
    {
        // Если new_capacity больше текущей capacity, память должна быть перевыделена
        if (new_capacity > capacity_)
        {
            ArrayPtr<Type> temp(new_capacity);
            fill(temp.Get(), (temp.Get() + new_capacity));
            std::move(begin(), (begin() + size_), temp.Get());
            items_.swap(temp);
            capacity_ = new_capacity;
        }
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept 
    {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        items_.swap(other.items_);
    }

private:

    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;

    // Некопирующий аналог std::fill
    void fill(Iterator first, Iterator last)
    {
        for (; first != last; ++first)
        {
            *first = std::move(Type());
        }
    }
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) 
{
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return !std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return !(lhs > rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return !(lhs < rhs);
}
