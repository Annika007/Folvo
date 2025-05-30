#ifndef MYPROFILEWINDOW_H
#define MYPROFILEWINDOW_H

#include <QWidget>
#include "DataModel.h" // 需要访问数据模型

class QLabel; // 前置声明
class QVBoxLayout;
class QGridLayout; // 用于更灵活的布局成就

class MyProfileWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MyProfileWindow(DataModel* model, QWidget *parent = nullptr);
    ~MyProfileWindow();

private:
    void setupUI();
    void loadProfileData(); // 加载并计算展示的数据

    // UI 元素
    QLabel* m_totalDaysLabel;
    QLabel* m_totalTransactionsLabel;

    // 成就相关UI元素 (示例)
    QLabel* m_achievementTitleLabel;
    QLabel* m_consecutiveWeekLabel;
    QLabel* m_hundredTransactionsLabel;
    QLabel* m_consecutiveWeekStatusLabel;   // 显示 "已解锁" 或 "未解锁"
    QLabel* m_hundredTransactionsStatusLabel;

    DataModel* m_dataModel;

    // 计算逻辑的辅助函数
    int calculateTotalActiveDays();
    int calculateTotalTransactions();
    bool checkConsecutiveWeekAchievement();
    bool checkHundredTransactionsAchievement();
};

#endif // MYPROFILEWINDOW_H
