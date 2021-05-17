
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
        if (std::cin.eof()) break;
        //std::cout << "line: " << line << std::endl;
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

    int std_thresh[] = {1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 5, 6, 7, 10, 12, 14,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0};

    std::cout << "CIDR bans:" << std::endl;
    
    root.visit_topdown_dfs([&] (Record& rec) -> bool { 
                            auto wildcard = 32-rec.prefix; 
                            auto t = std_thresh[wildcard];
                            if (t) {
                                if (rec.count >= t) {
                                    std::cout << (int)rec.bytes[3] << "." << (int)rec.bytes[2] << "." << (int)rec.bytes[1] << "." << (int)rec.bytes[0] << "/" << rec.prefix << std::endl;
                                    return false;
                                } else {
                                    return true;
                                }
                            } else {
                                return true;
                            }
                            
                           });


    return 0;
}