#ifndef SAVEBUTTON_H
#define SAVEBUTTON_H

#include <QPushButton>

class SaveButton : public QPushButton {
    Q_OBJECT

public:
    explicit SaveButton(QWidget *parent = nullptr);

// signals:
//     void saveClicked();

// private slots:
//     void handleClick();
};

#endif // SAVEBUTTON_H