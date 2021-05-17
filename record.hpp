#pragma once

#include <stdint.h>
#include <list>
#include <string>

struct Record;

/**
 * A CIDR record in a hierarchical tree of CIDR records
 */
struct Record {
  enum CompareResult { INCOMPARABLE, EQUAL, STRICT_LT, STRICT_GT, CONTAINS, WITHIN };

  union {
    uint8_t bytes[4];
    uint32_t addr;
  };
  int prefix;
  int count;
  
  Record* parent;

  // INVARIANT: A non-root inner node always has two children
  // Note: this is slightly inaccurate if we allow the user to add CIDR blocks -- a leaf block could later become an inner block.
  Record* left;
  // INVARIANT: A non-root inner node always has two children
  // Note: this is slightly inaccurate if we allow the user to add CIDR blocks -- a leaf block could later become an inner block.
  Record* right;

  Record() : addr(0), prefix(0), count(0), parent(nullptr), left(nullptr), right(nullptr) {}
  Record(const Record& other) = delete;
  Record(Record&& other);
  Record(uint32_t _addr, int _prefix)
      : prefix(_prefix), addr(_addr), count(0), parent(nullptr), left(nullptr), right(nullptr) {}
  ~Record();

  // move assignment
  Record& operator= (Record&& other);

  // Returns the netmask corresponding to this prefix
  static uint32_t mask(int prefix);

  // Compare two records
  static uint32_t compare(const Record& r1, const Record& r2);

  // Perform a 3-way merge of ordered records using the least combined prefix rule
  void merge_ordered (Record& r1, Record& r2, Record& r3);

  // Merge two records to create an "inner node" record
  static Record combine(const Record& r1, const Record& r2);

  // Parse a record from a line
  static Record* from_line(const std::string& line);

  // returns success
  bool add(Record* record);

  // pretty-prints the tree
  void print(int level=0);

};