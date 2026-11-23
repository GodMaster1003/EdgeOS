//
// Created by godmaster on 2026/11/21.
//

#ifndef SELF_TEST_FILESTORAGER_HPP
#define SELF_TEST_FILESTORAGER_HPP
#include<memory>
#include<unordered_map>
#include"Scheduler.hpp"

class File_Storager {
public:
    explicit File_Storager(Scheduler &scheduler) : scheduler(scheduler) {}

    bool save(const std::string &filename) const;

    bool load(const std::string &filename);

    const std::string &snapshot(const std::string &base_name);

    bool restore(const std::string &name);

private:
    //TODO 改为泛型HashTable
    std::unordered_map<std::string, std::unique_ptr<Scheduler> > snapshots;
    Scheduler &scheduler;
};
#endif //SELF_TEST_FILESTORAGER_HPP
