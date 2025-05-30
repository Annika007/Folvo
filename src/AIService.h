#ifndef AISERVICE_H
#define AISERVICE_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QTimer>
// 新增DeepSeek专用请求结构
struct DeepSeekPayload {
    QString model = "deepseek-r1";
    QJsonArray messages;
    double temperature = 0.7;
};
class AIService : public QObject
{
    Q_OBJECT
public:
    explicit AIService(const QString& apiKey, QObject* parent = nullptr);

    void requestAdvice(const QString& prompt);
    void abortRequest();

signals:
    void analysisComplete(const QString& result);
    void errorOccurred(const QString& message);

private slots:
    void handleNetworkReply();
    void handleTimeout();

private:
    QNetworkAccessManager* m_networkManager;
    QNetworkReply* m_currentReply = nullptr;
    QTimer* m_timeoutTimer;
    QString m_apiKey;

    // 禁用拷贝
    Q_DISABLE_COPY(AIService)
};
#endif // AISERVICE_H
