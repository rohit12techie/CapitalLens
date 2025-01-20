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
    mainLayout->addWidget(entryGroupBox);

    initializeForm();

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

    connect(yearMonthSelector, &MonthSelector::calenderChanged, this, &InvestmentForm::onCalenderChange);
}

void InvestmentForm::initializeForm() {
    loadEntries(); // Load data from the database
    addEntryRow(); // Add a blank row for new entry
}

void InvestmentForm::loadEntries() {
    // Clear any existing rows
    clearRows();

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
            addEntryRow(type, amount, comment);
        }
    } else {
        qWarning() << "Database connection is not open!";
    }

    QString connectionName = QString("DB_%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    DatabaseManager::instance().releaseConnection(connectionName);
}

void InvestmentForm::addEntryRow(const QString &type, const QString &amount, const QString &comment) {
    int row = entryLayout->rowCount();

    // Investment type dropdown or custom input
    QComboBox *typeInput = new QComboBox(this);
    typeInput->setEditable(true); // Allow custom types
    typeInput->addItems({"Stocks", "Bonds", "Mutual Funds", "Real Estate", "FD", "Others"});
    typeInput->setCurrentText(type);
    entryLayout->addWidget(typeInput, row, 0);
    typeInputs.push_back(typeInput);

    // Investment amount input
    QLineEdit *amountInput = new QLineEdit(this);
    amountInput->setPlaceholderText("Enter amount");
    amountInput->setText(amount);
    entryLayout->addWidget(amountInput, row, 1);
    amountInputs.push_back(amountInput);

    // Comment input
    QLineEdit *commentInput = new QLineEdit(this);
    commentInput->setPlaceholderText("Comment");
    commentInput->setText(comment);
    entryLayout->addWidget(commentInput, row, 2);
    commentInputs.push_back(commentInput);

    //updateTotal();
    // Connect signals to dynamically update total
    connect(amountInput, &QLineEdit::textChanged, this, &InvestmentForm::updateTotal);
    connect(amountInput, &QLineEdit::editingFinished, this, &InvestmentForm::updateTotal);

    // Remove row button for loaded entries
    QPushButton *removeRowButton = new QPushButton("-", this);
    entryLayout->addWidget(removeRowButton, row, 3);

    // Connect the remove button to remove the corresponding row
    connect(removeRowButton, &QPushButton::clicked, this, [this, row]() {
        removeEntryRow(row);
    });
}

// void InvestmentForm::removeEntryRow(int row) {
//     // Remove widgets for the row
//     QLayoutItem *typeItem = entryLayout->itemAtPosition(row, 0);
//     QLayoutItem *amountItem = entryLayout->itemAtPosition(row, 1);
//     QLayoutItem *commentItem = entryLayout->itemAtPosition(row, 2);
//     QLayoutItem *buttonItem = entryLayout->itemAtPosition(row, 3);

//     delete typeItem->widget();
//     delete amountItem->widget();
//     delete commentItem->widget();
//     delete buttonItem->widget();

//     // Remove the row from the layout
//     for (int col = 0; col < 4; ++col) {
//         entryLayout->removeItem(entryLayout->itemAtPosition(row, col));
//     }

//     // Update the database to delete the entry
//     // (Here, you'd write code to delete the entry from the database based on the row)
//     qDebug() << "Row" << row << "removed and database updated.";
//     updateTotal();
// }

void InvestmentForm::removeEntryRow(int row) {
    // Ensure the row index is valid
    if (row < 0 || row >= typeInputs.size()) {
        qWarning() << "Invalid row index for removal:" << row;
        return;
    }

    // Remove and delete widgets for the row
    QComboBox *typeInput = typeInputs[row];
    QLineEdit *amountInput = amountInputs[row];
    QLineEdit *commentInput = commentInputs[row];

    entryLayout->removeWidget(typeInput);
    entryLayout->removeWidget(amountInput);
    entryLayout->removeWidget(commentInput);

    delete typeInput;
    delete amountInput;
    delete commentInput;

    // Remove the row's button (if any)
    QLayoutItem *buttonItem = entryLayout->itemAtPosition(row, 3);
    if (buttonItem) {
        QWidget *buttonWidget = buttonItem->widget();
        entryLayout->removeWidget(buttonWidget);
        delete buttonWidget;
    }

    // Erase the corresponding elements from the vectors
    typeInputs.erase(typeInputs.begin() + row);
    amountInputs.erase(amountInputs.begin() + row);
    commentInputs.erase(commentInputs.begin() + row);

    // Adjust remaining rows in the layout
    for (int i = row + 1; i <= entryLayout->rowCount(); ++i) {
        for (int col = 0; col < 4; ++col) {
            QLayoutItem *item = entryLayout->itemAtPosition(i, col);
            if (item) {
                QWidget *widget = item->widget();
                entryLayout->removeWidget(widget);
                entryLayout->addWidget(widget, i - 1, col);
            }
        }
    }

    updateTotal();

    qDebug() << "Row" << row << "removed successfully.";
}

void InvestmentForm::clearRows() {
    // Clear all dynamically created rows
    for (auto *typeInput : typeInputs) {
        delete typeInput;
    }
    for (auto *amountInput : amountInputs) {
        delete amountInput;
    }
    for (auto *commentInput : commentInputs) {
        delete commentInput;
    }

    typeInputs.clear();
    amountInputs.clear();
    commentInputs.clear();

    // Clear the layout
    while (entryLayout->count()) {
        QLayoutItem *item = entryLayout->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}



void InvestmentForm::addEntryRow() {
    qDebug() << "Adding new row XXXXXXXXXXXXXXXXXXXXXXXXXX";

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

    QLineEdit *commentInput = new QLineEdit(this);
    commentInput->setPlaceholderText("Comment");
    entryLayout->addWidget(commentInput, row, 2);
    commentInputs.push_back(commentInput);

    // Remove row button for loaded entries
    if(row - 1 > 1){
        QPushButton *removeRowButton = new QPushButton("-", this);
        entryLayout->addWidget(removeRowButton, row - 1, 3) ;
    }

    QPushButton *addRowButton = new QPushButton("+", this);
    addRowButton->setEnabled(false); // Initially disabled
    entryLayout->addWidget(addRowButton, row, 3);

    // Connect signal to dynamically update total
    connect(amountInput, &QLineEdit::textChanged, this, &InvestmentForm::updateTotal);
    connect(amountInput, &QLineEdit::editingFinished, this, &InvestmentForm::updateTotal);
    // Connect signals to enable addRowButton only if all fields are filled
    auto enableAddRowButton = [addRowButton, typeInput, amountInput, commentInput]() {
        addRowButton->setEnabled(!typeInput->currentText().isEmpty() && !amountInput->text().isEmpty() && !commentInput->text().isEmpty());
    };

    connect(typeInput, &QComboBox::editTextChanged, this, enableAddRowButton);
    connect(amountInput, &QLineEdit::textChanged, this, enableAddRowButton);
    connect(commentInput, &QLineEdit::textChanged, this, enableAddRowButton);

    // Connect signal to add a new row
    connect(addRowButton, &QPushButton::clicked, this, [this]() {
        addEntryRow();
    });
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

    for (int i = 0; i < typeInputs.size(); ++i) {
        bool ok;
        QString type = typeInputs[i]->currentText();
        QString comment = commentInputs[i]->text();
        double amount = amountInputs[i]->text().toDouble(&ok);

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

void InvestmentForm::updateFormForMonth() {
    QString selectedMonth = monthSelector->date().toString("MMMM yyyy");
    monthHeaderLabel->setText("Selected Month: " + selectedMonth);
}


void InvestmentForm::onCalenderChange() {
    qWarning() << "Signals Month changed!";
    initializeForm();
}