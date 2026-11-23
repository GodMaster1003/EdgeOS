//
// Created by godmaster on 2026/11/18.
//

#ifndef SELF_TEST_SCHEDULER_HPP
#define SELF_TEST_SCHEDULER_HPP

#include <memory>
#include <vector>
#include<array>
#include<utility>
#include "AVL.hpp"
#include "BinaryHeap.hpp"
#include "HashTable.hpp"
#include "HistoryStack.hpp"
#include "Task.hpp"

class Scheduler {
public:
    using STATUS = Task::STATUS;

    Scheduler() = default;

    Scheduler(const Scheduler &rhs);

    Scheduler &operator=(const Scheduler &rhs);

    Scheduler(Scheduler &&rhs) noexcept = default;

    Scheduler &operator=(Scheduler &&rhs) noexcept = default;

    ~Scheduler() = default;

    const Task *find(const std::string &id) const;

    bool add_sensor_task(const std::string &id, int basePriority, int value, int threshold);

    bool add_control_task(const std::string &id, int basePriority, int urgency);

    bool cancel(const std::string &id);

    Task *peek();                                                                           // 查看当前优先级最高的待执行任务
    Task *run();                                                                            // 执行当前优先级最高的待执行任务
    Task *undo();                                                                           // 撤销最近一次尚未撤销的成功run
    std::vector<const Task *> list(std::size_t k) const;                                    // 按执行优先级显示前k个待执行任务
    std::vector<const Task *> ids(const std::string &left, const std::string &right) const; // 按id字典序显示指定范围内的pending任务
    std::vector<const Task *> history(std::size_t k) const;                                 // 按最近执行顺序显示最多k条尚未撤销的执行记录
    std::array<std::pair<STATUS, std::size_t>, 3> stat() const;

    bool serialize(std::ostream &output) const;

    bool deserialize(std::istream &input);

private:
    std::size_t next_sequence = 0;

    std::vector<std::unique_ptr<Task> > tasks; // 任务队列
    AVL id_avl;                                // 查询pending_id
    History_Stack stack;                       // 查询运行历史
    BinaryHeap heap;                           // 任务优先级排序
    Hash_Table hashtable;                      // 查询all_id

    std::size_t pending_tasks_count = 0;
    std::size_t done_tasks_count = 0;
    std::size_t canceled_tasks_count = 0;

    // argument rangement check
    class ID_Check {
    public:
        bool operator()(const std::string &value, const Scheduler &scheduler) const;
    };

    class Range_Check {
    public:
        bool operator()(int value, int lower = 0, int upper = 100) const;
    };

    class Sensor_Check : public Range_Check {
    public:
        bool operator()(int value1, int value2, int value3) const;
    };

    class Control_Check : public Range_Check {
    public:
        bool operator()(int value1, int value2) const;
    };

    // add_task logic
    bool add_task(std::unique_ptr<Task> &&task);

    void register_new_task(Task *task);

    // cancel logic
    void pending_cancel(Task *task);

    // undo logic
    void restore_task(Task *task);

    //swap logic
    static void swap(Scheduler &lhs, Scheduler &rhs) noexcept;

    //for deserialize
    bool register_loaded_task(std::unique_ptr<Task> &task);

    bool parse_integer(const std::string &str, std::size_t &result);
};
#endif // SELF_TEST_SCHEDULER_HPP
