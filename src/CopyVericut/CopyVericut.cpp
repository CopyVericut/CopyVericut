#include "CopyVericut.h"
#include <RenderWindow.h>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <CncProcess.h>
using namespace TubeNext;

CopyVericut::CopyVericut(QWidget *par): QMainWindow(parent)
{
	parent = par;
    ui.setupUi(this);
	cncProcess = new CncProcess();
	cncProcess->SetTextBrowser(ui.textBrowser);
	millSimulation = new MillSimulation();
	millSimulation->SetTextBrowser(ui.textBrowser);
	renderWindow = new RenderWindow(parent);
	millSimulation->SetDisPlayCore(renderWindow->DisplayCoreManager);
	ui.tabWidget->addTab(renderWindow->myOccView, QString::fromLocal8Bit("渲染仿真窗口"));
	/*视角转换*/
	connect(ui.actionView_Top, SIGNAL(triggered()), renderWindow->myOccView, SLOT(View_Top()));
	connect(ui.actionView_Bottom, SIGNAL(triggered()), renderWindow->myOccView, SLOT(View_Bottom()));
	connect(ui.actionView_Left, SIGNAL(triggered()), renderWindow->myOccView, SLOT(View_Left()));
	connect(ui.actionView_Right, SIGNAL(triggered()), renderWindow->myOccView, SLOT(View_Right()));
	connect(ui.actionView_Front, SIGNAL(triggered()), renderWindow->myOccView, SLOT(View_Front()));
	connect(ui.actionView_Rear, SIGNAL(triggered()), renderWindow->myOccView, SLOT(View_Rear()));
	connect(ui.actionView_Iso, SIGNAL(triggered()), renderWindow->myOccView, SLOT(View_Iso()));
	/*退出*/
	connect(ui.quit, SIGNAL(triggered()), this, SLOT(Quit()));
	/*导入零件*/
	connect(ui.import_machining_part, SIGNAL(triggered()), this, SLOT(ImportPart()));
	/*导入机床*/
	connect(ui.import_machine_model, SIGNAL(triggered()), this, SLOT(ImportMachine()));
	/*导入cnc程序*/
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(ReadCNCFile()));
	/*路径仿真*/
	connect(ui.pushButton_3, SIGNAL(clicked()), this, SLOT(PathSimulation()));
	/*切削仿真*/
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(CuttingSimulation()));
	setFixedSize(855, 815);

}

CopyVericut::~CopyVericut()
{
}

void CopyVericut::ImportPart()
{
	QString filePath = QFileDialog::getOpenFileName(
		parent,
		QString::fromLocal8Bit("选择打开文件"),
		"c:/",
		tr("step(*step *stp )"));
	if (filePath.size() != 0)
	{
		QStringList fileNameList = filePath.split("/");
		QString fileName = fileNameList[fileNameList.size() - 1];
		fileNameList = fileName.split(".");
		fileName = fileNameList[0];
		renderWindow->DisplayCoreManager->Read_step_file(filePath.toStdString(), fileName.toStdString());
	}
}

void CopyVericut::ImportMachine()
{
	QString CurrentPath = QCoreApplication::applicationDirPath();
	QString __filePath = CurrentPath+u8"./machine/仿真机床/Machine_work_table.step";
	QString filePath = QString::fromUtf8(__filePath.toUtf8().constData());
	QStringList fileNameList = filePath.split("/");
	QString fileName = fileNameList[fileNameList.size() - 1];
	fileNameList = fileName.split(".");
	fileName = fileNameList[0];
	/*导入工作台*/
	renderWindow->DisplayCoreManager->Read_step_file(filePath.toStdString(), fileName.toStdString());
	/*导入主轴*/
	__filePath = CurrentPath+u8"./machine/仿真机床/Machine_spindle.step";
	filePath = QString::fromUtf8(__filePath.toUtf8().constData());
	fileNameList = filePath.split("/");
	fileName = fileNameList[fileNameList.size() - 1];
	fileNameList = fileName.split(".");
	fileName = fileNameList[0];
	renderWindow->DisplayCoreManager->Read_step_file(filePath.toStdString(), fileName.toStdString());

}

void CopyVericut::ReadCNCFile()
{
	QString filePath = QFileDialog::getOpenFileName(
		parent,
		QString::fromLocal8Bit("选择打开文件"),
		"c:/",
		tr("NC(*NC *CNC )"));
	if (filePath.size() != 0)
	{
		cncProcess->ReadCncFile(filePath);
	}

	/*解析CNC代码*/
	cncProcess->parseCNC();
	statusBar()->showMessage(QString::fromLocal8Bit("状态：CNC文件读取/解析完成"));
}

void CopyVericut::PathSimulation()
{
	millSimulation->SetCncPathDataList(cncProcess->cncPathDataList);
	statusBar()->showMessage(QString::fromLocal8Bit("状态：正在模拟刀路轨迹"));
	millSimulation->DisPlayToolPath();
	statusBar()->showMessage(QString::fromLocal8Bit("状态：模拟刀路轨迹完成"));
}

void CopyVericut::CuttingSimulation()
{
	statusBar()->showMessage(QString::fromLocal8Bit("状态：正在模拟切削过程"));
	cncProcess->CuttingSimulation(renderWindow->DisplayCoreManager);
	statusBar()->showMessage(QString::fromLocal8Bit("状态：模拟切削过程完成"));
}

void CopyVericut::Quit()
{
	QApplication::quit();
}
