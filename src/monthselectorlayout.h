#ifndef MONTHYEARSELECTORLAYOUT_H
#define MONTHYEARSELECTORLAYOUT_H

#include <QVBoxLayout>
#include <QGroupBox>
#include "monthselector.h" // Assuming this is your custom widget

class MonthYearSelectorLayout : public QGroupBox {
    Q_OBJECT

public:
    explicit MonthYearSelectorLayout(QWidget *parent = nullptr);

    MonthSelector *getMonthSelector() const;

private:
    MonthSelector *yearMonthSelector;
};

#endif // MONTHYEARSELECTORLAYOUT_H
