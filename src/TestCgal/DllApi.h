#pragma once
#include <qglobal.h>
#ifdef MYDLL_EXPORTS   // 当我们编译 DLL 时定义这个宏
#define MYDLL_API __declspec(dllexport)  // 导出符号
#else
#define MYDLL_API __declspec(dllimport)  // 导入符号
#endif