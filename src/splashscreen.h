#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget> // 基类
#include <QTimer>  // 用于定时更新进度条

class QLabel;
class QProgressBar;
class QVBoxLayout;
class QPixmap;

class SplashScreen : public QWidget
{
    Q_OBJECT

public:
    // 构造函数接收品牌图片的Pixmap
    explicit SplashScreen(const QPixmap &brandPixmap, QWidget *parent = nullptr);
    ~SplashScreen();

    // 开始显示启动画面并启动进度条动画
    // durationMilliseconds: 进度条走完的总时长（毫秒）
    void showAndAnimate(int durationMilliseconds = 3000);

signals:
    // 当进度条动画完成时发射此信号
    void animationFinished();

private slots:
    // 定时器触发时更新进度条
    void updateProgress();

private:
    QLabel* m_brandImageLabel;      // 用于显示品牌图片
    QProgressBar* m_progressBar;    // 显示进度
    QTimer* m_timer;                // 控制动画的定时器

    int m_animationDurationMs;      // 动画总时长
    int m_progressSteps;            // 进度条的总步数 (例如0-100)
    int m_currentProgressStep;      // 当前进度步数
};

#endif // SPLASHSCREEN_H
