#ifndef DATAMODEL_H
#define DATAMODEL_H

#endif // DATAMODEL_H
// DataModel.h
#pragma once
#include <QList>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include "Transaction.h"
#include <QDebug>  // 添加qDebug头文件

class DataModel {
public:
    // 添加新交易记录
    void addTransaction(float amount, const QString& location) {
        transactions.append(Transaction(amount, location));
    }

    // 保存到JSON文件
    bool saveToJson(const QString& filename) {
        QJsonArray array;
        for (const auto& t : transactions) {
            array.append(t.toJson());
        }

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }
        file.write(QJsonDocument(array).toJson());
        return true;
    }

    // 从JSON文件加载
    bool loadFromJson(const QString& filename) {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }

        QJsonArray array = QJsonDocument::fromJson(file.readAll()).array();
        transactions.clear();
        for (const auto& val : array) {
            transactions.append(Transaction::fromJson(val.toObject()));
        }
        return true;
    }

    // 获取所有交易（只读）
    const QList<Transaction>& getTransactions() const {
        return transactions;
    }

private:
    QList<Transaction> transactions; // 核心数据存储
};
