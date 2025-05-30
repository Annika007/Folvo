#ifndef FINANCIALANALYZER_H
#define FINANCIALANALYZER_H

#include <QObject>
#include <QList>
#include "Transaction.h"

class FinancialAnalyzer : public QObject {
    Q_OBJECT
public:
    explicit FinancialAnalyzer(const QList<Transaction>& transactions, QObject* parent = nullptr);

    Q_INVOKABLE QVariantMap generateSummary() const;
    Q_INVOKABLE QVariantMap analyzeHabits() const;

private:
    QList<Transaction> m_transactions;
};
#endif // FINANCIALANALYZER_H
