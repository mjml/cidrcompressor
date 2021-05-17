
#include "record.hpp"

#include <stdint.h>
#include <list>
#include <algorithm>
#include <string>
#include <stdexcept>
using namespace std;

Record::~Record () {
    if (left) delete left;
    if (right) delete right;
}

Record::Record(Record&& other) 
:addr(other.addr),
prefix(other.prefix),
count(other.count)
{
    std::swap(left, other.left);
    std::swap(right, other.right);
}

uint32_t Record::compare (const Record& r1, const Record& r2)
{
    if ((r1.addr & r2.addr) && (r1.prefix == r2.prefix)) {
        return Record::EQUAL;
    }
    if ((r1.addr & r2.addr) == r1.addr && r1.prefix < r2.prefix) {
        return Record::CONTAINS;
    } 
    if ((r1.addr & r2.addr) == r2.addr && r2.prefix < r1.prefix ) {
        return Record::WITHIN;
    } 
    if (r1.addr < r2.addr) {
        return Record::STRICT_LT;
    }
    if (r1.addr > r2.addr) {
        return Record::STRICT_GT;
    }
    return Record::INCOMPARABLE;
}

uint32_t Record::mask (int prefix) {
    // Shift left it will pad on the right with zeros
    return 0xffffffff << prefix;
}


Record* Record::from_line (const std::string& line)
{
    return NULL;
}

Record& Record::operator= (Record&& other)
{
    addr = other.addr;
    prefix = other.prefix;
    count = other.count;
    std::swap(left, other.left);
    std::swap(right, other.right);
    return *this;
}

void Record::merge_ordered (Record& r1, Record& r2, Record& r3) {
    Record a = combine(r1,r2);
    Record b = combine(r2,r3);
    if (a.prefix > b.prefix) {
        a.parent = this;
        a.add(&r1);
        a.add(&r2);
        left = new Record(std::move(a));
        r3.parent = this;
        right = &r3;
    } else {
        b.parent = this;
        b.add(&r2);
        b.add(&r3);
        right = new Record(std::move(b));
        r1.parent = this;
        left = &r1;
    }
}

Record&& Record::combine (const Record& r1, const Record& r2) 
{
    // Find the leftmost common mask between these two records
    auto cprefix = std::min(r1.prefix,r2.prefix);
    uint32_t caddr = r1.addr & r2.addr & mask(cprefix);
    Record com(caddr,cprefix);
    com.count = r1.count + r2.count;
    return std::move(com);
}

bool Record::add (Record* other) 
{
    auto cmpl = left ? Record::compare(*other, *left) : INCOMPARABLE;
    auto cmpr = right? Record::compare(*other, *right) : INCOMPARABLE;

    // Try to fit other into the ordered tree.
    // Handle all the easy cases first.
    // Prefer containment to imply ancestry,
    // But sometimes fullness and adjacency will also imply ancestry.

    // If there is no left child, use it
    if (left == nullptr) { // easy case: current has no children
        left = other;
        left->parent = this;
    } else if (right == nullptr) { // current has just one left child
        // IMPLICATION: The current node must be the root node or a newly created inner node, since non-root inner nodes always have two children.
        if (cmpl == STRICT_GT) {
            right = other;
            right->parent = this;
        } else if (cmpl == STRICT_LT) {
            right = left;
            left = other;
            left->parent = this;
        } else if (cmpl == WITHIN) {
            left->add(other);
        } else if (cmpl == CONTAINS) {
            other->parent = this;
            other->add(left);
            left = other;
        } else if (cmpl == EQUAL) {
            left->count += other->count;
            return false;
        } else if (cmpr == EQUAL) {
            right->count += other->count;
            return false;
        } else {
            throw std::logic_error("Comparisons to left child didn't make sense. Bail!");
        }
    } else { // medium cases: current has both left and right children
        // L&R both full
        if (cmpl == CONTAINS) {
            left->add(other);
        } else if (cmpr == CONTAINS) {
            right->add(other);
        } else if (cmpl == WITHIN) {
            other->parent = this;
            other->add(left);
            left = other;
        } else if (cmpr == WITHIN) {
            other->parent = this;
            other->add(right);
            right = other;
        } else if (cmpl == STRICT_LT) { // hard cases start here: left and right children don't contain the new record, need to create inner nodes
            merge_ordered(*other,*left,*right);
        } else if (cmpr == STRICT_GT) {
            merge_ordered(*left,*right,*other);
        } else if (cmpl == STRICT_GT && cmpr == STRICT_LT) {
            merge_ordered(*left,*other,*right);
        } else if (cmpl == EQUAL) {
            // This can happen if the ip address is given more than once.
            // In this case, just bump the count and return a false since the record wasn't added.
            left->count += other->count;
            return false;
        } else if (cmpr == EQUAL) {
            right->count += other->count;
            return false;
        } else {
            throw std::logic_error("We should never get here!!!");
        }
    }
    count += other->count;
    return true;
}



