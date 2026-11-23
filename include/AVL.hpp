//
// Created by shdff on 2026/11/18.
//

#ifndef SELF_TEST_AVL_HPP
#define SELF_TEST_AVL_HPP
#include <memory>
#include<vector>
#include "Task.hpp"

class AVL {
    friend class Scheduler;
    friend class Hash_Table;

public:
    AVL() = default;

    AVL(const AVL &rhs);

    AVL &operator=(const AVL &rhs);

    AVL(AVL &&rhs) = default;

    AVL &operator=(AVL &&rhs) = default;

    ~AVL() = default;

    void insert(Task *task);

    void remove(const std::string &id);

    const Task &findMax() const;

    const Task &findMin() const;

    Task *find(const std::string &id) const;

    bool contains(const std::string &id);

    std::vector<const Task *> ranges(const std::string &left, const std::string &right) const;

    static constexpr int ALLOWED_DISTANCE = 1;

private:
    struct Node {
        std::string id;
        Task *task;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        int height = 0;

        explicit Node(Task *task) : id(task->id()), task(task) {}
    };

public:
    using Node_t = std::unique_ptr<Node>;

private:
    Node_t root;

    static void insert(Task *task, Node_t &root);

    static void remove(const std::string &id, Node_t &root);

    static const Node_t &findMax(const Node_t &root);

    static const Node_t &findMin(const Node_t &root);

    static Task *find(const Node_t &root, const std::string &id);

    static bool contains(const std::string &id, const Node_t &root);

    static void balance(Node_t &root);

    static void rotate_with_left_child(Node_t &root);

    static void rotate_with_right_child(Node_t &root);

    static void double_rotate_with_left_child(Node_t &root);

    static void double_rotate_with_right_child(Node_t &root);

    static int height(const Node_t &root);

    static void clone(Node_t &root, const Node_t &cloned_root);

    static void range_and_insert(const Node_t &root, const std::string &left, const std::string &right,
                                 std::vector<const Task *> &ans);
};
#endif // SELF_TEST_AVL_HPP
