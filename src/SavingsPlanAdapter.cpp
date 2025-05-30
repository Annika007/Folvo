#include "SavingsPlanAdapter.h"
#include <QVariant>  // 添加QVariant支持
#include <QMap>      // 添加QMap支持
#include <QDebug>
#include <QtMath>

SavingsPlanAdapter::SavingsPlanAdapter(QObject* parent)
    : QObject(parent)
{
    // 构造函数实现
    qDebug() << "SavingsPlanAdapter initialized";
}

QVariantMap SavingsPlanAdapter::calculatePlan(double income, double expense, double goal, int months)
{
    QVariantMap result;

    // 基础计算逻辑
    double monthlySaving = qFabs(goal) / qMax(1, months);
    bool isAchievable = (income - expense) >= monthlySaving;

    // 构建返回结果
    result["monthly_saving"] = monthlySaving;
    result["achievable"] = isAchievable;
    result["advice"] = isAchievable ?
                           QString("建议每月存储 %1 元").arg(monthlySaving, 0, 'f', 2) :
                           QString("当前收支状况无法达成目标");

    return result;
}
