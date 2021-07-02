#pragma once

#include <QWidget>
#include "ui_Moravecui.h"
#include"qgsmapcanvas.h"
#include <Eigen/Core>
#include <Eigen\Dense> 
using namespace std;
using namespace Eigen;
class Moravecui : public QWidget
{
	Q_OBJECT

public:
	Moravecui(QgsMapCanvas*& canvas, QWidget *parent = Q_NULLPTR);
	~Moravecui();
	void freshui(QgsMapCanvas*& can);
	void freshui1();
	
signals:
	 void rraster(QString);
public slots:
	void save();
	void yunsuan();
	void suanzi(QString text);
	void huofu();
private:
	Ui::Moravecui ui;
	MatrixXd line_cnt1;
	int max_r;
	int degrees;
	double* sin_;
	double* cos_;
	QgsMapCanvas* m_canvas = nullptr;
	//QList<QgsMapLayer*> m_layerlist;
	QList<QString> name;
};
