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
#include "savebutton.h"
#include "dbservice.h"

class investmentEntry {
public:
    unsigned int id;
    QString type;
    QString amount;
    QString month;
    QString comment;

    investmentEntry() = default;
    investmentEntry(unsigned int id, const QString &type, const QString &amount, const QString &month, const QString &comment)
        : id(id), type(type), amount(amount), month(month), comment(comment) {}
};

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
    void enableSaveButton();
    void disableSaveButton();
    void onCalenderChange();
    // void loadPersistentEntries();

private:
    EntryGroupBox* entryGroupBox;
    TotalLabel* totalLabel;
    SaveButton* saveButton;
    MonthSelector* yearMonthSelector;
    bool saveEntry(const QString &type, double amount, const QString &month, const QString &comment); // Saves an individual entry
    DBService* dbservice;
    QMap<unsigned int, investmentEntry> investmentEntries;
};

#endif // INVESTMENTFORM_H
