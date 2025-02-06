#include "RenderWindow.h"
#include"OccView.h"
#include<QtCore/Qstring>
#include<QFileDialog>
#include"DisplayCore.h"
#include<opencascade/Graphic3d_GraphicDriver.hxx>
#include "InteractiveModule.h"
using namespace std;
using namespace TubeNext;

class TubeNext::OccView;
class TubeNext::DisplayCore;

static Handle(Graphic3d_GraphicDriver)& GetGraphicDriver()
{
	static Handle(Graphic3d_GraphicDriver) aGraphicDriver;
	return aGraphicDriver;
}

TubeNext::RenderWindow::RenderWindow(QWidget* parent) : QMainWindow(parent)
{
	myOccView = new OccView(this);
	DisplayCoreManager = new DisplayCore(myOccView);
	interactivemanager = new InteractiveManager(DisplayCoreManager);
	myOccView->reset();
}
