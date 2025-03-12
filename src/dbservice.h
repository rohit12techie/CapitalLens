#ifndef DBSERVICE_H
#define DBSERVICE_H

#include <string>
#include <vector>
#include <QtSql/QSqlDatabase>
#include "dbmanager.h"

class DBService{
public:
    DBService();
    ~DBService();

    bool createUser(const QString &userName);
    bool addYearForUser(const QString &userName, int year);
    bool addMonthForYear(const QString &userName, int year, const QString &month);
    bool addEntryForMonth(const QString &userName, int year, const QString &month, const QString &type, double amount, const QString &comment);
    QSqlQuery readUserData(const QString &userName);
    QSqlQuery readUserDataYear(const QString &userName, int year);
    QSqlQuery readUserDataMonth(const QString &userName, int year, const QString &month);
    bool updateEntry(const QString &userName, int year, const QString &month, uint id, const QString &entryType, double newAmount, const QString &newComment);
    bool updateEntryAmount(const QString &userName, int year, const QString &month, const QString &entryType, double newAmount);
    bool updateEntryComment(const QString &userName, int year, const QString &month, const QString &entryType, const QString &newComment);
    bool deleteEntry(const QString &userName, int year, const QString &month, const unsigned int id);
    bool deleteMonth(const QString &userName, int year, const QString &month);
    bool deleteYear(const QString &userName, int year);
    bool deleteUser(const QString &userName);

private:
    QSqlDatabase dbconn;
    QString getCurrentTimestamp();
};

#endif // DBSERVICE_H