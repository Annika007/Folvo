#include "mainwindow.h" //
#include "DataModel.h"  //
#include "transactionviewwindow.h"
#include "myprofilewindow.h"
// Qt UI 相关的头文件
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QIcon>
#include <QApplication>
// 对话框和输入控件相关的头文件
#include <QDialog>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QCompleter>
#include <QStringListModel>
#include <QStatusBar>
// 其他 Qt 工具类
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

// #include "settingsdialog.h"
MainWindow::MainWindow(DataModel* model, QWidget *parent)
    : QMainWindow(parent), // 基类是 QMainWindow
    m_model(model),
    m_centralWidget(nullptr),
    m_mainGridLayout(nullptr),
    m_mainImageLabel(nullptr),
    m_plusButton(nullptr),
    m_bottomBar_ViewTransactionsButton(nullptr),
    m_bottomBar_ReportsButton(nullptr),
    m_bottomBar_LoadDataButton(nullptr),
    m_rightBar_SaveDataButton(nullptr),
    m_rightBar_ProfileButton(nullptr),
    m_rightBar_SettingsButton(nullptr),
    m_addDialog(nullptr),
    m_amountEdit(nullptr),
    m_locationEdit(nullptr),
    m_categoryCombo(nullptr),
    m_dateTimeEdit(nullptr),
    m_notesEdit(nullptr),
    m_locationCompleter(nullptr),
    m_statusLabel_main(nullptr)
{
    initNewUI();
    initAddTransactionDialog(); // 初始化添加交易对话框
    loadHistoryLocations();     // 加载历史地点
    refreshMainStatus(); // 初始化时也刷新一下状态

    setWindowTitle("Folvo 记账本"); // 设置窗口标题
    resize(1024, 768);                       // 设置一个初始窗口大小

}
MainWindow::~MainWindow()
{

}
// mainwindow.cpp
void MainWindow::initNewUI() {
    m_centralWidget = new QWidget(this);
    m_mainGridLayout = new QGridLayout(m_centralWidget);
    // 保持网格单元间的一些间距，或者设为0，主要通过布局内部和QSS控制
    m_mainGridLayout->setSpacing(5);
    m_mainGridLayout->setContentsMargins(0, 0, 0, 0); // 整体外边距
    // --- 1. 主体图片 Label (cat.png) ---
    m_mainImageLabel = new QLabel(m_centralWidget);
    QPixmap catPixmap(":/cat.png"); // *** 确认路径: 如果cat.png在根目录下就是这个，如果在/images/下就是":/images/cat.png" ***
    if (catPixmap.isNull()) {
        qDebug() << "主图片 ':/cat.png' 加载失败! 请仔细检查qrc文件中的路径和实际文件名。";
        m_mainImageLabel->setText("Folvo");
        m_mainImageLabel->setAlignment(Qt::AlignCenter);
        m_mainImageLabel->setStyleSheet("font-size: 48px; color: grey; border: 1px solid lightgrey; background-color: #f0f0f0;");
        m_mainImageLabel->setMinimumSize(200, 150); // 至少给个大小
    } else {
        m_mainImageLabel->setPixmap(catPixmap);
    }
    m_mainImageLabel->setScaledContents(true);
    // 设置尺寸策略，使其可以扩展但也有最小限制
    QSizePolicy imagePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 让图片倾向于扩展
    m_mainImageLabel->setSizePolicy(imagePolicy);
    // m_mainImageLabel->setMinimumSize(300, 200); // 设置一个最小尺寸防止图片被过度压缩
    m_mainGridLayout->addWidget(m_mainImageLabel, 0, 0); // (行0, 列0)

    // --- 定义统一的方形按钮尺寸 ---
    const int squareButtonSize = 70;

    // --- 2. 底部按钮栏 ---
    QHBoxLayout* bottomBarLayout = new QHBoxLayout();
    bottomBarLayout->setSpacing(0); // ***按钮紧靠：将间距设为0***
    bottomBarLayout->setContentsMargins(0, 5, 0, 0); // 底部栏自身的边距

    // 创建并设置底部按钮 (示例)
    m_bottomBar_ViewTransactionsButton = new QPushButton(QIcon(":/icons/list.png"),"查看账单", m_centralWidget); // 假设有图标，文字可以省略或通过QSS添加
    m_bottomBar_ViewTransactionsButton->setFixedSize(squareButtonSize, squareButtonSize);
    m_bottomBar_ViewTransactionsButton->setObjectName("viewTransactionsButton");
    m_bottomBar_ViewTransactionsButton->setToolTip("查看账单");

    m_bottomBar_ReportsButton = new QPushButton(QIcon(":/icons/reports.png"), "统计报表", m_centralWidget);
    m_bottomBar_ReportsButton->setFixedSize(squareButtonSize, squareButtonSize);
    m_bottomBar_ReportsButton->setObjectName("reportsButton");
    m_bottomBar_ReportsButton->setToolTip("统计报表");

    // 新增 "存钱计划" 按钮
    m_bottomBar_SavingsPlanButton = new QPushButton(QIcon(":/icons/savings.png"), "存钱计划", m_centralWidget); // ***你需要有savings.png图标***
    m_bottomBar_SavingsPlanButton->setFixedSize(squareButtonSize, squareButtonSize);
    m_bottomBar_SavingsPlanButton->setObjectName("savingsPlanButton");
    m_bottomBar_SavingsPlanButton->setToolTip("存钱计划");

    // 新增 "AI分析" 按钮
    m_bottomBar_AiAnalysisButton = new QPushButton(QIcon(":/icons/ai.png"), "AI分析", m_centralWidget); // ***你需要有ai.png图标***
    m_bottomBar_AiAnalysisButton->setFixedSize(squareButtonSize, squareButtonSize);
    m_bottomBar_AiAnalysisButton->setObjectName("aiAnalysisButton");
    m_bottomBar_AiAnalysisButton->setToolTip("AI 分析");

    m_bottomBar_LoadDataButton = new QPushButton(QIcon(":/icons/load.png"), "加载数据", m_centralWidget);
    m_bottomBar_LoadDataButton->setFixedSize(squareButtonSize, squareButtonSize);
    m_bottomBar_LoadDataButton->setObjectName("loadDataButton");
    m_bottomBar_LoadDataButton->setToolTip("加载数据");


    // 将按钮添加到布局
    bottomBarLayout->addWidget(m_bottomBar_ViewTransactionsButton,1);
    bottomBarLayout->addWidget(m_bottomBar_ReportsButton,1);
    bottomBarLayout->addWidget(m_bottomBar_SavingsPlanButton,1); // 添加新按钮
    bottomBarLayout->addWidget(m_bottomBar_AiAnalysisButton,1);  // 添加新按钮
    bottomBarLayout->addWidget(m_bottomBar_LoadDataButton,1);
    m_mainGridLayout->addLayout(bottomBarLayout, 1, 0); // (行1, 列0)

    // --- 3. 右侧按钮栏 ---
    QVBoxLayout* rightBarLayout = new QVBoxLayout();
    rightBarLayout->setSpacing(0); // ***按钮紧靠：将间距设为0***
    rightBarLayout->setContentsMargins(0, 0, 0, 0); // 右侧栏自身的边距

    m_rightBar_ProfileButton = new QPushButton(QIcon(":/icons/profile.png"), "我的", m_centralWidget);
    m_rightBar_ProfileButton->setFixedSize(squareButtonSize, squareButtonSize);
    m_rightBar_ProfileButton->setObjectName("profileButton");
    m_rightBar_ProfileButton->setToolTip("我的");

    m_rightBar_SettingsButton = new QPushButton(QIcon(":/icons/settings.png"), "设置", m_centralWidget);
    m_rightBar_SettingsButton->setFixedSize(squareButtonSize, squareButtonSize);
    m_rightBar_SettingsButton->setObjectName("settingsButton");
    m_rightBar_SettingsButton->setToolTip("设置");

    m_rightBar_SaveDataButton = new QPushButton("保存数据", m_centralWidget);
    m_rightBar_SaveDataButton->setFixedSize(squareButtonSize, squareButtonSize);
    m_rightBar_SaveDataButton->setObjectName("rightButtonSave"); // 给一个新的 objectName
    m_rightBar_SaveDataButton->setToolTip("将当前账目数据保存到文件");

    rightBarLayout->addWidget(m_rightBar_ProfileButton);
    rightBarLayout->addWidget(m_rightBar_SettingsButton);
    rightBarLayout->addWidget(m_rightBar_SaveDataButton, 1);

    m_mainGridLayout->addLayout(rightBarLayout, 0, 1); // (行0, 列1)

    // --- 4. "+" 添加按钮 ---
    m_plusButton = new QPushButton(QIcon(":/plus.png"), "", m_centralWidget); // ***替换为你的+号图标路径***
    m_plusButton->setFixedSize(squareButtonSize + 10, squareButtonSize + 10); // "+" 按钮可以稍微大一点
    m_plusButton->setIconSize(QSize(squareButtonSize - 15, squareButtonSize - 15)); // 图标大小
    m_plusButton->setObjectName("plusButton");
    m_plusButton->setToolTip("添加新账目");
    m_mainGridLayout->addWidget(m_plusButton, 1, 1, Qt::AlignCenter); // (行1, 列1), 尝试居中

    // --- 5. 调整行和列的伸展因子以缩小图片占比 ---
    // 通过调整伸展因子比例来控制区域大小。值越大，占据的空间越多。
    // 尝试不同的比例，例如让图片区域的伸展因子是按钮栏区域的2倍或3倍。
    m_mainGridLayout->setRowStretch(0, 2);    // 图片所在行，伸展因子设为 2 (相对较大)
    m_mainGridLayout->setRowStretch(1, 0);    // 底部按钮栏行，伸展因子设为 0 (高度固定或由内容决定)
    // m_mainGridLayout->setRowMinimumHeight(1, squareButtonSize + 10); // 确保底部行有足够高度

    m_mainGridLayout->setColumnStretch(0, 2); // 图片所在列，伸展因子设为 2 (相对较大)
    m_mainGridLayout->setColumnStretch(1, 0); // 右侧按钮栏列，伸展因子设为 0 (宽度固定或由内容决定)

    setCentralWidget(m_centralWidget); // !!! 将 m_centralWidget 设置为 QMainWindow 的中心部件

    // --- 6. 连接信号槽 (为新按钮添加连接) ---
    connect(m_plusButton, &QPushButton::clicked, this, &MainWindow::onPlusButtonClicked);
    connect(m_bottomBar_LoadDataButton, &QPushButton::clicked, this, &MainWindow::onBottomBar_LoadDataClicked);
   // connect(m_bottomBar_SaveDataButton, &QPushButton::clicked, this, &MainWindow::onBottomBar_SaveDataClicked);
    connect(m_bottomBar_ViewTransactionsButton, &QPushButton::clicked, this, &MainWindow::onBottomBar_ViewTransactionsClicked);
    connect(m_bottomBar_ReportsButton, &QPushButton::clicked, this, &MainWindow::onBottomBar_ReportsClicked);
    connect(m_bottomBar_SavingsPlanButton, &QPushButton::clicked, this, &MainWindow::onBottomBar_SavingsPlanClicked); // ***连接新按钮***
    connect(m_bottomBar_AiAnalysisButton, &QPushButton::clicked, this, &MainWindow::onBottomBar_AiAnalysisClicked);    // ***连接新按钮***
    connect(m_rightBar_SettingsButton, &QPushButton::clicked, this, &MainWindow::onRightBar_SettingsClicked);
    connect(m_rightBar_ProfileButton, &QPushButton::clicked, this, &MainWindow::onRightBar_ProfileClicked);
    connect(m_rightBar_SaveDataButton, &QPushButton::clicked, this, &MainWindow::onRightBar_SaveDataClicked);

    // --- 7. 状态栏 ---
    m_statusLabel_main = new QLabel("Folvo 已准备就绪");
    statusBar()->addWidget(m_statusLabel_main, 1); // 1表示可以伸展
    statusBar()->setStyleSheet("background-color: #e9ecef; color: #495057;"); // 给状态栏一点样式


    QString appStyleSheet = R"(
    QMainWindow {
        background-color: #f8f9fa;
    }
    QLabel#m_mainImageLabel { /* 如果需要给图片标签特定样式，例如边框 */
        /* border: 1px solid #eeeeee; */
    }
    /* 通用按钮样式，会被特定ID的样式覆盖 */
    QPushButton {
        color: white;           /* 文字颜色，如果图标是彩色的，文字可以省略 */
        border: none;           /* 无边框 */
        padding: 0px;           /* 内边距为0，让图标填满 */
        /* border-radius: 8px; */ /* 方形按钮通常不需要或很小的圆角，如果想要完全方形，设为0或不设置 */
        font-size: 10px;        /* 如果按钮内有文字，设置字体大小 */
        text-align: center;
        /* 如果按钮主要是图标，确保图标清晰 */
        /* icon-size: 48px 48px; */ /* 根据按钮大小调整图标大小，也可以在创建QIcon时指定 */
    }
    QPushButton:hover {
        opacity: 0.85; /* 悬停时轻微变暗或变亮 */
    }
    QPushButton:pressed {
        opacity: 0.7;
    }

    /* 为不同按钮设置不同背景颜色 */
    QPushButton#viewTransactionsButton { background-color: #17a2b8; } /* Info Blue */
    QPushButton#reportsButton { background-color: #ffc107; }          /* Warning Yellow */
    QPushButton#savingsPlanButton { background-color: #28a745; }      /* Success Green */
    QPushButton#aiAnalysisButton { background-color: #6f42c1; }       /* Indigo */
    QPushButton#loadDataButton { background-color: #fd7e14; }         /* Orange */
    QPushButton#rightButtonSave { background-color: #dc3545; }
    QPushButton#profileButton { background-color: #6610f2; }        /* Purple */
    QPushButton#settingsButton { background-color: #343a40; }       /* Dark Grey */

    QPushButton#plusButton {
        background-color: #f8f9fa;
        border-radius: 35px;       /* 使其看起来是圆形 */
        border: 3px solid white;  /* 加个白色边框使其更醒目 */
    }
    QDoubleSpinBox#amountSpinBox {
        background-color: white; /* 确保背景是白色或其他浅色 */
        color: black;            /* !!! 确保文字是黑色或其他深色 !!! */
        border: 1px solid #ced4da; /*  添加一个边框 */
        border-radius: 4px;       /*  轻微圆角 */
        padding: 2px 5px;         /*  内边距 */
        font-size: 13px;          /*  字体大小 */
    }
    QPushButton#plusButton:hover { background-color: #005cbf; }
    QPushButton#plusButton:pressed { background-color: #0052b3; }

    QStatusBar QLabel { color: #212529; padding-left: 5px; }
)";

    this->setStyleSheet(appStyleSheet); // 应用到整个 MainWindow
}
// --- 实现新增按钮的槽函数 ---
void MainWindow::onBottomBar_SavingsPlanClicked() {
    QMessageBox::information(this, "存钱计划", "存钱计划功能模块正在快马加鞭开发中！");
}
void MainWindow::onBottomBar_AiAnalysisClicked() {
    QMessageBox::information(this, "AI 分析", "智能 AI 分析功能模块即将上线，敬请期待！");
}
void MainWindow::initAddTransactionDialog()
{
    m_addDialog = new QDialog(this);
    m_addDialog->setWindowTitle("添加一笔新账目");
    m_addDialog->setModal(true);
    QFormLayout* formLayout = new QFormLayout(m_addDialog);
    m_amountEdit = new QDoubleSpinBox(m_addDialog);
    m_amountEdit->setRange(0.01, 9999999.99);
    m_amountEdit->setObjectName("amountSpinBox");      //输入黑体
    m_amountEdit->setDecimals(2);
    m_amountEdit->setPrefix("¥ ");
    m_amountEdit->setAlignment(Qt::AlignRight);
    m_amountEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);

    m_locationEdit = new QLineEdit(m_addDialog);
    m_locationCompleter = new QCompleter(m_locationHistory, m_addDialog);
    m_locationCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_locationEdit->setCompleter(m_locationCompleter);

    m_categoryCombo = new QComboBox(m_addDialog);
    m_categoryCombo->setEditable(true);
    m_categoryCombo->addItems({"餐饮", "交通", "购物", "娱乐", "住房", "医疗", "教育", "其他"});
    m_categoryCombo->insertItem(0, "请选择或输入分类");
    m_categoryCombo->setCurrentIndex(0);

    m_dateTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), m_addDialog);
    m_dateTimeEdit->setCalendarPopup(true);
    m_dateTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");


    m_notesEdit = new QLineEdit(m_addDialog);
    m_notesEdit->setPlaceholderText("选填，记录更多细节");

    QPushButton* confirmButton = new QPushButton("确认添加", m_addDialog);
    QPushButton* cancelButton = new QPushButton("取消", m_addDialog);

    QHBoxLayout* dialogButtonLayout = new QHBoxLayout(); // 用于对话框底部的按钮
    dialogButtonLayout->addStretch(); // 将按钮推到右边
    dialogButtonLayout->addWidget(cancelButton);
    dialogButtonLayout->addWidget(confirmButton);

    formLayout->addRow("金  额:", m_amountEdit);
    formLayout->addRow("地  点:", m_locationEdit);
    formLayout->addRow("分  类:", m_categoryCombo);
    formLayout->addRow("时  间:", m_dateTimeEdit);
    formLayout->addRow("备  注:", m_notesEdit);
    formLayout->addRow(dialogButtonLayout); // 添加按钮行

    connect(confirmButton, &QPushButton::clicked, this, &MainWindow::onConfirmAddTransaction);
    connect(cancelButton, &QPushButton::clicked, m_addDialog, &QDialog::reject); // 点击取消直接关闭对话框
    connect(m_amountEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::updateAmountFormat);

}
// --- 槽函数实现 ---
void MainWindow::onPlusButtonClicked() {
    if (!m_addDialog) {
        qWarning() << "MainWindow::onPlusButtonClicked: 添加交易对话框未初始化!";
        // 可以尝试重新初始化，或者给用户错误提示
        QMessageBox::critical(this, "错误", "无法打开账目添加窗口，请联系开发者。");
        return;
    }
    resetTransactionForm(); // 确保每次打开都是干净的表单
    m_addDialog->exec();    // 以模态方式显示对话框
}
void MainWindow::onBottomBar_LoadDataClicked() {
    // (你之前的 onLoadData 代码)
    QString fileName = QFileDialog::getOpenFileName(this, "打开数据文件", QDir::homePath(), "JSON文件 (*.json)");
    if (!fileName.isEmpty()) {
        if (m_model && m_model->loadFromJson(fileName)) {
            loadHistoryLocations(); // 加载数据后更新自动补全
            refreshMainStatus();    // 更新主窗口状态
            statusBar()->showMessage("数据从 " + QFileInfo(fileName).fileName() + " 加载成功!", 3000);
        } else {
            statusBar()->showMessage("数据加载失败!", 3000);
            QMessageBox::critical(this, "加载失败", "无法从文件加载数据。");
        }
    }
}
void MainWindow::onRightBar_SaveDataClicked() {
    // (你之前的 onSaveData 代码)
    QString defaultFileName = QDir::homePath() + "/Folvo_Data_" + QDate::currentDate().toString("yyyyMMdd") + ".json";
    QString fileName = QFileDialog::getSaveFileName(this, "保存数据文件", defaultFileName, "JSON文件 (*.json)");
    if (!fileName.isEmpty()) {
        if (m_model && m_model->saveToJson(fileName)) {
            statusBar()->showMessage("数据已保存到 " + QFileInfo(fileName).fileName(), 3000);
        } else {
            statusBar()->showMessage("数据保存失败!", 3000);
            QMessageBox::critical(this, "保存失败", "无法将数据保存到文件。");
        }
    }
}
// --- 次级窗口的槽函数 (暂时用提示信息替代) ---
void MainWindow::onBottomBar_ViewTransactionsClicked() {
    TransactionViewWindow *viewWindow = new TransactionViewWindow(m_model, this); // 父窗口设为 this

    viewWindow->show(); // 非模态显示
}
void MainWindow::onBottomBar_ReportsClicked() {
    QMessageBox::information(this, "功能提示", "统计报表功能正在开发中！\n将在此处打开报表分析窗口。");
}
void MainWindow::onRightBar_SettingsClicked() {
    QMessageBox::information(this, "功能提示", "应用设置功能正在开发中！\n将在此处打开设置对话框。");
}
void MainWindow::onRightBar_ProfileClicked() {
    MyProfileWindow *profileWin = new MyProfileWindow(m_model, this);
    profileWin->show();
}
void MainWindow::onConfirmAddTransaction()
{
    if (!m_model || !m_addDialog || !m_amountEdit || !m_locationEdit || !m_categoryCombo || !m_dateTimeEdit || !m_notesEdit) {
        QMessageBox::critical(m_addDialog, "内部错误", "表单控件未初始化。"); // 父窗口是对话框
        return;
    }
    // 1. 验证金额
    if (m_amountEdit->value() < 0.01) {
        showValidationError(m_amountEdit, "金额必须大于等于0.01元");
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

    // 3. 验证分类 (确保不是提示项)
    QString category = m_categoryCombo->currentText();
    if (m_categoryCombo->currentIndex() == 0 || category.isEmpty() || category == "请选择或输入分类") {
        showValidationError(m_categoryCombo, "请选择或输入一个有效的分类");
        return;
    } else {
        clearValidationError(m_categoryCombo);
    }

    // 4. 获取数据
    double amount = m_amountEdit->value();
    QDateTime time = m_dateTimeEdit->dateTime();
    QString notes = m_notesEdit->text().trimmed();

    // 5. 添加到模型
    Transaction newTrans(amount, location, category, time, notes);
    m_model->addTransaction(newTrans);

    // 6. 更新历史地点
    if (!m_locationHistory.contains(location)) {
        m_locationHistory.append(location);
        if (m_locationCompleter->model()) delete m_locationCompleter->model();
        m_locationCompleter->setModel(new QStringListModel(m_locationHistory, m_locationCompleter));
    }

    // 7. 更新主窗口状态
    refreshMainStatus();
    statusBar()->showMessage(QString("账目已添加: %1元 @ %2").arg(newTrans.amount, 0, 'f', 2).arg(newTrans.location), 3000);

    // 8. 成功提示 (可选)
    if (m_showSuccessPrompt) {
        QMessageBox::information(m_addDialog, "添加成功", "新账目已成功记录！"); // 父窗口是对话框
    }

    m_addDialog->accept(); // 关闭对话框
}

void MainWindow::updateAmountFormat() {
    // QDoubleSpinBox 通常会自动处理，此函数可能不需要特别操作
    if(m_amountEdit) m_amountEdit->setValue(m_amountEdit->value());
}
void MainWindow::loadHistoryLocations() {
    if (!m_model || !m_locationCompleter || !m_addDialog) return;
    m_locationHistory.clear();
    const QList<Transaction>& transactions = m_model->getTransactions();
    for (const Transaction& t : transactions) {
        if (!t.location.isEmpty() && !m_locationHistory.contains(t.location)) {
            m_locationHistory.append(t.location);
        }
    }
    if (m_locationCompleter->model()) delete m_locationCompleter->model();
    m_locationCompleter->setModel(new QStringListModel(m_locationHistory, m_locationCompleter));
}
void MainWindow::showValidationError(QWidget* widget, const QString& message) {
    if (!widget) return;
    QMessageBox::warning(m_addDialog, "输入不正确", message); // 父窗口是对话框
    QPalette palette = widget->palette();
    palette.setColor(QPalette::Base, QColor(255, 220, 220));
    widget->setPalette(palette);
    widget->setFocus();
}
void MainWindow::clearValidationError(QWidget* widget) {
    if (!widget) return;
    widget->setPalette(QApplication::style()->standardPalette());
}
void MainWindow::resetTransactionForm() {
    if (!m_amountEdit || !m_locationEdit || !m_categoryCombo || !m_dateTimeEdit || !m_notesEdit) return;
    m_amountEdit->setValue(0.00);
    m_locationEdit->clear();
    m_categoryCombo->setCurrentIndex(0);
    m_dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    m_notesEdit->clear();
    clearValidationError(m_amountEdit);
    clearValidationError(m_locationEdit);
    clearValidationError(m_categoryCombo);
    m_amountEdit->setFocus(); // 焦点通常给金额输入框
}
void MainWindow::refreshMainStatus() {
    if (m_model && statusBar()) {
        int count = m_model->getTransactions().size();
        QString statusMsg = QString("当前共有 %1 条账目").arg(count);
        if (m_statusLabel_main) { // 如果自定义了状态栏标签
            m_statusLabel_main->setText(statusMsg);
        } else {
            statusBar()->showMessage(statusMsg, 0); // 0 表示永久显示，直到被新消息覆盖
        }
    }
}
