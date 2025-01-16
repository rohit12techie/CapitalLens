// Dashboard.cc
#include "dashboardpage.h"
#include "investmentpage.h"
#include "portfolio.h"
#include "cashincashout.h"
#include "reports.h"
#include <QVBoxLayout>
#include <QLabel>



Dashboard::Dashboard(QWidget *parent) : QWidget(parent) {
    // Navigation panel
    navigation = new QListWidget(this);
    navigation->addItem("Reports");
    navigation->addItem("Investment");
    navigation->addItem("Cash In/Cash Out");
    navigation->addItem("Current Portfolio");

    // Style the navigation
    navigation->setStyleSheet(
        "QListWidget {"
        "  background-color: lightblue;"
        "  border: none;"
        "  font-size: 16px;"
        "  padding: 5px;"
        "}"
        "QListWidget::item {"
        "  height: 40px;"
        "  margin: 5px;"
        "  background-color: #f0f8ff;"
        "  color: #333333;"
        "  border-radius: 5px;"
        "  padding: 8px;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #0078D7;"
        "  color: white;"
        "  border: 5px solid #005BB5;"
        "}"
    );


    connect(navigation, &QListWidget::currentRowChanged, [this](int row) {
        stackedWidget->setCurrentIndex(row);
    });

    // Stacked widget for pages
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(new Reports(this));
    stackedWidget->addWidget(new InvestmentForm(this));
    stackedWidget->addWidget(new CashInCashOut(this));
    stackedWidget->addWidget(new Portfolio(this));

    QHBoxLayout *layout = new QHBoxLayout(this);
    navigation->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(navigation);
    layout->addWidget(stackedWidget);

    setLayout(layout);
}


