//
// Created by Samuel He on 2025/10/2.
//

#pragma once

#include "BoardWidget.h"
#include "ColorChooserWidget.h"
#include "../Models/GameManager.h"

#include <QWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QPainter>
#include <QStackedWidget>

class GameWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit GameWidget(QWidget *parent = nullptr);
    
private:
    BoardWidget *board;
    QVBoxLayout *overlayLayout;
    ColorChooserWidget *colorChooser;
    QStackedWidget *boardStack;
    QPushButton *resetButton;
    QBoxLayout *mainLayout;
    QBoxLayout *buttonLayout;
    GameManager gameManager;
    
    void setupUI();
    void handleHumanMove(BoardPosition position);
    void startGame(bool playerIsBlack);
    void requestAiMove();
    
    const QString button_style_sheet = "QPushButton {"
            "  background-color: #6B4423;"
            "  color: #FFF8E7;"
            "  border-radius: 22px;"
            "  font-size: 15px;"
            "  font-weight: bold;"
            "  padding: 8px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #8B5A2B;"
            "}"
            "QPushButton:pressed {"
            "  background-color: #4A2F19;"
            "}";
};