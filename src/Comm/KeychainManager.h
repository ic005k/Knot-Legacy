#ifndef KEYCHAINMANAGER_H
#define KEYCHAINMANAGER_H

#include <QObject>
#include <QString>
#include <keychain.h>

class KeychainManager : public QObject
{
    Q_OBJECT

public:
    explicit KeychainManager(QObject *parent = nullptr);

    // 保存密码到系统密钥库
    void savePassword(const QString &account, const QString &password);

    // 从系统密钥库读取密码（异步）
    void readPassword(const QString &account);

    // 删除指定账户的密码
    void deletePassword(const QString &account);

    // 检查是否存在某个账户的密码（异步）
    void hasPassword(const QString &account);

signals:
    // 密码读取完成
    void passwordRead(const QString &password);

    // 操作完成（保存/删除）
    void operationCompleted(bool success, const QString &error);

    // 检查是否存在密码的结果
    void passwordExists(bool exists);

private:
    const QString m_serviceName = "YourAppName"; // 修改为你的应用名称
};

#endif // KEYCHAINMANAGER_H