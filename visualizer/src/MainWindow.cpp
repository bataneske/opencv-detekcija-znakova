
#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setWindowTitle("visualizer");

	setGeometry(400, 250, 542, 390);


	customPlot = ui->customPlot;

	customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

	customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
	customPlot->legend->setVisible(true);
	QFont legendFont = font();  // start out with MainWindow's font..
	legendFont.setPointSize(9); // and make a bit smaller for legend
	customPlot->legend->setFont(legendFont);
	customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
	// by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
	customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

	int line_width = 3;
	// Hardcoded.
	customPlot->addGraph();
	customPlot->graph(0)->setPen(QPen(Qt::red, line_width));
	customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);

	customPlot->addGraph();
	customPlot->graph(1)->setPen(QPen(Qt::green, line_width));
	customPlot->graph(1)->setLineStyle(QCPGraph::lsLine);

	customPlot->addGraph();
	customPlot->graph(2)->setPen(QPen(Qt::blue, line_width));
	customPlot->graph(2)->setLineStyle(QCPGraph::lsLine);

	customPlot->addGraph();
	customPlot->graph(3)->setPen(QPen(Qt::magenta, line_width));
	customPlot->graph(3)->setLineStyle(QCPGraph::lsLine);

	customPlot->xAxis->setRange(0, 100);
	customPlot->yAxis->setRange(0, 1000);

	customPlot->replot();
}
MainWindow::~MainWindow()
{
  delete ui;
}
void MainWindow::drawSomething(

) {
}