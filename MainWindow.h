#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QMap>
#include <QVector>
#include <vector>

#include "RecordParser.h"

/**
 * @brief GBFR 因子编辑器主窗口
 *
 * 功能：
 *   - 打开/解析 .tbl 二进制数据文件
 *   - 按因子（skillId）分组展示所有等级的参数
 *   - 支持实时编辑参数、批量等差数列填充
 *   - 自动加载 JSON 格式因子名称对照表
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void openFile();
    void saveFile();
    void onSkillChanged(int index);
    void onTableItemChanged(QTableWidgetItem *item);
    void onSearchChanged(const QString &text);
    void autoFill();

private:
    // ── UI 构建 ──
    void setupUI();

    // ── 数据操作 ──
    void populateSkills();          // 从 m_records 构建因子索引
    void filterSkills();            // 按搜索词刷新因子下拉框
    void loadSkillData();           // 加载选中因子的数据到表格
    void loadIdMap(const QString &path);
    void tryAutoLoadIdMap();        // 自动读取同目录的 JSON 对照表

    // ── 数据结构 ──
    std::vector<SkillRecord>  m_records;         // 全部原始记录
    QMap<uint32_t, QString>   m_idMap;           // skillId → 名称
    QString                   m_currentFilePath; // 当前打开的文件路径
    QByteArray                m_fileHeader;      // 8 字节文件头

    // 因子索引：记录每个 skillId 在 m_records 中的范围
    struct SkillEntry {
        uint32_t skillId;   // 因子 ID
        int      firstRow;  // 在 m_records 中的起始行
        int      maxLevel;  // 该因子的最高等级
    };
    QVector<SkillEntry> m_skills;
    int                 m_currentSkillIdx = -1;

    // ── 控件 ──
    // 工具栏
    QPushButton *m_openBtn;
    QPushButton *m_saveBtn;
    QLabel      *m_fileLabel;

    // 搜索 / 选择
    QLineEdit *m_searchEdit;
    QComboBox *m_skillCombo;
    QLabel    *m_statusLabel;

    // 表格（双表实现冻结列）
    QTableWidget *m_frozenTable;   // 等级列（冻结，不横向滚动）
    QTableWidget *m_table;         // 参数列（可横向滚动）

    // 批量填充
    QComboBox      *m_fillParamCombo;
    QDoubleSpinBox *m_fillBase;
    QDoubleSpinBox *m_fillStep;
    QPushButton    *m_fillBtn;

    // ── 状态 ──
    bool m_updating = false;   // 防止信号-槽循环
};

#endif // MAINWINDOW_H
