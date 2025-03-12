#include "cashincashout.h"
#include "dbmanager.h"
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <iostream>
#include <QGroupBox>
#include <QScrollArea>
#include <QDebug>

CashInCashOut::CashInCashOut(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    MonthYearSelectorLayout *monthYearSelectorLayout = new MonthYearSelectorLayout();
    mainLayout->addWidget(monthYearSelectorLayout, 0, Qt::AlignCenter);
    yearMonthSelector = monthYearSelectorLayout->getMonthSelector();

    QHBoxLayout *cashSectionsLayout = new QHBoxLayout();
    cashInEntryGroupBox = new EntryGroupBox("Cash IN", this);
    cashSectionsLayout->addWidget(cashInEntryGroupBox);

    cashOutEntryGroupBox = new EntryGroupBox("Cash Out", this);
    cashSectionsLayout->addWidget(cashOutEntryGroupBox);

    mainLayout->addLayout(cashSectionsLayout);

    QHBoxLayout *cashTotalLayout = new QHBoxLayout();
    cashInTotalLabel = new TotalLabel(this, "Cash-In Total");
    cashOutTotalLabel = new TotalLabel(this, "Cash-Out Total");
    cashTotalLayout->addWidget(cashInTotalLabel);
    cashTotalLayout->addWidget(cashOutTotalLabel);
    mainLayout->addLayout(cashTotalLayout);

    totalLabel = new TotalLabel(this);
    mainLayout->addWidget(totalLabel);

    saveButton = new SaveButton(this);
    mainLayout->addWidget(saveButton, 0, Qt::AlignCenter);

    connect(cashInEntryGroupBox, &EntryGroupBox::updateTotal, this, &CashInCashOut::updateCashInTotal);
    connect(cashOutEntryGroupBox, &EntryGroupBox::updateTotal, this, &CashInCashOut::updateCashOutTotal);
    connect(saveButton, &QPushButton::clicked, this, &CashInCashOut::saveToDatabase);
    connect(yearMonthSelector, &MonthSelector::calenderChanged, this, &CashInCashOut::onCalenderChange);
    connect(cashInEntryGroupBox, &EntryGroupBox::updateRow, this, &CashInCashOut::enableSaveButton);
    connect(cashOutEntryGroupBox, &EntryGroupBox::updateRow, this, &CashInCashOut::enableSaveButton);
    initializeForm();
}

void CashInCashOut::initializeForm() {
    loadEntries(); // Load data from the database
    addEntryRow(); // Add a blank row for new entry
}

void CashInCashOut::loadEntries() {
    auto loadEntriesFromDatabase = [this](const QString &tableName, EntryGroupBox *entryGroupBox) {
        entryGroupBox->clearRows();

        QString selectedMonth = yearMonthSelector->getSelectedMonth();
        QSqlDatabase db = DatabaseManager::instance().getConnection();
        if (db.isOpen()) {
            QSqlQuery query(db);
            query.prepare(QString("SELECT type, amount, comment FROM %1 WHERE month = :month").arg(tableName));
            query.bindValue(":month", selectedMonth);

            if (!query.exec()) {
                qWarning() << "Failed to load entries from the database:" << query.lastError().text();
                return;
            }

            while (query.next()) {
                uint id = 0;
                QString type = query.value(0).toString();
                QString amount = query.value(1).toString();
                QString comment = query.value(2).toString();
                qWarning() << "Debugging warning ::" << "Type: " << type << " Amount: " << amount << " Comment: " << comment;

                // Add a new row with the retrieved data
                entryGroupBox->loadEntryRow(id, type, amount, comment);
            }
        } else {
            qWarning() << "Database connection is not open!";
        }

        QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
        DatabaseManager::instance().releaseConnection(connectionName);
    };

    loadEntriesFromDatabase("cashin", cashInEntryGroupBox);
    loadEntriesFromDatabase("cashout", cashOutEntryGroupBox);
}

void CashInCashOut::addEntryRow() {
    cashInEntryGroupBox->addEntryRow(0, "", "", "");
    cashOutEntryGroupBox->addEntryRow(0, "", "", "");
}

void CashInCashOut::updateCashInTotal() {
    totalCashIn = 0;
    bool valid = true;

    qDebug() << "Updating total cash-in investment...";

    QList<EntryRow *> entries = cashInEntryGroupBox->getEntries();

    for (auto *entry : entries) {
        bool ok;
        double value = entry->getAmount().toDouble(&ok);
        qDebug() << "Value: " << value;
        qDebug() << "Ok: " << ok;

        if (!ok && !entry->getAmount().isEmpty()) {
            valid = false;
            continue;
        }
        totalCashIn += value;
    }

    cashInTotalLabel->setTotal(QString::number(totalCashIn, 'f', 2));
    updateTotal();
}

void CashInCashOut::updateCashOutTotal() {
    totalCashOut = 0;
    bool valid = true;

    qDebug() << "Updating total cash-out investment...";

    QList<EntryRow *> entries = cashOutEntryGroupBox->getEntries();

    for (auto *entry : entries) {
        bool ok;
        double value = entry->getAmount().toDouble(&ok);
        qDebug() << "Value: " << value;
        qDebug() << "Ok: " << ok;

        if (!ok && !entry->getAmount().isEmpty()) {
            valid = false;
            continue;
        }
        totalCashOut += value;
    }

    cashOutTotalLabel->setTotal(QString::number(totalCashOut, 'f', 2));
    updateTotal();
}

void CashInCashOut::updateTotal() {
    double totalInvestment = 0;
    bool valid = true;

    qDebug() << "Updating total investment...";
    totalInvestment = totalCashIn - totalCashOut;
    totalLabel->setTotal(QString::number(totalInvestment, 'f', 2));
}

void CashInCashOut::enableSaveButton() {
    saveButton->setEnabled(true);
}

void CashInCashOut::disableSaveButton() {
    saveButton->setEnabled(false);
}

bool CashInCashOut::saveEntry(const QString tableName, const QString &type, double amount, const QString &month, const QString &comment) {
    bool ok = false;
    QSqlDatabase db = DatabaseManager::instance().getConnection();
    if (db.isOpen()) {
        QSqlQuery query(db);
        QString user = "admin";
        query.prepare(QString("INSERT INTO %1 (user, month, type, amount, comment, created_at) \
                                VALUES (:user, :month, :type, :amount, :comment, CURRENT_TIMESTAMP)").arg(tableName));
        query.bindValue(":user", user);
        query.bindValue(":month", month);
        query.bindValue(":type", type);
        query.bindValue(":amount", amount);
        query.bindValue(":comment", comment);
        query.bindValue(":created_at", QDateTime::currentDateTime());

        ok = query.exec();
        if (!ok) {
            qWarning() << "Failed to execute query:" << query.lastError().text();
        } else {
            qDebug() << "Investment saved successfully!";
        }
    } else {
        qWarning() << "Database connection is not open!";
    }

    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(connectionName);

    return ok;
}

void CashInCashOut::saveToDatabase() {
    QString selectedMonth = yearMonthSelector->getSelectedMonth();

    auto saveEntries = [this, &selectedMonth](EntryGroupBox *entryGroupBox, const QString &tableName) {
        QList<EntryRow *> entries = entryGroupBox->getEntries();

        for (auto *entry : entries) {
            bool ok;
            QString type = entry->getInvestmentType();
            QString comment = entry->getComment();
            double amount = entry->getAmount().toDouble(&ok);

            if (type.isEmpty() || !ok || amount <= 0) {
                QMessageBox::warning(this, "Input Error", "Please ensure all entries are valid before saving.");
                return false;
            }

            if (!saveEntry(tableName, type, amount, selectedMonth, comment)) {
                QMessageBox::critical(this, "Database Error", "Failed to save data.");
                return false;
            }
        }
        return true;
    };

    if (!saveEntries(cashInEntryGroupBox, "cashin")) {
        return;
    }
    if(!saveEntries(cashOutEntryGroupBox, "cashout")) {
        return;
    }

    QMessageBox::information(this, "Success", "Data saved successfully for " + selectedMonth + "!");
    disableSaveButton();
}

void CashInCashOut::onCalenderChange() {
    qWarning() << "Signals Month changed!";
    disableSaveButton();
    initializeForm();
}