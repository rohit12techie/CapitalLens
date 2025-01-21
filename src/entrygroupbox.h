#include "entryrow.h"
#include <QGroupBox>
#include <QScrollArea>

class EntryGroupBox : public QGroupBox {
    Q_OBJECT

public:
    explicit EntryGroupBox(const QString &title, QWidget *parent = nullptr);

    void addEntryRow(const QString &investmentType = "", const QString &amount = "", const QString &comment = "");
    void loadEntryRow(const QString &investmentType = "", const QString &amount = "", const QString &comment = "");
    void clearRows();
    QList<EntryRow *> getEntries();

signals:
    void updateRow();
    void updateTotal();

private:
    QList<EntryRow *> rowList;
    QScrollArea *scrollArea;
    QVBoxLayout *entryLayout; // Use QVBoxLayout for rows
    QWidget *entryContainer;
    QPushButton *addButton;

    void setupUI();
};
