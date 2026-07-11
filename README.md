<div align="center">

# GBFR SkillEditor

**碧蓝幻想 Relink 因子编辑器**

![Windows](https://img.shields.io/badge/Windows-10%2F11-0078D6?logo=windows)
![Qt](https://img.shields.io/badge/Qt-6-41CD52?logo=qt)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-064F8C?logo=cmake)
![License](https://img.shields.io/badge/License-MIT-green)

一个用于查看和编辑《碧蓝幻想 Relink》游戏 `.tbl` 数据文件的桌面工具。支持按因子 ID 分组展示、参数编辑、批量等差数列填充等功能。

</div>

---

## 功能

- **打开 / 解析** `.tbl` 二进制数据文件
- **实时编辑** 双击参数格即可修改，修改即时写回内存
- **批量填充** 为指定参数按等差数列（起始值 + 每级增量）一键填充全部等级
- **搜索筛选** 支持通过因子 ID（十六进制）或中文名称搜索
- **因子名称映射** 自动加载**同目录**下的 `skill_id_map.json`

## 数据格式

`.tbl` 文件结构（小端序，每条记录 52 字节）：

| 偏移 | 大小 | 说明 |
|------|------|------|
| 0x00~0x27 | 40 字节 (10×float) | 参数值数组（参数1~参数10） |
| 0x28~0x2B | 4 字节 (uint32) | 因子 ID（skillId） |
| 0x2C~0x2F | 4 字节 (uint32) | 未知保留字段 |
| 0x30~0x33 | 4 字节 (uint32) | 等级（1~N） |

文件前 8 字节为文件头，在保存时被原样保留。

## 环境要求

| 依赖 | 版本 |
|------|------|
| 操作系统 | Windows 10 / 11 |
| [Qt](https://www.qt.io/download) | 6.x（推荐 6.5+） |
| [CMake](https://cmake.org/download/) | 3.16+ |
| 编译器 | MinGW (Qt 配套) 或 MSVC |

## 构建

```bash
# 克隆仓库
git clone https://github.com/CCoisini/GBFR_SkillEditor.git
cd GBFR_SkillEditor

# 使用 CMake 构建
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

或在 **Qt Creator** 中直接打开 `CMakeLists.txt`，配置 kit 后构建运行。

## 使用

1. **打开 TBL** — 点击工具栏「打开 TBL」按钮，选择 `.tbl` 文件
2. **选择因子** — 在下拉框中选择要编辑的因子，或通过搜索框筛选
3. **编辑参数** — 双击参数格，输入新数值
4. **批量填充** — 在底部面板选择参数、设置 Lv1 值和每级增量，点击应用
5. **保存** — 点击「保存 TBL」写回文件（建议先备份原文件）

### 因子名称映射

在 `.tbl` 同目录下放置 `skill_id_map.json`，程序会自动加载并显示中文名称。格式：

```json
{
  "0x50079A1C": "攻击力",
  "0xD176D262": "群青的剑光"
}
```

> 项目根目录已附带一份完整的 `skill_id_map.json`。

## 项目结构

```
GBFR_SkillEditor/
├── CMakeLists.txt          # 构建配置
├── MainWindow.h / .cpp     # 主窗口视图 + 逻辑
├── RecordParser.h          # 数据格式定义与解析（仅头文件）
├── main.cpp                # 程序入口
├── style.qss               # 暗色主题样式表
├── app_icon.ico            # 应用图标
├── app_icon.rc             # Windows 资源脚本
├── skill_id_map.json       # 因子 ID ↔ 中文名称对照表
├── deploy.bat              # 发布打包脚本
└── data/                   # 示例 / 参考数据文件
```

## 许可证

本项目基于 [MIT License](LICENSE) 开源。

## 免责声明

- 本工具仅用于**学习与交流目的**
- 修改游戏数据文件可能导致存档异常或游戏崩溃，请**提前备份**原始文件
- 与 Cygames / 碧蓝幻想 Relink 无关联，未经官方授权
