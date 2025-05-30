#include "myprofilewindow.h"
#include "Transaction.h" // 需要 Transaction 类

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox> // 用于分组显示
#include <QSet>      // 用于计算独立天数
#include <QDate>
#include <algorithm> // 用于 std::sort
#include <QDebug>

MyProfileWindow::MyProfileWindow(DataModel* model, QWidget *parent)
    : QWidget(parent, Qt::Window), // 确保作为独立窗口显示
    m_dataModel(model),
    m_totalDaysLabel(nullptr),
    m_totalTransactionsLabel(nullptr),
    m_achievementTitleLabel(nullptr),
    m_consecutiveWeekLabel(nullptr),
    m_hundredTransactionsLabel(nullptr),
    m_consecutiveWeekStatusLabel(nullptr),
    m_hundredTransactionsStatusLabel(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("我的成就与统计");
    setMinimumSize(400, 300);
    QString profileStyleSheet = R"(
        /* MyProfileWindow 本身的背景色 */
        MyProfileWindow {
            background-color: #f0f8ff; /* AliceBlue，一个淡蓝色 */
        }

        /* 设置该窗口内所有 QLabel 的文字颜色为黑色 */
        QLabel {
            color: black;
        }

        /* 设置 QGroupBox 的样式 */
        QGroupBox {
            color: black; /* GroupBox 标题文字颜色 */
            font-weight: bold;
            border: 2px solid #00008B; /* !!! 设置边框：2像素宽，实线，深蓝色 (#00008B) !!! */
            border-radius: 5px;       /* 给边框添加圆角 */
            margin-top: 10px;         /* GroupBox 标题上方的边距，让标题和边框有空间 */
        }

        /* QGroupBox 标题的具体样式 (确保标题在边框内或边框上正确显示) */
        QGroupBox::title {
            subcontrol-origin: margin; /* 关键：使标题相对于 margin 定位 */
            subcontrol-position: top left;
            padding: 0 5px 0 5px;      /* 标题文字的左右内边距 */
            left: 10px;                /* 标题从左边框向右偏移一点 */
            background-color: #f0f8ff;  /*  给标题一个背景色，使其在边框上“镂空”出来，颜色与窗口背景一致 */
        }


    )";
    this->setStyleSheet(profileStyleSheet);

    setupUI();
    loadProfileData();
}

MyProfileWindow::~MyProfileWindow()
{
    qDebug() << "MyProfileWindow 已销毁.";
}

void MyProfileWindow::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // --- 统计信息组 ---
    QGroupBox* statsGroup = new QGroupBox("记账统计", this);
    QFormLayout* statsLayout = new QFormLayout(statsGroup); // 使用表单布局更整齐
    statsLayout->setLabelAlignment(Qt::AlignRight); // 标签右对齐

    m_totalDaysLabel = new QLabel("计算中...", statsGroup);
    m_totalTransactionsLabel = new QLabel("计算中...", statsGroup);

    QFont valueFont = m_totalDaysLabel->font();
    valueFont.setBold(true);
    valueFont.setPointSize(valueFont.pointSize() + 2);
    m_totalDaysLabel->setFont(valueFont);
    m_totalTransactionsLabel->setFont(valueFont);

    statsLayout->addRow("累计记账天数:", m_totalDaysLabel);
    statsLayout->addRow("累计记账笔数:", m_totalTransactionsLabel);
    mainLayout->addWidget(statsGroup);

    // --- 成就列表组 ---
    QGroupBox* achievementsGroup = new QGroupBox("我的成就", this);
    QGridLayout* achievementsLayout = new QGridLayout(achievementsGroup); // 使用网格布局

    m_achievementTitleLabel = new QLabel("成就状态", achievementsGroup); // 可以作为小标题
    QFont titleFont = m_achievementTitleLabel->font();
    titleFont.setBold(true);
    achievementsLayout->addWidget(m_achievementTitleLabel, 0, 0, 1, 2, Qt::AlignCenter); // 跨两列居中

    m_consecutiveWeekLabel = new QLabel("🏅 连续一周记账:", achievementsGroup);
    m_consecutiveWeekStatusLabel = new QLabel("检查中...", achievementsGroup);
    m_consecutiveWeekStatusLabel->setFont(valueFont);

    m_hundredTransactionsLabel = new QLabel("🏆 记账达到100笔:", achievementsGroup);
    m_hundredTransactionsStatusLabel = new QLabel("检查中...", achievementsGroup);
    m_hundredTransactionsStatusLabel->setFont(valueFont);

    achievementsLayout->addWidget(m_consecutiveWeekLabel, 1, 0);
    achievementsLayout->addWidget(m_consecutiveWeekStatusLabel, 1, 1);
    achievementsLayout->addWidget(m_hundredTransactionsLabel, 2, 0);
    achievementsLayout->addWidget(m_hundredTransactionsStatusLabel, 2, 1);

    mainLayout->addWidget(achievementsGroup);
    mainLayout->addStretch(); // 将内容推向上方
    setLayout(mainLayout);
}

void MyProfileWindow::loadProfileData()
{
    if (!m_dataModel) {
        qWarning() << "MyProfileWindow::loadProfileData - DataModel is null!";
        m_totalDaysLabel->setText("错误:无数据");
        m_totalTransactionsLabel->setText("错误:无数据");
        m_consecutiveWeekStatusLabel->setText("无法检查");
        m_hundredTransactionsStatusLabel->setText("无法检查");
        return;
    }

    // 1. 计算并显示记账天数
    int totalDays = calculateTotalActiveDays();
    m_totalDaysLabel->setText(QString::number(totalDays) + " 天");

    // 2. 计算并显示记账笔数
    int totalTransactions = calculateTotalTransactions();
    m_totalTransactionsLabel->setText(QString::number(totalTransactions) + " 笔");

    // 3. 检查并显示成就状态
    if (checkConsecutiveWeekAchievement()) {
        m_consecutiveWeekStatusLabel->setText("已解锁 🎉");
        m_consecutiveWeekStatusLabel->setStyleSheet("color: green;");
    } else {
        m_consecutiveWeekStatusLabel->setText("未解锁");
        m_consecutiveWeekStatusLabel->setStyleSheet("color: grey;");
    }

    if (checkHundredTransactionsAchievement()) {
        m_hundredTransactionsStatusLabel->setText("已解锁 🎉");
        m_hundredTransactionsStatusLabel->setStyleSheet("color: green;");
    } else {
        m_hundredTransactionsStatusLabel->setText("未解锁");
        m_hundredTransactionsStatusLabel->setStyleSheet("color: grey;");
    }
}

// --- 计算逻辑的辅助函数实现 ---

int MyProfileWindow::calculateTotalTransactions()
{
    return m_dataModel->getTransactions().size();
}

int MyProfileWindow::calculateTotalActiveDays()
{
    const QList<Transaction>& transactions = m_dataModel->getTransactions();
    if (transactions.isEmpty()) {
        return 0;
    }
    QSet<QDate> uniqueDates;
    for (const Transaction& t : transactions) {
        uniqueDates.insert(t.time.date()); // QDateTime::date() 返回 QDate
    }
    return uniqueDates.size();
}

bool MyProfileWindow::checkConsecutiveWeekAchievement()
{
    const QList<Transaction>& transactions = m_dataModel->getTransactions();
    if (transactions.size() < 7) { // 至少需要7条记录才可能连续7天
        return false;
    }

    QList<QDate> dates;
    for (const Transaction& t : transactions) {
        dates.append(t.time.date());
    }
    // 去重并排序日期
    std::sort(dates.begin(), dates.end());
    dates.erase(std::unique(dates.begin(), dates.end()), dates.end());

    if (dates.size() < 7) { // 去重后如果独立日期少于7天，也不可能
        return false;
    }

    for (int i = 0; i <= dates.size() - 7; ++i) {
        bool consecutive = true;
        for (int j = 0; j < 6; ++j) { // 检查从 dates[i] 开始的连续7天
            if (dates[i+j].addDays(1) != dates[i+j+1]) {
                consecutive = false;
                break;
            }
        }
        if (consecutive) {
            return true; // 找到连续7天
        }
    }
    return false;
}

bool MyProfileWindow::checkHundredTransactionsAchievement()
{
    return calculateTotalTransactions() >= 100;
}
