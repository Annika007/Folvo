#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QList>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include "Transaction.h"
#include <QDebug>

class DataModel {
public:
    // 添加新交易记录
    void addTransaction(const Transaction& transaction);

    // 保存到JSON文件
    bool saveToJson(const QString& filename);

    // 从JSON文件加载
    bool loadFromJson(const QString& filename);

    // 获取所有交易（只读）
    const QList<Transaction>& getTransactions() const;

private:
    QList<Transaction> transactions; // 核心数据存储
};

#endif // DATAMODEL_H
