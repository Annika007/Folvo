#include "mainwindow.h"
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include<QWidget>
#include <QFormLayout>
#include <QMessageBox>
#include<QDoubleValidator>
#include <QStringListModel>

MainWindow::MainWindow(DataModel* model, QWidget *parent)
    : QMainWindow(parent), m_model(model)
{
    initUI();
    refreshTransactionsView();
    loadHistoryLocations();
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


    // 初始化添加交易对话框
    m_addDialog = new QDialog(this);
    m_addDialog->setWindowTitle("添加新交易");
    m_addDialog->setModal(true);  // 设置为模态对话框
    m_addDialog->show();          // 测试时直接显示（正式代码应在按钮点击时显示）
    QFormLayout* formLayout = new QFormLayout(m_addDialog);

    // 金额输入（使用QDoubleSpinBox自动处理格式）
    m_amountEdit = new QDoubleSpinBox(m_addDialog);
    m_amountEdit->setRange(0, 999999);
    m_amountEdit->setDecimals(2);
    m_amountEdit->setPrefix("¥ ");
    m_amountEdit->setAlignment(Qt::AlignRight);

    // 地点输入（带自动补全）
    m_locationEdit = new QLineEdit(m_addDialog);
    m_locationCompleter = new QCompleter(m_locationHistory, m_addDialog);
    m_locationCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_locationEdit->setCompleter(m_locationCompleter);

    // 分类选择（可编辑）
    m_categoryCombo = new QComboBox(m_addDialog);
    m_categoryCombo->setEditable(true);
    m_categoryCombo->addItems({"餐饮", "交通", "购物", "娱乐", "住房", "医疗", "教育", "其他"});

    // 日期时间选择
    m_dateTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), m_addDialog);
    m_dateTimeEdit->setCalendarPopup(true);
    //备注
    m_notesEdit=new QLineEdit(m_addDialog);
    QPushButton* confirmButton = new QPushButton("确认添加", m_addDialog);

    formLayout->addRow("金额:", m_amountEdit);
    formLayout->addRow("地点:", m_locationEdit);
    formLayout->addRow("分类:", m_categoryCombo);
    formLayout->addRow("时间:", m_dateTimeEdit);
    formLayout->addRow("备注:", m_notesEdit);
    formLayout->addRow(confirmButton);
    connect(confirmButton, &QPushButton::clicked, this, &MainWindow::onConfirmAddTransaction);
}
void MainWindow::updateAmountFormat()
{
    double value = m_amountEdit->value(); // 直接获取浮点值
    m_amountEdit->setValue(value);        // 重新设置，自动格式化为指定精度
}

void MainWindow::loadHistoryLocations()
{
    // 从模型加载历史地点（简单实现）
    foreach (const Transaction& t, m_model->getTransactions()) {
        if (!m_locationHistory.contains(t.location)) {
            m_locationHistory.append(t.location);
        }
    }
    m_locationCompleter->setModel(new QStringListModel(m_locationHistory, m_addDialog));
}
void MainWindow::showValidationError(QWidget* widget, const QString& message)
{
    QMessageBox::warning(this, "输入错误", message);

    QPalette palette = widget->palette();
    palette.setColor(QPalette::Base, QColor(255, 200, 200));
    widget->setPalette(palette);
    widget->setFocus();
}
void MainWindow::refreshTransactionsView()
{
    // 1. 获取当前交易数据
    const QList<Transaction>& transactions = m_model->getTransactions();

    // 2. 更新表格视图（示例代码，需根据实际UI调整）
    if (m_transactionsView && m_transactionsView->model()) {
        m_transactionsView->model()->layoutChanged();
    }

    // 3. 更新状态栏
    m_statusLabel->setText(QString("共 %1 条交易记录").arg(transactions.size()));

    qDebug() << "视图已刷新，当前记录数:" << transactions.size();
}
void MainWindow::clearValidationError(QWidget* widget)
{
    widget->setPalette(style()->standardPalette()); // 恢复默认样式
}

void MainWindow::resetTransactionForm()
{
    m_amountEdit->setValue(0);
    m_locationEdit->clear();
    m_notesEdit->clear();
    m_dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    m_categoryCombo->setCurrentIndex(0);
}
void MainWindow::onConfirmAddTransaction()
{
    // 1. 验证金额
    if (m_amountEdit->value() <= 0) {
        showValidationError(m_amountEdit, "金额必须大于0");
        return;
    } else {
        clearValidationError(m_amountEdit);
    }

    // 2. 验证地点
    QString location = m_locationEdit->text().trimmed();
    if (location.isEmpty()) {
        showValidationError(m_locationEdit, "请输入交易地点");
        return;
    } else {
        clearValidationError(m_locationEdit);
    }

    // 3. 获取所有必要字段
    double amount = m_amountEdit->value();
    QString category = m_categoryCombo->currentText();  // 添加这行
    QDateTime time = m_dateTimeEdit->dateTime();
    QString notes = m_notesEdit->text();               // 添加这行

    // 4. 创建交易对象
    Transaction newTrans(amount, location, category, time, notes);
    // 4. 添加到模型
    m_model->addTransaction(newTrans);

    // 5. 更新历史地点（您的优秀实现保留）
    if (!m_locationHistory.contains(location)) {
        m_locationHistory.append(location);
        m_locationCompleter->setModel(new QStringListModel(m_locationHistory));
    }

    // 6. 清理表单
    resetTransactionForm();

    // 7. 更新界面
    refreshTransactionsView();
    m_statusLabel->setText(QString("成功添加: %1元 @ %2").arg(newTrans.amount).arg(newTrans.location));

    // 8. 可选成功提示（根据用户设置决定是否显示）
    if (m_showSuccessPrompt) {
        QMessageBox::information(this, "成功", "交易已添加");
    }
}
void MainWindow::onAddTransaction()
{
    // 1. 重置对话框输入状态
    m_amountEdit->setValue(0);
    m_locationEdit->clear();
    m_categoryCombo->setCurrentIndex(0);
    m_dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    m_notesEdit->clear();

    // 2. 显示对话框（模态方式）
    qDebug() << "显示交易录入对话框";
    m_addDialog->exec(); // 使用exec()确保模态阻塞

    // 注意：此处不需要直接添加数据或刷新视图
    // 实际数据添加应在 onConfirmAddTransaction() 中处理
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
