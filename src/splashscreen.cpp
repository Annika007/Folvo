#include "splashscreen.h"

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QPixmap>
#include <QApplication> // 用于屏幕居中
#include <QScreen>      // 用于屏幕居中
#include <QGuiApplication> // QScreen在QGuiApplication中
#include <QDebug>       // 用于调试输出

SplashScreen::SplashScreen(const QPixmap &brandPixmap, QWidget *parent)
    : QWidget(parent, Qt::SplashScreen | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint), // 设置窗口标志
    m_brandImageLabel(new QLabel(this)),
    m_progressBar(new QProgressBar(this)),
    m_timer(new QTimer(this)),
    m_animationDurationMs(3000), // 默认动画时长3秒
    m_progressSteps(100),        // 进度条分为100步
    m_currentProgressStep(0)
{
    setAttribute(Qt::WA_DeleteOnClose); // 窗口关闭时自动删除对象

    // --- 1. 定义启动画面的目标窗口尺寸 ---
    // 这个尺寸是你希望启动画面在屏幕上显示的大小。
    // 考虑到你的图片是1000x600，我们不能直接用这个尺寸。
    // 我们设定一个较小的窗口尺寸，然后图片会按比例缩放来适应。
    const int splashWindowWidth = 500;  // 例如，启动画面窗口宽度
    const int splashWindowHeight = 350; // 例如，启动画面窗口高度
    setFixedSize(splashWindowWidth, splashWindowHeight); // 固定启动画面的大小

    // --- 2. 设置品牌图片 ---
    m_brandImageLabel->setAlignment(Qt::AlignCenter); // 图片在QLabel中居中
    if (!brandPixmap.isNull()) {
        // 计算图片标签可用的最大尺寸 (减去布局边距和进度条的预估空间)
        int margins = 20 * 2; // 假设上下左右各20px的内边距
        int progressBarHeightEst = 15; // 进度条高度+与图片的间距
        int availableWidthForImage = splashWindowWidth - margins;
        int availableHeightForImage = splashWindowHeight - margins - progressBarHeightEst;

        if (availableWidthForImage > 0 && availableHeightForImage > 0) {
            // 按比例缩放原始图片以适应可用空间
            QPixmap scaledPixmap = brandPixmap.scaled(
                availableWidthForImage,
                availableHeightForImage,
                Qt::KeepAspectRatio,        // 关键：保持图片的原始宽高比
                Qt::SmoothTransformation    // 使用平滑的缩放算法，效果更好
                );
            m_brandImageLabel->setPixmap(scaledPixmap);
        } else {
            qWarning() << "SplashScreen: 可用图片区域尺寸无效，窗口可能太小。";
            m_brandImageLabel->setPixmap(brandPixmap.scaled(100,60,Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 备用小图
        }
    } else {
        qDebug() << "SplashScreen: 传入的品牌图片为空!";
        m_brandImageLabel->setText("Folvo"); // 如果图片加载失败，显示文字
        m_brandImageLabel->setStyleSheet("font-size: 30px; color: grey;");
    }
    // 注意：这里我们不设置 m_brandImageLabel->setScaledContents(true);
    // 因为我们已经手动将 QPixmap 缩放到了合适的尺寸。

    // --- 3. 设置进度条 ---
    m_progressBar->setRange(0, m_progressSteps); // 进度范围0到100
    m_progressBar->setValue(0);                 // 初始值为0
    m_progressBar->setTextVisible(false);       // 不显示百分比文字
    m_progressBar->setFixedHeight(12);          // 设置一个合适的高度
    // (可选) 给进度条一些基本样式
    m_progressBar->setStyleSheet(R"(
        QProgressBar {
            border: 1px solid #A0A0A0;
            border-radius: 5px;
            background-color: #E0E0E0;
        }
        QProgressBar::chunk {
            background-color: #0078D4; /* 蓝色进度条 */
            border-radius: 4px;
        }
    )");

    // --- 4. 设置布局 ---
    QVBoxLayout *mainLayout = new QVBoxLayout(this); // 主垂直布局
    mainLayout->addWidget(m_brandImageLabel, 1, Qt::AlignCenter); // 图片占据主要空间并居中
    mainLayout->addSpacing(10); // 图片和进度条之间的间距
    mainLayout->addWidget(m_progressBar);
    mainLayout->setContentsMargins(20, 20, 20, 20); // 窗口的内边距
    setLayout(mainLayout);

    // --- 5. 连接定时器的timeout信号到updateProgress槽 ---
    connect(m_timer, &QTimer::timeout, this, &SplashScreen::updateProgress);
}

SplashScreen::~SplashScreen()
{
    qDebug() << "SplashScreen destroyed.";
}

void SplashScreen::showAndAnimate(int durationMilliseconds)
{
    m_animationDurationMs = durationMilliseconds;
    m_currentProgressStep = 0;
    m_progressBar->setValue(0);

    // 将启动画面窗口移动到屏幕中央
    if (const QScreen *screen = QGuiApplication::primaryScreen()) {
        QRect screenGeometry = screen->geometry();
        move((screenGeometry.width() - width()) / 2,
             (screenGeometry.height() - height()) / 2);
    }

    show(); // 显示启动画面

    // 计算定时器触发的间隔，以在durationMilliseconds内完成m_progressSteps步
    if (m_progressSteps > 0 && m_animationDurationMs > 0) {
        int interval = m_animationDurationMs / m_progressSteps;
        m_timer->start(interval > 0 ? interval : 1); // 确保间隔至少为1ms
    } else {
        // 如果步数或时长不合法，直接完成
        m_currentProgressStep = m_progressSteps;
        updateProgress();
    }
}

void SplashScreen::updateProgress()
{
    if (m_currentProgressStep < m_progressSteps) {
        m_currentProgressStep++;
        m_progressBar->setValue(m_currentProgressStep);
    }

    if (m_currentProgressStep >= m_progressSteps) {
        m_timer->stop();          // 停止定时器
        emit animationFinished(); // 发射动画完成信号

    }
}
