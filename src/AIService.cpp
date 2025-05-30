#include "AIService.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QSsl>

AIService::AIService(const QString& apiKey, QObject* parent)
    : QObject(parent),
    m_apiKey(apiKey),
    m_networkManager(new QNetworkAccessManager(this)),
    m_timeoutTimer(new QTimer(this))
{
    qDebug() << "SSL支持状态：" << QSslSocket::supportsSsl()
        << "\nSSL库版本：" << QSslSocket::sslLibraryVersionString();
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    QSslConfiguration::setDefaultConfiguration(sslConfig);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &AIService::handleTimeout);
}

void AIService::requestAdvice(const QString& prompt) {
    qDebug() << "当前API密钥长度：" << m_apiKey.length() << "字符";
    qDebug() << "请求URL：" << "https://api.deepseek.com/v1/chat/completions";
    // 1. 验证API密钥
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API密钥未配置，请提供有效的DeepSeek API密钥");
        return;
    }

    // 仅警告但不阻止使用示例密钥（开发测试用）
    if (m_apiKey.startsWith("sk-example") ||
        m_apiKey == "sk-8388e833ee4c4ef5a4fb7855da513885") {
        qWarning() << "警告：您正在使用示例API密钥，生产环境请替换为真实密钥";
    }

    // 2. 准备请求
    QNetworkRequest request(QUrl("https://api.deepseek.com/v1/chat/completions"));
    // 添加以下配置
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Folvo/1.0");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());
    request.setRawHeader("Accept", "application/json");
    request.setTransferTimeout(30000); // 15秒超时
    // HTTPS安全配置
     QSslConfiguration sslConfig = request.sslConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    request.setSslConfiguration(sslConfig);
    // 3. 构建请求体
    QJsonObject payload{
        {"model", "deepseek-chat"},
        {"messages", QJsonArray{
                         QJsonObject{{"role", "system"}, {"content", "你是一名专业的财务顾问，用简洁清晰的中文回答"}},
                         QJsonObject{{"role", "user"}, {"content", prompt}}
                     }},
        {"temperature", 0.7},
        {"max_tokens", 1000}
    };

    // 4. 发送请求
    m_currentReply = m_networkManager->post(request, QJsonDocument(payload).toJson());
    connect(m_currentReply, &QNetworkReply::finished, this, &AIService::handleNetworkReply);

    // 5. 启动30秒超时计时器
    m_timeoutTimer->start(35000);
}

void AIService::handleNetworkReply() {
    m_timeoutTimer->stop();
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    reply->deleteLater();
    m_currentReply = nullptr;
    // 新增：获取HTTP状态码
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "HTTP状态码：" << statusCode;
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg;
        if (reply->error() == QNetworkReply::OperationCanceledError) {
            errorMsg = "请求被取消（可能原因：\n"
                       "1. 手动取消了操作\n"
                       "2. 网络连接中断\n"
                       "3. 请求超时\n"
                       "4. 服务器拒绝连接）";
        } else {
            errorMsg = QString("网络错误[%1]：%2").arg(reply->error()).arg(reply->errorString());
        }
        emit errorOccurred(errorMsg);
        reply->deleteLater();
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit errorOccurred("JSON解析错误: " + parseError.errorString());
        return;
    }

    QJsonObject response = doc.object();
    if (response.contains("error")) {
        QString apiError = response["error"].toObject()["message"].toString();
        emit errorOccurred("API返回错误：" + apiError);
        return;
    }

    QString result = response["choices"].toArray()[0]
                         .toObject()["message"].toObject()["content"].toString();
    emit analysisComplete(result);
}
void AIService::handleTimeout() {
    if (m_currentReply) {
        m_currentReply->abort();
        emit errorOccurred("请求超时，请检查网络连接");
    }
}

void AIService::abortRequest() {
    if (m_currentReply) {
        m_currentReply->abort();
    }
}
