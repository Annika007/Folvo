#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "DataModel.h"    // 包含DataModel头文件
#include "Transaction.h"  // 包含Transaction头文件
#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QCompleter>
#include <QDoubleSpinBox>


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
    void onConfirmAddTransaction();
    void updateAmountFormat();

private:
    // 初始化UI
    void initUI();
    // 更新数据显示
    void refreshTransactionsView();
    void loadHistoryLocations();
    bool m_showSuccessPrompt = true; // 可配置是否显示提示

    void showValidationError(QWidget* widget, const QString& message);
    void clearValidationError(QWidget* widget);
    void resetTransactionForm();
    // 数据模型指针
    DataModel* m_model;

    // UI组件
    QTableView* m_transactionsView;
    QPushButton* m_addButton;
    QPushButton* m_loadButton;
    QPushButton* m_saveButton;
    QLabel* m_statusLabel;

    // 添加交易对话框相关
    QDialog* m_addDialog;
    QDoubleSpinBox* m_amountEdit;  // 改用SpinBox更好控制格式
    QLineEdit* m_locationEdit;
    QComboBox* m_categoryCombo;
    QDateTimeEdit* m_dateTimeEdit;
    QLineEdit* m_notesEdit;
    QCompleter* m_locationCompleter;
    QStringList m_locationHistory;
};

#endif // MAINWINDOW_H
