#ifndef PORTFOLIO_H
#define PORTFOLIO_H

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
#include "totallabel.h"
#include "savebutton.h"
#include "entryrow.h"
#include "entrygroupbox.h"

class Portfolio : public QWidget {
    Q_OBJECT

public:
    explicit Portfolio(QWidget *parent = nullptr);

private slots:
    void initializeForm();
    void addEntryRow();
    void updateTotal();
    void saveToDatabase();
    void loadEntries();
    void enableSaveButton();
    void disableSaveButton();
    void onCalenderChange();

private:
    EntryGroupBox *entryGroupBox;
    TotalLabel* totalLabel;
    SaveButton *saveButton;
    MonthSelector* yearMonthSelector;
    bool saveEntry(const QString &type, double amount, const QString &month, const QString &comment); // Saves an individual entry
};

#endif // PORTFOLIO_H
