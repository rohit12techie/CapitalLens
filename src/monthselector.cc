#include "monthselector.h"
#include <QDate> // For fetching the current date

MonthSelector::MonthSelector(QWidget *parent)
    : QWidget(parent), activeMonth(nullptr) {
    // Get the current date

    //this->setStyleSheet("border: 2px solid #3498db; border-radius: 10px; padding: 5px;");

    QDate currentDate = QDate::currentDate();
    currentYear = currentDate.year();
    int currentMonth = currentDate.month();

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Year navigation layout
    QHBoxLayout *yearLayout = new QHBoxLayout();
    QPushButton *prevYearButton = new QPushButton("<");
    QPushButton *nextYearButton = new QPushButton(">");
    yearLabel = new QLabel(QString::number(currentYear), this);
    yearLabel->setAlignment(Qt::AlignCenter);

    yearLayout->addWidget(prevYearButton);
    yearLayout->addWidget(yearLabel);
    yearLayout->addWidget(nextYearButton);

    // Connect year navigation buttons
    connect(prevYearButton, &QPushButton::clicked, [this]() { handleYearChange(-1); });
    connect(nextYearButton, &QPushButton::clicked, [this]() { handleYearChange(1); });

    // Month grid layout
    monthGrid = new QGridLayout();

    for (int i = 0; i < months.size(); ++i) {
        QPushButton *monthButton = new QPushButton(months[i]);
        monthGrid->addWidget(monthButton, i / 3, i % 3);

        // Connect month button click
        connect(monthButton, &QPushButton::clicked, this, &MonthSelector::handleMonthClick);

        // Highlight the current month if it matches
        if (currentYear == QDate::currentDate().year() && (i + 1) == currentMonth) {
            activeMonth = monthButton;
            activeMonth->setStyleSheet("background-color: #ff6e6e; color: white; font-weight: bold;");
        }
    }

    // Add layouts to the main layout
    mainLayout->addLayout(yearLayout);
    mainLayout->addLayout(monthGrid);
}

MonthSelector::~MonthSelector() {}

void MonthSelector::handleYearChange(int delta) {
    currentYear += delta;
    yearLabel->setText(QString::number(currentYear));

    // Update the active month highlighting
    QDate currentDate = QDate::currentDate();
    int currentMonth = currentDate.month();

    for (int i = 0; i < monthGrid->count(); ++i) {
        QLayoutItem *item = monthGrid->itemAt(i);
        if (item) {
            QPushButton *monthButton = qobject_cast<QPushButton *>(item->widget());
            if (monthButton) {
                // Reset style for all buttons
                monthButton->setStyleSheet("");

                // Highlight the active month for the current year
                if ((i + 1) == currentMonth) {
                    activeMonth = monthButton;
                    activeMonth->setStyleSheet("background-color: #ff6e6e; color: white; font-weight: bold;");
                }
            }
        }
    }

    emit calenderChanged();
}

void MonthSelector::handleMonthClick() {
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    if (clickedButton) {
        // Reset the style of the previously active button
        if (activeMonth) {
            activeMonth->setStyleSheet("");
        }

        // Highlight the clicked button
        activeMonth = clickedButton;
        activeMonth->setStyleSheet("background-color: #ff6e6e; color: white; font-weight: bold;");
        emit calenderChanged();
    }
}

QString MonthSelector::getSelectedMonth() {
    if (activeMonth) {
        return activeMonth->text();
    }
    return months[0];
}

int MonthSelector::getSelectedYear() {
    return currentYear;
}
