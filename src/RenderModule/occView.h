﻿/*
*    Copyright (c) 2018 Shing Liu All Rights Reserved.
*
*           File : OccView.h
*         Author : Shing Liu(eryar@163.com)
*           Date : 2018-01-09 21:00
*        Version : OpenCASCADE7.2.0 & Qt5.7.1
*
*    Description : Adapte OpenCASCADE view for Qt.
*/

#pragma once

#include <map>
#include <rendermodule_global.h>
#include <opencascade/AIS_InteractiveContext.hxx>
#include <QObject>
#include <qwidget.h>
#include <QMainWindow>

class QMenu;
class QRubberBand;

//! Adapted a QWidget for OpenCASCADE viewer.
namespace TubeNext
{
    class RENDERMODULE_EXPORT OccView : public QMainWindow
    {
        Q_OBJECT

    public:
        //! mouse actions.
        enum CurrentAction3d
        {
            CurAction3d_Nothing,
            CurAction3d_DynamicZooming,
            CurAction3d_WindowZooming,
            CurAction3d_DynamicPanning,
            CurAction3d_GlobalPanning,
            CurAction3d_DynamicRotation
        };
        //define course icons
        std::map<std::string, QPixmap> course;
        int mid_button_press_x;
        int mid_button_press_y;
    public:
        //! constructor.
        OccView(QWidget* parent);
        const Handle(AIS_InteractiveContext)& getContext() const;
        const Handle(V3d_Viewer)& getV3dviewer() const;
        const Handle(V3d_View)& getV3dview() const;
        Standard_Integer PanStatus{0};
        Standard_Integer _dragPosX;
        Standard_Integer _dragPosY;
        Standard_Real XP;
        Standard_Real YP;
        Standard_Real ZP;
        Standard_Real XV;
        Standard_Real YV;
        Standard_Real ZV;



    signals:
        void selectionChanged(void);
        void mousePressSignal(int value);
        void mousePressAndShiftSignal(int value);
        void mouseReleseSignal(int value);
        void mouseMoveSignal(int value);

    public slots:
        //! operations for the view.
        void pan(void);
        void fitAll(void);
        void reset(void);
        void zoom(void);
        void rotate(void);

    protected:
        virtual QPaintEngine* paintEngine() const;

        // Paint events.
        virtual void paintEvent(QPaintEvent* theEvent);
        virtual void resizeEvent(QResizeEvent* theEvent);

        // Mouse events.
        virtual void mousePressEvent(QMouseEvent* theEvent);
        virtual void mouseReleaseEvent(QMouseEvent* theEvent);
        virtual void mouseMoveEvent(QMouseEvent* theEvent);
        virtual void wheelEvent(QWheelEvent* theEvent);

        // Button events.
        virtual void onLButtonDown(const int theFlags, const QPoint thePoint);
        virtual void onMButtonDown(const int theFlags, const QPoint thePoint);
        virtual void onRButtonDown(const int theFlags, const QPoint thePoint);
        virtual void onMouseWheel(const int theFlags, const int theDelta, const QPoint thePoint);
        virtual void onLButtonUp(const int theFlags, const QPoint thePoint);
        virtual void onMButtonUp(const int theFlags, const QPoint thePoint);
        virtual void onRButtonUp(const int theFlags, const QPoint thePoint);
        virtual void onMouseMove(QMouseEvent* event, const QPoint thePoint);

        // Popup menu.
        virtual void addItemInPopup(QMenu* theMenu);

    protected:
        void init(void);
        void popup(const int x, const int y);
        void dragEvent(const int x, const int y);
        void inputEvent(const int x, const int y);
        void moveEvent(const int x, const int y);
        void multiMoveEvent(const int x, const int y);
        void multiDragEvent(const int x, const int y);
        void multiInputEvent(const int x, const int y);
        void drawRubberBand(const int minX, const int minY, const int maxX, const int maxY);
        void panByMiddleButton(const QPoint& thePoint);

    private:

        //! the occ viewer.
        Handle(V3d_Viewer) myViewer;

        //! the occ view.
        Handle(V3d_View) myView;

        //! the occ context.
        Handle(AIS_InteractiveContext) myContext;

        //! save the mouse position.
        Standard_Integer myXmin;
        Standard_Integer myYmin;
        Standard_Integer myXmax;
        Standard_Integer myYmax;

        //! the mouse current mode.
        CurrentAction3d myCurrentMode;

        //! save the degenerate mode state.
        Standard_Boolean myDegenerateModeIsOn;

        //! rubber rectangle for the mouse selection.
        QRubberBand* myRectBand;
    public:
        int View_Top();
        int View_Bottom();
        int View_Left();
        int View_Right();
        int View_Front();
        int View_Rear();
        int View_Iso();

    public:
        double zoom_factor;
    public slots:
        void mySlot();
    };
}