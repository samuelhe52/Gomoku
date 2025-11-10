//
// Created by Samuel He on 2025/10/9.
//

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>

class ColorChooserWidget : public QWidget {
    Q_OBJECT

public:
    explicit ColorChooserWidget(QWidget *parent = nullptr);

signals:
    void colorChosen(bool playerIsBlack);

private:
    QPushButton *blackButton;
    QPushButton *whiteButton;
    QLabel *titleLabel;
    QVBoxLayout *containerLayout;
    QHBoxLayout *buttonsLayout;
    QVBoxLayout *mainLayout;
    
    void setupUI();
};

