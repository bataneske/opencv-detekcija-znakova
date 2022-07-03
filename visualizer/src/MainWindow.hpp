
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <string>


#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h"

class QCustomPlot;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void drawSomething();

private:
	Ui::MainWindow *ui;
public:
	QCustomPlot* customPlot;
};

#endif // MAINWINDOW_H
