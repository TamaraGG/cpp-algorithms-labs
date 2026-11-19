#ifndef LABS_ARRAY_H
#define LABS_ARRAY_H
#include <concepts>


template<typename T>
class Array final {
    static constexpr float resize_coef = 1.6f;

    int capacity_;
    int size_;
    T* buf_;

    template <bool IsConst>
    class BaseIterator {
        using Pointer = std::conditional_t<IsConst, const T*, T*>;
        friend class Array<T>;

        Pointer current_;
        Pointer end_;
        bool isReverse_;

        BaseIterator(Pointer current, Pointer end, bool isReverse = false);

    public:
        const T& get() const;
        void set(const T& value) requires (!IsConst && std::is_copy_assignable_v<T>);
        void set(T&& value) requires (!IsConst && std::is_move_assignable_v<T>);
        void next();
        bool hasNext() const;
    };

    using Iterator = BaseIterator<false>;
    using ConstIterator = BaseIterator<true>;

public:
    explicit Array(int capacity = 8);
    Array(const Array<T>& rhs);
    Array(Array<T>&& rhs) noexcept;
    Array& operator=(const Array<T>& rhs);
    Array& operator=(Array<T>&& rhs) noexcept;
    void Swap(Array<T>& rhs) noexcept;
    ~Array();

    int Insert(int index, const T& value)
        requires (std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>);
    int Insert(const T& value);

    void Remove(int index) requires (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>);

    const T& operator[](int index) const;
    T& operator[](int index);

    int size() const;

    Iterator iterator();
    ConstIterator iterator() const;
    Iterator reverseIterator();
    ConstIterator reverseIterator() const;


private:
    void ConstructMoveOrCopy(T* dst, T* src);
    void AssignMoveOrCopy(T* dst, T* src);
    bool CheckIndex(int index) const;
};




#endif //LABS_ARRAY_H
