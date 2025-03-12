#ifndef ENTRYGROUPBOX_H
#define ENTRYGROUPBOX_H

#include "entryrow.h"
#include <QGroupBox>
#include <QScrollArea>

class EntryGroupBox : public QGroupBox {
    Q_OBJECT

public:
    explicit EntryGroupBox(const QString &title, QWidget *parent = nullptr);

    void addEntryRow(unsigned int id = 0, const QString &investmentType = "", const QString &amount = "", const QString &comment = "");
    void loadEntryRow(unsigned int id = 0, const QString &investmentType = "", const QString &amount = "", const QString &comment = "");
    void clearRows();
    QList<EntryRow *> getEntries();
    QList<unsigned int> getRemovedEntries();

signals:
    void updateRow();
    void updateTotal();

private:
    QList<EntryRow *> rowList;
    QList<unsigned int> removeRowList;
    QScrollArea *scrollArea;
    QVBoxLayout *entryLayout; // Use QVBoxLayout for rows
    QWidget *entryContainer;
    QPushButton *addButton;

    void setupUI();
};

#endif // ENTRYGROUPBOX_H