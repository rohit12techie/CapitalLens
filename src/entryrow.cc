#include "entryrow.h"

EntryRow::EntryRow(QWidget *parent)
    : QWidget(parent), investmentTypeEdit(new QLineEdit(this)),
      amountEdit(new QLineEdit(this)), commentEdit(new QLineEdit(this)),
      actionButton(new QPushButton("+", this)) {

    setupUI();

    investmentTypeEdit->setPlaceholderText("Investment Type");
    amountEdit->setPlaceholderText("Amount");
    commentEdit->setPlaceholderText("Comment");

    connect(actionButton, &QPushButton::clicked, this, &EntryRow::onActionButtonClicked);
}


void EntryRow::toggleButtonSign() {
    if (actionButton->text() == "+") {
        actionButton->setText("-");
    }
}

void EntryRow::onActionButtonClicked() {
    if (actionButton->text() == "+") {
        actionButton->setText("-");
        emit addRequested();
    } else {
        emit removeRequested(this);
    }
}

void EntryRow::setupUI() {
    layout = new QHBoxLayout(this);
    layout->addWidget(investmentTypeEdit);
    layout->addWidget(amountEdit);
    layout->addWidget(commentEdit);
    layout->addWidget(actionButton);
    layout->setContentsMargins(0, 0, 0, 0); // Optional: reduce margins
    setLayout(layout);
}

QString EntryRow::getInvestmentType() const {
    return investmentTypeEdit->text();
}

QString EntryRow::getAmount() const {
    return amountEdit->text();
}

QString EntryRow::getComment() const {
    return commentEdit->text();
}

void EntryRow::setInvestmentType(const QString &type) {
    investmentTypeEdit->setText(type);
}

void EntryRow::setAmount(const QString &amount) {
    amountEdit->setText(amount);
}

void EntryRow::setComment(const QString &comment) {
    commentEdit->setText(comment);
}
