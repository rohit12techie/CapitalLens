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

    yearMonthSelector = new MonthSelector(this);
    QGroupBox *smallContainer = new QGroupBox("Select Year and Month", this);
    smallContainer->setAlignment(Qt::AlignCenter);
    smallContainer->setFixedSize(300, 200); // Adjust size as needed
    QVBoxLayout *containerLayout = new QVBoxLayout(smallContainer);
    containerLayout->addWidget(yearMonthSelector);
    mainLayout->addWidget(smallContainer, 0, Qt::AlignCenter);

    // Create a group box to encapsulate the grid layout
    QGroupBox *entryGroupBox = new QGroupBox("Investment Entries", this);
    entryGroupBox->setStyleSheet(
        "QGroupBox {"
        "border: 2px solid #cccccc;"
        "border-radius: 8px;"
        "margin-top: 10px;"
        "padding: 10px;"
        "font-weight: bold;"
        "background-color: #f9f9f9;"
        "}"
        "QGroupBox::title {"
        "subcontrol-origin: margin;"
        "subcontrol-position: top center;"
        "padding: 0 5px;"
        "}"
    );

    // Scroll area to make the grid scrollable
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    // Container widget for the grid layout
    QWidget *entryContainer = new QWidget();
    entryLayout = new QGridLayout(entryContainer);
    entryLayout->setAlignment(Qt::AlignTop);
    entryContainer->setLayout(entryLayout);

    // Set the container as the scroll area's widget
    scrollArea->setWidget(entryContainer);

    // Create a layout for the group box and add the scroll area
    QVBoxLayout *entryGroupBoxLayout = new QVBoxLayout();
    entryGroupBoxLayout->addWidget(scrollArea);
    entryGroupBox->setLayout(entryGroupBoxLayout);

    // Add the group box to the main layout
    mainLayout->addWidget(entryGroupBox);
    addEntryRow();
    // Add row button
    addRowButton = new QPushButton("+ Add Row", this);
    mainLayout->addWidget(addRowButton);
    connect(addRowButton, &QPushButton::clicked, this, &InvestmentForm::addEntryRow);

    // Total label
    totalLabel = new QLabel("Total Investment: 0", this);
    totalLabel->setStyleSheet(
    "QLabel {"
    "font-size: 18px;"
    "font-weight: bold;"
    "color: #1b5e20;"
    "}");
    totalLabel->setAlignment(Qt::AlignLeft);
    mainLayout->addWidget(totalLabel);

    // Save button
    saveButton = new QPushButton("Save to Database", this);
    saveButton->setEnabled(false); // Initially disabled
    mainLayout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &InvestmentForm::saveToDatabase);
}

void InvestmentForm::addEntryRow() {
    int row = entryLayout->rowCount();

    // Investment type dropdown or custom input
    QComboBox *typeInput = new QComboBox(this);
    typeInput->setEditable(true); // Allow custom types
    typeInput->addItems({"Stocks", "Bonds", "Mutual Funds", "Real Estate", "FD", "Others"});
    entryLayout->addWidget(typeInput, row, 0);
    typeInputs.push_back(typeInput);

    // Investment amount input
    QLineEdit *amountInput = new QLineEdit(this);
    amountInput->setPlaceholderText("Enter amount");
    entryLayout->addWidget(amountInput, row, 1);
    amountInputs.push_back(amountInput);

    // Connect signal to dynamically update total
    connect(amountInput, &QLineEdit::textChanged, this, &InvestmentForm::updateTotal);
    connect(amountInput, &QLineEdit::editingFinished, this, &InvestmentForm::updateTotal);
}

void InvestmentForm::updateTotal() {
    totalInvestment = 0;
    bool valid = true;

    for (auto *amountInput : amountInputs) {
        bool ok;
        double value = amountInput->text().toDouble(&ok);
        if (!ok && !amountInput->text().isEmpty()) {
            valid = false;
            continue;
        }
        totalInvestment += value;
    }

    totalLabel->setText("Total Investment: " + QString::number(totalInvestment, 'f', 2));
    saveButton->setEnabled(valid && totalInvestment > 0);
}

bool InvestmentForm::saveEntry(const QString &type, double amount, const QString &month) {
    bool ok = false;
    QSqlDatabase db = DatabaseManager::instance().getConnection();
    if (db.isOpen()) {
        QSqlQuery query(db);
        QString user = "admin";
        QString comment = "Investment";
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

    // Release the connection
    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(connectionName);

    return ok;
}

void InvestmentForm::saveToDatabase() {
    //QString selectedMonth = monthSelector->date().toString("MMMM yyyy");
    QString selectedMonth = yearMonthSelector->getSelectedMonth();

    for (int i = 0; i < typeInputs.size(); ++i) {
        QString type = typeInputs[i]->currentText();
        bool ok;
        double amount = amountInputs[i]->text().toDouble(&ok);

        if (type.isEmpty() || !ok || amount <= 0) {
            QMessageBox::warning(this, "Input Error", "Please ensure all entries are valid before saving.");
            return;
        }

        if (!saveEntry(type, amount, selectedMonth)) {
            QMessageBox::critical(this, "Database Error", "Failed to save data.");
            return;
        }
    }

    QMessageBox::information(this, "Success", "Data saved successfully for " + selectedMonth + "!");
}

void InvestmentForm::updateFormForMonth() {
    QString selectedMonth = monthSelector->date().toString("MMMM yyyy");
    monthHeaderLabel->setText("Selected Month: " + selectedMonth);
}
