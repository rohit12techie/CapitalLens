#ifndef MONTHSELECTOR_H
#define MONTHSELECTOR_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class MonthSelector : public QWidget {
    Q_OBJECT
    QStringList months = {"January", "February", "March", "April", "May", "June",
                          "July", "August", "September", "October", "November", "December"};
public:
    explicit MonthSelector(QWidget *parent = nullptr);
    ~MonthSelector();
    QString getSelectedMonth();
    int getSelectedYear();

private slots:
    void handleYearChange(int delta);
    void handleMonthClick();

signals:
    void calenderChanged();

private:
    int currentYear;
    QLabel *yearLabel;
    QGridLayout *monthGrid;
    QPushButton *activeMonth;
};

#endif // MONTHSELECTOR_H
