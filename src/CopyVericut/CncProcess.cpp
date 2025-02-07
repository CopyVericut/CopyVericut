#include "CncProcess.h"
#include <QFileDialog>
#include <RenderWindow.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <regex>

CncProcess::CncProcess()
{}
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
		cncContentList.push_back(line);
	}
	file.close();  // 关闭文件

}
void CncProcess::parseCNC()
{
	std::regex patternX(R"([X]([\d]+(?:\.\d+)?))");  // 匹配大写字母X后面的数字
	std::regex patternY(R"([Y]([\d]+(?:\.\d+)?))");  // 匹配大写字母Y后面的数字
	std::regex patternZ(R"([Z]([\d]+(?:\.\d+)?))");  // 匹配大写字母Z后面的数字
	std::regex patternI(R"([I]([\d]+(?:\.\d+)?))");  // 匹配大写字母I后面的数字
	std::regex patternJ(R"([J]([\d]+(?:\.\d+)?))");  // 匹配大写字母J后面的数字
	std::smatch match;
	std::string text;
	CncPathData cncPathData;
	double currentPointX{ 0 }, currentPointY{ 0 }, currentPointZ{100};
	for (auto i : cncContentList)
	{
		//解析CNC文件
		if (i.contains("G0"))
		{
			/*设置轨迹几何形状*/
			cncPathData.pathType = Line;
			/*设置起点坐标*/
			cncPathData.startPointX = currentPointX;
			cncPathData.startPointY = currentPointY;
			cncPathData.startPointZ = currentPointZ;
			/*设置终点坐标*/
			text = i.toStdString();
			if (std::regex_search(text, match, patternX)) {
				cncPathData.endPointX = std::stod(match[1]);
				currentPointX = cncPathData.endPointX;
			}
			if (std::regex_search(text, match, patternY)) {
				cncPathData.endPointY = std::stod(match[1]);
				currentPointY = cncPathData.endPointY;
			}
			if (std::regex_search(text, match, patternZ)) {
				cncPathData.endPointZ = std::stod(match[1]);
				currentPointZ = cncPathData.endPointZ;
			}
		}
		else if (i.contains("G01")or i.contains("G1"))
		{
			/*设置轨迹几何形状*/
			cncPathData.pathType = Line;
			/*设置起点坐标*/
			cncPathData.startPointX = currentPointX;
			cncPathData.startPointY = currentPointY;
			cncPathData.startPointZ = currentPointZ;
			/*设置终点坐标*/
			text = i.toStdString();
			if (std::regex_search(text, match, patternX)) {
				cncPathData.endPointX = std::stod(match[1]);
				currentPointX = cncPathData.endPointX;
			}
			if (std::regex_search(text, match, patternY)) {
				cncPathData.endPointY = std::stod(match[1]);
				currentPointY = cncPathData.endPointY;
			}
			if (std::regex_search(text, match, patternZ)) {
				cncPathData.endPointZ = std::stod(match[1]);
				currentPointZ = cncPathData.endPointZ;
			}
		}
		else if (i.contains("G02"))
		{
			qDebug() << "G02";
		}
		else if (i.contains("G03"))
		{
			qDebug() << "G03";
		}
		else if (i.contains("G04"))
		{
			qDebug() << "G04";
		}
		else if (i.contains("G05"))
		{
			qDebug() << "G05";
		}
		else if (i.contains("G06"))
		{
			qDebug() << "G06";
		}
		
		qDebug() << i;
	}
}

// Compare this snippet from CncProcess.cpp: