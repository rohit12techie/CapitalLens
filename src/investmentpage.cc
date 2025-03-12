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

    saveButton = new SaveButton(this);
    mainLayout->addWidget(saveButton, 0, Qt::AlignCenter);

    connect(entryGroupBox, &EntryGroupBox::updateTotal, this, &InvestmentForm::updateTotal);
    connect(saveButton, &QPushButton::clicked, this, &InvestmentForm::saveToDatabase);
    connect(yearMonthSelector, &MonthSelector::calenderChanged, this, &InvestmentForm::onCalenderChange);
    connect(entryGroupBox, &EntryGroupBox::updateRow, this, &InvestmentForm::enableSaveButton);

    dbservice = new DBService();
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
    int selectedYear = yearMonthSelector->getSelectedYear();

    qDebug() << "LoadEntries:: Selected Month: " << selectedMonth;
    qDebug() << "LoadEntries:: Selected Year: " << selectedYear;

    QSqlQuery query = dbservice->readUserDataMonth(QString("admin"), selectedYear, selectedMonth);
    qDebug() << "rohit XXX :::" << query.isValid();

    while (query.next()) {
        unsigned int id = query.value(0).toUInt();
        QString type = query.value(1).toString();
        double amount = query.value(2).toDouble();
        QString comment = query.value(3).toString();
        QString createdAt = query.value(4).toString();
        entryGroupBox->loadEntryRow(id, type, QString::number(amount, 'f', 2), comment);
        qDebug() << "|entry_id: "<< id <<"| Type:" << type << "| Amount:" << amount << "| Comment:" << comment << "| Created At:" << createdAt;
    }
}

void InvestmentForm::addEntryRow() {
    entryGroupBox->addEntryRow(0, "", "", "");
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
}

void InvestmentForm::enableSaveButton() {
    saveButton->setEnabled(true);
}

void InvestmentForm::disableSaveButton() {
    saveButton->setEnabled(false);
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

    return true;

}

void InvestmentForm::saveToDatabase() {
    QString selectedMonth = yearMonthSelector->getSelectedMonth();
    int selectedYear = yearMonthSelector->getSelectedYear();

    dbservice->addYearForUser("admin", selectedYear);
    dbservice->addMonthForYear("admin", selectedYear, selectedMonth);

    QList<EntryRow *> entries = entryGroupBox->getEntries();
    for (auto *entry : entries) {
        bool ok;
        unsigned int id = entry->getId();
        QString type = entry->getInvestmentType();
        QString comment = entry->getComment();
        double amount = entry->getAmount().toDouble(&ok);

        qDebug() << __func__ << "::" << "Saving entry...";
        qDebug() << "ID: " << id;
        qDebug() << "Type: " << type;
        qDebug() << "Amount: " << amount;
        qDebug() << "Comment: " << comment;
        qDebug() << "Updated: " << entry->isUpdated();

        if (type.isEmpty() || !ok || amount <= 0) {
            QMessageBox::warning(this, "Input Error", "Move this validatioan logic at the top of the function.");
            return;
        }

        if(id == 0) {
            if(! dbservice->addEntryForMonth("admin", selectedYear, selectedMonth, type, amount, comment)) {
                QMessageBox::critical(this, "Database Error", "Failed to save data.");
                return;
            }
        } else if(entry->isUpdated()) {
            if(! dbservice->updateEntry("admin", selectedYear, selectedMonth, id, type, amount, comment)) {
                QMessageBox::critical(this, "Database Error", "Failed to save data.");
                return;
            }
        }
    }

    QList<unsigned int> removedEntries = entryGroupBox->getRemovedEntries();
    qDebug() << "Removed Entries: " << removedEntries.size();
    for (auto entryId : removedEntries) {

        qDebug() << __func__ << "::" << "removing entry...";
        qDebug() << "ID: " << entryId;

        if(! dbservice->deleteEntry("admin", selectedYear, selectedMonth, entryId)) {
            QMessageBox::critical(this, "Database Error", "Failed to save data.");
            return;
        }
    }


    QMessageBox::information(this, "Success", "Data saved successfully for " + selectedMonth + "!");
    disableSaveButton();
}

void InvestmentForm::onCalenderChange() {
    qWarning() << "Signals Month changed!";
    disableSaveButton();
    initializeForm();
}