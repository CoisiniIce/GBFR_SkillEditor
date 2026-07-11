#include <QApplication>
#include <QFile>
#include <QIcon>

#include "MainWindow.h"

/**
 * @brief 程序入口
 *
 * 1. 加载 QSS 样式表，实现 Dracula Dark 主题
 * 2. 设置应用图标
 * 3. 创建并显示主窗口
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 应用元信息
    app.setApplicationName("GBFR 因子编辑器");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("GBFR Tools");

    // 加载暗色主题样式表
    QFile qss("style.qss");
    if (qss.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(qss.readAll());
        qss.close();
    }

    // 设置窗口图标（.rc 资源已嵌入 EXE，供任务栏/Alt+Tab 使用）
    app.setWindowIcon(QIcon("app_icon.ico"));

    MainWindow w;
    w.show();

    return app.exec();
}
