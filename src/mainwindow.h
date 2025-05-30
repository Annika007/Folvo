#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow> // 保留 QMainWindow 作为基类
#include <QList>
#include "DataModel.h"
#include "Transaction.h"
#include <QDialog>
#include "transactionviewwindow.h"
#include <QComboBox>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QCompleter>
#include <QDoubleSpinBox>

// 前置声明
class QTableView;     // 如果次级窗口使用
class QPushButton;
class QLabel;
class QGridLayout;    // 将会用到
class QHBoxLayout;    // 将会用到
class QVBoxLayout;    // 将会用到

class MainWindow : public QMainWindow // 继续使用 QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(DataModel* model, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // --- 新的或调整的槽函数 ---
    void onPlusButtonClicked();         // "+" 按钮的槽函数
    void onConfirmAddTransaction();     // 这个保留，由添加对话框触发
    void updateAmountFormat();          // 这个保留

    // 底部按钮栏示例槽函数 (可以复用旧的逻辑)
    void onBottomBar_LoadDataClicked(); // 对应原来的 onLoadData
    void onBottomBar_ViewTransactionsClicked(); // 新增：用于打开交易列表窗口
    void onBottomBar_ReportsClicked();      // 新增：用于打开报表窗口
    void onBottomBar_SavingsPlanClicked(); // 新增：存钱计划按钮槽
    void onBottomBar_AiAnalysisClicked();  // 新增：AI分析按钮槽

    // 右侧按钮栏示例槽函数
    void onRightBar_SettingsClicked();
    void onRightBar_ProfileClicked();
    void onRightBar_SaveDataClicked();







private:
    void initNewUI();                   // 新的UI初始化方法
    void initAddTransactionDialog();    // 提取对话框初始化
    void refreshMainStatus();           // 用于更新主窗口可能有的状态显示
    void loadHistoryLocations();
    bool m_showSuccessPrompt = true;

    void showValidationError(QWidget* widget, const QString& message);
    void clearValidationError(QWidget* widget);
    void resetTransactionForm();

    DataModel* m_model;

    // --- 新的 UI 组件 for Main Window Layout ---
    QWidget* m_centralWidget;       // QMainWindow 的中心部件，我们的网格布局将应用在此
    QGridLayout* m_mainGridLayout;  // 主网格布局

    QLabel* m_mainImageLabel;       // 主体图片
    QPushButton* m_plusButton;      // "+" 添加按钮

    // 底部按钮栏
    QPushButton* m_bottomBar_LoadDataButton;
    QPushButton* m_bottomBar_ViewTransactionsButton;
    QPushButton* m_bottomBar_ReportsButton;
    QPushButton* m_bottomBar_SavingsPlanButton;
    QPushButton* m_bottomBar_AiAnalysisButton;
    // ... 更多底部按钮 ...

    // 右侧按钮栏
    QPushButton* m_rightBar_SettingsButton;
    QPushButton* m_rightBar_ProfileButton;
    QPushButton* m_rightBar_BudgetsButton;
    QPushButton* m_rightBar_SaveDataButton;
    // ... 更多右侧按钮 ...



    QLabel* m_statusLabel_main;         // 主窗口的状态标签 (可以放在状态栏或自定义位置)

    // 添加交易对话框相关
    QDialog* m_addDialog;
    QDoubleSpinBox* m_amountEdit;
    QLineEdit* m_locationEdit;
    QComboBox* m_categoryCombo;
    QDateTimeEdit* m_dateTimeEdit;
    QLineEdit* m_notesEdit;
    QCompleter* m_locationCompleter;
    QStringList m_locationHistory;
};

#endif // MAINWINDOW_H
