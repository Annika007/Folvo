#include "transactionviewwindow.h"
#include "Transaction.h"      // 引用你的 Transaction.h (重要!)

#include <QVBoxLayout>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <algorithm>          // 用于 std::sort
#include <QDebug>

TransactionViewWindow::TransactionViewWindow(DataModel* model, QWidget *parent)
    : QWidget(parent,Qt::Window),
    m_dataModel(model),
    m_tableView(nullptr),
    m_itemModel(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("账单流水详情");
    setMinimumSize(700, 450);

    setupUI();
    loadAndDisplayData();
}

TransactionViewWindow::~TransactionViewWindow()
{
    qDebug() << "TransactionViewWindow 已销毁.";
}

void TransactionViewWindow::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    m_tableView = new QTableView(this);
    m_itemModel = new QStandardItemModel(0, 5, this); // 0行, 5列 (时间, 地点, 分类, 金额, 备注)

    m_itemModel->setHorizontalHeaderLabels({
        "日期时间",
        "地点/商家",
        "分类",
        "金额 (元)",
        "备注"
    });

    m_tableView->setModel(m_itemModel);

    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setSortingEnabled(true); // 允许点击表头排序

    mainLayout->addWidget(m_tableView);
    setLayout(mainLayout);
}

// 自定义比较器，用于按 QDateTime 排序 Transaction 对象
struct CompareTransactionsByTime {
    bool descending; // true = 新的在前, false = 旧的在前
    explicit CompareTransactionsByTime(bool desc = true) : descending(desc) {}

    bool operator()(const Transaction& a, const Transaction& b) const {
        if (descending) {
            return a.time > b.time; // 对于 QDateTime，'>' 表示时间更晚（新）
        } else {
            return a.time < b.time; // '<' 表示时间更早（旧）
        }
    }
};

void TransactionViewWindow::loadAndDisplayData()
{
    if (!m_dataModel || !m_itemModel) {
        qWarning() << "TransactionViewWindow::loadAndDisplayData - DataModel 或 ItemModel 为空!";
        return;
    }

    m_itemModel->removeRows(0, m_itemModel->rowCount()); // 清空现有数据

    QList<Transaction> transactions = m_dataModel->getTransactions();

    // 按时间降序排序 (最新的在最前面)
    std::sort(transactions.begin(), transactions.end(), CompareTransactionsByTime(true));

    for (const Transaction& t : transactions) {
        QList<QStandardItem*> rowItems;

        QStandardItem* timeItem = new QStandardItem(t.time.toString("yyyy-MM-dd hh:mm:ss"));
        QStandardItem* locationItem = new QStandardItem(t.location);
        QStandardItem* categoryItem = new QStandardItem(t.category);

        // *** 修改点：处理 float类型的 amount ***
        // QString::number 可以接受 float，第二个参数 'f' 表示浮点数格式，第三个参数 2 表示保留两位小数
        QStandardItem* amountItem = new QStandardItem(QString::number(t.amount, 'f', 2));
        amountItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // 金额右对齐

        QStandardItem* notesItem = new QStandardItem(t.notes);

        rowItems.append(timeItem);
        rowItems.append(locationItem);
        rowItems.append(categoryItem);
        rowItems.append(amountItem);
        rowItems.append(notesItem);

        m_itemModel->appendRow(rowItems);
    }

    m_tableView->resizeColumnsToContents(); // 自动调整列宽
    // (可选) 你可以为特定列设置更精确的宽度
    // m_tableView->setColumnWidth(0, 160); // 日期时间列
    // m_tableView->setColumnWidth(3, 80);  // 金额列
}
