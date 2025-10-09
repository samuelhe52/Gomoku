//
// Created by Samuel He on 2025/10/9.
//

#include "ColorChooserWidget.h"

ColorChooserWidget::ColorChooserWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setupUI();
}

void ColorChooserWidget::setupUI() {
    // Main layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    
    // Container widget with background
    auto *container = new QWidget(this);
    container->setFixedSize(400, 160);
    container->setStyleSheet(
        "QWidget {"
        "  background-color: rgba(40, 40, 40, 230);"
        "  border: 2px solid rgba(255, 255, 255, 200);"
        "  border-radius: 15px;"
        "}"
    );
    
    containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(20, 10, 20, 10);
    containerLayout->setSpacing(10);
    
    // Title label
    titleLabel = new QLabel("Choose Your Color", container);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "  color: white;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  background: transparent;"
        "  border: none;"
        "}"
    );
    containerLayout->addWidget(titleLabel);

    // Buttons layout
    buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(40);
    
    // Black button
    blackButton = new QPushButton("Black", container);
    blackButton->setFixedSize(150, 50);
    blackButton->setCursor(Qt::PointingHandCursor);
    blackButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgb(30, 30, 30);"
        "  color: white;"
        "  border: 2px solid rgb(200, 200, 200);"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgb(50, 50, 50);"
        "}"
        "QPushButton:pressed {"
        "  background-color: rgb(10, 10, 10);"
        "}"
    );
    
    // White button
    whiteButton = new QPushButton("White", container);
    whiteButton->setFixedSize(150, 50);
    whiteButton->setCursor(Qt::PointingHandCursor);
    whiteButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgb(220, 220, 220);"
        "  color: black;"
        "  border: 2px solid rgb(100, 100, 100);"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgb(240, 240, 240);"
        "}"
        "QPushButton:pressed {"
        "  background-color: rgb(200, 200, 200);"
        "}"
    );
    
    buttonsLayout->addWidget(blackButton);
    buttonsLayout->addWidget(whiteButton);
    containerLayout->addLayout(buttonsLayout);
    containerLayout->addSpacing(10);
    
    mainLayout->addWidget(container);
    
    // Connect button signals
    connect(blackButton, &QPushButton::clicked, this, [this]() {
        emit colorChosen(true); // Player is black (goes first)
    });
    
    connect(whiteButton, &QPushButton::clicked, this, [this]() {
        emit colorChosen(false); // Player is white (AI goes first)
    });
}
