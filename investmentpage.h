#ifndef INVESTMENTFORM_H
#define INVESTMENTFORM_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDate>
#include <QDateEdit>
#include <vector>
#include "monthselector.h"

class InvestmentForm : public QWidget {
    Q_OBJECT

public:
    explicit InvestmentForm(QWidget *parent = nullptr);

private slots:
    void addEntryRow();            // Adds a new row for investment type and amount
    void updateTotal();            // Updates the total investment dynamically
    void saveToDatabase();         // Saves all entries to the database
    void updateFormForMonth();     // Updates form header for selected month

private:
    QGridLayout *entryLayout;      // Layout for dynamic rows
    QLabel *totalLabel;            // Label to display total investment
    QPushButton *addRowButton;     // Button to add new rows
    QPushButton *saveButton;       // Button to save data to DB
    QDateEdit *monthSelector;      // Dropdown for selecting month/year
    QLabel *monthHeaderLabel;      // Label to show the selected month
    MonthSelector* yearMonthSelector; // Year-Month selector widget
    QString currentMonthString;    // String representation of the current month

    std::vector<QComboBox*> typeInputs;    // Store investment type inputs
    std::vector<QLineEdit*> amountInputs;  // Store investment amount inputs

    double totalInvestment = 0;   // Tracks the total investment
    QDate currentMonth;           // Tracks the currently selected month
    bool saveEntry(const QString &type, double amount, const QString &month); // Saves an individual entry
};

#endif // INVESTMENTFORM_H
