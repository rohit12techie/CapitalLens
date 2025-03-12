#include "entrygroupbox.h"
#include <QDebug>

EntryGroupBox::EntryGroupBox(const QString &title, QWidget *parent)
    : QGroupBox(title, parent), scrollArea(new QScrollArea(this)) {
    setupUI();
}

void EntryGroupBox::setupUI() {
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

    entryContainer = new QWidget();
    entryLayout = new QVBoxLayout(entryContainer);
    entryLayout->setAlignment(Qt::AlignTop);
    entryContainer->setLayout(entryLayout);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(entryContainer);
    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBoxLayout->addWidget(scrollArea);
    this->setLayout(groupBoxLayout);
}

void EntryGroupBox::loadEntryRow(unsigned int id, const QString &investmentType, const QString &amount, const QString &comment) {
    auto *row = new EntryRow(this);

    // Set initial values
    row->setId(id);
    row->setInvestmentType(investmentType);
    row->setAmount(amount);
    row->setComment(comment);
    entryLayout->addWidget(row);
    row->toggleButtonSign();
    rowList.append(row);

    // Connect the remove signal to delete the row
    connect(row, &EntryRow::removeRequested, this, [this, row]() {
        removeRowList.append(row->getId());
        entryLayout->removeWidget(row);
        row->deleteLater();
        rowList.removeOne(row);
        emit updateRow();
        emit updateTotal();
    });
}

void EntryGroupBox::addEntryRow(unsigned int id, const QString &investmentType, const QString &amount, const QString &comment) {
    auto *row = new EntryRow(this);

    // Set initial values
    row->setId(id);
    row->setInvestmentType(investmentType);
    row->setAmount(amount);
    row->setComment(comment);
    entryLayout->addWidget(row);
    emit updateTotal();

    // Connect the remove signal to delete the row
    connect(row, &EntryRow::removeRequested, this, [this, row]() {
        removeRowList.append(row->getId());
        entryLayout->removeWidget(row);
        row->deleteLater();
        rowList.removeOne(row);
        emit updateRow();
        emit updateTotal();

        qDebug() << "Removed Rows: " << removeRowList.size();
    });

    connect(row, &EntryRow::addRequested, this, [this, row]() {
        rowList.append(row);
        addEntryRow();
        emit updateRow();
        emit updateTotal();
    });
}

void EntryGroupBox::clearRows() {
    while (entryLayout->count() > 0) {
        QLayoutItem *item = entryLayout->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    rowList.clear();
}

QList<EntryRow*> EntryGroupBox::getEntries() {
    return rowList;
}

QList<unsigned int> EntryGroupBox::getRemovedEntries() {
    return removeRowList;
}