//
// Created by shdff on 2026/11/18.
//

#ifndef SELF_TEST_HASHTABLE_HPP
#define SELF_TEST_HASHTABLE_HPP
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "AVL.hpp"
#include "Task.hpp"

class Hash_Table {
public:
    explicit Hash_Table(std::size_t size = defined_size)
        : tasks(size > 0 ? size : defined_size),
          treeified(tasks.size(), 0),
          bucket_size(tasks.size(), 0) {};

    bool insert(Task *task);

    bool remove(const std::string &x);

    bool contains(const std::string &x) const;

    Task *find(const std::string &x) const;

    void clear();

    static constexpr int defined_size = 101;

private:
    static constexpr std::size_t TREEIFIED_THRESHOLD = 6;
    static constexpr std::size_t UNTREEIFIED_THRESHOLD = 3;
    std::vector<AVL::Node_t> tasks;
    std::vector<bool> treeified;
    std::vector<std::size_t> bucket_size;
    std::size_t current_size = 0;

    void rehash();

    static void rehash_nodes(const AVL::Node_t &root, Hash_Table &hashtable);

    std::size_t myhash(const std::string &x) const;

    static inline bool is_prime(std::size_t x);

    static inline std::size_t next_prime(std::size_t x);

    void insert_avl_node(Task *task, AVL::Node_t &root, std::size_t whichlist);

    void insert_node(Task *task, AVL::Node_t &root, std::size_t whichlist);

    void remove_avl_node(const std::string &id, AVL::Node_t &root, std::size_t whichlist);

    void remove_node(const std::string &id, AVL::Node_t &root, std::size_t whichlist);

    static void treeify(AVL::Node_t &root);

    static void untreeify(AVL::Node_t &root);

    static void flatten(AVL::Node_t &&node, AVL::Node_t &head);

    bool contains_hash_table(const std::string &id, const AVL::Node_t &root, std::size_t whichlist) const;

    static bool contains_node(const std::string &id, const AVL::Node_t &root);

    static bool contains_avl_node(const std::string &id, const AVL::Node_t &root);
};
#endif // SELF_TEST_HASHTABLE_HPP
