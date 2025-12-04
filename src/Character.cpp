#include "Character.h"

#include <algorithm>

namespace {
constexpr int kMinAffection = -100;
constexpr int kMaxAffection = 100;
constexpr int kMinStage = 0;
constexpr int kMaxStage = 3;
}  // namespace

Character::Character() : Character("Unknown") {}

Character::Character(std::string name)
    : name_(std::move(name)),
      affection_(10),
      relationshipStage_(0) {}

const std::string& Character::GetName() const {
    return name_;
}

void Character::SetName(const std::string& name) {
    name_ = name;
}

int Character::GetAffection() const {
    return affection_;
}

void Character::SetAffection(int value) {
    affection_ = std::clamp(value, kMinAffection, kMaxAffection);
}

void Character::AddAffection(int delta) {
    SetAffection(affection_ + delta);
}

int Character::GetRelationshipStage() const {
    return relationshipStage_;
}

void Character::SetRelationshipStage(int stage) {
    relationshipStage_ = std::clamp(stage, kMinStage, kMaxStage);
}

void Character::AdvanceRelationshipStage() {
    SetRelationshipStage(relationshipStage_ + 1);
}

const std::vector<std::string>& Character::GetTraits() const {
    return traits_;
}

void Character::SetTraits(const std::vector<std::string>& traits) {
    traits_ = traits;
}

const std::vector<std::string>& Character::GetMemories() const {
    return memories_.Entries();
}

void Character::AddMemory(const std::string& memory) {
    memories_.Add(memory);
}

void Character::SetMemories(const std::vector<std::string>& memories) {
    memories_.Set(memories);
}

void Character::ClearMemories() {
    memories_.Clear();
}

void Character::SetFlag(const std::string& flag, bool value) {
    flags_[flag] = value;
}

bool Character::GetFlag(const std::string& flag) const {
    auto it = flags_.find(flag);
    return it != flags_.end() ? it->second : false;
}

const std::unordered_map<std::string, bool>& Character::GetFlags() const {
    return flags_;
}

void Character::MarkEventTriggered(int threshold) {
    triggeredEvents_[threshold] = true;
}

bool Character::HasTriggeredEvent(int threshold) const {
    auto it = triggeredEvents_.find(threshold);
    return it != triggeredEvents_.end() && it->second;
}

const std::unordered_map<int, bool>& Character::GetTriggeredEvents() const {
    return triggeredEvents_;
}

std::unordered_map<int, bool>& Character::EditableTriggeredEvents() {
    return triggeredEvents_;
}
