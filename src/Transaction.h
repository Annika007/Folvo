#ifndef TRANSACTION_H
#define TRANSACTION_H



// Transaction.h
#pragma once
#include <QDateTime>
#include <QString>
#include <QJsonObject>

struct Transaction {
    QDateTime time;   // 交易时间（自动记录）
    float amount;     // 金额（单位：元）
    QString location; // 消费地点
    QString category; // 分类（初始为"未分类"）
    QString notes;

    // 构造函数（简化版）
    Transaction(float amt, const QString& loc, const QString& cat = "未分类")
        : amount(amt), location(loc), category(cat) {
        time = QDateTime::currentDateTime(); // 自动记录当前时间
    }
    // 新增完整构造函数
    Transaction(double amt, const QString& loc, const QString& cat,
                const QDateTime& dt, const QString& nt)
        : amount(amt), location(loc), category(cat),
        time(dt), notes(nt) {}
    // 转换为JSON对象（用于存储）
    QJsonObject toJson() const {
        QJsonObject obj;
        obj.insert("time", time.toString(Qt::ISODate));
        obj.insert("amount", amount);
        obj.insert("location", location);
        obj.insert("category", category);
        obj.insert("notes",notes);
        return obj;
    }

    // 从JSON解析（用于加载）
    static Transaction fromJson(const QJsonObject& obj) {
        double amount = obj["amount"].toDouble();
        QString loc = obj["location"].toString();
        QString cat = obj.contains("category") ? obj["category"].toString() : "未分类";
        Transaction t(amount, loc, cat);
        t.time = QDateTime::fromString(obj["time"].toString(), Qt::ISODate);
        t.notes = obj.contains("notes") ? obj["notes"].toString() : "";
        return t;
    }
};


#endif // TRANSACTION_H
