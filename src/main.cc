// Main.cpp
#include "dashboardpage.h"
#include "login.h"
#include "dbmanager.h"
#include <QApplication>
#include <QDebug>

void setupDatabase() {
    // Set database parameters
    DatabaseManager::instance().setDatabaseParameters(
        "QSQLITE",       // Database driver
        "",              // Host (not required for SQLite)
        0,               // Port (not required for SQLite)
        "investments.db", // Database name
        "",              // Username
        ""               // Password
    );
    qDebug() << "Database parameters set";
    DatabaseManager::instance().initializeDatabase();
    qDebug() << "Database initialized";
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // LoginDialog loginDialog;
    // if (loginDialog.exec() != QDialog::Accepted) {
    //     return 0; // Exit if login failed or canceled
    // }

    setupDatabase();

    Dashboard dashboard;
    dashboard.setWindowTitle("Financial Tracker");
    dashboard.resize(800, 600);
    dashboard.show();

    return app.exec();
}