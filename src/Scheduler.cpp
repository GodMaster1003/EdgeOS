#include "../include/Scheduler.hpp"
#include<limits>
#include <cctype>
#include<unordered_set>
#include <iostream>
#include<sstream>

Scheduler::Scheduler(const Scheduler &rhs) : next_sequence(rhs.next_sequence), stack(rhs.stack),
                                             pending_tasks_count(rhs.pending_tasks_count),
                                             done_tasks_count(rhs.done_tasks_count),
                                             canceled_tasks_count(rhs.canceled_tasks_count)
{
    tasks.reserve(rhs.tasks.size());
    for (const auto &task: rhs.tasks)
    {
        auto new_task = task->clone();
        Task *observer_task_ptr = new_task.get();
        tasks.push_back(std::move(new_task));
        hashtable.insert(observer_task_ptr);
        if (task->status() == STATUS::Pending)
        {
            id_avl.insert(observer_task_ptr);
            heap.insert(observer_task_ptr);
        }
    }
}

Scheduler &Scheduler::operator=(const Scheduler &rhs)
{
    if (this != &rhs)
    {
        Scheduler temp(rhs);
        swap(*this, temp);
    }
    return *this;
}

void Scheduler::swap(Scheduler &lhs, Scheduler &rhs) noexcept
{
    using std::swap;
    swap(lhs.next_sequence, rhs.next_sequence);
    swap(lhs.tasks, rhs.tasks);
    swap(lhs.id_avl, rhs.id_avl);
    swap(lhs.heap, rhs.heap);
    swap(lhs.stack, rhs.stack);
    swap(lhs.hashtable, rhs.hashtable);
    swap(lhs.pending_tasks_count, rhs.pending_tasks_count);
    swap(lhs.done_tasks_count, rhs.done_tasks_count);
    swap(lhs.canceled_tasks_count, rhs.canceled_tasks_count);
}

const Task *Scheduler::find(const std::string &id) const { return hashtable.find(id); }

bool Scheduler::add_sensor_task(const std::string &id, int basePriority, int value, int threshold)
{
    if (!ID_Check()(id, *this))
        return false;
    if (!Sensor_Check()(basePriority, value, threshold))
        return false;
    std::unique_ptr<Task> task(new SensorTask(id, basePriority, next_sequence, value, threshold));
    if (!add_task(std::move(task)))
        return false;
    ++next_sequence;
    return true;
}

bool Scheduler::add_control_task(const std::string &id, int basePriority, int urgency)
{
    if (!ID_Check()(id, *this))
        return false;
    if (!Control_Check()(basePriority, urgency))
        return false;
    std::unique_ptr<Task> task(new ControlTask(id, basePriority, next_sequence, urgency));
    if (!add_task(std::move(task)))
        return false;
    ++next_sequence;
    return true;
}

bool Scheduler::cancel(const std::string &id)
{
    auto *temp_task = hashtable.find(id);
    if (temp_task == nullptr || temp_task->status() != STATUS::Pending)
        return false;
    pending_cancel(temp_task);
    return true;
}

static void discard_invalid_heap_top(BinaryHeap &heap)
{
    while (!heap.empty() && heap.findMax()->status() != Task::STATUS::Pending)
        heap.deleteMax();
}

Task *Scheduler::peek()
{
    discard_invalid_heap_top(heap);
    if (heap.empty())
        return nullptr;
    return heap.findMax();
}

Task *Scheduler::run()
{
    discard_invalid_heap_top(heap);
    if (heap.empty())
        return nullptr;
    Task *ready_to_run = heap.findMax();
    if (ready_to_run == nullptr)
        return nullptr;
    stack.push(ready_to_run->id());
    heap.deleteMax();
    id_avl.remove(ready_to_run->id());
    ready_to_run->_status_ = STATUS::Done;
    --pending_tasks_count;
    ++done_tasks_count;
    return ready_to_run;
}

Task *Scheduler::undo()
{
    if (stack.empty())
        return nullptr;
    const std::string &id = stack.top();
    Task *task = hashtable.find(id);
    if (task == nullptr || task->status() != STATUS::Done)
        return nullptr;
    restore_task(task);
    --done_tasks_count;
    ++pending_tasks_count;
    stack.pop();
    return task;
}

std::vector<const Task *> Scheduler::list(std::size_t k) const
{
    std::size_t times = k > heap.size() ? heap.size() : k;
    std::vector<const Task *> result;
    BinaryHeap temp(heap);
    discard_invalid_heap_top(temp);
    while (!temp.empty() && result.size() < times)
    {
        Task *task = temp.deleteMax();
        if (task->status() != STATUS::Pending)
            continue;
        result.push_back(task);
    }
    return result;
}

std::vector<const Task *> Scheduler::ids(const std::string &left, const std::string &right) const
{
    if (left > right)
        return {};
    return id_avl.ranges(left, right);
}

std::vector<const Task *> Scheduler::history(std::size_t k) const
{
    auto recent_command = std::move(stack.recent(k));
    std::vector<const Task *> result;
    for (const auto &id: recent_command)
        result.push_back(hashtable.find(id));
    return result;
}

std::array<std::pair<Scheduler::STATUS, std::size_t>, 3> Scheduler::stat() const
{
    return {
        {
            {STATUS::Pending, pending_tasks_count}, {STATUS::Done, done_tasks_count},
            {STATUS::Canceled, canceled_tasks_count}
        }
    };
}

bool Scheduler::add_task(std::unique_ptr<Task> &&task)
{
    if (!task)
        return false;
    Task *task_ptr = task.get();
    tasks.push_back(std::move(task));
    register_new_task(task_ptr);
    ++pending_tasks_count;
    return true;
}

void Scheduler::register_new_task(Task *task)
{
    id_avl.insert(task);
    heap.insert(task);
    hashtable.insert(task);
}

void Scheduler::pending_cancel(Task *task)
{
    id_avl.remove(task->id());
    task->_status_ = STATUS::Canceled;
    --pending_tasks_count;
    ++canceled_tasks_count;
}

void Scheduler::restore_task(Task *task)
{
    task->_status_ = STATUS::Pending;
    id_avl.insert(task);
    heap.insert(task);
}

bool Scheduler::ID_Check::operator()(const std::string &id, const Scheduler &scheduler) const
{
    if (id.empty() || id.size() > 32)
        return false;
    if (scheduler.hashtable.contains(id))
        return false;
    for (unsigned char c: id)
        if (!std::isdigit(c) && !std::isalpha(c) && c != '_')
            return false;
    return true;
}

bool Scheduler::Range_Check::operator()(int value, int lower, int upper) const
{
    return value >= lower && value <= upper;
}

bool Scheduler::Sensor_Check::operator()(int value1, int value2, int value3) const
{
    return Range_Check::operator()(value1) && Range_Check::operator()(value2) && Range_Check::operator()(value3);
}

bool Scheduler::Control_Check::operator()(int value1, int value2) const
{
    return Range_Check::operator()(value1) && Range_Check::operator()(value2, 0, 10);
}

bool Scheduler::serialize(std::ostream &output) const
{
    output << "SCHEDULER" << ' ' << tasks.size() << ' ' << next_sequence << '\n';
    for (const auto &task: tasks)
    {
        std::string status;
        task->status(status);
        switch (task->type())
        {
            case Task::TYPE::Sensor_Task: {
                const SensorTask *sensor_task = dynamic_cast<SensorTask *>(task.get());
                output << "S " << task->id() << ' '
                        << sensor_task->basePriority() << ' '
                        << sensor_task->value() << ' '
                        << sensor_task->threshold() << ' '
                        << sensor_task->sequence() << ' '
                        << status << '\n';
                break;
            }
            case Task::TYPE::Control_Task: {
                const ControlTask *control_task = dynamic_cast<ControlTask *>(task.get());
                output << "C " << control_task->id() << ' '
                        << control_task->basePriority() << ' '
                        << control_task->urgency() << ' '
                        << control_task->sequence() << ' '
                        << status << '\n';
                break;
            }
            default:
                return false;
        }
    }
    output << "HISTORY " << stack.size() << '\n';
    for (const auto &name: stack)
        output << name << '\n';
    return static_cast<bool>(output);
}

static Task::STATUS change_status(const std::string &status)
{
    if (status == "PENDING")
        return Task::STATUS::Pending;
    if (status == "CANCELED")
        return Task::STATUS::Canceled;
    if (status == "DONE")
        return Task::STATUS::Done;
    return Task::STATUS::Unknown;
}

bool Scheduler::deserialize(std::istream &input)
{
    if (!input)
        return false;
    Scheduler temp;
    std::string line;
    std::string extra;
    if (!std::getline(input, line))
        return false;
    std::stringstream header(line);
    std::string command;
    std::string count_text;
    std::string next_text;
    if (!(header >> command >> count_text >> next_text))
        return false;
    if (command != "SCHEDULER" || header >> extra)
        return false;
    std::size_t tasks_count = 0;
    std::size_t loaded_next_sequence = 0;
    if (!parse_integer(count_text, tasks_count) || !parse_integer(next_text, loaded_next_sequence))
        return false;
    std::unordered_set<std::size_t> used_sequences;
    for (std::size_t i = 0; i < tasks_count; ++i)
    {
        if (!std::getline(input, line))
            return false;
        std::istringstream ss(line);
        std::string type;
        std::string id;
        std::string sequence_text;
        std::string status_text;
        int base_priority = 0;
        std::size_t sequence = 0;
        if (!(ss >> type))
            return false;
        std::unique_ptr<Task> task;
        if (type == "S")
        {
            int value = 0;
            int threshold = 0;
            if (!(ss >> id >> base_priority
                  >> value >> threshold
                  >> sequence_text >> status_text))
                return false;
            if (ss >> extra)
                return false;
            if (!parse_integer(sequence_text, sequence))
                return false;
            if (!ID_Check()(id, temp) ||
                !Sensor_Check()(
                    base_priority, value, threshold))
                return false;
            task.reset(new SensorTask(
                id, base_priority, sequence,
                value, threshold));
        }
        else if (type == "C")
        {
            int urgency = 0;
            if (!(ss >> id >> base_priority
                  >> urgency >> sequence_text
                  >> status_text))
                return false;
            if (ss >> extra)
                return false;
            if (!parse_integer(sequence_text, sequence))
                return false;
            if (!ID_Check()(id, temp) ||
                !Control_Check()(base_priority, urgency))
                return false;
            task.reset(new ControlTask(
                id, base_priority, sequence, urgency));
        }
        else { return false; }
        if (sequence >= loaded_next_sequence)
            return false;
        if (!used_sequences.insert(sequence).second)
            return false;
        STATUS loaded_status = change_status(status_text);
        if (loaded_status == STATUS::Unknown)
            return false;
        task->_status_ = loaded_status;
        if (!temp.register_loaded_task(task))
            return false;
    }
    if (!std::getline(input, line))
        return false;
    std::istringstream history_header(line);
    std::string history_count_text;
    if (!(history_header >> command >> history_count_text))
        return false;
    if (command != "HISTORY" || history_header >> extra)
        return false;
    std::size_t history_count = 0;
    if (!parse_integer(history_count_text, history_count))
        return false;
    if (history_count > temp.done_tasks_count)
        return false;
    std::unordered_set<std::string> used_history_ids;
    for (std::size_t i = 0; i < history_count; ++i)
    {
        if (!std::getline(input, line))
            return false;
        std::istringstream ss(line);
        std::string id;
        if (!(ss >> id) || ss >> extra)
            return false;
        const Task *task = temp.find(id);
        if (task == nullptr)
            return false;
        if (task->status() != STATUS::Done)
            return false;
        if (!used_history_ids.insert(id).second)
            return false;
        temp.stack.push(id);
    }
    temp.next_sequence = loaded_next_sequence;
    swap(*this, temp);
    return true;
}

bool Scheduler::register_loaded_task(std::unique_ptr<Task> &task)
{
    if (!task)
        return false;
    Task *task_ptr = task.get();
    tasks.push_back(std::move(task));
    if (!hashtable.insert(task_ptr))
        return false;
    switch (task_ptr->status())
    {
        case STATUS::Pending:
            id_avl.insert(task_ptr);
            heap.insert(task_ptr);
            ++pending_tasks_count;
            break;
        case STATUS::Done:
            ++done_tasks_count;
            break;
        case STATUS::Canceled:
            ++canceled_tasks_count;
            break;
        default:
            return false;
    }
    return true;
}

bool Scheduler::parse_integer(const std::string &str, std::size_t &result)
{
    if (str.empty())
        return false;
    for (unsigned char c: str)
        if (!std::isdigit(c))
            return false;
    std::size_t pos = 0;
    try
    {
        unsigned long long value = std::stoull(str, &pos);
        if (pos != str.size())
            return false;
        if (value > std::numeric_limits<std::size_t>::max())
            return false;
        result = static_cast<std::size_t>(value);
        return true;
    }
    catch (const std::invalid_argument &e) { return false; }
    catch (const std::out_of_range &e) { return false; }
}

//
// Created by shdff on 2026/11/19.
//
