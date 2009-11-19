#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    glWidget = new GLWidget(50, this);

    xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();

    xSlider->setValue(16 * 256);
    ySlider->setValue(16 * 128);

    connect(xSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setRed(int)));
    connect(ySlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setGreen(int)));
    connect(zSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setBlue(int)));

    renderButtons = new QButtonGroup;
    twoDButton = new QRadioButton("2D");
    twoDButton->setChecked(true);
    threeDButton = new QRadioButton("3D");
    renderButtons->addButton(twoDButton, 0);
    renderButtons->addButton(threeDButton, 1);

    connect(renderButtons, SIGNAL(buttonClicked(int)), glWidget, SLOT(setDirection(int)));

    renderGroup = new QGroupBox;
    QHBoxLayout *renderGroupLayout = new QHBoxLayout;
    renderGroupLayout->addWidget(twoDButton);
    renderGroupLayout->addWidget(threeDButton);
    renderGroup->setLayout(renderGroupLayout);

    directionButtons = new QButtonGroup;
    xButton = new QRadioButton("X");
    xButton->setChecked(true);
    yButton = new QRadioButton("Y");
    zButton = new QRadioButton("Z");
    directionButtons->addButton(xButton, 0);
    directionButtons->addButton(yButton, 1);
    directionButtons->addButton(zButton, 2);

	connect(directionButtons, SIGNAL(buttonClicked(int)), glWidget, SLOT(setColor(int)));

    sliderGroup = new QGroupBox;
    QGridLayout *sliderGroupLayout = new QGridLayout;
    sliderGroupLayout->addWidget(xButton, 0, 0);
    sliderGroupLayout->addWidget(yButton, 1, 0);
    sliderGroupLayout->addWidget(zButton, 2, 0);
    sliderGroupLayout->addWidget(xSlider, 0, 1);
    sliderGroupLayout->addWidget(ySlider, 1, 1);
    sliderGroupLayout->addWidget(zSlider, 2, 1);
    sliderGroup->setLayout(sliderGroupLayout);

    sideBar = new QGroupBox;
    QVBoxLayout *sideBarLayout = new QVBoxLayout;
    sideBarLayout->addWidget(renderGroup);
    sideBarLayout->addWidget(sliderGroup);
    sideBarLayout->insertStretch(0);
    sideBar->setFixedWidth(300);
    sideBar->setLayout(sideBarLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(sideBar);
    mainLayout->addWidget(glWidget);
    setLayout(mainLayout);

    setMinimumSize(800, 600);
    setWindowTitle("GUI Test");

	glWidget->loadDataSet("dat/lobster.dat");
}

QSlider *MainWindow::createSlider()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 255 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(16 * 16);
    slider->setTickInterval(16 * 16);
    slider->setTickPosition(QSlider::TicksBelow);
    return slider;
}

MainWindow::~MainWindow()
{

}
