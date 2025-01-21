#include "savebutton.h"

SaveButton::SaveButton(QWidget *parent)
    : QPushButton("Save to Database", parent) {

    setFixedSize(200, 50);
    setStyleSheet(
        "QPushButton {"
        "font-weight: bold;"
        "font-size: 12px;" /* Larger font size for visibility */
        "padding: 8px 16px;" /* Extra padding for better aesthetics */
        "text-align: center;" /* Center the text */
        "}"
    );
    setEnabled(false);
}
