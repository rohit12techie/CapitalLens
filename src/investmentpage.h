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
#include "monthselectorlayout.h"
#include "entrygroupbox.h"
#include "totallabel.h"

class InvestmentForm : public QWidget {
    Q_OBJECT

public:
    explicit InvestmentForm(QWidget *parent = nullptr);

private slots:
    void initializeForm();
    void addEntryRow();
    void updateTotal();
    void saveToDatabase();
    void loadEntries();

public slots:
    void onCalenderChange();

private:
    EntryGroupBox *entryGroupBox;
    TotalLabel* totalLabel;
    QPushButton *saveButton;
    MonthSelector* yearMonthSelector;
    bool saveEntry(const QString &type, double amount, const QString &month, const QString &comment); // Saves an individual entry
};

#endif // INVESTMENTFORM_H
