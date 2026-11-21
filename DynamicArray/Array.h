#ifndef LABS_ARRAY_H
#define LABS_ARRAY_H
#include <algorithm>
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <type_traits>


template<typename T>
class Array final {
    static constexpr float resize_coef = 1.6f;

    int capacity_;
    int size_;
    T* buf_;

    template <bool IsConst>
    class BaseIterator {
        using Pointer = std::conditional_t<IsConst, const T*, T*>;
        using Reference = std::conditional_t<IsConst, const T&, T&>;
        friend class Array<T>;

        Pointer current_;
        Pointer end_;
        bool isReverse_;

        BaseIterator(Pointer current, Pointer end, bool isReverse = false);

    public:
        Reference operator*() const;
        BaseIterator& operator++();
        BaseIterator operator++(int);
        bool operator==(const BaseIterator& rhs) const;
        bool operator!=(const BaseIterator& rhs) const;

        const T& get() const;
        void set(const T& value) requires (!IsConst && std::is_copy_assignable_v<T>);
        void set(T&& value) requires (!IsConst && std::is_move_assignable_v<T>);
        void next();
        bool hasNext() const; // it'd be better name it as isValid (checks if value in current_ is valid )
    };

public:
    using Iterator = BaseIterator<false>;
    using ConstIterator = BaseIterator<true>;

    explicit Array(int capacity = 8);
    Array(std::initializer_list<T> list);
    Array(const Array<T>& rhs);
    Array(Array<T>&& rhs) noexcept;
    Array& operator=(const Array<T>& rhs);
    Array& operator=(Array<T>&& rhs) noexcept;
    void Swap(Array<T>& rhs) noexcept;
    ~Array();

    int Insert(int index, const T& value)
        requires (std::is_copy_constructible_v<T>);
    int Insert(const T& value) requires (std::is_copy_constructible_v<T>);

    void Remove(int index) requires (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T> ||
              std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>);

    const T& operator[](int index) const;
    T& operator[](int index);

    int size() const;

    Iterator begin();
    ConstIterator begin() const;
    ConstIterator cbegin() const;
    Iterator end();
    ConstIterator end() const;
    ConstIterator cend() const;

    Iterator rbegin();
    ConstIterator rbegin() const;
    ConstIterator crbegin() const;
    Iterator rend();
    ConstIterator rend() const;
    ConstIterator crend() const;

    Iterator iterator();
    ConstIterator iterator() const;
    Iterator reverseIterator();
    ConstIterator reverseIterator() const;


private:
    void ConstructMoveOrCopy(T* dst, T* src);
    void AssignMoveOrCopy(T* dst, T* src);
};


template <typename T>
template <bool IsConst>
Array<T>::BaseIterator<IsConst>::BaseIterator(Pointer current, Pointer end, bool isReverse)
    : current_(current), end_(end), isReverse_(isReverse) {}


template <typename T>
template <bool IsConst>
Array<T>::template BaseIterator<IsConst>::Reference Array<T>::BaseIterator<IsConst>::operator*() const {
    assert(hasNext());
    return isReverse_ ? *(current_ - 1) : *current_;
}

template <typename T>
template <bool IsConst>
Array<T>::template BaseIterator<IsConst>& Array<T>::BaseIterator<IsConst>::operator++() {
    next();
    return *this;
}

template <typename T>
template <bool IsConst>
Array<T>::template BaseIterator<IsConst> Array<T>::BaseIterator<IsConst>::operator++(int) {
    BaseIterator previous = *this;
    ++(*this);
    return previous;
}

template <typename T>
template <bool IsConst>
bool Array<T>::BaseIterator<IsConst>::operator==(const BaseIterator& rhs) const {
    return current_ == rhs.current_ && end_ == rhs.end_ && isReverse_ == rhs.isReverse_;
}

template <typename T>
template <bool IsConst>
bool Array<T>::BaseIterator<IsConst>::operator!=(const BaseIterator& rhs) const {
    return !(*this == rhs);
}


template <typename T>
template <bool IsConst>
const T& Array<T>::BaseIterator<IsConst>::get() const {
    assert(hasNext());
    return isReverse_ ? *(current_ - 1) : *current_;
}

template <typename T>
template <bool IsConst>
void Array<T>::BaseIterator<IsConst>::set(const T& value) requires (!IsConst && std::is_copy_assignable_v<T>) {
    assert(hasNext());
    (isReverse_ ? *(current_ - 1) : *current_) = value;
}

template <typename T>
template <bool IsConst>
void Array<T>::BaseIterator<IsConst>::set(T&& value) requires (!IsConst && std::is_move_assignable_v<T>) {
    assert(hasNext());
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
    assert(capacity >= 0);
    buf_ = static_cast<T*>(malloc(capacity * sizeof(T)));
}

template <typename T>
Array<T>::Array(std::initializer_list<T> list) : Array(list.size()) {
    for (auto p = list.begin(); p != list.end(); ++p) {
        new(buf_ + size_) T(*p);
        ++size_;
    }
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
    requires (std::is_copy_constructible_v<T>) {

    // index = std::clamp(index, 0, size_);
    assert(0 <= index && index <= size_);

    if (size_ == capacity_) {
        int new_capacity = std::max(capacity_ + 1, static_cast<int>(std::ceil(capacity_ * resize_coef)));
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

            if constexpr (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>) {
                for (--p; p > buf_ + index; --p) {
                    AssignMoveOrCopy(p, p-1);
                }

                AssignMoveOrCopy(p, &value_copy);
            } else {
                for (--p; p > buf_ + index; --p) {
                    p->~T();
                    ConstructMoveOrCopy(p, p-1);
                }

                p->~T();
                ConstructMoveOrCopy(p, &value_copy);
            }
        }
    }

    return index;
}

template <typename T>
int Array<T>::Insert(const T& value)
    requires (std::is_copy_constructible_v<T>){
    return Insert(size_, value);
}

template <typename T>
void Array<T>::Remove(int index)
requires (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T> ||
          std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>) {
    assert(0 <= index && index < size_);

    if constexpr (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>) {
        for (T* p = buf_ + index + 1; p < buf_ + size_; ++p) {
            AssignMoveOrCopy(p - 1, p);
        }
        (buf_ + size_ - 1)->~T();
    } else {
        (buf_ + index)->~T();
        for (T* p = buf_ + index + 1; p < buf_ + size_; ++p) {
            ConstructMoveOrCopy(p - 1, p);
            p->~T();
        }
    }
    --size_;
}

template <typename T>
const T& Array<T>::operator[](int index) const {
    assert(0 <= index && index < size_);
    return buf_[index];
}

template <typename T>
T& Array<T>::operator[](int index) {
    assert(0 <= index && index < size_);
    return buf_[index];
}

template <typename T>
int Array<T>::size() const {
    return size_;
}

template <typename T>
Array<T>::Iterator Array<T>::begin() {
    return Iterator(buf_, buf_ + size_);
}

template <typename T>
Array<T>::ConstIterator Array<T>::begin() const {
    return ConstIterator(buf_, buf_ + size_);
}

template <typename T>
Array<T>::ConstIterator Array<T>::cbegin() const {
    return begin();
}

template <typename T>
Array<T>::Iterator Array<T>::end() {
    return Iterator(buf_ + size_, buf_ + size_);
}

template <typename T>
Array<T>::ConstIterator Array<T>::end() const {
    return ConstIterator(buf_ + size_, buf_ + size_);
}

template <typename T>
Array<T>::ConstIterator Array<T>::cend() const {
    return end();
}

template <typename T>
Array<T>::Iterator Array<T>::rbegin() {
    return Iterator(buf_ + size_, buf_, true);
}

template <typename T>
Array<T>::ConstIterator Array<T>::rbegin() const {
    return ConstIterator(buf_ + size_, buf_, true);
}

template <typename T>
Array<T>::ConstIterator Array<T>::crbegin() const {
    return rbegin();
}

template <typename T>
Array<T>::Iterator Array<T>::rend() {
    return Iterator(buf_, buf_, true);
}

template <typename T>
Array<T>::ConstIterator Array<T>::rend() const {
    return ConstIterator(buf_, buf_, true);
}

template <typename T>
Array<T>::ConstIterator Array<T>::crend() const {
    return rend();
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
    return Iterator(buf_ + size_, buf_, true);
}

template <typename T>
Array<T>::ConstIterator Array<T>::reverseIterator() const {
    return ConstIterator(buf_ + size_, buf_, true);
}

template <typename T>
void Array<T>::ConstructMoveOrCopy(T* dst, T* src) {
    if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
        new(dst) T(std::move(*src));
    } else {
        new(dst) T(*src);
    }
}

template <typename T>
void Array<T>::AssignMoveOrCopy(T* dst, T* src) {
    if constexpr (std::is_nothrow_move_assignable_v<T> || !std::is_copy_assignable_v<T>) {
        *dst = std::move(*src);
    } else {
        *dst = *src;
    }
}



#endif //LABS_ARRAY_H
