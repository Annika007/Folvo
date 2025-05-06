#include "mainwindow.h"
#include <QApplication>
#include "DataModel.h"
#include <QDebug>  // 添加qDebug头文件
#include <QFile>
#include <QStandardPaths>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 检查数据文件是否存在
    QString dataPath = "data.json";
    if (!QFile::exists(dataPath)) {
        qDebug() << "数据文件不存在，创建默认文件";
        QFile defaultFile(dataPath);
        if (defaultFile.open(QIODevice::WriteOnly)) {
            defaultFile.write(R"([{"time":"2023-01-01T00:00:00","amount":0,"location":"默认","category":"未分类"}])");
            defaultFile.close();
        }
    }

    DataModel model;
    if (!model.loadFromJson(dataPath)) {
        qDebug() << "Failed to load data.json!";
        return -1; // 退出程序
    }
    model.loadFromJson(dataPath); // 加载数据
    MainWindow w(&model);
    w.show();
    return a.exec();
}
