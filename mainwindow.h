
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "glwidget.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    QSlider *createSlider();

    QGroupBox *sideBar, *renderGroup, *sliderGroup;
    QButtonGroup *renderButtons, *directionButtons;
    QRadioButton *twoDButton, *threeDButton, *xButton, *yButton, *zButton;
    QSlider *xSlider, *ySlider, *zSlider;
    GLWidget *glWidget;
};

#endif // MAINWINDOW_H
