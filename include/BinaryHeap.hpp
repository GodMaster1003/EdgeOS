//
// Created by shdff on 2026/11/18.
//

#ifndef SELF_TEST_BINARY_HEAP_HPP
#define SELF_TEST_BINARY_HEAP_HPP
#include <vector>
#include "Task.hpp"

class BinaryHeap {
    friend class Scheduler;

public:
    explicit BinaryHeap(int capacity = INITIAL_CAPACITY) : heap(capacity > 0 ? capacity : INITIAL_CAPACITY,
                                                                {nullptr}) {}

    void insert(Task *task);

    Task *findMax() const;

    Task *deleteMax();

    std::size_t size() const;

    bool empty() const noexcept;

    void clear();

private:
    static constexpr int INITIAL_CAPACITY = 100;
    std::vector<Task *> heap;
    std::size_t current_size = 0;

    void build_heap();

    void parcelote_down(std::size_t hole);

    static bool higher(const Task *lhs, const Task *rhs);

    Task *find_pending_max() const;
};
#endif // SELF_TEST_BINARY_HEAP_HPP
