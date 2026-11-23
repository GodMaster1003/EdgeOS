#include "../include/FileStorager.hpp"
#include<fstream>
#include<sstream>
#include<cctype>

const std::string &File_Storager::snapshot(const std::string &base_name)
{
    std::string actual_name = base_name;
    std::size_t offset = 1;
    while (snapshots.find(actual_name) != snapshots.end())
    {
        actual_name = base_name + '_' + std::to_string(offset);
        ++offset;
    }
    std::unique_ptr<Scheduler> current_snapshot(new Scheduler(scheduler));
    snapshots.emplace(actual_name, std::move(current_snapshot));
    return snapshots.find(actual_name)->first;
}

bool File_Storager::restore(const std::string &name)
{
    auto it = snapshots.find(name);
    if (it == snapshots.end())
        return false;
    scheduler = *(it->second);
    return true;
}

bool File_Storager::save(const std::string &filename) const
{
    std::ofstream file(filename.c_str());
    if (!file)
        return false;
    file << "EDGEOS 1" << '\n';
    file << "ACTIVE" << '\n';
    if (!scheduler.serialize(file))
        return false;
    file << "SNAPSHOTS " << snapshots.size() << '\n';
    for (const auto &snapshot: snapshots)
    {
        file << "SNAPSHOT " << snapshot.first << '\n';
        if (!snapshot.second->serialize(file))
            return false;
    }
    file << "END\n";
    file.flush();
    return static_cast<bool>(file);
}

bool File_Storager::load(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    if (!file)
        return false;
    std::string line;
    if (!std::getline(file, line) || line != "EDGEOS 1")
        return false;
    if (!std::getline(file, line) || line != "ACTIVE")
        return false;

    Scheduler temporary_scheduler;
    if (!temporary_scheduler.deserialize(file))
        return false;
    if (!std::getline(file, line))
        return false;
    std::istringstream ss(line);
    std::string command;
    std::string count_text;
    std::string extra;
    if (!(ss >> command >> count_text))
        return false;
    if (command != "SNAPSHOTS" || ss >> extra)
        return false;
    if (count_text.empty())
        return false;
    for (unsigned char c: count_text)
    {
        if (!isdigit(c))
            return false;
    }
    std::size_t snapshot_count = 0;
    std::istringstream count_stream(count_text);
    if (!(count_stream >> snapshot_count))
        return false;
    std::unordered_map<std::string, std::unique_ptr<Scheduler> > temporary_snapshots;
    for (std::size_t i = 0; i < snapshot_count; ++i)
    {
        if (!std::getline(file, line))
            return false;
        std::istringstream snapshot_header(line);
        std::string name;
        if (!(snapshot_header >> command >> name))
            return false;
        if (command != "SNAPSHOT" ||
            snapshot_header >> extra)
            return false;
        if (temporary_snapshots.find(name) !=
            temporary_snapshots.end())
            return false;
        std::unique_ptr<Scheduler> snapshot(
            new Scheduler);
        if (!snapshot->deserialize(file))
            return false;
        temporary_snapshots.emplace(
            name, std::move(snapshot)
        );
    }
    if (!std::getline(file, line) || line != "END")
        return false;
    if (std::getline(file, line))
        return false;
    if (!file.eof())
        return false;
    scheduler = std::move(temporary_scheduler);
    snapshots.swap(temporary_snapshots);
    return true;
}

//
// Created by godmaster on 2026/11/21.
//
