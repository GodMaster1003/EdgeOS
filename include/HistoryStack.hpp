//
// Created by shdff on 2026/11/18.
//

#ifndef SELF_TEST_STATCK_HPP
#define SELF_TEST_STATCK_HPP
#include <memory>
#include <vector>
#include "Task.hpp"

class History_Stack {
    friend class Scheduler;

public:
    using iterator = std::vector<std::string>::iterator;
    using const_iterator = std::vector<std::string>::const_iterator;

    History_Stack() = default;

    History_Stack(const History_Stack &rhs) = default;

    History_Stack(History_Stack &&rhs) = default;

    History_Stack &operator=(const History_Stack &rhs) = default;

    History_Stack &operator=(History_Stack &&rhs) = default;

    ~History_Stack() = default;

    void push(const std::string &id);

    std::string pop();

    void pop(std::string &id);

    const std::string &top() const;

    bool empty() const noexcept;

    std::size_t size() const noexcept;

    std::vector<std::string> recent(std::size_t times) const;

    void clear();

    iterator begin() noexcept;

    iterator end() noexcept;

    const_iterator begin() const noexcept;

    const_iterator end() const noexcept;

    const_iterator cbegin() const noexcept;

    const_iterator cend() const noexcept;

private:
    std::vector<std::string> tasks;
};
#endif // SELF_TEST_STATCK_HPP
