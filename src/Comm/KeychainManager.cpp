#include "KeychainManager.h"
#include <QDebug>

KeychainManager::KeychainManager(QObject *parent) : QObject(parent) {}

void KeychainManager::savePassword(const QString &account, const QString &password) {
    auto *job = new QKeychain::WritePasswordJob(m_serviceName, this);
    job->setAutoDelete(true);
    job->setKey(account);
    job->setTextData(password);

    connect(job, &QKeychain::Job::finished, this, [this](QKeychain::Job *job) {
        if (job->error()) {
            emit operationCompleted(false, job->errorString());
            qWarning() << "Save password error:" << job->errorString();
        } else {
            emit operationCompleted(true, "");
        }
    });

    job->start();
}

void KeychainManager::readPassword(const QString &account) {
    auto *job = new QKeychain::ReadPasswordJob(m_serviceName, this);
    job->setAutoDelete(true);
    job->setKey(account);

    connect(job, &QKeychain::Job::finished, this, [this](QKeychain::Job *job) {
        auto *readJob = static_cast<QKeychain::ReadPasswordJob*>(job);
        if (job->error()) {
            emit passwordRead("");
            qWarning() << "Read password error:" << job->errorString();
        } else {
            emit passwordRead(readJob->textData());
        }
    });

    job->start();
}

void KeychainManager::deletePassword(const QString &account) {
    auto *job = new QKeychain::DeletePasswordJob(m_serviceName, this);
    job->setAutoDelete(true);
    job->setKey(account);

    connect(job, &QKeychain::Job::finished, this, [this](QKeychain::Job *job) {
        if (job->error()) {
            emit operationCompleted(false, job->errorString());
            qWarning() << "Delete password error:" << job->errorString();
        } else {
            emit operationCompleted(true, "");
        }
    });

    job->start();
}

void KeychainManager::hasPassword(const QString &account) {
    auto *job = new QKeychain::ReadPasswordJob(m_serviceName, this);
    job->setAutoDelete(true);
    job->setKey(account);

    connect(job, &QKeychain::Job::finished, this, [this](QKeychain::Job *job) {
        emit passwordExists(job->error() == QKeychain::NoError);
    });

    job->start();
}