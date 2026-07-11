#ifndef RECORDPARSER_H
#define RECORDPARSER_H

#include <cstdint>
#include <vector>
#include <array>
#include <cstring>

/**
 * @brief 因子记录结构体（紧凑对齐 1 字节）
 *
 * 文件格式（小端序）：
 *   偏移 0x00 ~ 0x27 : 10 个 float 参数 (40 字节)
 *   偏移 0x28 ~ 0x2B : skillId 因子 ID  (4 字节)
 *   偏移 0x2C ~ 0x2F : 未知固定值      (4 字节)
 *   偏移 0x30 ~ 0x33 : 等级             (4 字节)
 *   合计 : 52 字节
 */
#pragma pack(push, 1)
struct SkillRecord {
    std::array<float, 10> values;   // 参数值数组
    uint32_t               skillId;  // 因子唯一标识
    uint32_t               unknown;  // 保留字段（固定值）
    uint32_t               level;    // 等级（1~N）
};
#pragma pack(pop)

/// 单条记录字节数
static constexpr int RECORD_SIZE      = sizeof(SkillRecord);

/// 文件头字节数（前 8 字节为文件元数据）
static constexpr int FILE_HEADER_SIZE = 8;

/// 每条记录的参数数量
static constexpr int PARAM_COUNT      = 10;

/**
 * @brief 从原始字节数据解析记录数组
 * @param data  输入字节序列
 * @return      解析后的 SkillRecord 数组
 */
inline std::vector<SkillRecord> parseRecords(const std::vector<uint8_t>& data) {
    std::vector<SkillRecord> records;
    if (data.size() < RECORD_SIZE) return records;

    size_t count = data.size() / RECORD_SIZE;
    records.resize(count);
    std::memcpy(records.data(), data.data(), count * RECORD_SIZE);
    return records;
}

/**
 * @brief 将记录数组打包为字节数据
 * @param records  输入记录数组
 * @return         可用于写回文件的字节序列
 */
inline std::vector<uint8_t> packRecords(const std::vector<SkillRecord>& records) {
    std::vector<uint8_t> data(records.size() * RECORD_SIZE);
    std::memcpy(data.data(), records.data(), data.size());
    return data;
}

#endif // RECORDPARSER_H
