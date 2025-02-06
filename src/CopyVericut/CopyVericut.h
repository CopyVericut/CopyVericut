#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CopyVericut.h"

class CopyVericut : public QMainWindow
{
    Q_OBJECT

public:
    CopyVericut(QWidget *parent = nullptr);
    ~CopyVericut();

private:
    Ui::CopyVericutClass ui;
};
