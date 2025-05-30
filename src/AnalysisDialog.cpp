#include "AnalysisDialog.h"
#include "AnalysisDialog.h" // 由Qt生成的UI头文件
#include "FinancialAnalyzer.h"
#include "AIService.h"
#include <QMessageBox>
#include <QThread>
#include <QProgressDialog>
#include<QTimer>
AnalysisDialog::AnalysisDialog(DataModel* model, QWidget* parent)
    : QDialog(parent),
    m_model(model),
    m_analyzer(new FinancialAnalyzer(m_model->getTransactions(), this)),
    m_aiService(new AIService("sk-8388e833ee4c4ef5a4fb7855da513885", this)) // 替换为你的DeepSeek API密钥
{
    // 设置窗口属性
    setWindowTitle("AI财务分析");
    setFixedSize(500, 400);

    // 主布局
    QVBoxLayout* layout = new QVBoxLayout(this);

    // 结果显示区域
    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setPlaceholderText("正在生成分析报告...");
    m_textEdit->setStyleSheet(
        "QTextEdit {"
        "  border: 1px solid #ddd;"
        "  border-radius: 5px;"
        "  padding: 10px;"
        "  font-size: 14px;"
        "}"
        );
    layout->addWidget(m_textEdit);

    // 关闭按钮
    m_closeButton = new QPushButton("关闭", this);
    m_closeButton->setEnabled(false);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(m_closeButton, 0, Qt::AlignRight);

    // 连接信号槽
    connect(m_aiService, &AIService::analysisComplete, this, &AnalysisDialog::onAnalysisComplete);
    connect(m_aiService, &AIService::errorOccurred, this, &AnalysisDialog::onAnalysisError);

    // 延迟启动分析（确保UI渲染完成）
    QTimer::singleShot(100, this, &AnalysisDialog::startAnalysis);

}

AnalysisDialog::~AnalysisDialog()
{
    // 自动释放子对象
}


void AnalysisDialog::startAnalysis() {
    // 显示加载状态
    m_textEdit->setPlainText("正在准备分析数据...");
    m_closeButton->setEnabled(false);

    // 创建进度对话框
    QProgressDialog* progress = new QProgressDialog("连接AI分析服务...", "取消", 0, 0, this);
    progress->setWindowModality(Qt::WindowModal);
    connect(progress, &QProgressDialog::canceled, this, [this]() {
        m_aiService->abortRequest();
        reject();
    });

    // 生成提示词
    QVariantMap summary = m_analyzer->generateSummary();
    QString prompt = QString(
                         "请分析以下财务数据（用中文回答）：\n"
                         "收入：%1元 | 支出：%2元 | 主要消费：%3\n\n"
                         "请提供：\n"
                         "1. 消费行为总结\n"
                         "2. 三条优化建议\n"
                         "3. 六个月的存钱计划"
                         ).arg(summary["total_income"].toDouble(), 0, 'f', 2)
                         .arg(summary["total_expense"].toDouble(), 0, 'f', 2)
                         .arg(summary["top_category"].toString());

    // 调用AI服务
    m_aiService->requestAdvice(prompt);

    // 30秒后检查是否超时
    QTimer::singleShot(30000, this, [this, progress]() {
        if (!m_closeButton->isEnabled()) {
            progress->setLabelText("请求时间较长，请稍候...");
        }
    });
}

void AnalysisDialog::onAnalysisComplete(const QString& result)
{
    m_textEdit->setHtml(QString(
                            "<h3 style='color:#2c3e50;'>📊 财务分析报告</h3>"
                            "<div style='color:#34495e; line-height:1.6;'>%1</div>"
                            ).arg(result));

    m_closeButton->setEnabled(true);
}

void AnalysisDialog::onAnalysisError(const QString& message)
{
    m_textEdit->setPlainText("⚠️ 分析失败: " + message);
    m_closeButton->setEnabled(true);

    QMessageBox::warning(this, "错误",
                         "DeepSeek API请求失败:\n" + message +
                             "\n请检查网络连接和API密钥");
}
