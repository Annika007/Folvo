#ifndef TRANSACTIONVIEWWINDOW_H
#define TRANSACTIONVIEWWINDOW_H

#include <QWidget>
#include "DataModel.h" // 引用你的数据模型

// 前置声明 Qt 类
class QTableView;
class QStandardItemModel;

class TransactionViewWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionViewWindow(DataModel* model, QWidget *parent = nullptr);
    ~TransactionViewWindow();

private:
    void setupUI();
    void loadAndDisplayData(); // 注意：这里可能需要调整以适应 Transaction 中 amount 为 float

    DataModel* m_dataModel;
    QTableView* m_tableView;
    QStandardItemModel* m_itemModel;
};

#endif // TRANSACTIONVIEWWINDOW_H
