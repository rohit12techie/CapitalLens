#include <QDialog>
#include <QLineEdit>

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    bool isAuthenticated() const;

private slots:
    void validateLogin();

private:
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
    bool authenticated = false;
};
