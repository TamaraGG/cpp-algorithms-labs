#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>

#include "Array.h"

TEST(ArrayConstructorTest, DefaultConstructorCreatesEmptyArray) {
    Array<int> a;

    EXPECT_EQ(a.size(), 0);
}

TEST(ArrayConstructorTest, CapacityConstructorCreatesEmptyArray) {
    Array<int> a(10);

    EXPECT_EQ(a.size(), 0);
}

TEST(ArrayInsertTest, CanInsertIntoZeroCapacityArray) {
    Array<int> a(0);

    EXPECT_EQ(a.Insert(42), 0);

    ASSERT_EQ(a.size(), 1);
    EXPECT_EQ(a[0], 42);
}

TEST(ArrayInitializerListConstructorTest, InitializerListPreservesOrder) {
    Array<int> a{10, 20, 30};

    ASSERT_EQ(a.size(), 3);
    EXPECT_EQ(a[0], 10);
    EXPECT_EQ(a[1], 20);
    EXPECT_EQ(a[2], 30);
}

TEST(ArrayInsertTest, AppendReturnsIndexAndPreservesOrder) {
    Array<int> a;

    EXPECT_EQ(a.Insert(10), 0);
    EXPECT_EQ(a.size(), 1);
    EXPECT_EQ(a.Insert(20), 1);
    EXPECT_EQ(a.size(), 2);
    EXPECT_EQ(a.Insert(30), 2);

    ASSERT_EQ(a.size(), 3);
    EXPECT_EQ(a[0], 10);
    EXPECT_EQ(a[1], 20);
    EXPECT_EQ(a[2], 30);
}

TEST(ArrayInsertTest, InsertAtBeginningShiftsElementsRight) {
    for (int capacity : {3, 8}) {
        Array<int> a(capacity);
        a.Insert(10);
        a.Insert(20);
        a.Insert(30);

        EXPECT_EQ(a.Insert(0, 5), 0);

        ASSERT_EQ(a.size(), 4);
        EXPECT_EQ(a[0], 5);
        EXPECT_EQ(a[1], 10);
        EXPECT_EQ(a[2], 20);
        EXPECT_EQ(a[3], 30);
    }
}

TEST(ArrayInsertTest, InsertInMiddleShiftsElementsRight) {
    for (int capacity : {3, 8}) {
        Array<int> a(capacity);
        a.Insert(10);
        a.Insert(20);
        a.Insert(30);

        EXPECT_EQ(a.Insert(1, 15), 1);

        ASSERT_EQ(a.size(), 4);
        EXPECT_EQ(a[0], 10);
        EXPECT_EQ(a[1], 15);
        EXPECT_EQ(a[2], 20);
        EXPECT_EQ(a[3], 30);
    }
}

TEST(ArrayInsertTest, InsertAtSizeAppendsElement) {
    for (int capacity : {3, 8}) {
        Array<int> a(capacity);
        a.Insert(10);
        a.Insert(20);
        a.Insert(30);

        EXPECT_EQ(a.Insert(a.size(), 40), 3);

        ASSERT_EQ(a.size(), 4);
        EXPECT_EQ(a[0], 10);
        EXPECT_EQ(a[1], 20);
        EXPECT_EQ(a[2], 30);
        EXPECT_EQ(a[3], 40);
    }
}

TEST(ArrayInsertTest, RepeatedGrowthPreservesAllElements) {
    Array<int> a(1);

    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(a.Insert(i), i);
        EXPECT_EQ(a.size(), i + 1);
    }

    ASSERT_EQ(a.size(), 100);
    for (int i = 0; i < a.size(); ++i) {
        EXPECT_EQ(a[i], i);
    }
}

TEST(ArrayRemoveTest, RemoveFirstElementShiftsElementsLeft) {
    Array<int> a{10, 20, 30};

    a.Remove(0);

    ASSERT_EQ(a.size(), 2);
    EXPECT_EQ(a[0], 20);
    EXPECT_EQ(a[1], 30);
}

TEST(ArrayRemoveTest, RemoveMiddleElementPreservesOrder) {
    Array<int> a{10, 20, 30};

    a.Remove(1);

    ASSERT_EQ(a.size(), 2);
    EXPECT_EQ(a[0], 10);
    EXPECT_EQ(a[1], 30);
}

TEST(ArrayRemoveTest, RemoveLastElementPreservesOtherElements) {
    Array<int> a{10, 20, 30};

    a.Remove(2);

    ASSERT_EQ(a.size(), 2);
    EXPECT_EQ(a[0], 10);
    EXPECT_EQ(a[1], 20);
}

TEST(ArrayInsertTest, CanInsertAfterRemovingAllElements) {
    Array<int> a{10, 20};

    a.Remove(0);
    a.Remove(0);
    EXPECT_EQ(a.size(), 0);

    EXPECT_EQ(a.Insert(42), 0);

    ASSERT_EQ(a.size(), 1);
    EXPECT_EQ(a[0], 42);
}

TEST(ArraySubscriptOperatorTest, IndexingAllowsModificationAndConstReading) {
    Array<int> a{10, 20, 30};

    a[1] = 42;

    const Array<int>& constArray = a;
    EXPECT_EQ(constArray[0], 10);
    EXPECT_EQ(constArray[1], 42);
    EXPECT_EQ(constArray[2], 30);
}

TEST(ArrayCopyConstructorTest, CopyConstructorCreatesIndependentArray) {
    Array<std::string> original{"one", "two"};

    Array<std::string> copy(original);

    ASSERT_EQ(copy.size(), 2);
    EXPECT_EQ(copy[0], "one");
    EXPECT_EQ(copy[1], "two");

    copy[0] = "changed";
    EXPECT_EQ(original[0], "one");
}

TEST(ArrayCopyAssignmentTest, CopyAssignmentReplacesContentsWithIndependentCopy) {
    Array<std::string> original{"one", "two"};
    Array<std::string> copy{"1", "2", "3"};

    copy = original;

    ASSERT_EQ(copy.size(), 2);
    EXPECT_EQ(copy[0], "one");
    EXPECT_EQ(copy[1], "two");

    copy[1] = "changed";
    EXPECT_EQ(original[1], "two");
}

TEST(ArrayCopyConstructorAndAssignmentTest, CanCopyEmptyArray) {
    Array<int> empty;
    Array<int> copy(empty);
    Array<int> assigned{10, 20};

    assigned = empty;

    EXPECT_EQ(copy.size(), 0);
    EXPECT_EQ(assigned.size(), 0);

    copy.Insert(30);
    EXPECT_EQ(empty.size(), 0);
    ASSERT_EQ(copy.size(), 1);
    EXPECT_EQ(copy[0], 30);
}

TEST(ArrayCopyAssignmentTest, CopySelfAssignmentPreservesContents) {
    Array<int> a{10, 20};

    a = a;

    ASSERT_EQ(a.size(), 2);
    EXPECT_EQ(a[0], 10);
    EXPECT_EQ(a[1], 20);
}

TEST(ArrayMoveConstructorTest, MoveConstructorTransfersContents) {
    Array<std::string> original{"one", "two"};

    Array<std::string> moved(std::move(original));

    ASSERT_EQ(moved.size(), 2);
    EXPECT_EQ(moved[0], "one");
    EXPECT_EQ(moved[1], "two");

    EXPECT_EQ(original.size(), 0);
    original.Insert("new");
    ASSERT_EQ(original.size(), 1);
    EXPECT_EQ(original[0], "new");
    EXPECT_EQ(moved[0], "one");
}

TEST(ArrayMoveAssignmentTest, MoveAssignmentReplacesExistingContents) {
    Array<std::string> original{"one", "two"};
    Array<std::string> moved{"old", "values", "here"};

    moved = std::move(original);

    ASSERT_EQ(moved.size(), 2);
    EXPECT_EQ(moved[0], "one");
    EXPECT_EQ(moved[1], "two");

    EXPECT_EQ(original.size(), 0);
    original.Insert("new");
    ASSERT_EQ(original.size(), 1);
    EXPECT_EQ(original[0], "new");
    EXPECT_EQ(moved[0], "one");
}

TEST(ArraySwapTest, SwapExchangesContentsAndSizes) {
    Array<int> first{10, 20};
    Array<int> second{30};

    first.Swap(second);

    ASSERT_EQ(first.size(), 1);
    EXPECT_EQ(first[0], 30);
    ASSERT_EQ(second.size(), 2);
    EXPECT_EQ(second[0], 10);
    EXPECT_EQ(second[1], 20);
}

TEST(ArrayIteratorTest, ForwardIteratorVisitsEveryElement) {
    Array<int> a{10, 20, 30};
    auto it = a.iterator();

    for (int expected : {10, 20, 30}) {
        ASSERT_TRUE(it.hasNext());
        EXPECT_EQ(it.get(), expected);
        it.next();
    }

    EXPECT_FALSE(it.hasNext());
}

TEST(ArrayReverseIteratorTest, ReverseIteratorVisitsEveryElement) {
    Array<int> a{10, 20, 30};
    auto it = a.reverseIterator();

    for (int expected : {30, 20, 10}) {
        ASSERT_TRUE(it.hasNext());
        EXPECT_EQ(it.get(), expected);
        it.next();
    }

    EXPECT_FALSE(it.hasNext());
}

TEST(ArrayIteratorAndReverseIteratorTest, ConstIteratorsTraverseInBothDirections) {
    const Array<int> a{10, 20, 30};
    auto forward = a.iterator();
    auto reverse = a.reverseIterator();

    for (int i = 0; i < a.size(); ++i) {
        ASSERT_TRUE(forward.hasNext());
        ASSERT_TRUE(reverse.hasNext());
        EXPECT_EQ(forward.get(), a[i]);
        EXPECT_EQ(reverse.get(), a[a.size() - 1 - i]);
        forward.next();
        reverse.next();
    }

    EXPECT_FALSE(forward.hasNext());
    EXPECT_FALSE(reverse.hasNext());
}

TEST(ArrayIteratorAndReverseIteratorTest, EmptyArrayIteratorsHaveNoElements) {
    Array<int> a;
    const Array<int>& constArray = a;

    EXPECT_FALSE(a.iterator().hasNext());
    EXPECT_FALSE(a.reverseIterator().hasNext());
    EXPECT_FALSE(constArray.iterator().hasNext());
    EXPECT_FALSE(constArray.reverseIterator().hasNext());
}

TEST(ArrayIteratorAndReverseIteratorTest, SingleElementIteratorsVisitElementOnce) {
    Array<int> a{42};
    auto forward = a.iterator();
    auto reverse = a.reverseIterator();

    ASSERT_TRUE(forward.hasNext());
    ASSERT_TRUE(reverse.hasNext());
    EXPECT_EQ(forward.get(), 42);
    EXPECT_EQ(reverse.get(), 42);

    forward.next();
    reverse.next();

    EXPECT_FALSE(forward.hasNext());
    EXPECT_FALSE(reverse.hasNext());
}

TEST(ArrayIteratorSetTest, IteratorSetChangesArrayElements) {
    Array<int> a{10, 20, 30};
    auto forward = a.iterator();
    auto reverse = a.reverseIterator();

    int value = 100;
    forward.set(value);
    forward.next();
    forward.set(200);
    reverse.set(300);

    EXPECT_EQ(a[0], 100);
    EXPECT_EQ(a[1], 200);
    EXPECT_EQ(a[2], 300);
}

TEST(ArrayInsertAndRemoveTest, StringsSurviveGrowthInsertionAndRemoval) {
    Array<std::string> a(1);
    a.Insert("one");
    a.Insert("three");
    a.Insert(1, "two");

    ASSERT_EQ(a.size(), 3);
    EXPECT_EQ(a[0], "one");
    EXPECT_EQ(a[1], "two");
    EXPECT_EQ(a[2], "three");

    a.Remove(0);

    ASSERT_EQ(a.size(), 2);
    EXPECT_EQ(a[0], "two");
    EXPECT_EQ(a[1], "three");
}

namespace {

// Вспомогательный тип для проверки времени жизни
struct TrackedObject {
    inline static int alive = 0;

    TrackedObject() { ++alive; }
    TrackedObject(const TrackedObject&) { ++alive; }
    TrackedObject(TrackedObject&&) noexcept { ++alive; }
    TrackedObject& operator=(const TrackedObject&) = default;
    TrackedObject& operator=(TrackedObject&&) noexcept = default;
    ~TrackedObject() { --alive; }
};

}

// Проверяем вызовы деструкторов по счётчику: при расширении не остаются лишние живые объекты, удаление уничтожает один,
// выход из области массива уничтожает оставшиеся элементы, а после уничтожения исходного объекта счётчик равен нулю.
TEST(ArrayRemoveAndDestructorTest, RemovalAndDestructionDestroyStoredObjects) {
    ASSERT_EQ(TrackedObject::alive, 0);

    {
        TrackedObject value;
        EXPECT_EQ(TrackedObject::alive, 1);

        {
            Array<TrackedObject> a(1);
            EXPECT_EQ(TrackedObject::alive, 1);

            a.Insert(value);
            a.Insert(value);
            a.Insert(value);
            EXPECT_EQ(TrackedObject::alive, 4);

            a.Remove(1);
            EXPECT_EQ(TrackedObject::alive, 3);
        }

        EXPECT_EQ(TrackedObject::alive, 1);
    }

    EXPECT_EQ(TrackedObject::alive, 0);
}

TEST(ArrayInsertTest, ResourceOwnersSurviveGrowthAndInsertion) {
    for (int capacity : {2, 8}) {
        auto first = std::make_shared<int>(10);
        auto middle = std::make_shared<int>(20);
        auto last = std::make_shared<int>(30);
        Array<std::shared_ptr<int>> a(capacity);

        a.Insert(first);
        a.Insert(last);
        a.Insert(1, middle);

        ASSERT_EQ(a.size(), 3);
        EXPECT_EQ(a[0], first);
        EXPECT_EQ(a[1], middle);
        EXPECT_EQ(a[2], last);
        ASSERT_NE(a[0], nullptr);
        ASSERT_NE(a[1], nullptr);
        ASSERT_NE(a[2], nullptr);
        EXPECT_EQ(*a[0], 10);
        EXPECT_EQ(*a[1], 20);
        EXPECT_EQ(*a[2], 30);

        EXPECT_EQ(first.use_count(), 2);
        EXPECT_EQ(middle.use_count(), 2);
        EXPECT_EQ(last.use_count(), 2);
    }
}

struct ConstructOnly {
    std::shared_ptr<int> value;

    explicit ConstructOnly(int number) : value(std::make_shared<int>(number)) {}
    ConstructOnly(const ConstructOnly&) = default;
    ConstructOnly(ConstructOnly&&) noexcept = default;
    ConstructOnly& operator=(const ConstructOnly&) = delete;
    ConstructOnly& operator=(ConstructOnly&&) = delete;
};

struct CopyConstructOnly {
    std::shared_ptr<int> value;

    explicit CopyConstructOnly(int number) : value(std::make_shared<int>(number)) {}
    CopyConstructOnly(const CopyConstructOnly&) = default;
    CopyConstructOnly(CopyConstructOnly&&) = delete;
    CopyConstructOnly& operator=(const CopyConstructOnly&) = delete;
    CopyConstructOnly& operator=(CopyConstructOnly&&) = delete;
};


TEST(ArrayInsertTest, SupportsConstructibleTypesWithoutAssignment) {
    for (int capacity : {0, 2, 8}) {
        Array<ConstructOnly> a(capacity);
        ConstructOnly first(10);
        ConstructOnly middle(20);
        ConstructOnly last(30);

        EXPECT_EQ(a.Insert(first), 0);
        EXPECT_EQ(a.Insert(last), 1);
        EXPECT_EQ(a.Insert(1, middle), 1);

        ASSERT_EQ(a.size(), 3);
        EXPECT_EQ(a[0].value, first.value);
        EXPECT_EQ(a[1].value, middle.value);
        EXPECT_EQ(a[2].value, last.value);

        EXPECT_EQ(a.Insert(0, a[2]), 0);

        ASSERT_EQ(a.size(), 4);
        EXPECT_EQ(a[0].value, last.value);
        EXPECT_EQ(a[1].value, first.value);
        EXPECT_EQ(a[2].value, middle.value);
        EXPECT_EQ(a[3].value, last.value);
    }
}

TEST(ArrayInsertTest, SupportsCopyOnlyTypesWithoutAssignment) {
    for (int capacity : {0, 2, 8}) {
        CopyConstructOnly first(10);
        CopyConstructOnly middle(20);
        CopyConstructOnly last(30);
        {
            Array<CopyConstructOnly> a(capacity);
            EXPECT_EQ(a.Insert(first), 0);
            EXPECT_EQ(a.Insert(last), 1);
            EXPECT_EQ(a.Insert(1, middle), 1);

            ASSERT_EQ(a.size(), 3);
            EXPECT_EQ(a[0].value, first.value);
            EXPECT_EQ(a[1].value, middle.value);
            EXPECT_EQ(a[2].value, last.value);

            EXPECT_EQ(a.Insert(0, a[2]), 0);

            ASSERT_EQ(a.size(), 4);
            EXPECT_EQ(a[0].value, last.value);
            EXPECT_EQ(a[1].value, first.value);
            EXPECT_EQ(a[2].value, middle.value);
            EXPECT_EQ(a[3].value, last.value);
            EXPECT_EQ(first.value.use_count(), 2);
            EXPECT_EQ(middle.value.use_count(), 2);
            EXPECT_EQ(last.value.use_count(), 3);
        }
        EXPECT_EQ(first.value.use_count(), 1);
        EXPECT_EQ(middle.value.use_count(), 1);
        EXPECT_EQ(last.value.use_count(), 1);
    }
}

TEST(ArrayRemoveTest, RemoveReleasesOwnedResource) {
    Array<std::shared_ptr<int>> a;
    a.Insert(std::make_shared<int>(10));
    a.Insert(std::make_shared<int>(20));
    std::weak_ptr<int> removedResource = a[0];
    std::weak_ptr<int> remainingResource = a[1];

    a.Remove(0);

    EXPECT_TRUE(removedResource.expired());
    EXPECT_FALSE(remainingResource.expired());
    ASSERT_EQ(a.size(), 1);
    ASSERT_NE(a[0], nullptr);
    EXPECT_EQ(*a[0], 20);
    EXPECT_EQ(a[0].use_count(), 1);
}

TEST(ArrayDestructorTest, DestructionReleasesAllOwnedResources) {
    std::weak_ptr<int> firstResource;
    std::weak_ptr<int> secondResource;

    {
        Array<std::shared_ptr<int>> a(1);
        a.Insert(std::make_shared<int>(10));
        firstResource = a[0];
        a.Insert(std::make_shared<int>(20));
        secondResource = a[1];

        EXPECT_FALSE(firstResource.expired());
        EXPECT_FALSE(secondResource.expired());
    }

    EXPECT_TRUE(firstResource.expired());
    EXPECT_TRUE(secondResource.expired());
}

TEST(ArrayCopyConstructorTest, CopiedArrayKeepsSharedResourceAlive) {
    Array<std::shared_ptr<int>> original;
    original.Insert(std::make_shared<int>(42));
    std::weak_ptr<int> resource = original[0];

    {
        Array<std::shared_ptr<int>> copy(original);
        original.Remove(0);

        EXPECT_EQ(original.size(), 0);
        EXPECT_FALSE(resource.expired());
        ASSERT_EQ(copy.size(), 1);
        ASSERT_NE(copy[0], nullptr);
        EXPECT_EQ(*copy[0], 42);
        EXPECT_EQ(copy[0].use_count(), 1);
    }

    EXPECT_TRUE(resource.expired());
}

TEST(ArrayCopyAssignmentTest, CopyAssignmentReleasesOldResourceAndSharesNewOne) {
    Array<std::shared_ptr<int>> original;
    original.Insert(std::make_shared<int>(42));
    Array<std::shared_ptr<int>> copy;
    copy.Insert(std::make_shared<int>(99));
    std::weak_ptr<int> oldResource = copy[0];

    copy = original;

    EXPECT_TRUE(oldResource.expired());
    ASSERT_EQ(copy.size(), 1);
    EXPECT_EQ(copy[0], original[0]);
    EXPECT_EQ(copy[0].use_count(), 2);

    copy.Remove(0);
    ASSERT_EQ(original.size(), 1);
    ASSERT_NE(original[0], nullptr);
    EXPECT_EQ(*original[0], 42);
    EXPECT_EQ(original[0].use_count(), 1);
}

TEST(ArrayMoveAssignmentTest, MoveAssignmentReleasesOldResourceAndTransfersNewOne) {
    Array<std::shared_ptr<int>> original;
    original.Insert(std::make_shared<int>(42));
    std::weak_ptr<int> transferredResource = original[0];
    Array<std::shared_ptr<int>> moved;
    moved.Insert(std::make_shared<int>(99));
    std::weak_ptr<int> oldResource = moved[0];

    moved = std::move(original);

    EXPECT_TRUE(oldResource.expired());
    EXPECT_FALSE(transferredResource.expired());
    EXPECT_EQ(original.size(), 0);
    ASSERT_EQ(moved.size(), 1);
    ASSERT_NE(moved[0], nullptr);
    EXPECT_EQ(*moved[0], 42);
    EXPECT_EQ(moved[0].use_count(), 1);

    moved.Remove(0);
    EXPECT_TRUE(transferredResource.expired());
}

TEST(ArrayInsertTest, NestedArraysSurviveGrowthAndInsertion) {
    for (int capacity : {2, 8}) {
        Array<int> first{1, 2};
        Array<int> empty;
        Array<int> last{3, 4, 5};
        Array<Array<int>> a(capacity);

        a.Insert(first);
        a.Insert(last);
        a.Insert(1, empty);

        ASSERT_EQ(a.size(), 3);
        ASSERT_EQ(a[0].size(), 2);
        EXPECT_EQ(a[1].size(), 0);
        ASSERT_EQ(a[2].size(), 3);
        EXPECT_EQ(a[0][0], 1);
        EXPECT_EQ(a[0][1], 2);
        EXPECT_EQ(a[2][0], 3);
        EXPECT_EQ(a[2][1], 4);
        EXPECT_EQ(a[2][2], 5);

        first[0] = 99;
        EXPECT_EQ(a[0][0], 1);
    }
}

TEST(ArrayRemoveTest, RemovingNestedArrayPreservesRemainingRows) {
    Array<Array<int>> a{{1, 2}, Array<int>{}, {3, 4, 5}};

    a.Remove(0);

    ASSERT_EQ(a.size(), 2);
    EXPECT_EQ(a[0].size(), 0);
    ASSERT_EQ(a[1].size(), 3);
    EXPECT_EQ(a[1][0], 3);
    EXPECT_EQ(a[1][1], 4);
    EXPECT_EQ(a[1][2], 5);

    a.Remove(0);

    ASSERT_EQ(a.size(), 1);
    ASSERT_EQ(a[0].size(), 3);
    EXPECT_EQ(a[0][0], 3);
    EXPECT_EQ(a[0][1], 4);
    EXPECT_EQ(a[0][2], 5);
}

TEST(ArrayCopyConstructorTest, NestedArrayCopyConstructorCreatesIndependentRows) {
    Array<Array<int>> original{{1, 2}, Array<int>{}, {3}};
    Array<Array<int>> copy(original);

    ASSERT_EQ(copy.size(), 3);
    ASSERT_EQ(copy[0].size(), 2);
    EXPECT_EQ(copy[0][0], 1);
    EXPECT_EQ(copy[0][1], 2);
    EXPECT_EQ(copy[1].size(), 0);
    ASSERT_EQ(copy[2].size(), 1);
    EXPECT_EQ(copy[2][0], 3);

    copy[0][0] = 99;
    copy[2].Insert(4);

    EXPECT_EQ(original[0][0], 1);
    ASSERT_EQ(original[2].size(), 1);
    EXPECT_EQ(original[2][0], 3);
    EXPECT_EQ(copy[0][0], 99);
    ASSERT_EQ(copy[2].size(), 2);
    EXPECT_EQ(copy[2][1], 4);
}

TEST(ArrayCopyAssignmentTest, NestedArrayCopyAssignmentCreatesIndependentRows) {
    Array<Array<int>> original{{1, 2}, {3}};
    Array<Array<int>> copy{{9}, {8, 7}, {6}};

    copy = original;

    ASSERT_EQ(copy.size(), 2);
    ASSERT_EQ(copy[0].size(), 2);
    ASSERT_EQ(copy[1].size(), 1);
    EXPECT_EQ(copy[0][0], 1);
    EXPECT_EQ(copy[0][1], 2);
    EXPECT_EQ(copy[1][0], 3);

    copy[0][0] = 99;
    copy[1].Insert(4);
    EXPECT_EQ(original[0][0], 1);
    ASSERT_EQ(original[1].size(), 1);
    EXPECT_EQ(original[1][0], 3);
}

TEST(ArrayMoveConstructorTest, NestedArrayMoveConstructorPreservesRows) {
    Array<Array<int>> original{{1, 2}, Array<int>{}, {3}};

    Array<Array<int>> moved(std::move(original));

    ASSERT_EQ(moved.size(), 3);
    ASSERT_EQ(moved[0].size(), 2);
    EXPECT_EQ(moved[0][0], 1);
    EXPECT_EQ(moved[0][1], 2);
    EXPECT_EQ(moved[1].size(), 0);
    ASSERT_EQ(moved[2].size(), 1);
    EXPECT_EQ(moved[2][0], 3);
    EXPECT_EQ(original.size(), 0);

    original.Insert(Array<int>{9});
    ASSERT_EQ(original.size(), 1);
    ASSERT_EQ(original[0].size(), 1);
    EXPECT_EQ(original[0][0], 9);
    EXPECT_EQ(moved[0][0], 1);
}

TEST(ArrayMoveAssignmentTest, NestedArrayMoveAssignmentPreservesRows) {
    Array<Array<int>> original{{1, 2}, {3}};
    Array<Array<int>> moved{{9}, {8, 7}, {6}};

    moved = std::move(original);

    EXPECT_EQ(original.size(), 0);
    ASSERT_EQ(moved.size(), 2);
    ASSERT_EQ(moved[0].size(), 2);
    ASSERT_EQ(moved[1].size(), 1);
    EXPECT_EQ(moved[0][0], 1);
    EXPECT_EQ(moved[0][1], 2);
    EXPECT_EQ(moved[1][0], 3);
}

TEST(ArrayDestructorTest, NestedArrayDestructionReleasesOwnedResources) {
    std::weak_ptr<int> resource;

    {
        Array<std::shared_ptr<int>> row;
        row.Insert(std::make_shared<int>(42));
        resource = row[0];
        Array<Array<std::shared_ptr<int>>> a(1);
        a.Insert(row);
        a.Insert(row);
        row.Remove(0);

        EXPECT_FALSE(resource.expired());
        EXPECT_EQ(resource.use_count(), 2);

        a.Remove(0);
        EXPECT_FALSE(resource.expired());
        EXPECT_EQ(resource.use_count(), 1);
    }

    EXPECT_TRUE(resource.expired());
}
