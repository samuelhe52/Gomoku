//
// Created by Samuel He on 2025/10/9.
//

#pragma once
#include <QWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>

#ifndef GOMOKU_COLORCHOOSERWIDGET_H
#define GOMOKU_COLORCHOOSERWIDGET_H

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

#endif // GOMOKU_COLORCHOOSERWIDGET_H
