#include "dbservice.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>
#include <iostream>

DBService::DBService() {
    qDebug() << __func__ << "::"  << "DBService constructor called.";
    // dbconn = DatabaseManager::instance().getConnection();
    // if(dbconn.isOpen()) {
    //     qDebug() << __func__ << "::"  << "DBService:: Database connection is open!";
    // } else {
    //     qDebug() << __func__ << "::"  << "DBService:: Database connection is not open!";
    // }
}

DBService::~DBService() {
    qDebug() << __func__ << "::"  << "DBService destructor called.";
    // QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    // DatabaseManager::instance().releaseConnection(dbconn.connectionName());
}

QString DBService::getCurrentTimestamp() {
    return QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
}

bool DBService::createUser(const QString &userName) {
    dbconn = DatabaseManager::instance().getConnection();
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::" <<"Database connection is not open!";
        return false;
    }

    QSqlQuery query(dbconn);
    query.prepare("INSERT INTO Users (user_name) VALUES (:userName)");
    query.bindValue(":userName", userName);

    if (!query.exec()) {
        qDebug() << __func__ << "::"  << "createUser execution failed:" << query.lastError().text();
        return false;
    } else {
        qDebug() << __func__ << "::"  << QString("User '%1' created successfully.").arg(userName);
        return true;
    }
    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());
}

bool DBService::addYearForUser(const QString &userName, int year) {
    dbconn = DatabaseManager::instance().getConnection();
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::"  << "Database connection is not open!";
        return false;
    }

    QSqlQuery query(dbconn);
    query.prepare("INSERT INTO Years (user_id, year) SELECT user_id, :year FROM Users WHERE user_name = :userName");
    query.bindValue(":year", year);
    query.bindValue(":userName", userName);

    if (!query.exec()) {
        if (query.lastError().type() == QSqlError::StatementError) {
            qDebug() << __func__ << "::"  << QString("Year %1 already exists for user '%2'.").arg(year).arg(userName);
            return true; // Not an error in this example, year already exists
        } else {
            qDebug() << __func__ << "::"  << "addYearForUser execution failed:" << query.lastError().text();
            return false;
        }
    } else {
        qDebug() << __func__ << "::"  << QString("Year %1 added for user '%2'.").arg(year).arg(userName);
        return true;
    }
    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());
}

bool DBService::addMonthForYear(const QString &userName, int year, const QString &month) {
    dbconn = DatabaseManager::instance().getConnection();
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::"  << "Database connection is not open!";
        return false;
    }

    QSqlQuery query(dbconn);
    query.prepare("INSERT INTO Months (year_id, month) SELECT year_id, :month FROM Years JOIN Users ON Years.user_id = Users.user_id WHERE Users.user_name = :userName AND Years.year = :year");
    query.bindValue(":month", month);
    query.bindValue(":userName", userName);
    query.bindValue(":year", year);

    if (!query.exec()) {
        if (query.lastError().type() == QSqlError::StatementError) {
            qDebug() << __func__ << "::"  << QString("Month '%1' already exists for user '%2', year %3.").arg(month).arg(userName).arg(year);
            return true; // Not an error in this example, month already exists
        } else {
            qDebug() << __func__ << "::"  << "addMonthForYear execution failed:" << query.lastError().text();
            return false;
        }
    } else {
        qDebug() << __func__ << "::"  << QString("Month '%1' added for user '%2', year %3.").arg(month).arg(userName).arg(year);
        return true;
    }
    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());
}

bool DBService::addEntryForMonth(const QString &userName, int year, const QString &month, const QString &type, double amount, const QString &comment) {
    dbconn = DatabaseManager::instance().getConnection();
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::"  << "Database connection is not open!";
        return false;
    }
    QSqlQuery query(dbconn);
    query.prepare("INSERT INTO Investments (month_id, type, amount, comment, created_at) SELECT month_id, :type, :amount, :comment, :createdAt FROM Months JOIN Years ON Months.year_id = Years.year_id JOIN Users ON Years.user_id = Users.user_id WHERE Users.user_name = :userName AND Years.year = :year AND Months.month = :month");
    query.bindValue(":type", type);
    query.bindValue(":amount", amount);
    query.bindValue(":comment", comment);
    query.bindValue(":createdAt", getCurrentTimestamp());
    query.bindValue(":userName", userName);
    query.bindValue(":year", year);
    query.bindValue(":month", month);

    std::cout << "addEntry :: " << query.lastQuery().toStdString() << std::endl;

    if (!query.exec()) {
        qDebug() << __func__ << "::"  << "addEntryForMonth execution failed:" << query.lastError().text();
        return false;
    } else {
        qDebug() << __func__ << "::"  << QString("Entry added for user '%1', year %2, month '%3'.").arg(userName).arg(year).arg(month);
        return true;
    }
    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());
}


// Read Operations

QSqlQuery DBService::readUserData(const QString &userName) {
    dbconn = DatabaseManager::instance().getConnection();
    QSqlQuery query(dbconn);
    query.prepare("SELECT Years.year, Months.month, Investments.investment_data FROM Users "
                  "JOIN Years ON Users.user_id = Years.user_id "
                  "JOIN Months ON Years.year_id = Months.year_id "
                  "JOIN Investments ON Months.month_id = Investments.month_id "
                  "WHERE Users.user_name = :userName");
    query.bindValue(":userName", userName);

    if (!query.exec()) {
        qDebug() << __func__ << "::"  << "readUserData query execution failed:" << query.lastError().text();
        // Return an invalid QSqlQuery to signal an error
        return QSqlQuery(); // Returns an invalid QSqlQuery
    }
    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());
    return query;
}
    // qDebug() << __func__ << "::"  << "\n--- Data for User: " << userName << "---";
    // while (query.next()) {
    //     int year = query.value(0).toInt();
    //     QString month = query.value(1).toString();
    //     QString type = query.value(2).toString();
    //     double amount = query.value(3).toDouble();
    //     QString comment = query.value(4).toString();
    //     QString createdAt = query.value(5).toString();

    //     qDebug() << __func__ << "::"  << QString("  Year: %1, Month: %2, Entry Type: %3, Amount: %4, Comment: %5, Created At: %6")
    //                 .arg(year).arg(month).arg(type).arg(amount).arg(comment).arg(createdAt);
    // }
    // return true;

QSqlQuery DBService::readUserDataYear(const QString &userName, int year) {
    dbconn = DatabaseManager::instance().getConnection();
    QSqlQuery query(dbconn);
    query.prepare("SELECT Months.month, Investments.type, Investments.amount, Investments.comment, Investments.created_at FROM Users "
                  "JOIN Years ON Users.user_id = Years.user_id "
                  "JOIN Months ON Years.year_id = Months.year_id "
                  "JOIN Investments ON Months.month_id = Investments.month_id "
                  "WHERE Users.user_name = :userName AND Years.year = :year");
    query.bindValue(":userName", userName);
    query.bindValue(":year", year);


    return query;
    // qDebug() << __func__ << "::"  << "\n--- Data for User: " << userName << ", Year: " << year << "---";
    // while (query.next()) {
    //     QString month = query.value(0).toString();
    //     QString type = query.value(1).toString();
    //     double amount = query.value(2).toDouble();
    //     QString comment = query.value(3).toString();
    //     QString createdAt = query.value(4).toString();

    //     qDebug() << __func__ << "::"  << QString("  Month: %1, Entry Type: %2, Amount: %3, Comment: %4, Created At: %5")
    //                 .arg(month).arg(type).arg(amount).arg(comment).arg(createdAt);
    // }
    // return true;
}

QSqlQuery DBService::readUserDataMonth(const QString &userName, int year, const QString &month) {
    dbconn = DatabaseManager::instance().getConnection();
    qDebug() << __func__ << "::"  << "Database connection is open!";
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::"  << "Database connection is not open!";
        return QSqlQuery(); // Returns an invalid QSqlQuery
    }
    qDebug() << __func__ << "::"  << "Database connection is open! 11";
    QSqlQuery query(dbconn);
    qDebug() << __func__ << "::"  << "Database connection is open! 22";
    query.prepare("SELECT Investments.entry_id, Investments.type, Investments.amount, Investments.comment, Investments.created_at FROM Users "
                  "JOIN Years ON Users.user_id = Years.user_id "
                  "JOIN Months ON Years.year_id = Months.year_id "
                  "JOIN Investments ON Months.month_id = Investments.month_id "
                  "WHERE Users.user_name = :userName AND Years.year = :year AND Months.month = :month");
    query.bindValue(":userName", userName);
    query.bindValue(":year", year);
    query.bindValue(":month", month);

    if (!query.exec()) {
        qDebug() << __func__ << "::"  << "rohit :: readUserDataMonth query execution failed:" << query.lastError().text();
        return QSqlQuery();
    }

    qDebug() << __func__ << "::"  << "Database connection is open! 33" << query.size() << "IsValid::" << query.isValid();
    return query;
}


// Update Operations

bool DBService::updateEntryAmount(const QString &userName, int year, const QString &month, const QString &entryType, double newAmount) {
    dbconn = DatabaseManager::instance().getConnection();
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::"  << "Database connection is not open!";
        return false;
    }
    QSqlQuery query(dbconn);
    query.prepare("UPDATE Investments SET amount = :newAmount WHERE month_id = (SELECT month_id FROM Months JOIN Years ON Months.year_id = Years.year_id JOIN Users ON Years.user_id = Users.user_id WHERE Users.user_name = :userName AND Years.year = :year AND Months.month = :month) AND type = :entryType");
    query.bindValue(":newAmount", newAmount);
    query.bindValue(":userName", userName);
    query.bindValue(":year", year);
    query.bindValue(":month", month);
    query.bindValue(":entryType", entryType);

    if (!query.exec()) {
        qDebug() << __func__ << "::"  << "updateEntryAmount execution failed:" << query.lastError().text();
        return false;
    } else {
        qDebug() << __func__ << "::"  << QString("Entry amount updated for user '%1', year %2, month '%3', type '%4'.").arg(userName).arg(year).arg(month).arg(entryType);
        return true;
    }

    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());
}

bool DBService::updateEntryComment(const QString &userName, int year, const QString &month, const QString &entryType, const QString &newComment) {
    dbconn = DatabaseManager::instance().getConnection();
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::"  << "Database connection is not open!";
        return false;
    }
    QSqlQuery query(dbconn);
    query.prepare("UPDATE Investments SET comment = :newComment WHERE month_id = (SELECT month_id FROM Months JOIN Years ON Months.year_id = Years.year_id JOIN Users ON Years.user_id = Users.user_id WHERE Users.user_name = :userName AND Years.year = :year AND Months.month = :month) AND type = :entryType");
    query.bindValue(":newComment", newComment);
    query.bindValue(":userName", userName);
    query.bindValue(":year", year);
    query.bindValue(":month", month);
    query.bindValue(":entryType", entryType);

    if (!query.exec()) {
        qDebug() << __func__ << "::"  << "updateEntryComment execution failed:" << query.lastError().text();
        return false;
    } else {
        qDebug() << __func__ << "::"  << QString("Entry comment updated for user '%1', year %2, month '%3', type '%4'.").arg(userName).arg(year).arg(month).arg(entryType);
        return true;
    }

    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());
}


// Delete Operations

bool DBService::deleteEntry(const QString &userName, int year, const QString &month, unsigned int entryId) {
    dbconn = DatabaseManager::instance().getConnection();
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::"  << "Database connection is not open!";
        return false;
    }

    QSqlQuery query(dbconn);
    query.prepare("DELETE FROM Investments WHERE month_id = (SELECT month_id FROM Months JOIN Years ON Months.year_id = Years.year_id JOIN Users ON Years.user_id = Users.user_id WHERE Users.user_name = :userName AND Years.year = :year AND Months.month = :month) AND entry_id = :entryId");
    query.bindValue(":userName", userName);
    query.bindValue(":year", year);
    query.bindValue(":month", month);
    query.bindValue(":entryId", entryId);

    qDebug() << __func__ << "::"  << "deleteEntry :: " << query.lastQuery();

    if (!query.exec()) {
        qDebug() << __func__ << "::"  << "deleteEntry execution failed:" << query.lastError().text();
        return false;
    } else {
        qDebug() << __func__ << "::"  << QString("Entry deleted for user '%1', year %2, month '%3', entry_id '%4'.").arg(userName).arg(year).arg(month).arg(entryId);
    }

    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());

    return true;
}

bool DBService::deleteMonth(const QString &userName, int year, const QString &month) {
    dbconn = DatabaseManager::instance().getConnection();
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::"  << "Database connection is not open!";
        return false;
    }
    QSqlQuery query(dbconn);
    query.prepare("DELETE FROM Months WHERE year_id = (SELECT year_id FROM Years JOIN Users ON Years.user_id = Users.user_id WHERE Users.user_name = :userName AND Years.year = :year) AND month = :month");
    query.bindValue(":userName", userName);
    query.bindValue(":year", year);
    query.bindValue(":month", month);

    if (!query.exec()) {
        qDebug() << __func__ << "::"  << "deleteMonth execution failed:" << query.lastError().text();
        return false;
    } else {
        qDebug() << __func__ << "::"  << QString("Month '%1' deleted for user '%2', year %3.").arg(month).arg(userName).arg(year);
        return true;
    }
    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());
}


bool DBService::deleteYear(const QString &userName, int year) {
    dbconn = DatabaseManager::instance().getConnection();
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::"  << "Database connection is not open!";
        return false;
    }
    QSqlQuery query(dbconn);
    query.prepare("DELETE FROM Years WHERE user_id = (SELECT user_id FROM Users WHERE user_name = :userName) AND year = :year");
    query.bindValue(":userName", userName);
    query.bindValue(":year", year);

    if (!query.exec()) {
        qDebug() << __func__ << "::"  << "deleteYear execution failed:" << query.lastError().text();
        return false;
    } else {
        qDebug() << __func__ << "::"  << QString("Year %1 deleted for user '%2'.").arg(year).arg(userName);
        return true;
    }
    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());
}


bool DBService::deleteUser(const QString &userName) {
    dbconn = DatabaseManager::instance().getConnection();
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::"  << "Database connection is not open!";
        return false;
    }
    QSqlQuery query(dbconn);
    query.prepare("DELETE FROM Users WHERE user_name = :userName");
    query.bindValue(":userName", userName);

    if (!query.exec()) {
        qDebug() << __func__ << "::"  << "deleteUser execution failed:" << query.lastError().text();
        return false;
    } else {
        qDebug() << __func__ << "::"  << QString("User '%1' deleted.").arg(userName);
        return true;
    }
    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());
}

bool DBService::updateEntry(const QString &userName, int year, const QString &month, uint id, const QString &entryType, double newAmount, const QString &newComment) {
    dbconn = DatabaseManager::instance().getConnection();
    if (!dbconn.isOpen()) {
        qDebug() << __func__ << "::"  << "Database connection is not open!";
        return false;
    }
    QSqlQuery query(dbconn);
    query.prepare("UPDATE Investments SET type = :entryType, amount = :newAmount, comment = :newComment WHERE month_id = (SELECT month_id FROM Months JOIN Years ON Months.year_id = Years.year_id JOIN Users ON Years.user_id = Users.user_id WHERE Users.user_name = :userName AND Years.year = :year AND Months.month = :month) AND entry_id = :id");
    query.bindValue(":newAmount", newAmount);
    query.bindValue(":newComment", newComment);
    query.bindValue(":userName", userName);
    query.bindValue(":year", year);
    query.bindValue(":month", month);
    query.bindValue(":id", id);
    query.bindValue(":entryType", entryType);

    if (!query.exec()) {
        qDebug() << __func__ << "::"  << "updateEntry execution failed:" << query.lastError().text();
        return false;
    } else {
        qDebug() << __func__ << "::"  << QString("Entry updated for user '%1', year %2, month '%3', id '%4'.").arg(userName).arg(year).arg(month).arg(id);
        return true;
    }
    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(dbconn.connectionName());
    return true;
}