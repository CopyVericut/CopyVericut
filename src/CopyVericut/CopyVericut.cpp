#include "CopyVericut.h"
#include <RenderWindow.h>
#include <QString>
using namespace TubeNext;

CopyVericut::CopyVericut(QWidget *parent): QMainWindow(parent)
{
    ui.setupUi(this);
	RenderWindow* renderWindow = new RenderWindow(parent);
	ui.tabWidget->addTab(renderWindow->myOccView, QString::fromLocal8Bit("渲染仿真窗口"));
}

CopyVericut::~CopyVericut()
{
}
