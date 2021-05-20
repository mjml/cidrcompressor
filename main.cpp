
#include <iostream>
#include <string>
#include <regex>
#include <boost/program_options.hpp>
#include "record.hpp"

std::list<Record*> RecordList;
Record root;

namespace po = boost::program_options;

Record parse_record (const std::string& line);
void node_to_ios(std::ostream& ios, Record& rec);
void edge_to_ios(std::ostream&ios, Record& rec, Record* child);

int std_thresh[] = {1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 5, 6, 7, 10, 12, 14, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0};

int main (int argc, char* argv[])  
{
    int linenum;
    std::string line;

    po::options_description desc("\nCIDR Compressor v0.1\n  http://github.com/mjml/cidrcompressor\n  written by Michael Joya (mike@michaeljoya.com)\n\nInput taken from stdin, line format:\n  a.b.c.d(/prefix) (count)\n\nAllowed options");
    desc.add_options()
    ("help,h",        "Print help message")
    ("show-tree,t",   "Print CIDR block tree")
    ("block-only,b",  "Print only blocks with prefix smaller than 32\nThis will omit individual addresses.")
    ("graph,g",       "Instead of a text tree, print a graphviz DOT tree")
    ("iptables,i",    "Instead of a tree, write out iptables DROP options");

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
    } else if (vm.count("graph")) {
      std::cout << "graph {" << std::endl;
      root.visit_topdown_dfs([&](Record& rec) -> bool {
          auto wildcard = 32 - rec.prefix;
          auto t = std_thresh[wildcard];

          node_to_ios(std::cout, rec);
          return (t==0 || rec.count < t);
      });
      root.visit_topdown_dfs([](Record& rec) -> bool {
          auto wildcard = 32 - rec.prefix;
          auto t = std_thresh[wildcard];
          if (!t || rec.count < t) {
            edge_to_ios(std::cout, rec, rec.left);
            edge_to_ios(std::cout, rec, rec.right);
            return true;
          } else return false;
      });

      std::cout << "}" << std::endl;
    } else {
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

void node_to_ios(std::ostream& os, Record& rec)
{
    os << "node" << (int)rec.bytes[3] << "_" << (int)rec.bytes[2] << "_" << (int)rec.bytes[1] << "_" << (int)rec.bytes[0] << "_" << rec.prefix;
    os << " [ label=\"" << (int)rec.bytes[3] << "." << (int)rec.bytes[2] << "." << (int)rec.bytes[1] << "." << (int)rec.bytes[0] << "/" << rec.prefix << "\" ";
    auto wildcard = 32 - rec.prefix;
    auto t = std_thresh[wildcard];
    if (t) {
        if (wildcard==0) {
            os << "style=\"filled\" fillcolor=\"#f0f0f0\"";
        } else if (rec.count >= t) {
            os << "style=\"filled\" fillcolor=\"#e0c0f0\"";
        }
    }
    os << "];" << std::endl;
}

void edge_to_ios(std::ostream&os, Record& rec, Record* child)
{
    if (!child) return;
    os << "node" << (int)rec.bytes[3] << "_" << (int)rec.bytes[2] << "_" << (int)rec.bytes[1] << "_" << (int)rec.bytes[0] << "_" << rec.prefix;
    os << " -- ";
    os << "node" << (int)child->bytes[3] << "_" << (int)child->bytes[2] << "_" << (int)child->bytes[1] << "_" << (int)child->bytes[0] << "_" << child->prefix;
    os << std::endl;
}