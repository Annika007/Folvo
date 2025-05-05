#include "mainwindow.h"
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include<QWidget>

MainWindow::MainWindow(DataModel* model, QWidget *parent)
    : QMainWindow(parent), m_model(model)
{
    initUI();
    refreshTransactionsView();
}

MainWindow::~MainWindow()
{
    // 注意：不删除m_model，由外部管理
}

void MainWindow::initUI()
{
    // 创建中央部件
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // 交易列表视图
    m_transactionsView = new QTableView(this);
    mainLayout->addWidget(m_transactionsView);

    // 按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton("添加交易", this);
    m_loadButton = new QPushButton("加载数据", this);
    m_saveButton = new QPushButton("保存数据", this);

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_loadButton);
    buttonLayout->addWidget(m_saveButton);
    mainLayout->addLayout(buttonLayout);

    // 状态栏
    m_statusLabel = new QLabel("就绪", this);
    mainLayout->addWidget(m_statusLabel);

    setCentralWidget(centralWidget);

    // 连接信号槽
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddTransaction);
    connect(m_loadButton, &QPushButton::clicked, this, &MainWindow::onLoadData);
    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::onSaveData);
}

void MainWindow::refreshTransactionsView()
{
    // 实现数据刷新逻辑
    m_statusLabel->setText(QString("共 %1 条记录").arg(m_model->getTransactions().size()));
}

void MainWindow::onAddTransaction()
{
    // 实现添加交易逻辑
    m_model->addTransaction(0, "新交易");
    refreshTransactionsView();
}

void MainWindow::onLoadData()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开数据文件", "", "JSON文件 (*.json)");
    if (!fileName.isEmpty()) {
        if (m_model->loadFromJson(fileName)) {
            refreshTransactionsView();
            m_statusLabel->setText("数据加载成功");
        }
    }
}

void MainWindow::onSaveData()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存数据文件", "", "JSON文件 (*.json)");
    if (!fileName.isEmpty()) {
        if (m_model->saveToJson(fileName)) {
            m_statusLabel->setText("数据保存成功");
        }
    }
}
