//
// Created by godmaster on 2026/11/21.
//

#ifndef SELF_TEST_WORDPROCESSOR_HPP
#define SELF_TEST_WORDPROCESSOR_HPP
#include "Scheduler.hpp"
#include "FileStorager.hpp"
#include<iostream>
#include<string>
#include<cstddef>

class Word_Processor {
public:
    using Tokenized_Vector = std::vector<std::string>;

    explicit Word_Processor(Scheduler &scheduler, File_Storager &file_storager) : scheduler(scheduler),
        file_storager(file_storager) {}

    void execute(const std::string &line, std::ostream &output);

private:
    enum class STATUS {
        Add_Sensor,
        Add_Control,
        Find,
        Cancel,
        Stats,
        Peek,
        Run,
        List,
        Ids,
        Undo,
        History,
        Save,
        Load,
        Snapshot,
        Restore,
        Invalid,
        Exit,
    };

    enum class Result {
        Empty,
        Error,
        Not_Found,
        Invalid,
    };

    Scheduler &scheduler;
    File_Storager &file_storager;

    Tokenized_Vector tokenizer(std::istringstream &input);

    void add_sensor(const Tokenized_Vector &args, std::ostream &output);

    void add_control(const Tokenized_Vector &args, std::ostream &output);

    void find(const Tokenized_Vector &args, std::ostream &output);

    void cancel(const Tokenized_Vector &args, std::ostream &output);

    void stats(std::ostream &output);

    void peek(std::ostream &output);

    void run(std::ostream &output);

    void list(std::size_t k, std::ostream &output);

    void ids(const Tokenized_Vector &args, std::ostream &output);

    void undo(std::ostream &output);

    void history(std::size_t k, std::ostream &output);

    void save(const Tokenized_Vector &args, std::ostream &output);

    void load(const Tokenized_Vector &args, std::ostream &output);

    void snapshot(const Tokenized_Vector &args, std::ostream &output);

    void restore(const Tokenized_Vector &args, std::ostream &output);

    void print_error(std::ostream &output, Result result);

    STATUS parse_command(std::istringstream &input);

    bool check_argument_count(const Tokenized_Vector &vec, STATUS status);

    bool parse_integer(const std::string &str, int &result);

    bool parse_integer(const std::string &str, std::size_t &result);

    void process_k_command(const std::string &str, STATUS status, std::ostream &output);
};
#endif //SELF_TEST_WORDPROCESSOR_HPP
