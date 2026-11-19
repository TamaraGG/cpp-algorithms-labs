#include "Array.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <bits/locale_facets_nonio.h>


template <typename T>
template <bool IsConst>
Array<T>::BaseIterator<IsConst>::BaseIterator(Pointer current, Pointer end, bool isReverse)
    : current_(current), end_(end), isReverse_(isReverse) {}


template <typename T>
template <bool IsConst>
const T& Array<T>::BaseIterator<IsConst>::get() const {
    assert(hasNext(current_));
    return isReverse_ ? *(current_ - 1) : *current_;
}

template <typename T>
template <bool IsConst>
void Array<T>::BaseIterator<IsConst>::set(const T& value) requires (!IsConst && std::is_copy_assignable_v<T>) {
    assert(hasNext(current_));
    (isReverse_ ? *(current_ - 1) : *current_) = value;
}

template <typename T>
template <bool IsConst>
void Array<T>::BaseIterator<IsConst>::set(T&& value) requires (!IsConst && std::is_move_assignable_v<T>) {
    assert(hasNext(current_));
    (isReverse_ ? *(current_ - 1) : *current_) = std::move(value);
}

template <typename T>
template <bool IsConst>
void Array<T>::BaseIterator<IsConst>::next() {
    assert(hasNext());
    current_ = isReverse_ ? (current_ - 1) : (current_ + 1);
}

template <typename T>
template <bool IsConst>
bool Array<T>::BaseIterator<IsConst>::hasNext() const {
    return isReverse_ ? current_ > end_ : current_ < end_;
}

template<typename T>
Array<T>::Array(int capacity) : capacity_(capacity), size_(0) {
    buf_ = static_cast<T*>(malloc(capacity * sizeof(T)));
}

template <typename T>
Array<T>::Array(const Array<T>& rhs) {
    buf_ = static_cast<T*>(malloc(rhs.capacity_ * sizeof(T)));
    size_ = rhs.size_;
    capacity_ = rhs.capacity_;
    for (int i = 0; i < size_; ++i) {
        new (buf_ + i) T(*(rhs.buf_ + i));
    }
}

template <typename T>
Array<T>::Array(Array<T>&& rhs) noexcept {
    buf_ = rhs.buf_;
    size_ = rhs.size_;
    capacity_ = rhs.capacity_;

    rhs.buf_ = nullptr;
    rhs.size_ = 0;
    rhs.capacity_ = 0;
}

template <typename T>
Array<T>& Array<T>::operator=(const Array<T>& rhs) {
    if (this != &rhs) {
        Array<T> copy(rhs);
        Swap(copy);
    }
    return *this;
}

template <typename T>
Array<T>& Array<T>::operator=(Array<T>&& rhs) noexcept {
    Array<T> temp(std::move(rhs));
    Swap(temp);
    return *this;
}

template <typename T>
void Array<T>::Swap(Array<T>& rhs) noexcept {
    std::swap(buf_, rhs.buf_);
    std::swap(size_, rhs.size_);
    std::swap(capacity_, rhs.capacity_);
}

template<typename T>
Array<T>::~Array() {
    for (int i = 0; i < size_; ++i) {
        buf_[i].~T();
    }
    free(buf_);
}

template <typename T>
int Array<T>::Insert(int index, const T& value)
    requires (std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>) {

    // index = std::clamp(index, 0, size_);
    assert(CheckIndex(index));

    if (size_ == capacity_) {
        int new_capacity = std::max(capacity_ + 1, static_cast<int>(capacity_ * resize_coef));
        T* new_buf = static_cast<T*>(malloc(new_capacity * sizeof(T)));

        new(new_buf + index) T(value);

        for (int i = 0; i < index; ++i) {
            ConstructMoveOrCopy(new_buf + i, buf_ + i);
            (buf_ + i)->~T();
        }

        for (int i = index; i < size_; ++i) {
            ConstructMoveOrCopy(new_buf + i + 1, buf_ + i);
            (buf_ + i)->~T();
        }

        free(buf_);
        buf_ = new_buf;
        capacity_ = new_capacity;
        ++size_;
    } else {
        if (index == size_) {
            new(buf_ + index) T(value);
            ++size_;
        } else {
            T* p = buf_ + size_;
            T value_copy = value;

            ConstructMoveOrCopy(p, p-1);
            ++size_;

            for (--p; p > buf_ + index; --p) {
                AssignMoveOrCopy(p, p-1);
            }

            AssignMoveOrCopy(p, &value_copy);
        }
    }

    return index;
}

template <typename T>
int Array<T>::Insert(const T& value) {
    return Insert(size_, value);
}

template <typename T>
void Array<T>::Remove(int index) requires (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>) {
    for (T* p = buf_ + index + 1; p < buf_ + size_; ++p) {
        AssignMoveOrCopy(p-1, p);
    }
    (buf_ + size_ - 1)->~T();
    --size_;
}

template <typename T>
const T& Array<T>::operator[](int index) const {
    assert(CheckIndex(index));
    return buf_[index];
}

template <typename T>
T& Array<T>::operator[](int index) {
    assert(CheckIndex(index));
    return buf_[index];
}

template <typename T>
int Array<T>::size() const {
    return size_;
}

template <typename T>
Array<T>::Iterator Array<T>::iterator() {
    return Iterator(buf_, buf_ + size_);
}

template <typename T>
Array<T>::ConstIterator Array<T>::iterator() const {
    return ConstIterator(buf_, buf_ + size_);
}

template <typename T>
Array<T>::Iterator Array<T>::reverseIterator() {
    return Iterator(buf_, buf_ + size_, true);
}

template <typename T>
Array<T>::ConstIterator Array<T>::reverseIterator() const {
    return ConstIterator(buf_, buf_ + size_, true);
}

template <typename T>
void Array<T>::ConstructMoveOrCopy(T* dst, T* src) {
    if constexpr (std::is_nothrow_move_constructible_v<T>) {
        new(dst) T(std::move(*src));
    } else {
        new(dst) T(*src);
    }
}

template <typename T>
void Array<T>::AssignMoveOrCopy(T* dst, T* src) {
    if constexpr (std::is_move_assignable_v<T>) {
        *dst = std::move(*src);
    } else {
        *dst = *src;
    }
}

template <typename T>
bool Array<T>::CheckIndex(int index) const {
    return 0 <= index && index < size_;
}



