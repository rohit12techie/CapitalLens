#include "investmentpage.h"
#include "dbmanager.h"
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <iostream>
#include <QGroupBox>
#include <QScrollArea>
#include <QDebug>

InvestmentForm::InvestmentForm(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    MonthYearSelectorLayout *monthYearSelectorLayout = new MonthYearSelectorLayout();
    mainLayout->addWidget(monthYearSelectorLayout, 0, Qt::AlignCenter);
    yearMonthSelector = monthYearSelectorLayout->getMonthSelector();

    entryGroupBox = new EntryGroupBox("Investment Entries", this);
    mainLayout->addWidget(entryGroupBox);

    totalLabel = new TotalLabel(this);
    mainLayout->addWidget(totalLabel);

    // Save button
    saveButton = new QPushButton("Save to Database", this);
    saveButton->setFixedSize(200, 50);
    saveButton->setStyleSheet(
        "QPushButton {"
        "font-weight: bold;"
        "font-size: 12px;" /* Larger font size for visibility */
        "padding: 8px 16px;" /* Extra padding for better aesthetics */
        "text-align: center;" /* Center the text */
        "}"
    );
    saveButton->setEnabled(false);
    mainLayout->addWidget(saveButton, 0, Qt::AlignCenter);

    connect(entryGroupBox, &EntryGroupBox::rowUpdate, this, &InvestmentForm::updateTotal);
    connect(saveButton, &QPushButton::clicked, this, &InvestmentForm::saveToDatabase);
    connect(yearMonthSelector, &MonthSelector::calenderChanged, this, &InvestmentForm::onCalenderChange);

    initializeForm();
}

void InvestmentForm::initializeForm() {
    loadEntries(); // Load data from the database
    addEntryRow(); // Add a blank row for new entry
}

void InvestmentForm::loadEntries() {
    entryGroupBox->clearRows();

    bool ok = false;
    QString selectedMonth = yearMonthSelector->getSelectedMonth();
    QSqlDatabase db = DatabaseManager::instance().getConnection();
    if (db.isOpen()) {
        QSqlQuery query(db);
        query.prepare("SELECT type, amount, comment FROM investments WHERE month = :month");
        query.bindValue(":month", selectedMonth);
        ok = query.exec();

        if (!query.exec()) {
            qWarning() << "Failed to load entries from the database:" << query.lastError().text();
            return;
        }

        while (query.next()) {
            QString type = query.value(0).toString();
            QString amount = query.value(1).toString();
            QString comment = query.value(2).toString();
            qWarning() << "Debugging warning ::" << "Type: " << type << " Amount: " << amount << " Comment: " << comment;

            // Add a new row with the retrieved data
            entryGroupBox->loadEntryRow(type, amount, comment);
        }
    } else {
        qWarning() << "Database connection is not open!";
    }

    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(connectionName);
}




void InvestmentForm::addEntryRow() {
    entryGroupBox->addEntryRow("", "", "");
}

void InvestmentForm::updateTotal() {
    double totalInvestment = 0;
    bool valid = true;

    qDebug() << "Updating total investment...";

    QList<EntryRow *> entries = entryGroupBox->getEntries();

    for (auto *entry : entries) {
        bool ok;
        double value = entry->getAmount().toDouble(&ok);
        qDebug() << "Value: " << value;
        qDebug() << "Ok: " << ok;

        if (!ok && !entry->getAmount().isEmpty()) {
            valid = false;
            continue;
        }
        totalInvestment += value;
    }

    totalLabel->setTotal(QString::number(totalInvestment, 'f', 2));
    saveButton->setEnabled(valid && totalInvestment > 0);
}

bool InvestmentForm::saveEntry(const QString &type, double amount, const QString &month, const QString &comment) {
    bool ok = false;
    QSqlDatabase db = DatabaseManager::instance().getConnection();
    if (db.isOpen()) {
        QSqlQuery query(db);
        QString user = "admin";
        query.prepare("INSERT INTO investments (user, month, type, amount, comment, created_at) VALUES (:user, :month, :type, :amount, :comment, CURRENT_TIMESTAMP)");
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

void InvestmentForm::saveToDatabase() {
    QString selectedMonth = yearMonthSelector->getSelectedMonth();
    QList<EntryRow *> entries = entryGroupBox->getEntries();

    for (auto *entry : entries) {
        bool ok;
        QString type = entry->getInvestmentType();
        QString comment = entry->getComment();
        double amount = entry->getAmount().toDouble(&ok);

        if (type.isEmpty() || !ok || amount <= 0) {
            QMessageBox::warning(this, "Input Error", "Please ensure all entries are valid before saving.");
            return;
        }

        if (!saveEntry(type, amount, selectedMonth, comment)) {
            QMessageBox::critical(this, "Database Error", "Failed to save data.");
            return;
        }
    }

    QMessageBox::information(this, "Success", "Data saved successfully for " + selectedMonth + "!");
}

void InvestmentForm::onCalenderChange() {
    qWarning() << "Signals Month changed!";
    initializeForm();
}