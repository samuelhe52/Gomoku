//
// Created by Samuel He on 2025/10/2.
//

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QPainter>

class BoardWidget;
class QPushButton;

class GameWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit GameWidget(QWidget *parent = nullptr);
    
private:
    BoardWidget *board{};
    QPushButton *resetButton{};
    QBoxLayout *mainLayout{};
    QBoxLayout *buttonLayout{};
    
    void setupUI();
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