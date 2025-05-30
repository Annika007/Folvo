#include "mainwindow.h"
#include "DataModel.h"
#include "splashscreen.h" // 包含 SplashScreen 的头文件
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QPixmap>        // 包含 QPixmap 用于加载图片
#include <QMessageBox>    // 用于更友好的错误提示
#include <QEventLoop>     // 用于确保启动画面能完整显示和处理事件

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);


    QPixmap brandPixmap(":/qidong.png");

    SplashScreen* splash = nullptr;

    if (!brandPixmap.isNull()) {
        splash = new SplashScreen(brandPixmap); // *** 图片有效才创建 SplashScreen ***
        splash->show(); // 先显示启动画面，让用户立即看到东西


    } else {
        qCritical() << "错误: 无法加载启动画面品牌图片 ':/qidong.png'. 将不显示启动画面。";
    }


    QString dataPath = "data.json";
    if (!QFile::exists(dataPath)) {
        qDebug() << "数据文件不存在，创建默认文件";
        QFile defaultFile(dataPath);
        if (defaultFile.open(QIODevice::WriteOnly)) {
            defaultFile.write(R"([{"time":"2023-01-01T00:00:00","amount":0.0,"location":"默认","category":"未分类","notes":""}])");
            defaultFile.close();
        } else {
            qWarning() << "无法创建默认数据文件: " << defaultFile.errorString();
        }
    }

    DataModel model;
    bool dataLoadedSuccessfully = model.loadFromJson(dataPath);
    if (!dataLoadedSuccessfully) {
        qWarning() << "警告: 加载 " << dataPath << " 失败! 程序将以空数据或默认数据启动。";

    }

    MainWindow w(&model);

    if (splash) {
        QObject::connect(splash, &SplashScreen::animationFinished, &w, &MainWindow::show); // 主窗口显示
        QObject::connect(splash, &SplashScreen::animationFinished, splash, &SplashScreen::close); // 关闭启动画面

        splash->showAndAnimate(3000);
    } else {

        w.show();
    }

    return a.exec();
}
