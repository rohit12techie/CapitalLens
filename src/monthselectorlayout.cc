#include "monthselectorlayout.h"

MonthYearSelectorLayout::MonthYearSelectorLayout(QWidget *parent)
    : QGroupBox("Select Year and Month", parent),
      yearMonthSelector(new MonthSelector(this)) {

    this->setStyleSheet(
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

    setAlignment(Qt::AlignCenter);
    setFixedSize(300, 200); // Adjust size as needed
    QVBoxLayout *containerLayout = new QVBoxLayout(this);
    containerLayout->addWidget(yearMonthSelector);
    setLayout(containerLayout);
}

MonthSelector *MonthYearSelectorLayout::getMonthSelector() const {
    return yearMonthSelector;
}
