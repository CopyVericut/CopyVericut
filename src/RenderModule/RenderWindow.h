#pragma once
#include "rendermodule_global.h"
#include<QWidget>
#include<QtCore/Qstring>
#include <QMainWindow>
#include"occView.h"
#include "DisplayCore.h"
#include "InteractiveModule.h"

using TubeNext::DisplayCore;
using TubeNext::OccView;

class TubeNext::OccView;
class TubeNext::DisplayCore;

namespace TubeNext
{
	class RENDERMODULE_EXPORT RenderWindow : public QMainWindow
	{
	public:
		RenderWindow(QWidget* parent);
	public:
		QWidget* parent;
		OccView* myOccView;
		DisplayCore* DisplayCoreManager;
		InteractiveManager* interactivemanager;
	};

}

