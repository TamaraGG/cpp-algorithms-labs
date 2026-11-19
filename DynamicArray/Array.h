#ifndef LABS_ARRAY_H
#define LABS_ARRAY_H
#include <concepts>


template<typename T>
class Array final {
    static constexpr float resize_coef = 1.6f;

    int capacity_;
    int size_;
    T* buf_;

public:
    explicit Array(int capacity = 8);
    ~Array();

    int Insert(int index, const T& value)
        requires (std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>);
    int Insert(const T& value);

    void Remove(int index) requires (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>);

    int size() const;

private:
    void ConstructMoveOrCopy(T* dst, T* src);
    void AssignMoveOrCopy(T* dst, T* src);
    bool CheckIndex(int index) const;
};




#endif //LABS_ARRAY_H
