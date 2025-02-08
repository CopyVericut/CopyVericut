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
bool CncProcess::parseCNC()//解析CNC文件
{
	std::regex patternX(R"([X]([\d]+(?:\.\d+)?))");  // 匹配大写字母X后面的数字
	std::regex patternY(R"([Y]([\d]+(?:\.\d+)?))");  // 匹配大写字母Y后面的数字
	std::regex patternZ(R"([Z]([\d]+(?:\.\d+)?))");  // 匹配大写字母Z后面的数字
	std::regex patternI(R"([I]([\d]+(?:\.\d+)?))");  // 匹配大写字母I后面的数字
	std::regex patternJ(R"([J]([\d]+(?:\.\d+)?))");  // 匹配大写字母J后面的数字
	std::smatch match;
	std::string text;
	CncPathData cncPathData;
	string Gstatus{""};
	double currentPointX{ 0.0 }, currentPointY{ 0.0 }, currentPointZ{ 100.0 }, currentI{ 0.0 }, currentJ{0.0};
	for (auto i : cncContentList)
	{
		/*确定Gstatus状态*/
		if (i.contains("G0") or i.contains("G01") or i.contains("G1")) { Gstatus = "G01/G0"; }
		else if (i.contains("G2") or i.contains("G02")) { Gstatus = "G02"; }
		else if (i.contains("G3") or i.contains("G03")) { Gstatus = "G03"; }
		/*判断Gstatus状态*/
		if (Gstatus=="G01/G0")
		{
			/*设置轨迹几何形状*/
			cncPathData.pathType = Line;
			/*设置起点坐标*/
			cncPathData.startPointX = currentPointX;
			cncPathData.startPointY = currentPointY;
			cncPathData.startPointZ = currentPointZ;
			/*设置终点坐标*/
			text = i.toStdString();
			if (std::regex_search(text, match, patternX))
			{
				cncPathData.endPointX = std::stod(match[1]);
				currentPointX = cncPathData.endPointX;
			}
			else
			{
				cncPathData.endPointX = currentPointX;
			}
			if (std::regex_search(text, match, patternY))
			{
				cncPathData.endPointY = std::stod(match[1]);
				currentPointY = cncPathData.endPointY;
			}
			else
			{
				cncPathData.endPointY = currentPointY;
			}
			if (std::regex_search(text, match, patternZ))
			{
				cncPathData.endPointZ = std::stod(match[1]);
				currentPointZ = cncPathData.endPointZ;
			}
			else
			{
				cncPathData.endPointZ = currentPointZ;
			}
			cncPathDataList.push_back(cncPathData);
		}
		else if (Gstatus=="G02" or Gstatus=="G03")
		{
			/*设置轨迹几何形状*/
			cncPathData.pathType = Arc;
			/*设置起点坐标*/
			cncPathData.startPointX = currentPointX;
			cncPathData.startPointY = currentPointY;
			cncPathData.startPointZ = currentPointZ;
			cncPathData.I = 0.0;
			cncPathData.J = 0.0;
			/*设置终点坐标*/
			text = i.toStdString();
			/*终点X坐标值*/
			if (std::regex_search(text, match, patternX))
			{
				cncPathData.endPointX = std::stod(match[1]);
				currentPointX = cncPathData.endPointX;
			}
			else
			{
				cncPathData.endPointX = currentPointX;
			}
			/*终点Y坐标值*/
			if (std::regex_search(text, match, patternY))
			{
				cncPathData.endPointY = std::stod(match[1]);
				currentPointY = cncPathData.endPointY;
			}
			else
			{
				cncPathData.endPointY = currentPointY;
			}
			/*终点Z坐标值*/
			if (std::regex_search(text, match, patternZ))
			{
				cncPathData.endPointZ = std::stod(match[1]);
				currentPointZ = cncPathData.endPointZ;
			}
			else
			{
				cncPathData.endPointZ = currentPointZ;
			}
			cncPathDataList.push_back(cncPathData);
			/*I值*/
			if (std::regex_search(text, match, patternI))
			{
				cncPathData.I = std::stod(match[1]);
			}
			/*J值*/
			if (std::regex_search(text, match, patternJ))
			{
				cncPathData.J = std::stod(match[1]);
			}
			

			cncPathDataList.push_back(cncPathData);

		}
	
	}
	qDebug() << "解析完成";
	return true;
}

void CncProcess::GetLinearInterpolationPoints(CncPathData cncPathData)
{

}

void CncProcess::GetArcInterpolationPoints(CncPathData cncPathData)
{

}

// Compare this snippet from CncProcess.cpp: