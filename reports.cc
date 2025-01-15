#include "reports.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSizePolicy>

Reports::Reports(QWidget *parent) : QWidget(parent) {
    // Create a layout for the reports page
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Title label
    QLabel *titleLabel = new QLabel("Reports Page", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #1b5e20;"
        "}");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // Placeholder text
    QLabel *placeholderLabel = new QLabel("Reports will be available here soon!", this);
    placeholderLabel->setStyleSheet(
        "QLabel {"
        "font-size: 18px;"
        "color: #333333;"
        "}");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(placeholderLabel);

    // Set the layout for the reports page
    setLayout(layout);
}