
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
    std::regex  cidrpat4("(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})(/(\\d{1,2}))?");

    // Read CIDR records and counts from stdin
    while (!std::cin.eof()) {   
        
        std::getline(std::cin, line, '\n');
        
        Record* record = Record::from_line(line);
        if (!root.add(record)) {
            delete record;
            record = nullptr;
        } else {
            std::cerr << "Warning, couldn't parse line " << linenum << std::endl << std::flush;
        }
        
    }


    
    return 0;
}