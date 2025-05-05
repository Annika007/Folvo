#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "DataModel.h"    // 包含DataModel头文件
#include "Transaction.h"  // 包含Transaction头文件

// 前置声明
class QTableView;
class QPushButton;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(DataModel* model, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 按钮点击槽函数
    void onAddTransaction();
    void onLoadData();
    void onSaveData();

private:
    // 初始化UI
    void initUI();
    // 更新数据显示
    void refreshTransactionsView();

    // 数据模型指针
    DataModel* m_model;

    // UI组件
    QTableView* m_transactionsView;
    QPushButton* m_addButton;
    QPushButton* m_loadButton;
    QPushButton* m_saveButton;
    QLabel* m_statusLabel;
};

#endif // MAINWINDOW_H
