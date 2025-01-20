#include "monthselectorlayout.h"

MonthYearSelectorLayout::MonthYearSelectorLayout(QWidget *parent)
    : QGroupBox("Select Year and Month", parent),
      yearMonthSelector(new MonthSelector(this)) {

    setAlignment(Qt::AlignCenter);
    setFixedSize(300, 200); // Adjust size as needed
    QVBoxLayout *containerLayout = new QVBoxLayout(this);
    containerLayout->addWidget(yearMonthSelector);
    setLayout(containerLayout);
}

MonthSelector *MonthYearSelectorLayout::getMonthSelector() const {
    return yearMonthSelector;
}
