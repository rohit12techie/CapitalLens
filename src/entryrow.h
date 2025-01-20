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

    // Setters for input fields
    void setInvestmentType(const QString &type);
    void setAmount(const QString &amount);
    void setComment(const QString &comment);
    void onActionButtonClicked();
    void toggleButtonSign();

signals:
    void removeRequested(EntryRow *row); // Signal emitted when the remove button is clicked
    void addRequested();

private:
    QLineEdit *investmentTypeEdit;
    QLineEdit *amountEdit;
    QLineEdit *commentEdit;
    QPushButton *actionButton;
    QHBoxLayout *layout;

    void setupUI();
};

#endif // ENTRYROW_H
