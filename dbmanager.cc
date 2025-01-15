#include "dbmanager.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent), port(0) {}

DatabaseManager::~DatabaseManager() {
    // Close and remove all thread-specific connections
    for (const QString &connectionName : threadConnectionNames.localData()) {
        if (QSqlDatabase::contains(connectionName)) {
            QSqlDatabase::removeDatabase(connectionName);
        }
    }
}

DatabaseManager &DatabaseManager::instance() {
    static DatabaseManager instance; // Singleton instance
    return instance;
}

void DatabaseManager::setDatabaseParameters(const QString &driver, const QString &host, int port,
                                             const QString &dbName, const QString &user, const QString &password) {
    QMutexLocker locker(&mutex);
    this->driver = driver;
    this->host = host;
    this->port = port;
    this->dbName = dbName;
    this->user = user;
    this->password = password;
}

QSqlDatabase DatabaseManager::getConnection() {
    QString threadId = QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    QString connectionName = QString("DB_%1").arg(threadId);

    if (!QSqlDatabase::contains(connectionName)) {
        QSqlDatabase db = QSqlDatabase::addDatabase(driver, connectionName);
        db.setHostName(host);
        db.setPort(port);
        db.setDatabaseName(dbName);
        db.setUserName(user);
        db.setPassword(password);

        if (!db.open()) {
            qWarning() << "Failed to open database connection:" << db.lastError().text();
        }

        threadConnectionNames.setLocalData(connectionName);
    }

    return QSqlDatabase::database(connectionName);
}

void DatabaseManager::initializeDatabase() {
    QSqlDatabase db = getConnection();
    if (db.isOpen()) {
        QFile schemaFile(":/schema.sql");
        if (schemaFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString schemaContent = schemaFile.readAll();
            QSqlQuery query(db);
            if (!query.exec(schemaContent)) {
                qWarning() << "Failed to execute schema:" << query.lastError().text();
            } else {
                qDebug() << "Database schema loaded successfully.";
            }
        } else {
            qWarning() << "Failed to open schema file.";
        }
    } else {
        qWarning() << "Database connection is not open!";
    }
}


void DatabaseManager::releaseConnection(const QString &connectionName) {
    QMutexLocker locker(&mutex);
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }
}
