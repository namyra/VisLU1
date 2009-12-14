
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "glwidget.h"
#include "tfview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    QSlider *createSlider();

    QGroupBox *sideBar, *renderGroup, *sliderGroup;
    QButtonGroup *renderButtons;
    QRadioButton *twoDButton, *threeDButton;
    QSlider *xSlider, *ySlider, *zSlider;
    GLWidget *glWidget;
	QGraphicsScene *transferScene;
	TFView *transferView;
	QMenu *fileMenu;
	QAction *openAct;

private slots:
	void loadDataset();
};

#endif // MAINWINDOW_H
