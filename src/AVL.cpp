#include "../include/AVL.hpp"
#include "../include/Task.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

AVL::AVL(const AVL &rhs) : root({nullptr}) { clone(root, rhs.root); }

AVL &AVL::operator=(const AVL &rhs)
{
    if (this != &rhs)
        clone(root, rhs.root);
    return *this;
}

void AVL::insert(Task *task) { insert(task, root); }

void AVL::remove(const std::string &id)
{
    if (root == nullptr)
        throw std::out_of_range("AVL is empty");
    remove(id, root);
}

const Task &AVL::findMax() const
{
    if (root == nullptr)
        throw std::out_of_range("AVL is empty");
    return *(findMax(root))->task;
}

const Task &AVL::findMin() const
{
    if (root == nullptr)
        throw std::out_of_range("AVL is empty");
    return *(findMin(root))->task;
}

bool AVL::contains(const std::string &id) { return contains(id, root); }

Task *AVL::find(const std::string &id) const { return find(root, id); }

std::vector<const Task *> AVL::ranges(const std::string &left, const std::string &right) const
{
    std::vector<const Task *> answer;
    range_and_insert(root, left, right, answer);
    return answer;
}

void AVL::insert(Task *task, Node_t &root)
{
    if (root == nullptr)
        root.reset(new Node(task));
    else if (task->id() < root->id)
        insert(task, root->left);
    else if (task->id() > root->id)
        insert(task, root->right);
    else
        return;
    balance(root);
}

void AVL::remove(const std::string &id, Node_t &root)
{
    if (root == nullptr)
        return;
    if (id < root->id)
        remove(id, root->left);
    else if (id > root->id)
        remove(id, root->right);
    else if (root->left != nullptr && root->right != nullptr)
    {
        root->id = findMin(root->right)->id;
        root->task = findMin(root->right)->task;
        remove(root->id, root->right);
    }
    else
    {
        auto temp = std::move(root);
        root = temp->left != nullptr ? std::move(temp->left) : std::move(temp->right);
    }
    balance(root);
}

const AVL::Node_t &AVL::findMax(const Node_t &root)
{
    if (root->right == nullptr)
        return root;
    return findMax(root->right);
}

const AVL::Node_t &AVL::findMin(const Node_t &root)
{
    if (root->left == nullptr)
        return root;
    return findMin(root->left);
}

bool AVL::contains(const std::string &id, const Node_t &root)
{
    if (root == nullptr)
        return false;
    if (id < root->id)
        return contains(id, root->left);
    if (id > root->id)
        return contains(id, root->right);
    return true;
}

Task *AVL::find(const Node_t &root, const std::string &id)
{
    if (root == nullptr)
        return nullptr;
    if (id < root->id)
        return find(root->left, id);
    if (id > root->id)
        return find(root->right, id);
    return root->task;
}

inline int AVL::height(const Node_t &root) { return root == nullptr ? -1 : root->height; }

static inline int max(int h1, int h2) { return h1 > h2 ? h1 : h2; }

void AVL::balance(Node_t &root)
{
    if (root == nullptr)
        return;
    if (height(root->left) - height(root->right) > ALLOWED_DISTANCE)
    {
        if (height(root->left->left) >= height(root->left->right))
            rotate_with_left_child(root);
        else
            double_rotate_with_left_child(root);
    }
    else if (height(root->right) - height(root->left) > ALLOWED_DISTANCE)
    {
        if (height(root->right->right) >= height(root->right->left))
            rotate_with_right_child(root);
        else
            double_rotate_with_right_child(root);
    }
    root->height = max(height(root->left), height(root->right)) + 1;
}

void AVL::rotate_with_left_child(Node_t &root)
{
    Node_t temp = std::move(root->left);
    root->left = std::move(temp->right);
    temp->right = std::move(root);
    root = std::move(temp);
    root->right->height = max(height(root->right->left), height(root->right->right)) + 1;
    root->height = max(height(root->left), height(root->right)) + 1;
}

void AVL::rotate_with_right_child(Node_t &root)
{
    Node_t temp = std::move(root->right);
    root->right = std::move(temp->left);
    temp->left = std::move(root);
    root = std::move(temp);
    root->left->height = max(height(root->left->left), height(root->left->right)) + 1;
    root->height = max(height(root->left), height(root->right)) + 1;
}

void AVL::double_rotate_with_left_child(Node_t &root)
{
    rotate_with_right_child(root->left);
    rotate_with_left_child(root);
}

void AVL::double_rotate_with_right_child(Node_t &root)
{
    rotate_with_left_child(root->right);
    rotate_with_right_child(root);
}

void AVL::clone(Node_t &root, const Node_t &cloned_root)
{
    if (cloned_root == nullptr)
    {
        root.reset();
        return;
    }
    Node_t new_root(new Node(cloned_root->task));
    new_root->height = cloned_root->height;
    clone(new_root->left, cloned_root->left);
    clone(new_root->right, cloned_root->right);
    root = std::move(new_root);
}

void AVL::range_and_insert(const Node_t &root, const std::string &left, const std::string &right,
                           std::vector<const Task *> &ans)
{
    if (root == nullptr)
        return;
    if (left < root->id)
        range_and_insert(root->left, left, right, ans);
    if (left <= root->id && root->id <= right)
        ans.push_back(root->task);
    if (right > root->id)
        range_and_insert(root->right, left, right, ans);
}

//
// Created by godmaster on 2026/11/18.
//
