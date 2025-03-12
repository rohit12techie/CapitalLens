#ifndef ENTRYROW_H
#define ENTRYROW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

class EntryRow : public QWidget {
    Q_OBJECT

public:
    explicit EntryRow(QWidget *parent = nullptr);

    // Getters for input fields
    QString getInvestmentType() const;
    QString getAmount() const;
    QString getComment() const;
    unsigned int getId();
    bool isUpdated() const;

    // Setters for input fields
    void setId(unsigned int id);
    void setInvestmentType(const QString &type);
    void setAmount(const QString &amount);
    void setComment(const QString &comment);
    void onActionButtonClicked();
    void toggleButtonSign();
    void rowModified();

signals:
    void removeRequested(EntryRow *row); // Signal emitted when the remove button is clicked
    void addRequested();
    void updateRequested(EntryRow *row);

private:
    unsigned int id;
    QLineEdit *investmentTypeEdit;
    QLineEdit *amountEdit;
    QLineEdit *commentEdit;
    QPushButton *actionButton;
    QHBoxLayout *layout;
    bool isModified;

    void setupUI();
};

#endif // ENTRYROW_H
