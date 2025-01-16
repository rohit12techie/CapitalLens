#include "login.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Login");
    setFixedSize(300, 150);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Username label and input
    QLabel *usernameLabel = new QLabel("Username:", this);
    layout->addWidget(usernameLabel);

    usernameInput = new QLineEdit(this);
    layout->addWidget(usernameInput);

    // Password label and input
    QLabel *passwordLabel = new QLabel("Password:", this);
    layout->addWidget(passwordLabel);

    passwordInput = new QLineEdit(this);
    passwordInput->setEchoMode(QLineEdit::Password); // Hide password text
    layout->addWidget(passwordInput);

    // Login button
    QPushButton *loginButton = new QPushButton("Login", this);
    layout->addWidget(loginButton);

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::validateLogin);
}

bool LoginDialog::isAuthenticated() const { return authenticated; }

void LoginDialog::validateLogin() {
    QString username = usernameInput->text();
    QString password = passwordInput->text();

    // Example: Simple hardcoded check
    if (username == "admin" && password == "password") {
        authenticated = true;
        accept(); // Close the dialog and return success
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password!");
    }
}
