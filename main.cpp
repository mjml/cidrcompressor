
#include <iostream>
#include <string>
#include <regex>
#include "record.hpp"


std::list<Record*> RecordList;
Record root;

Record parse_record (const std::string& line);

int main (int argc, char* argv[])  
{
    int linenum;
    std::string line;
    uint8_t     prefix;

    // Read CIDR records and counts from stdin
    while (!std::cin.eof()) {   
        linenum++;
        std::getline(std::cin, line, '\n');
        std::cout << "line: " << line << std::endl;
        Record* record = Record::from_line(line);
        if (record) {
          if (!root.add(record)) {
            delete record;
            record = nullptr;
          } 
        } else {
            if (!std::cin.eof()) {
                std::cerr << "Malformed record @" << linenum << ": " << line << std::endl;
            }
        }
    }

    // print the tree for fun
    root.print();
    
    return 0;
}