#include "../include/HashTable.hpp"
#include <algorithm>

bool Hash_Table::insert(Task *task)
{
    std::size_t whichlist = myhash(task->id());
    if (contains(task->id()))
        return false;
    if (treeified[whichlist])
        insert_avl_node(task, tasks[whichlist], whichlist);
    else
        insert_node(task, tasks[whichlist], whichlist);
    if (++current_size > tasks.size())
        rehash();
    return true;
}

bool Hash_Table::remove(const std::string &task)
{
    std::size_t whichlist = myhash(task);
    if (!contains(task))
        return false;
    if (treeified[whichlist])
        remove_avl_node(task, tasks[whichlist], whichlist);
    else
        remove_node(task, tasks[whichlist], whichlist);
    --current_size;
    return true;
}

bool Hash_Table::contains(const std::string &x) const
{
    auto whichlist = myhash(x);
    return contains_hash_table(x, tasks[whichlist], whichlist);
}


Task *Hash_Table::find(const std::string &x) const
{
    auto whichlist = myhash(x);
    const auto &root = tasks[whichlist];
    if (treeified[whichlist])
        return AVL::find(root, x);
    else
        for (auto *p = root.get(); p != nullptr; p = p->right.get())
            if (x == p->id)
                return p->task;
    return nullptr;
}

void Hash_Table::clear()
{
    for (auto &x: tasks)
        x.reset();
    std::fill(treeified.begin(), treeified.end(), false);
    std::fill(bucket_size.begin(), bucket_size.end(), 0);
    current_size = 0;
}

std::size_t Hash_Table::myhash(const std::string &x) const { return std::hash<std::string>()(x) % tasks.size(); }

void Hash_Table::rehash()
{
    Hash_Table new_table(next_prime(2 * tasks.size()));
    for (const auto &root: tasks)
        rehash_nodes(root, new_table);
    std::swap(tasks, new_table.tasks);
    std::swap(treeified, new_table.treeified);
    std::swap(bucket_size, new_table.bucket_size);
    std::swap(current_size, new_table.current_size);
}

void Hash_Table::rehash_nodes(const AVL::Node_t &root, Hash_Table &new_table)
{
    if (root == nullptr)
        return;
    new_table.insert(root->task);
    rehash_nodes(root->left, new_table);
    rehash_nodes(root->right, new_table);
}

bool Hash_Table::is_prime(std::size_t x)
{
    if (x < 2)
        return false;
    if (x == 2)
        return true;
    if (x % 2 == 0)
        return false;
    for (std::size_t i = 3; i <= x / i; i += 2)
        if (x % i == 0)
            return false;
    return true;
}

std::size_t Hash_Table::next_prime(std::size_t x)
{
    if (x <= 2)
        return 2;
    if (x % 2 == 0)
        ++x;
    while (!is_prime(x))
        x += 2;
    return x;
}

void Hash_Table::insert_avl_node(Task *task, AVL::Node_t &root, std::size_t whichlist)
{
    AVL::insert(task, root);
    ++bucket_size[whichlist];
}

void Hash_Table::insert_node(Task *task, AVL::Node_t &root, std::size_t whichlist)
{
    AVL::Node_t newNode(new AVL::Node(task));
    newNode->right = std::move(root);
    root = std::move(newNode);
    ++bucket_size[whichlist];
    if (bucket_size[whichlist] == TREEIFIED_THRESHOLD)
    {
        treeify(root);
        treeified[whichlist] = true;
    }
}

void Hash_Table::remove_node(const std::string &id, AVL::Node_t &root, std::size_t whichlist)
{
    AVL::Node_t *link = &root;
    while (*link != nullptr)
    {
        if ((*link)->id == id)
        {
            AVL::Node_t removed = std::move(*link);
            *link = std::move(removed->right);
            --bucket_size[whichlist];
            return;
        }
        link = &(*link)->right;
    }
}

void Hash_Table::remove_avl_node(const std::string &id, AVL::Node_t &root, std::size_t whichlist)
{
    AVL::remove(id, root);
    --bucket_size[whichlist];
    if (bucket_size[whichlist] == UNTREEIFIED_THRESHOLD)
    {
        untreeify(root);
        treeified[whichlist] = false;
    }
}

bool Hash_Table::contains_hash_table(const std::string &id, const AVL::Node_t &root, std::size_t whichlist) const
{
    if (treeified[whichlist])
        return contains_avl_node(id, root);
    return contains_node(id, root);
}

bool Hash_Table::contains_node(const std::string &id, const AVL::Node_t &root)
{
    for (auto p = root.get(); p != nullptr; p = p->right.get())
        if (p->id == id)
            return true;
    return false;
}

bool Hash_Table::contains_avl_node(const std::string &id, const AVL::Node_t &root) { return AVL::contains(id, root); }

void Hash_Table::treeify(AVL::Node_t &root)
{
    AVL::Node_t newRoot;
    for (const auto *p = root.get(); p != nullptr; p = p->right.get())
        AVL::insert(p->task, newRoot);
    root = std::move(newRoot);
}

void Hash_Table::untreeify(AVL::Node_t &root)
{
    AVL::Node_t newRoot;
    flatten(std::move(root), newRoot);
    root = std::move(newRoot);
}

void Hash_Table::flatten(AVL::Node_t &&node, AVL::Node_t &root)
{
    if (node == nullptr)
        return;
    auto left = std::move(node->left);
    auto right = std::move(node->right);
    flatten(std::move(right), root);
    node->right = std::move(root);
    node->height = 0;
    root = std::move(node);
    flatten(std::move(left), root);
}

//
// Created by godmaster on 2026/11/18.
//
