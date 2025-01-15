// Dashboard.h
#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>

class Dashboard : public QWidget {
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);

private:
    QListWidget *navigation;
    QStackedWidget *stackedWidget;
};

#endif // DASHBOARD_H