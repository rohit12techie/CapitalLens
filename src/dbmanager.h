#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include <QThreadStorage>
#include <QFile>
#include <QSqlQuery>
#include <QThread>

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    static DatabaseManager &instance();

    QSqlDatabase getConnection();
    void releaseConnection(const QString &connectionName);

    void setDatabaseParameters(const QString &driver, const QString &host, int port,
                                const QString &dbName, const QString &user, const QString &password);

    void initializeDatabase();

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    QThreadStorage<QString> threadConnectionNames; // Unique connection for each thread
    QString driver;
    QString host;
    int port;
    QString dbName;
    QString user;
    QString password;

    QMutex mutex; // Mutex to synchronize shared resources
};

#endif // DATABASEMANAGER_H
