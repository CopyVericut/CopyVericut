#include "CncProcess.h"
#include <QFileDialog>
#include <RenderWindow.h>
#include <QFile>
#include <QTextStream>

CncProcess::CncProcess()
{

}

void CncProcess::ReadCncFile(QString filePath)
{
	/*创建 QFile 对象*/
	QFile file(filePath); 
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {}
	QTextStream in(&file);
	while (!in.atEnd())
	{
		/*读取一行*/
		QString line = in.readLine();  
		cncContent.push_back(line);
	}
	file.close();  // 关闭文件

}
// Compare this snippet from CncProcess.cpp: