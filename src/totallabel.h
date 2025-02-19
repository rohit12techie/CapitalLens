#ifndef TOTAL_LABEL_H
#define TOTAL_LABEL_H

#include <QLabel>
#include <QGroupBox>

class TotalLabel : public QGroupBox
{
    Q_OBJECT

public:
    explicit TotalLabel(QWidget *parent = nullptr, const QString &title = "Total");

signals:

public slots:
    void setTitle(const QString& title);
    void setTotal(const QString& total);
    void setupUI();
private:
    QLabel* m_label;
    void updateLabel();
};

#endif // TOTAL_LABEL_H