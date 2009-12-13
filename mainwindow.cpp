#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    glWidget = new GLWidget(50, this);

    xSlider = createSlider();
    ySlider = createSlider();
    zSlider = createSlider();

    connect(xSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setX(int)));
    connect(ySlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setY(int)));
    connect(zSlider, SIGNAL(valueChanged(int)), glWidget, SLOT(setZ(int)));

    renderButtons = new QButtonGroup;
    twoDButton = new QRadioButton("2D");
    twoDButton->setChecked(true);
    threeDButton = new QRadioButton("3D");
    renderButtons->addButton(twoDButton, 0);
    renderButtons->addButton(threeDButton, 1);

	connect(twoDButton, SIGNAL(toggled(bool)), glWidget, SLOT(setView(bool)));

//    connect(renderButtons, SIGNAL(buttonClicked(int)), glWidget, SLOT(setDirection(int)));

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

//	connect(directionButtons, SIGNAL(buttonClicked(int)), glWidget, SLOT(setColor(int)));

    sliderGroup = new QGroupBox;
    QGridLayout *sliderGroupLayout = new QGridLayout;
    sliderGroupLayout->addWidget(xButton, 0, 0);
    sliderGroupLayout->addWidget(yButton, 1, 0);
    sliderGroupLayout->addWidget(zButton, 2, 0);
    sliderGroupLayout->addWidget(xSlider, 0, 1);
    sliderGroupLayout->addWidget(ySlider, 1, 1);
    sliderGroupLayout->addWidget(zSlider, 2, 1);
    sliderGroup->setLayout(sliderGroupLayout);

	transferScene = new QGraphicsScene;
	transferView = new TFView(transferScene, glWidget->transferFunction());
	transferView->show();
	transferView->setMaximumHeight(150);

    sideBar = new QGroupBox;
    QVBoxLayout *sideBarLayout = new QVBoxLayout;
	sideBarLayout->addWidget(transferView);
    sideBarLayout->addWidget(renderGroup);
    sideBarLayout->addWidget(sliderGroup);
    sideBarLayout->insertStretch(0);
    sideBar->setFixedWidth(300);
    sideBar->setLayout(sideBarLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(sideBar);
    mainLayout->addWidget(glWidget);
    setLayout(mainLayout);

	transferView->setSceneRect(QRectF(0, 0, transferView->width(), transferView->height()));

    setMinimumSize(800, 600);
    setWindowTitle("SimpleVis");

	glWidget->loadDataSet("dat/stagbeetle277x277x164.dat");
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
