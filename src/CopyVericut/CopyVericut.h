#pragma once
#include "MillSimulation.h"
#include <QtWidgets/QMainWindow>
#include "ui_CopyVericut.h"
#include <RenderWindow.h>
#include <CncProcess.h>
using namespace TubeNext;
class CopyVericut : public QMainWindow
{
    Q_OBJECT

public:
    CopyVericut(QWidget *parent = nullptr);
    ~CopyVericut();
public slots:
    void Quit();
	void ImportPart();
	void ImportMachine();
	void ReadCNCFile();
    void PathSimulation();
    void CuttingSimulation();
    void CreateBlankShape();
    void ResetBlankShape();
private:
    Ui::CopyVericutClass ui;
	QWidget* parent;
    CncProcess* cncProcess;
	MillSimulation* millSimulation;
    RenderWindow* renderWindow;
};
