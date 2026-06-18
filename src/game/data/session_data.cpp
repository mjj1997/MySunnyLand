#include "session_data.h"

#include <glm/common.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <fstream>

namespace game::data {

void SessionData::setCurrentHealth(int health)
{
    // 将生命值限制在 0 到 m_maxHealth 之间
    m_currentHealth = glm::clamp(health, 0, m_maxHealth);
}

void SessionData::setMaxHealth(int maxHealth)
{
    if (maxHealth > 0) {
        m_maxHealth = maxHealth;
        // 确保当前生命值不超过最大生命值
        setCurrentHealth(m_currentHealth);
    } else {
        spdlog::warn("尝试将最大生命值设置为非正数: {}", maxHealth);
    }
}

void SessionData::addScore(int scoreToAdd)
{
    m_currentScore += scoreToAdd;
    // 如果当前得分超过最高分，更新最高分
    setHighestScore(glm::max(m_highestScore, m_currentScore));
}

void SessionData::reset()
{
    m_currentHealth = m_maxHealth;
    m_currentScore = 0;
    m_levelHealth = 3;
    m_levelScore = 0;
    m_mapPath = "assets/maps/level1.tmj";

    spdlog::info("游戏数据重置完成。");
}

void SessionData::setNextLevel(std::string_view mapPath)
{
    m_mapPath = mapPath;
    m_levelHealth = m_currentHealth;
    m_levelScore = m_currentScore;
}

bool SessionData::saveToFile(std::string_view fileName) const
{
    nlohmann::json json;
    try {
        // 将成员变量序列化到 JSON 对象中
        json["levelScore"] = m_levelScore;
        json["levelHealth"] = m_levelHealth;
        json["maxHealth"] = m_maxHealth;
        json["highestScore"] = m_highestScore;
        json["mapPath"] = m_mapPath;

        // 打开文件进行写入
        auto path = std::filesystem::path{ fileName };
        std::ofstream file{ path };
        if (!file.is_open()) {
            spdlog::error("无法打开存档文件进行写入: {}", fileName);
            return false;
        }

        // 将 JSON 对象写入文件（使用 4 个空格缩进美化格式）
        file << json.dump(4);
        file.close();

        spdlog::info("游戏数据已成功保存到文件: {}", fileName);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("存档时出现错误 {}: {}", fileName, e.what());
        return false;
    }
}

bool SessionData::loadFromFile(std::string_view fileName)
{
    try {
        // 打开文件进行读取
        auto path = std::filesystem::path{ fileName };
        std::ifstream file{ path };
        if (!file.is_open()) {
            // 如果存档文件不存在，不一定是错误，所以只报警告
            spdlog::warn("读档时找不到文件: {}", fileName);
            return false;
        }

        // 从文件解析 JSON 数据
        nlohmann::json json;
        file >> json;
        file.close();

        m_currentScore = m_levelScore = json.value("levelScore", 0);
        m_currentHealth = m_levelHealth = json.value("levelHealth", 3);
        m_maxHealth = json.value("maxHealth", 3); // 使用合理的默认值作为最大生命值
        m_highestScore = json.value("highestScore", 0);
        m_mapPath = json.value("mapPath", "assets/maps/level1.tmj"); // 默认起始地图

        spdlog::info("游戏数据已成功从文件加载: {}", fileName);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("读档时出现错误 {}: {}", fileName, e.what());
        // 如果读取文件时出错，重置游戏数据
        reset();
        return false;
    }
}

bool SessionData::syncHighestScore(std::string_view fileName)
{
    try {
        // 打开文件进行读取
        auto path = std::filesystem::path{ fileName };
        std::fstream file{ path };
        if (!file.is_open()) {
            spdlog::warn("找不到文件: {}, 无法进行同步", fileName);
            return false;
        }

        // 从文件解析 JSON 数据
        nlohmann::json j;
        file >> j;
        auto highestScoreInFile = j.value("highestScore", 0);

        // 根据文件中的最高分和当前最高分来决定处理方式
        if (highestScoreInFile < m_highestScore) { // 文件中的最高分 低于 当前最高分
            j["highestScore"] = m_highestScore;
            file.seekp(0);     // 文件指针回到文件开头
            file << j.dump(4); // 将JSON对象写入文件
            spdlog::debug("当前最高分高于存档文件，已将新的最高分保存到存档中");
        } else if (highestScoreInFile > m_highestScore) { // 文件中的最高分 高于 当前最高分
            m_highestScore = highestScoreInFile;
            spdlog::debug("存档文件中的最高分高于当前最高分，已更新当前最高分");
        } else {
            spdlog::debug("存档文件中的最高分与当前最高分相同，无需更新");
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        spdlog::error("同步最高分时出现错误 {}: {}", fileName, e.what());
        return false;
    }
}

} // namespace game::data
