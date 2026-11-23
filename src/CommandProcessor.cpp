#include "../include/CommandProcessor.hpp"
#include<string>
#include<sstream>
#include<iostream>
#include<cctype>
#include<limits>

void Word_Processor::execute(const std::string &line, std::ostream &output)
{
    std::istringstream ss(line);
    std::string command;
    auto status = parse_command(ss);
    auto args = tokenizer(ss);
    if (!check_argument_count(args, status))
    {
        print_error(output, Result::Error);
        return;
    }
    switch (status)
    {
        case STATUS::Add_Sensor:
            add_sensor(args, output);
            break;
        case STATUS::Add_Control:
            add_control(args, output);
            break;
        case STATUS::Find:
            find(args, output);
            break;
        case STATUS::Cancel:
            cancel(args, output);
            break;
        case STATUS::Stats:
            stats(output);
            break;
        case STATUS::Peek:
            peek(output);
            break;
        case STATUS::Run:
            run(output);
            break;
        case STATUS::Ids:
            ids(args, output);
            break;
        case STATUS::Undo:
            undo(output);
            break;
        case STATUS::List:
        case STATUS::History:
            process_k_command(args[0], status, output);
            break;
        case STATUS::Save:
            save(args, output);
            break;
        case STATUS::Load:
            load(args, output);
            break;
        case STATUS::Snapshot:
            snapshot(args, output);
            break;
        case STATUS::Restore:
            restore(args, output);
            break;
        default:
            print_error(output, Result::Invalid);
            break;
    }
}

Word_Processor::Tokenized_Vector Word_Processor::tokenizer(std::istringstream &ss)
{
    std::string word;
    Tokenized_Vector tokenizer;
    while (ss >> word)
        tokenizer.push_back(word);
    return tokenizer;
}

void Word_Processor::add_sensor(const Tokenized_Vector &vec, std::ostream &output)
{
    int basePriority, value, threshold;
    if (!parse_integer(vec[1], basePriority) || !parse_integer(vec[2], value) || !parse_integer(vec[3], threshold))
    {
        print_error(output, Result::Error);
        return;
    }
    if (!scheduler.add_sensor_task(vec[0], basePriority, value, threshold))
        print_error(output, Result::Error);
}

void Word_Processor::add_control(const Tokenized_Vector &vec, std::ostream &output)
{
    int basePriority, urgency;
    if (!parse_integer(vec[1], basePriority) || !parse_integer(vec[2], urgency))
    {
        print_error(output, Result::Error);
        return;
    }
    if (!scheduler.add_control_task(vec[0], basePriority, urgency))
        print_error(output, Result::Error);
}

void Word_Processor::find(const Tokenized_Vector &vec, std::ostream &output)
{
    const Task *desired_task = scheduler.find(vec[0]);
    std::string status;
    if (desired_task == nullptr)
    {
        print_error(output, Result::Not_Found);
        return;
    }
    desired_task->status(status);
    output << "FIND " << desired_task->id() << ' ' << status << ' ' << desired_task->finalPriority() << '\n';
}

void Word_Processor::cancel(const Tokenized_Vector &vec, std::ostream &output)
{
    if (!scheduler.cancel(vec[0]))
    {
        print_error(output, Result::Error);
        return;
    }
    output << "CANCEL " << vec[0] << '\n';
}

void Word_Processor::stats(std::ostream &output)
{
    auto result = scheduler.stat();
    output << "STATS";
    for (const auto &count: result)
        output << ' ' << count.second;
    output << '\n';
}

void Word_Processor::peek(std::ostream &output)
{
    Task *peek = scheduler.peek();
    if (peek == nullptr)
    {
        print_error(output, Result::Empty);
        return;
    }
    output << "PEEK " << peek->id() << ' ' << peek->finalPriority() << '\n';
}

void Word_Processor::run(std::ostream &output)
{
    Task *run = scheduler.run();
    if (run == nullptr)
    {
        print_error(output, Result::Empty);
        return;
    }
    output << "RUN " << run->id() << ' ' << run->finalPriority() << '\n';
}

void Word_Processor::process_k_command(const std::string &str, STATUS status, std::ostream &output)
{
    std::size_t k;
    if (!parse_integer(str, k))
    {
        print_error(output, Result::Error);
        return;
    }
    switch (status)
    {
        case STATUS::List:
            list(k, output);
            break;
        case STATUS::History:
            history(k, output);
            break;
        default:
            print_error(output, Result::Invalid);
    }
}

void Word_Processor::list(std::size_t k, std::ostream &output)
{
    auto tasks = scheduler.list(k);
    if (tasks.empty())
    {
        print_error(output, Result::Empty);
        return;
    }
    output << "LIST";
    for (const auto &task: tasks)
        output << ' ' << task->id() << ':' << task->finalPriority();
    output << '\n';
}

void Word_Processor::ids(const Tokenized_Vector &args, std::ostream &output)
{
    auto tasks = scheduler.ids(args[0], args[1]);
    if (tasks.empty())
    {
        print_error(output, Result::Empty);
        return;
    }
    output << "IDS";
    for (const auto &task: tasks)
        output << ' ' << task->id() << ':' << task->finalPriority();
    output << '\n';
}

void Word_Processor::undo(std::ostream &output)
{
    Task *undo = scheduler.undo();
    if (undo == nullptr)
    {
        print_error(output, Result::Empty);
        return;
    }
    output << "UNDO " << undo->id() << '\n';
}

void Word_Processor::history(std::size_t k, std::ostream &output)
{
    auto tasks = scheduler.history(k);
    if (tasks.empty())
    {
        print_error(output, Result::Empty);
        return;
    }
    output << "HISTORY";
    for (const auto &task: tasks)
        output << ' ' << task->id() << ':' << task->finalPriority();
    output << '\n';
}

void Word_Processor::snapshot(const Tokenized_Vector &args, std::ostream &output)
{
    auto actual_name = file_storager.snapshot(args[0]);
    if (actual_name.empty())
    {
        print_error(output, Result::Error);
        return;
    }
    output << "SNAPSHOT OK" << ' ' << actual_name << '\n';
}

void Word_Processor::restore(const Tokenized_Vector &args, std::ostream &output)
{
    if (!file_storager.restore(args[0]))
    {
        print_error(output, Result::Error);
        return;
    }
    output << "RESTORE OK\n";
}

void Word_Processor::save(const Tokenized_Vector &args, std::ostream &output)
{
    if (!file_storager.save(args[0]))
    {
        print_error(output, Result::Error);
        return;
    }
    output << "SAVE OK " << args[0] << '\n';
}

void Word_Processor::load(const Tokenized_Vector &args, std::ostream &output)
{
    if (!file_storager.load(args[0]))
    {
        print_error(output, Result::Error);
        return;
    }
    output << "LOAD OK " << args[0] << '\n';
}

Word_Processor::STATUS Word_Processor::parse_command(std::istringstream &input)
{
    std::string status;
    if (!(input >> status))
        return STATUS::Invalid;
    if (status == "ADD_S")
        return STATUS::Add_Sensor;
    if (status == "ADD_C")
        return STATUS::Add_Control;
    if (status == "FIND")
        return STATUS::Find;
    if (status == "CANCEL")
        return STATUS::Cancel;
    if (status == "STATS")
        return STATUS::Stats;
    if (status == "PEEK")
        return STATUS::Peek;
    if (status == "RUN")
        return STATUS::Run;
    if (status == "LIST")
        return STATUS::List;
    if (status == "IDS")
        return STATUS::Ids;
    if (status == "UNDO")
        return STATUS::Undo;
    if (status == "HISTORY")
        return STATUS::History;
    if (status == "SAVE")
        return STATUS::Save;
    if (status == "LOAD")
        return STATUS::Load;
    if (status == "SNAPSHOT")
        return STATUS::Snapshot;
    if (status == "RESTORE")
        return STATUS::Restore;
    return STATUS::Invalid;
}

bool Word_Processor::check_argument_count(const Tokenized_Vector &vec, STATUS status)
{
    auto size = vec.size();
    switch (status)
    {
        case STATUS::Add_Sensor:
            return size == 4;
        case STATUS::Add_Control:
            return size == 3;
        case STATUS::Ids:
            return size == 2;
        case STATUS::Find:
        case STATUS::Cancel:
        case STATUS::List:
        case STATUS::History:
        case STATUS::Snapshot:
        case STATUS::Restore:
        case STATUS::Save:
        case STATUS::Load:
            return size == 1;
        case STATUS::Stats:
        case STATUS::Peek:
        case STATUS::Run:
        case STATUS::Undo:
            return size == 0;
        case STATUS::Invalid:
            return false;
    }
    return false;
}

void Word_Processor::print_error(std::ostream &output, Result status)
{
    switch (status)
    {
        case Result::Error:
            output << "ERROR\n";
            break;
        case Result::Empty:
            output << "EMPTY\n";
            break;
        case Result::Not_Found:
            output << "NOT_FOUND\n";
            break;
        default:
            output << "INVALID\n";
            break;
    }
}

bool Word_Processor::parse_integer(const std::string &str, int &result)
{
    if (str.empty())
        return false;
    for (unsigned char c: str)
        if (!std::isdigit(c))
            return false;
    std::size_t pos = 0;
    try
    {
        int value = std::stoi(str, &pos);
        if (pos != str.size())
            return false;
        result = value;
        return true;
    }
    catch (const std::invalid_argument &) { return false; }
    catch (const std::out_of_range &e) { return false; }
}

bool Word_Processor::parse_integer(const std::string &str, std::size_t &result)
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
// Created by godmaster on 2026/11/21.
//
