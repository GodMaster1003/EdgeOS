#include "../include/BinaryHeap.hpp"
#include <stdexcept>
#include <utility>

std::size_t BinaryHeap::size() const { return current_size; }

Task *BinaryHeap::findMax() const
{
    if (empty())
        throw std::out_of_range("heap is empty");
    return heap[1];
}

void BinaryHeap::insert(Task *task)
{
    if (task == nullptr)
        throw std::invalid_argument("task is a nullptr");
    if (current_size == heap.size() - 1)
        heap.resize(heap.size() * 2);
    int hole = ++current_size;
    heap[0] = task;
    for (; higher(task, heap[hole / 2]); hole /= 2)
        heap[hole] = std::move(heap[hole / 2]);
    heap[hole] = std::move(heap[0]);
}

bool BinaryHeap::empty() const noexcept { return current_size == 0; }

void BinaryHeap::clear()
{
    heap.clear();
    heap.resize(1, nullptr);
    current_size = 0;
}

Task *BinaryHeap::deleteMax()
{
    if (empty())
        throw std::out_of_range("heap is empty");
    Task *result = std::move(heap[1]);
    heap[1] = std::move(heap[current_size]);
    heap[current_size] = nullptr;
    --current_size;
    parcelote_down(1);
    return result;
}

void BinaryHeap::build_heap()
{
    for (int hole = current_size / 2; hole >= 1; --hole)
        parcelote_down(hole);
}

void BinaryHeap::parcelote_down(std::size_t hole)
{
    int child;
    auto temp = std::move(heap[hole]);
    for (; hole * 2 <= current_size; hole = child)
    {
        child = 2 * hole;
        if (child != current_size && higher(heap[child + 1], heap[child]))
            ++child;
        if (higher(heap[child], temp))
            heap[hole] = std::move(heap[child]);
        else
            break;
    }
    heap[hole] = std::move(temp);
}

inline bool BinaryHeap::higher(const Task *lhs, const Task *rhs)
{
    if (lhs->finalPriority() != rhs->finalPriority())
        return lhs->finalPriority() > rhs->finalPriority();
    return lhs->sequence() < rhs->sequence();
}

Task *BinaryHeap::find_pending_max() const
{
    Task::STATUS PENDING = Task::STATUS::Pending;
    Task *best = nullptr;
    if (empty())
        return nullptr;
    for (int i = 1; i != current_size + 1; ++i)
    {
        Task *temp = heap[i];
        if (temp->status() != PENDING)
            continue;
        if (best == nullptr || higher(temp, best))
            best = temp;
    }
    return best;
}

//
// Created by godmaster on 2026/11/18.
//
