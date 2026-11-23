#include "../include/Scheduler.hpp"
#include "../include/CommandProcessor.hpp"
#include "../include/FileStorager.hpp"
#include<iostream>
#include<string>

int main()
{
    Scheduler scheduler;
    File_Storager file_storager(scheduler);
    Word_Processor processor(scheduler, file_storager);
    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line == "QUIT")
            break;
        processor.execute(line, std::cout);
    }
    return 0;
}

//
// Created by godmaster on 2026/11/21.
//
