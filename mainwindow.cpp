/*! \file mainwindow.cpp
	\brief MainWindow source file.

	Contains the source code for the MainWindow class, which is the GUI for the application window.
*/

#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow()
{
	QWidget *widget = new QWidget;
	setCentralWidget(widget);

    glWidget = new GLWidget(50, widget);

	xLabel = new QLabel("X");
	yLabel = new QLabel("Y");
	zLabel = new QLabel("Z");

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

    renderGroup = new QGroupBox;
    QHBoxLayout *renderGroupLayout = new QHBoxLayout;
    renderGroupLayout->addWidget(twoDButton);
    renderGroupLayout->addWidget(threeDButton);
    renderGroup->setLayout(renderGroupLayout);

    sliderGroup = new QGroupBox;
    QGridLayout *sliderGroupLayout = new QGridLayout;
    sliderGroupLayout->addWidget(xLabel, 0, 0);
    sliderGroupLayout->addWidget(yLabel, 1, 0);
    sliderGroupLayout->addWidget(zLabel, 2, 0);
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
    widget->setLayout(mainLayout);
	
	openAct = new QAction(tr("&Open"), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Load a dataset"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(loadDataset()));

	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);

	bool foundOne;
	do
	{
		foundOne = false;
		QList<QWidget *> allChildWidgets = this->findChildren<QWidget *>();
		allChildWidgets.prepend( this );

		foreach(QWidget* w, allChildWidgets)
		{
			if(w->testAttribute(Qt::WA_PendingResizeEvent))
			{
				QResizeEvent e(w->size(), QSize());
				QApplication::sendEvent(w, &e);
				w->setAttribute(Qt::WA_PendingResizeEvent, false);
				// hack: make QTabWidget think it's visible; no layout otherwise
				w->setAttribute(Qt::WA_WState_Visible, true);
				foundOne = true;
			}
		}
		// Process LayoutRequest events, in particular
		qApp->sendPostedEvents();
		 
		if(!foundOne)
		{
			// Reset visible flag, to avoid crashes in qt
			foreach(QWidget* w, allChildWidgets)
				w->setAttribute(Qt::WA_WState_Visible, false);
		}
	} while(foundOne);
	 
	qint32 sceneWidth = transferView->viewport()->width();
	qint32 sceneHeight = transferView->viewport()->height();

	transferView->setSceneRect(QRectF(0, 0, sceneWidth, sceneHeight));

    setMinimumSize(800, 600);
    setWindowTitle("SimpleVis");

//	glWidget->loadDataSet("dat/stagbeetle277x277x164.dat");

	transferView->drawTF();
}

MainWindow::~MainWindow()
{

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

void MainWindow::loadDataset()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Load Dataset"), "", tr("Volume Data (*.dat)"));
	if(!fileName.isNull())
		glWidget->loadDataSet(fileName.toStdString());
	transferView->drawTF();
}
