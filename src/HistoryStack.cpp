#include "../include/HistoryStack.hpp"

#include <stdexcept>
#include <utility>

void History_Stack::push(const std::string &id) { tasks.push_back(id); }

std::string History_Stack::pop()
{
    if (empty())
        throw std::out_of_range("stack is empty");
    auto id = std::move(tasks.back());
    tasks.pop_back();
    return id;
}

void History_Stack::pop(std::string &id)
{
    if (empty())
        throw std::out_of_range("stack is empty");
    id = std::move(tasks.back());
    tasks.pop_back();
}

const std::string &History_Stack::top() const
{
    if (tasks.empty())
        throw std::out_of_range("stack is empty");
    return tasks.back();
}

bool History_Stack::empty() const noexcept { return tasks.empty(); }

void History_Stack::clear() { tasks.clear(); }

std::size_t History_Stack::size() const noexcept { return tasks.size(); }

std::vector<std::string> History_Stack::recent(std::size_t times) const
{
    times = times >= tasks.size() ? tasks.size() : times;
    std::vector<std::string> result;
    auto it = tasks.crbegin();
    for (std::size_t i = 0; i != times; ++i, ++it)
        result.push_back(*it);
    return result;
}


History_Stack::iterator History_Stack::begin() noexcept { return tasks.begin(); }
History_Stack::iterator History_Stack::end() noexcept { return tasks.end(); }
History_Stack::const_iterator History_Stack::begin() const noexcept { return tasks.begin(); }
History_Stack::const_iterator History_Stack::end() const noexcept { return tasks.end(); }
History_Stack::const_iterator History_Stack::cbegin() const noexcept { return tasks.cbegin(); }
History_Stack::const_iterator History_Stack::cend() const noexcept { return tasks.cend(); }
//
// Created by godmaster on 2026/11/18.
//
