#ifndef TOTAL_LABEL_H
#define TOTAL_LABEL_H

#include <QLabel>
#include <QGroupBox>

class TotalLabel : public QGroupBox
{
    Q_OBJECT

public:
    explicit TotalLabel(QWidget *parent = nullptr);

signals:

public slots:
    void setTotal(const QString& total);

private:
    QLabel* m_label;

    void updateLabel();
};

#endif // TOTAL_LABEL_H