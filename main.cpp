
#include <iostream>
#include <string>
#include <regex>
#include <boost/program_options.hpp>
#include "record.hpp"

std::list<Record*> RecordList;
Record root;

namespace po = boost::program_options;

Record parse_record (const std::string& line);

int main (int argc, char* argv[])  
{
    int linenum;
    std::string line;

    po::options_description desc("Allowed options");
    desc.add_options()
    ("help",          "Produce help message")
    ("show-tree,t",   "Print CIDR block tree")
    ("block-only,b",  "Print only blocks with prefix smaller than 32\nThis will omit individual addresses.");

    po::variables_map vm;
    po::store(po::parse_command_line(argc,argv,desc),vm);

    if (vm.count("help")) {
        desc.print(std::cout);
        std::cout << std::endl;
        return 0;
    }

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
    if (vm.count("show-tree")) {
        root.print();
    } else {
      int std_thresh[] = {1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 5, 6, 7, 10, 12, 14, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0};

      root.visit_topdown_dfs([&](Record& rec) -> bool {
        auto wildcard = 32 - rec.prefix;
        auto t = std_thresh[wildcard];
        if (t) {
          if (rec.count >= t && (!vm.count("block-only") || (rec.prefix<32))) {
            std::cout << (int)rec.bytes[3] << "." << (int)rec.bytes[2] << "."
                      << (int)rec.bytes[1] << "." << (int)rec.bytes[0] << "/"
                      << rec.prefix << " " << rec.count << std::endl;
            return false;
          } else {
            return true;
          }
        } else {
          return true;
        }
      });
    }

    return 0;
}