#pragma once

#include <string>

namespace game::data {

/**
 * @brief 管理不同游戏场景之间的游戏状态
 *
 * 存储玩家生命值、分数、当前关卡等信息，
 * 使这些数据在场景切换时能够保持。
 */
class SessionData final
{
public:
    SessionData() = default;
    ~SessionData() = default;

    // 删除复制和移动操作以防止意外复制
    SessionData(const SessionData&) = delete;
    SessionData& operator=(const SessionData&) = delete;
    SessionData(SessionData&&) = delete;
    SessionData& operator=(SessionData&&) = delete;

    // --- Getters ---
    int currentHealth() const { return m_currentHealth; }
    int maxHealth() const { return m_maxHealth; }
    int currentScore() const { return m_currentScore; }
    int highestScore() const { return m_highestScore; }
    int levelHealth() const { return m_levelHealth; }
    int levelScore() const { return m_levelScore; }
    const std::string& mapPath() const { return m_mapPath; }

    // --- Setters ---
    void setCurrentHealth(int health);
    void setMaxHealth(int maxHealth);
    void addScore(int scoreToAdd);
    void setHighestScore(int highestScore) { m_highestScore = highestScore; }
    void setLevelHealth(int levelHealth) { m_levelHealth = levelHealth; }
    void setLevelScore(int levelScore) { m_levelScore = levelScore; }
    void setMapPath(const std::string& mapPath) { m_mapPath = mapPath; }

    ///< @brief 重置游戏数据以准备开始新游戏（保留最高分）
    void reset();
    ///< @brief 设置下一个场景信息（地图、关卡开始时的得分生命）
    void setNextLevel(const std::string& mapPath);
    ///< @brief 将当前游戏数据保存到 JSON 文件（存档）
    bool saveToFile(const std::string& fileName) const;
    ///< @brief 从 JSON 文件中读取游戏数据（读档）
    bool loadFromFile(const std::string& fileName);

private:
    int m_currentHealth{ 3 };
    int m_maxHealth{ 3 };
    int m_currentScore{ 0 };
    int m_highestScore{ 0 };

    int m_levelHealth{ 3 }; ///< @brief 进入关卡时的生命值（读/存档用）
    int m_levelScore{ 0 };  ///< @brief 进入关卡时的得分（读/存档用）
    std::string m_mapPath{ "assets/maps/level1.tmj" };
};

} // namespace game::data
