#ifndef ANALYSISDIALOG_H
#define ANALYSISDIALOG_H
#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include "DataModel.h"
#include "FinancialAnalyzer.h"
#include "AIService.h"
class FinancialAnalyzer;
class AIService;
class QProgressDialog;
class AnalysisDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnalysisDialog(DataModel* model, QWidget* parent = nullptr);
    ~AnalysisDialog();

private slots:
    void startAnalysis();
    void onAnalysisComplete(const QString& result);
    void onAnalysisError(const QString& message);

private:
    DataModel* m_model;
    FinancialAnalyzer* m_analyzer;
    AIService* m_aiService;

    // UI组件
    QTextEdit* m_textEdit;
    QPushButton* m_closeButton;
    QVBoxLayout* m_mainLayout;

    // 禁用拷贝
    Q_DISABLE_COPY(AnalysisDialog)
};
#endif // ANALYSISDIALOG_H
