#include "DataModel.h"

void DataModel::addTransaction(const Transaction& transaction) {
    transactions.append(transaction);
    qDebug() << "Added transaction:" <<transaction.amount << "at" <<transaction.location;
}

bool DataModel::saveToJson(const QString& filename) {
    QJsonArray array;
    for (const auto& t : transactions) {
        array.append(t.toJson());
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Failed to open file for writing:" << filename;
        return false;
    }
    if (file.write(QJsonDocument(array).toJson()) == -1) {
        qDebug() << "Error: Failed to write to file:" << file.errorString();
        return false;
    }
    return true;
}

bool DataModel::loadFromJson(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Failed to open file for reading:" << filename;
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << parseError.errorString();
        return false;
    }

    if (!doc.isArray()) {
        qDebug() << "Error: JSON root is not an array";
        return false;
    }

    QJsonArray array = doc.array();
    transactions.clear();
    for (const QJsonValue& val : array) {
        if (!val.isObject()) {
            qDebug() << "Warning: Skipping non-object JSON value";
            continue;
        }
        transactions.append(Transaction::fromJson(val.toObject()));
    }
    qDebug() << "Loaded" << transactions.size() << "transactions from" << filename;
    return true;
}

const QList<Transaction>& DataModel::getTransactions() const {
    return transactions;
}
