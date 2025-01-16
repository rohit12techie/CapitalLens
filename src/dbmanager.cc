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

    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return;
    }

    QFile schemaFile("resources/schema.sql");
    if (!schemaFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open schema file:" << schemaFile.errorString();
        return;
    }

    QStringList schemaStatements = QString(schemaFile.readAll()).split(';', QString::SkipEmptyParts);
    schemaFile.close();

    QSqlQuery query(db);
    for (const QString &statement : schemaStatements) {
        if (!statement.trimmed().isEmpty()) {
            if (!query.exec(statement.trimmed())) {
                qWarning() << "Failed to execute statement:" << query.lastError().text();
                qWarning() << "Statement:" << statement;
            }
        }
    }
}


void DatabaseManager::releaseConnection(const QString &connectionName) {
    QMutexLocker locker(&mutex);
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }
}
