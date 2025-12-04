#include "Memory.h"

void Memory::Add(const std::string& entry) {
    entries_.push_back(entry);
}

void Memory::Set(const std::vector<std::string>& entries) {
    entries_ = entries;
}

void Memory::Clear() {
    entries_.clear();
}

const std::vector<std::string>& Memory::Entries() const {
    return entries_;
}
