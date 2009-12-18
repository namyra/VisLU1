/*! \file mainwindow.h
	\brief MainWindow header file.

	Contains the declarations for the MainWindow class, which is the GUI for the application window.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "glwidget.h"
#include "tfview.h"

//! Widget for the main window of the application.
/*!
	Provides the widget for the main window, including all the GUI and the GLWidget that takes care of the actual visualisation.
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	//! Default constructor.
	/*!
		Creates a new MainWindow and sets up the GUI for it.
		\sa createSlider(), ~MainWindow()
	*/
    MainWindow();

	//! Default destructor.
	/*!
		\sa MainWindow()
	*/
    ~MainWindow();

private:
	//! Utility function that creates a slider.
	/*!
		\sa MainWindow()
	*/
    QSlider *createSlider();

	//! The widget group for the sidebar containing the UI elements.
    QGroupBox *sideBar;

	//! The widget group for the buttons to switch between 2d and 3d view.
	QGroupBox *renderGroup;

	//! The widget group for the sliders for the 3 axes.
	QGroupBox *sliderGroup;

	//! The button group for the buttons to switch between 2d and 3d view.
    QButtonGroup *renderButtons;

	//! The button that switches to 2d view.
    QRadioButton *twoDButton;

	//! The button that switches to 3d view.
	QRadioButton *threeDButton;

	//! The label for the x-axis slider.
	QLabel *xLabel;

	//! The label for the y-axis slider.
	QLabel *yLabel;

	//! The label for the z-axis slider.
	QLabel *zLabel;

	//! The slider determining the x coordinate.
    QSlider *xSlider;

	//! The slider determining the y coordinate.
	QSlider *ySlider;

	//! The slider determining the z coordinate.
	QSlider *zSlider;

	//! The widget displaying the actual render of the volume data.
    GLWidget *glWidget;

	//! The Qt scene used to display the transfer function.
	QGraphicsScene *transferScene;

	//! The widget displaying the transfer function.
	TFView *transferView;

	//! The File menu.
	QMenu *fileMenu;

	//! The Open file action.
	QAction *openAct;

private slots:

	//! Slot for the Open file action.
	/*!
		Opens a file dialog and if a new file is chosen, tells the display widget to use the new file for its volume data.
	*/
	void loadDataset();
};

#endif // MAINWINDOW_H
