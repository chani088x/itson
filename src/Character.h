#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

struct StageInfo {
    std::string name;
    std::string behavior;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(StageInfo, name, behavior)
};

/**
 * 호감도, 관계 단계, 이벤트 플래그를 가진 로맨스 대상을 나타냅니다.
 */
class Character {
public:
    // 기본 능력치로 빈 캐릭터를 생성합니다.
    Character();

    // 이름을 받는 생성자
    explicit Character(std::string name);

    // 캐릭터의 표시 이름을 반환합니다.
    const std::string& GetName() const;

    // 표시 이름을 업데이트합니다.
    void SetName(const std::string& name);

    // 현재 호감도 값(0-100)을 반환합니다.
    int GetAffection() const;

    // 호감도 값을 [0, 100] 범위로 제한하여 설정합니다.
    void SetAffection(int value);

    // 호감도 값을 [0, 100] 범위 내에서 증감시킵니다.
    void AddAffection(int delta);

    // 관계 단계(0-3)를 반환합니다.
    int GetRelationshipStage() const;

    // 관계 단계를 [0, 3] 범위로 제한하여 설정합니다.
    void SetRelationshipStage(int stage);

    // 가능하다면 관계 단계를 한 단계 올립니다.
    void AdvanceRelationshipStage();

    // 변경 불가능한 성격 특성을 반환합니다.
    const std::vector<std::string>& GetTraits() const;

    // 성격 특성을 할당합니다.
    void SetTraits(const std::vector<std::string>& traits);

    // 인덱스를 기반으로 단계 정보를 가져오는 헬퍼 함수입니다.
    StageInfo GetStageInfo(int stageIdx) const;

    // 임계값 기반 이벤트가 발생했음을 표시합니다.
    void MarkEventTriggered(int threshold);

    // 주어진 임계값 이벤트가 이미 발생했는지 반환합니다.
    bool HasTriggeredEvent(int threshold) const;

    // 영구 저장을 위해 모든 트리거된 임계값을 반환합니다.
    const std::unordered_map<int, bool>& GetTriggeredEvents() const;

    // 저장 불라오기를 위한 트리거된 맵의 가변 참조를 반환합니다.
    std::unordered_map<int, bool>& EditableTriggeredEvents();

private:
    std::string name_;
    int affection_;
    int relationshipStage_;
    std::vector<std::string> traits_;

    std::unordered_map<int, bool> triggeredEvents_;
    std::map<int, StageInfo> emotionStages_;

    friend void to_json(nlohmann::json& j, const Character& p) {
        j = nlohmann::json{
            {"name", p.name_},
            {"affection", p.affection_},
            {"relationshipStage", p.relationshipStage_},
            {"traits", p.traits_},
            {"triggered", p.triggeredEvents_},
            {"emotionStages", p.emotionStages_}
        };
    }

    friend void from_json(const nlohmann::json& j, Character& p) {
        p.name_ = j.value("name", "Unknown");
        p.affection_ = j.value("affection", 10);
        p.relationshipStage_ = j.value("relationshipStage", 0);
        p.traits_ = j.value("traits", std::vector<std::string>{});
        // nlohmann::json automatically handles conversion for std::unordered_map<int, bool>
        // if keys are string representations of integers.
        if (j.contains("triggered")) {
             p.triggeredEvents_ = j["triggered"].get<std::unordered_map<int, bool>>();
        }
        if (j.contains("emotionStages")) {
             p.emotionStages_ = j["emotionStages"].get<std::map<int, StageInfo>>();
        }

        // Fallback for template compatibility (optional)
        if (j.contains("initialAffection")) p.affection_ = j.value("initialAffection", 10);
        if (j.contains("initialStage")) p.relationshipStage_ = j.value("initialStage", 0);
    }
};
