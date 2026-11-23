#include "../include/Task.hpp"
#include<algorithm>
const std::string &Task::id() const noexcept { return _id_; }

int Task::basePriority() const noexcept { return _basePriority_; }

std::size_t Task::sequence() const noexcept { return _sequence_; }

Task::STATUS Task::status() const noexcept { return _status_; }

Task::TYPE Task::type() const noexcept { return _task_type_; }

void Task::status(std::string &status) const noexcept
{
    switch (_status_)
    {
        case STATUS::Pending:
            status = "PENDING";
            break;
        case STATUS::Done:
            status = "DONE";
            break;
        case STATUS::Canceled:
            status = "CANCELED";
            break;
    }
}


int SensorTask::value() const noexcept { return _value_; }
int SensorTask::threshold() const noexcept { return _threshold_; }
int SensorTask::finalPriority() const { return basePriority() + std::max(0, value() - threshold()); }

void SensorTask::print(std::ostream &output) const
{
    std::string basePriority = std::to_string(this->basePriority());
    std::string value = std::to_string(this->value());
    std::string threshold = std::to_string(this->threshold());
    std::string finalPriority = std::to_string(this->finalPriority());
    std::string status;
    Task::status(status);
    output << this->id() << ':' << '\n'
            << "Status: " << status << '\n'
            << "Base Priority: " << basePriority << '\n'
            << "Value: " << value << '\n'
            << "Threshold: " << threshold << '\n'
            << "Final Priority: " << finalPriority << '\n';
}

std::unique_ptr<Task> SensorTask::clone() const { return std::unique_ptr<Task>(new SensorTask(*this)); }
int ControlTask::urgency() const noexcept { return _urgency_; }
int ControlTask::finalPriority() const { return basePriority() + 2 * urgency(); }

void ControlTask::print (std::ostream &output) const
{
    std::string basePriority = std::to_string(this->basePriority());
    std::string urgency = std::to_string(this->urgency());
    std::string finalPriority = std::to_string(this->finalPriority());
    std::string status;
    Task::status(status);
    output << this->id() << ':' << '\n'
            << "Status: " << status << '\n'
            << "Base Priority: " << basePriority << '\n'
            << "Urgency: " << urgency << '\n'
            << "Final Priority: " << finalPriority << '\n';
}

std::unique_ptr<Task> ControlTask::clone() const { return std::unique_ptr<Task>(new ControlTask(*this)); }

std::ostream& operator<<(std::ostream& output,const Task& task)
{
    task.print(output);
    return output;
}
//
// Created by shdff on 2026/11/18.
//
