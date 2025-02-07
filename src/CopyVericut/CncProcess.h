#pragma once
#include <QString>
#include <vector>
using namespace std;
class CncProcess
{
public:
	CncProcess();
	~CncProcess()=default;
public:
	void ReadCncFile(QString filePath);
public:
	vector<QString> cncContent;
};

