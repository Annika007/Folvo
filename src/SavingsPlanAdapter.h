#ifndef SAVINGSPLANADAPTER_H
#define SAVINGSPLANADAPTER_H
#include <QObject>
#include <QVariantMap>  // 确保包含这个头文件
class SavingsPlanAdapter : public QObject
{
    Q_OBJECT
public:
    explicit SavingsPlanAdapter(QObject* parent = nullptr); // 确保有Q_OBJECT宏
    Q_INVOKABLE QVariantMap calculatePlan(double income, double expense, double goal, int months);

    // 添加元类型注册
    static void registerMetaType() {
        qRegisterMetaType<SavingsPlanAdapter*>("SavingsPlanAdapter*");
    }
};
#endif // SAVINGSPLANADAPTER_H
