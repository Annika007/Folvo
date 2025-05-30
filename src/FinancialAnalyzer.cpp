#include "FinancialAnalyzer.h"
#include <algorithm>
#include <cmath>

FinancialAnalyzer::FinancialAnalyzer(const QList<Transaction>& transactions, QObject* parent)
    : QObject(parent), m_transactions(transactions) {}

QVariantMap FinancialAnalyzer::generateSummary() const {
    double income = 0, expense = 0;
    QMap<QString, double> categoryExpenses;

    for (const auto& t : m_transactions) {
        if (t.amount > 0) income += t.amount;
        else {
            double absAmount = std::abs(t.amount);
            expense += absAmount;
            categoryExpenses[t.category] += absAmount;
        }
    }

    QString topCategory;
    if (!categoryExpenses.isEmpty()) {
        auto it = std::max_element(categoryExpenses.begin(), categoryExpenses.end());
        topCategory = it.key();
    }

    return {
        {"total_income", income},
        {"total_expense", expense},
        {"balance", income - expense},
        {"top_category", topCategory}
    };
}

QVariantMap FinancialAnalyzer::analyzeHabits() const {
    // 实现消费习惯分析...
}
