//
// Created by shdff on 2026/11/18.
//

#ifndef SELF_TEST_TASK_HPP
#define SELF_TEST_TASK_HPP
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include<iostream>

class Task {
    friend class Scheduler;

public:
    enum class STATUS {
        Pending,  //等待中
        Done,     //已完成
        Canceled, //已取消
        Unknown
    };

    enum class TYPE {
        Base_Task,
        Sensor_Task,
        Control_Task
    };

    const std::string &id() const noexcept;

    int basePriority() const noexcept;

    std::size_t sequence() const noexcept;

    STATUS status() const noexcept;

    TYPE type() const noexcept;

    void status(std::string &status) const noexcept;

    virtual int finalPriority() const = 0;

    virtual void print(std::ostream& output)const=0;

    virtual std::unique_ptr<Task> clone() const = 0;

    virtual ~Task() = default;

protected:
    explicit Task(const std::string &id, int basePriority, std::size_t sequence, TYPE type)
        : _id_(id),
          _basePriority_(basePriority),
          _sequence_(sequence),
          _task_type_(type) {}

    explicit Task(std::string &&id, int basePriority, std::size_t sequence, TYPE type)
        : _id_(std::move(id)),
          _basePriority_(basePriority),
          _sequence_(sequence),
          _task_type_(type) {}

    std::string _id_;       // 唯一任务编号
    int _basePriority_;     // 基础优先级
    std::size_t _sequence_; // 创建顺序
    STATUS _status_ = STATUS::Pending;
    TYPE _task_type_;
};

class SensorTask : public Task {
    friend class Scheduler;

public:
    using Task::TYPE;

    // finalPriority = basePriority + max(0, value - threshold)
    int value() const noexcept;

    int threshold() const noexcept;

    int finalPriority() const override;

    std::unique_ptr<Task> clone() const override;

    void print(std::ostream &output) const override;

    virtual ~SensorTask() = default;

private:
    SensorTask(const std::string &id, int basePriority, std::size_t sequence, int value, int threshold)
        : Task(id, basePriority, sequence, TYPE::Sensor_Task),
          _value_(value),
          _threshold_(threshold) {}

    SensorTask(std::string &&id, int basePriority, std::size_t sequence, int value, int threshold)
        : Task(std::move(id), basePriority, sequence, TYPE::Sensor_Task),
          _value_(value),
          _threshold_(threshold) {}

    int _value_;     // 测量值
    int _threshold_; // 预警阈值
};

class ControlTask : public Task {
    friend class Scheduler;

public:
    // finalPriority = basePriority + 2 * urgency;
    int urgency() const noexcept;

    int finalPriority() const override;

    std::unique_ptr<Task> clone() const override;

    void print(std::ostream &output) const override;

    virtual ~ControlTask() = default;

private:
    ControlTask(const std::string &id, int basePriority, std::size_t sequence, int urgency)
        : Task(id, basePriority, sequence, TYPE::Control_Task),
          _urgency_(urgency) {}

    ControlTask(std::string &&id, int basePriority, std::size_t sequence, int urgency)
        : Task(std::move(id), basePriority, sequence, TYPE::Control_Task),
          _urgency_(urgency) {}

    int _urgency_; // 紧急程度
};
std::ostream& operator<<(std::ostream& output,const Task& task);
#endif // SELF_TEST_TASK_HPP
