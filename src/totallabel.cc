#include "totallabel.h"
#include <QHBoxLayout>

TotalLabel::TotalLabel(QWidget *parent, const QString &title) : QGroupBox(title, parent) {
    setupUI();
}

void TotalLabel::setupUI() {
    m_label = new QLabel("0", this);
    m_label->setStyleSheet(
        "QLabel {"
        "font-size: 18px;"
        "font-weight: bold;"
        "color: #1b5e20;"
        "}"
    );
    m_label->setAlignment(Qt::AlignCenter);

    setLayout(new QHBoxLayout(this));
    layout()->addWidget(m_label);

    setStyleSheet(
        "QGroupBox {"
        "border: 2px solid #cccccc;"
        "border-radius: 8px;"
        "margin-top: 10px;"
        "padding: 10px;"
        "font-weight: bold;"
        "background-color: #f9f9f9;"
        "}"
        "QGroupBox::title {"
        "subcontrol-origin: margin;"
        "subcontrol-position: top center;"
        "padding: 0 5px;"
        "}"
    );
}

void TotalLabel::setTitle(const QString& title) {
    setTitle(title);
}

void TotalLabel::setTotal(const QString& total) {
    m_label->setText(total);
}