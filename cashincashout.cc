#include "cashincashout.h"
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <iostream>
#include <QGroupBox>
#include <QScrollArea>
#include <QDoubleSpinBox>

CashInCashOut::CashInCashOut(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Year and Month Selector
    yearMonthSelector = new MonthSelector(this);
    QGroupBox *smallContainer = new QGroupBox("Select Year and Month", this);
    smallContainer->setAlignment(Qt::AlignCenter);
    smallContainer->setFixedSize(300, 200);

    QVBoxLayout *containerLayout = new QVBoxLayout(smallContainer);
    containerLayout->addWidget(yearMonthSelector);
    mainLayout->addWidget(smallContainer, 0, Qt::AlignCenter);

    // Horizontal Layout for Cash In and Cash Out
    QHBoxLayout *cashSectionsLayout = new QHBoxLayout();

    // Cash In Section
    QGroupBox *cashInGroupBox = new QGroupBox("Cash In", this);
    cashInGroupBox->setStyleSheet(
        "QGroupBox {"
        "border: 2px solid #cccccc;"
        "border-radius: 8px;"
        "margin-top: 10px;"
        "padding: 10px;"
        "font-weight: bold;"
        "background-color: #f1f8e9;" // Light green background
        "}"
        "QGroupBox::title {"
        "subcontrol-origin: margin;"
        "subcontrol-position: top center;"
        "padding: 0 5px;"
        "}"
    );

    QScrollArea *cashInScrollArea = new QScrollArea(this);
    cashInScrollArea->setWidgetResizable(true);

    QWidget *cashInContainer = new QWidget();
    QGridLayout *cashInLayout = new QGridLayout(cashInContainer);
    cashInLayout->setAlignment(Qt::AlignTop);
    cashInContainer->setLayout(cashInLayout);

    cashInScrollArea->setWidget(cashInContainer);

    QVBoxLayout *cashInGroupBoxLayout = new QVBoxLayout();
    cashInGroupBoxLayout->addWidget(cashInScrollArea);
    cashInGroupBox->setLayout(cashInGroupBoxLayout);

    cashSectionsLayout->addWidget(cashInGroupBox);

    // Cash Out Section
    QGroupBox *cashOutGroupBox = new QGroupBox("Cash Out", this);
    cashOutGroupBox->setStyleSheet(
        "QGroupBox {"
        "border: 2px solid #cccccc;"
        "border-radius: 8px;"
        "margin-top: 10px;"
        "padding: 10px;"
        "font-weight: bold;"
        "background-color: #ffebee;" // Light red background
        "}"
        "QGroupBox::title {"
        "subcontrol-origin: margin;"
        "subcontrol-position: top center;"
        "padding: 0 5px;"
        "}"
    );

    QScrollArea *cashOutScrollArea = new QScrollArea(this);
    cashOutScrollArea->setWidgetResizable(true);

    QWidget *cashOutContainer = new QWidget();
    QGridLayout *cashOutLayout = new QGridLayout(cashOutContainer);
    cashOutLayout->setAlignment(Qt::AlignTop);
    cashOutContainer->setLayout(cashOutLayout);

    cashOutScrollArea->setWidget(cashOutContainer);

    QVBoxLayout *cashOutGroupBoxLayout = new QVBoxLayout();
    cashOutGroupBoxLayout->addWidget(cashOutScrollArea);
    cashOutGroupBox->setLayout(cashOutGroupBoxLayout);

    cashSectionsLayout->addWidget(cashOutGroupBox);

    // Add the horizontal layout to the main layout
    mainLayout->addLayout(cashSectionsLayout);

    // Add Row Buttons
    QPushButton *addCashInRowButton = new QPushButton("+ Add Cash In Row", this);
    QPushButton *addCashOutRowButton = new QPushButton("+ Add Cash Out Row", this);

    mainLayout->addWidget(addCashInRowButton);
    mainLayout->addWidget(addCashOutRowButton);

    connect(addCashInRowButton, &QPushButton::clicked, this, [=]() {
        int row = cashInLayout->rowCount();
        cashInLayout->addWidget(new QLabel("Source: ", this), row, 0);
        cashInLayout->addWidget(new QLineEdit(this), row, 1);
        cashInLayout->addWidget(new QDoubleSpinBox(this), row, 2); // Amount
    });

    connect(addCashOutRowButton, &QPushButton::clicked, this, [=]() {
        int row = cashOutLayout->rowCount();
        cashOutLayout->addWidget(new QLabel("Expense: ", this), row, 0);
        cashOutLayout->addWidget(new QLineEdit(this), row, 1);
        cashOutLayout->addWidget(new QDoubleSpinBox(this), row, 2); // Amount
    });

    // Totals Section
    QLabel *totalCashInLabel = new QLabel("Total Cash In: $0.00", this);
    QLabel *totalCashOutLabel = new QLabel("Total Cash Out: $0.00", this);
    QLabel *cashLeftLabel = new QLabel("Cash Left in Hand: $0.00", this);

    totalCashInLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2e7d32;"); // Green
    totalCashOutLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #c62828;"); // Red
    cashLeftLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #1565c0;"); // Blue

    mainLayout->addWidget(totalCashInLabel);
    mainLayout->addWidget(totalCashOutLabel);
    mainLayout->addWidget(cashLeftLabel);

    // // Save Button
    // saveButton = new QPushButton("Save to Database", this);
    // saveButton->setEnabled(false);
    // mainLayout->addWidget(saveButton);

    // connect(saveButton, &QPushButton::clicked, this, &CashInCashOut::saveToDatabase);



    //     // Total label
    //     totalLabel = new QLabel("Total Investment: 0", this);
    //     totalLabel->setStyleSheet(
    //     "QLabel {"
    //     "font-size: 18px;"
    //     "font-weight: bold;"
    //     "color: #1b5e20;"
    //     "}");
    //     totalLabel->setAlignment(Qt::AlignLeft);
    //     mainLayout->addWidget(totalLabel);

    //     // Save button
    //     saveButton = new QPushButton("Save to Database", this);
    //     saveButton->setEnabled(false); // Initially disabled
    //     mainLayout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &CashInCashOut::saveToDatabase);
}

void CashInCashOut::addEntryRow() {
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
    connect(amountInput, &QLineEdit::textChanged, this, &CashInCashOut::updateTotal);
    connect(amountInput, &QLineEdit::editingFinished, this, &CashInCashOut::updateTotal);
}

void CashInCashOut::updateTotal() {
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

    totalLabel->setText("Current total portfolio: " + QString::number(totalInvestment, 'f', 2));
    saveButton->setEnabled(valid && totalInvestment > 0);
}

bool CashInCashOut::saveEntry(const QString &type, double amount, const QString &month) {
    QSqlQuery query;
    QString user = "admin";
    QString comment = "";
    query.prepare("INSERT INTO cashin (user, month, source, amount, comment) VALUES (:user, :month, :source, :amount, :comment)");
    query.bindValue(":user", user);
    query.bindValue(":month", month);
    query.bindValue(":type", type);
    query.bindValue(":amount", amount);
    query.bindValue(":comment", comment);

    return query.exec();
}

void CashInCashOut::saveToDatabase() {
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

void CashInCashOut::updateFormForMonth() {
    QString selectedMonth = monthSelector->date().toString("MMMM yyyy");
    monthHeaderLabel->setText("Selected Month: " + selectedMonth);
}
