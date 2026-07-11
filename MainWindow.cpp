#include "MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QGroupBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

// ══════════════════════════════════════════════════════════════════
//  构造
// ══════════════════════════════════════════════════════════════════

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setWindowTitle("GBFR 因子编辑器");
    resize(860, 560);
}

// ══════════════════════════════════════════════════════════════════
//  UI 搭建
// ══════════════════════════════════════════════════════════════════

void MainWindow::setupUI() {
    auto *central = new QWidget(this);
    central->setObjectName("centralWidget");

    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(8);

    // ── 工具栏 ──
    auto *toolbar = new QHBoxLayout();

    m_openBtn = new QPushButton("打开 TBL");
    m_openBtn->setObjectName("openBtn");
    toolbar->addWidget(m_openBtn);

    m_saveBtn = new QPushButton("保存 TBL");
    m_saveBtn->setObjectName("saveBtn");
    toolbar->addWidget(m_saveBtn);

    m_fileLabel = new QLabel("未打开文件");
    m_fileLabel->setObjectName("fileLabel");
    toolbar->addWidget(m_fileLabel, 1);

    mainLayout->addLayout(toolbar);

    // ── 搜索 / 选择行 ──
    auto *selLayout = new QHBoxLayout();

    selLayout->addWidget(new QLabel("搜索:"));
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("输入 ID 或名称筛选...");
    m_searchEdit->setClearButtonEnabled(true);
    selLayout->addWidget(m_searchEdit);

    selLayout->addSpacing(12);
    selLayout->addWidget(new QLabel("选择因子:"));

    m_skillCombo = new QComboBox();
    m_skillCombo->setMinimumWidth(340);
    selLayout->addWidget(m_skillCombo, 1);

    m_statusLabel = new QLabel("");
    m_statusLabel->setObjectName("statusLabel");
    selLayout->addWidget(m_statusLabel);

    mainLayout->addLayout(selLayout);

    // ── 双表：冻结等级列 + 可滚动参数表 ──
    auto *tableContainer = new QWidget();
    auto *tableLayout = new QHBoxLayout(tableContainer);
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(0);

    // 冻结的等级列
    m_frozenTable = new QTableWidget(0, 1, this);
    m_frozenTable->setObjectName("frozenTable");
    m_frozenTable->setHorizontalHeaderLabels({"等级"});
    m_frozenTable->setFixedWidth(75);
    m_frozenTable->verticalHeader()->hide();
    m_frozenTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_frozenTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_frozenTable->setFocusPolicy(Qt::NoFocus);
    m_frozenTable->horizontalHeader()->setStretchLastSection(true);
    m_frozenTable->setAlternatingRowColors(true);
    m_frozenTable->verticalHeader()->setDefaultSectionSize(24);
    m_frozenTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_frozenTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 参数表（可横向滚动）
    m_table = new QTableWidget(0, PARAM_COUNT, this);
    m_table->setHorizontalHeaderLabels(
        {"参数1", "参数2", "参数3", "参数4", "参数5",
         "参数6", "参数7", "参数8", "参数9", "参数10"});
    m_table->verticalHeader()->hide();
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_table->verticalHeader()->setDefaultSectionSize(24);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked
                             | QAbstractItemView::SelectedClicked);
    m_table->setAlternatingRowColors(true);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);

    tableLayout->addWidget(m_frozenTable);
    tableLayout->addWidget(m_table, 1);
    mainLayout->addWidget(tableContainer, 1);

    // 同步两表垂直滚动
    connect(m_table->verticalScrollBar(), &QScrollBar::valueChanged,
            m_frozenTable->verticalScrollBar(), &QScrollBar::setValue);
    connect(m_frozenTable->verticalScrollBar(), &QScrollBar::valueChanged,
            m_table->verticalScrollBar(), &QScrollBar::setValue);

    // ── 批量填充面板 ──
    auto *fillGroup = new QGroupBox("批量填充（等差数列）");
    auto *fillLayout = new QHBoxLayout(fillGroup);

    fillLayout->addWidget(new QLabel("参数:"));
    m_fillParamCombo = new QComboBox();
    m_fillParamCombo->setObjectName("fillParamCombo");
    for (int i = 1; i <= PARAM_COUNT; ++i)
        m_fillParamCombo->addItem(QString("参数%1").arg(i));
    m_fillParamCombo->setCurrentIndex(0);
    fillLayout->addWidget(m_fillParamCombo);

    fillLayout->addSpacing(16);

    fillLayout->addWidget(new QLabel("等级1值:"));
    m_fillBase = new QDoubleSpinBox();
    m_fillBase->setDecimals(1);
    m_fillBase->setRange(-999999.0, 999999.0);
    m_fillBase->setSingleStep(0.1);
    m_fillBase->setMinimumWidth(120);
    fillLayout->addWidget(m_fillBase);

    fillLayout->addSpacing(8);

    fillLayout->addWidget(new QLabel("每级递增:"));
    m_fillStep = new QDoubleSpinBox();
    m_fillStep->setDecimals(1);
    m_fillStep->setRange(-999999.0, 999999.0);
    m_fillStep->setSingleStep(0.1);
    m_fillStep->setMinimumWidth(120);
    fillLayout->addWidget(m_fillStep);

    fillLayout->addSpacing(16);

    m_fillBtn = new QPushButton("▶ 应用到全部等级");
    m_fillBtn->setObjectName("fillBtn");
    fillLayout->addWidget(m_fillBtn);
    fillLayout->addStretch();

    mainLayout->addWidget(fillGroup);

    setCentralWidget(central);

    // ── 信号连接 ──
    connect(m_openBtn,    &QPushButton::clicked,        this, &MainWindow::openFile);
    connect(m_saveBtn,    &QPushButton::clicked,        this, &MainWindow::saveFile);
    connect(m_skillCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                                                       this, &MainWindow::onSkillChanged);
    connect(m_searchEdit, &QLineEdit::textChanged,      this, &MainWindow::onSearchChanged);
    connect(m_table,      &QTableWidget::itemChanged,   this, &MainWindow::onTableItemChanged);
    connect(m_fillBtn,    &QPushButton::clicked,        this, &MainWindow::autoFill);

    // ── 初始状态 ──
    m_saveBtn->setEnabled(false);
    m_fillBtn->setEnabled(false);
    m_table->setEnabled(false);
    m_frozenTable->setEnabled(false);
}

// ══════════════════════════════════════════════════════════════════
//  打开文件
// ══════════════════════════════════════════════════════════════════

void MainWindow::openFile() {
    const QString path = QFileDialog::getOpenFileName(
        this, "打开 .tbl 文件", QString(),
        "TBL Files (*.tbl);;所有文件 (*)");

    if (path.isEmpty()) return;

    // ── 读取全部字节 ──
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "错误", "无法打开文件");
        return;
    }
    const QByteArray allData = file.readAll();
    file.close();

    // ── 有效性检查 ──
    if (allData.size() < FILE_HEADER_SIZE + RECORD_SIZE) {
        QMessageBox::warning(this, "错误", "文件太小，格式可能不正确");
        return;
    }

    // ── 分离文件头与记录体 ──
    m_fileHeader = allData.left(FILE_HEADER_SIZE);
    const QByteArray body = allData.mid(FILE_HEADER_SIZE);

    const size_t count = body.size() / RECORD_SIZE;
    m_records.resize(count);
    std::memcpy(m_records.data(), body.constData(), count * RECORD_SIZE);

    // ── 更新界面 ──
    m_currentFilePath = path;
    m_fileLabel->setText(QFileInfo(path).fileName());
    m_saveBtn->setEnabled(true);

    populateSkills();
    tryAutoLoadIdMap();
}

// ══════════════════════════════════════════════════════════════════
//  保存文件
// ══════════════════════════════════════════════════════════════════

void MainWindow::saveFile() {
    if (m_currentFilePath.isEmpty()) return;

    // 将当前表格的编辑结果写回内存记录
    if (m_currentSkillIdx >= 0) {
        m_updating = true;
        const auto &entry = m_skills[m_currentSkillIdx];

        for (int lv = 0; lv < entry.maxLevel; ++lv) {
            const int row = entry.firstRow + lv;
            if (row >= static_cast<int>(m_records.size())) break;

            auto &rec = m_records[row];
            for (int p = 0; p < PARAM_COUNT; ++p) {
                if (auto *item = m_table->item(lv, p))
                    rec.values[p] = item->text().toFloat();
            }
        }
        m_updating = false;
    }

    // 打包为二进制
    QByteArray bodyData;
    bodyData.resize(static_cast<int>(m_records.size() * RECORD_SIZE));
    std::memcpy(bodyData.data(), m_records.data(), bodyData.size());

    // 写出文件
    QFile file(m_currentFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "错误", "无法写入文件");
        return;
    }
    file.write(m_fileHeader);
    file.write(bodyData);
    file.close();

    QMessageBox::information(this, "成功",
        QString("已保存 %1 条记录").arg(m_records.size()));
}

// ══════════════════════════════════════════════════════════════════
//  对照表自动加载
// ══════════════════════════════════════════════════════════════════

void MainWindow::tryAutoLoadIdMap() {
    // 在 .tbl 同目录下查找 skill_id_map.json
    const QFileInfo tblInfo(m_currentFilePath);
    const QString jsonPath = tblInfo.absolutePath() + "/skill_id_map.json";

    if (!QFile::exists(jsonPath)) return;

    loadIdMap(jsonPath);
    populateSkills();   // 重新填充下拉框（带上中文名称）
}

void MainWindow::loadIdMap(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return;

    const QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) return;

    // 格式：{ "0x50079A1C": "攻击力", ... }
    const QJsonObject obj = doc.object();
    m_idMap.clear();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        bool ok;
        const uint32_t id = it.key().toUInt(&ok, 16);
        if (ok)
            m_idMap.insert(id, it.value().toString());
    }
}

// ══════════════════════════════════════════════════════════════════
//  构建因子索引
// ══════════════════════════════════════════════════════════════════

void MainWindow::populateSkills() {
    m_skills.clear();
    m_currentSkillIdx = -1;

    m_table->setEnabled(false);
    m_frozenTable->setEnabled(false);
    m_fillBtn->setEnabled(false);

    // 扫描 m_records，将连续相同 skillId 的记录归为一组
    int i = 0;
    while (i < static_cast<int>(m_records.size())) {
        const uint32_t id = m_records[i].skillId;
        const int start = i;
        int maxLv = 0;

        while (i < static_cast<int>(m_records.size())
               && m_records[i].skillId == id) {
            const int lv = static_cast<int>(m_records[i].level);
            if (lv > maxLv) maxLv = lv;
            ++i;
        }

        m_skills.append({id, start, maxLv});
    }

    filterSkills();

    // 因子计数（排除 skillId == 0 的空记录）
    const int validCount = m_skills.size()
        - (m_skills.size() > 0 && m_skills[0].skillId == 0 ? 1 : 0);

    m_statusLabel->setText(
        QString("共 %1 条记录，%2 个因子")
            .arg(m_records.size())
            .arg(validCount));
}

// ══════════════════════════════════════════════════════════════════
//  搜索筛选
// ══════════════════════════════════════════════════════════════════

void MainWindow::onSearchChanged(const QString & /*text*/) {
    filterSkills();
}

void MainWindow::filterSkills() {
    const QString filter = m_searchEdit->text().trimmed().toLower();

    m_skillCombo->blockSignals(true);
    m_skillCombo->clear();

    for (int idx = 0; idx < m_skills.size(); ++idx) {
        const auto &entry = m_skills[idx];
        if (entry.skillId == 0) continue;   // 跳过空记录

        const QString name = m_idMap.value(entry.skillId, "");
        const QString hex =
            QString("%1").arg(entry.skillId, 8, 16, QChar('0')).toLower();

        // 筛选匹配
        if (!filter.isEmpty()) {
            if (!name.toLower().contains(filter)
                && !hex.contains(filter))
                continue;
        }

        // 构建显示文本
        QString label =
            QString("0x%1").arg(entry.skillId, 8, 16, QChar('0')).toUpper();
        if (!name.isEmpty())
            label = name + "  [" + label + "]";
        label += QString("  (%1级)").arg(entry.maxLevel);

        m_skillCombo->addItem(label, QVariant::fromValue(idx));
    }

    m_skillCombo->blockSignals(false);

    if (m_skillCombo->count() > 0) {
        m_skillCombo->setCurrentIndex(0);
        onSkillChanged(0);
    } else {
        m_table->setEnabled(false);
        m_frozenTable->setEnabled(false);
        m_fillBtn->setEnabled(false);
        m_currentSkillIdx = -1;
    }
}

// ══════════════════════════════════════════════════════════════════
//  切换因子 → 加载数据
// ══════════════════════════════════════════════════════════════════

void MainWindow::onSkillChanged(int index) {
    if (index < 0) {
        m_table->setEnabled(false);
        m_frozenTable->setEnabled(false);
        m_fillBtn->setEnabled(false);
        return;
    }

    const int skillIdx = m_skillCombo->itemData(index).toInt();
    m_currentSkillIdx = skillIdx;

    loadSkillData();

    const auto &entry = m_skills[skillIdx];
    m_statusLabel->setText(QString("%1 级").arg(entry.maxLevel));
    m_fillBtn->setEnabled(true);
}

void MainWindow::loadSkillData() {
    if (m_currentSkillIdx < 0) return;

    const auto &entry = m_skills[m_currentSkillIdx];
    const int rows = entry.maxLevel;

    m_updating = true;
    m_table->blockSignals(true);
    m_frozenTable->blockSignals(true);

    m_table->setEnabled(true);
    m_frozenTable->setEnabled(true);
    m_table->setRowCount(rows);
    m_frozenTable->setRowCount(rows);

    for (int lv = 0; lv < rows; ++lv) {
        const int recordRow = entry.firstRow + lv;
        if (recordRow >= static_cast<int>(m_records.size())) break;

        const auto &rec = m_records[recordRow];

        // 等级列（冻结，居中只读）
        auto *lvItem = new QTableWidgetItem(QString::number(rec.level));
        lvItem->setTextAlignment(Qt::AlignCenter);
        lvItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_frozenTable->setItem(lv, 0, lvItem);

        // 参数列（可编辑）
        for (int p = 0; p < PARAM_COUNT; ++p) {
            auto *cell = new QTableWidgetItem(
                QString::number(rec.values[p], 'f', 1));
            m_table->setItem(lv, p, cell);
        }
    }

    m_table->blockSignals(false);
    m_frozenTable->blockSignals(false);
    m_updating = false;

    setWindowTitle(QString("GBFR 因子编辑器 — 0x%1")
        .arg(entry.skillId, 8, 16, QChar('0')).toUpper());
}

// ══════════════════════════════════════════════════════════════════
//  表格编辑 → 实时写回
// ══════════════════════════════════════════════════════════════════

void MainWindow::onTableItemChanged(QTableWidgetItem *item) {
    if (m_updating || !item || m_currentSkillIdx < 0) return;

    const int lvIdx = item->row();
    const int col   = item->column();

    if (col < 0 || col >= PARAM_COUNT) return;

    const auto &entry = m_skills[m_currentSkillIdx];
    const int recordRow = entry.firstRow + lvIdx;
    if (recordRow >= static_cast<int>(m_records.size())) return;

    bool ok;
    const float val = item->text().toFloat(&ok);
    if (ok)
        m_records[recordRow].values[col] = val;
}

// ══════════════════════════════════════════════════════════════════
//  批量填充（等差数列）
// ══════════════════════════════════════════════════════════════════

void MainWindow::autoFill() {
    if (m_currentSkillIdx < 0) return;

    const int   paramIdx = m_fillParamCombo->currentIndex();
    const double base    = m_fillBase->value();
    const double step    = m_fillStep->value();

    const auto &entry = m_skills[m_currentSkillIdx];

    m_updating = true;
    m_table->blockSignals(true);

    for (int lv = 0; lv < entry.maxLevel; ++lv) {
        const float val = static_cast<float>(base + lv * step);
        const int recordRow = entry.firstRow + lv;

        // 写回内存记录
        m_records[recordRow].values[paramIdx] = val;

        // 刷新表格显示
        if (auto *cell = m_table->item(lv, paramIdx))
            cell->setText(QString::number(val, 'f', 1));
    }

    m_table->blockSignals(false);
    m_updating = false;

    QMessageBox::information(this, "填充完成",
        QString("已将 %1 的 1~%2 级按等差数列填充完毕")
            .arg(m_fillParamCombo->currentText())
            .arg(entry.maxLevel));
}
