#pragma once

#include <QDialog>
#include "ui_buildPyramid.h"
#include"qgsrasterdataprovider.h"

class buildPyramid : public QDialog
{
	Q_OBJECT

public:
	buildPyramid(QgsRasterDataProvider *&data,QWidget *parent = Q_NULLPTR);
	~buildPyramid();
public slots:
	void run();
	void return1();
private:
	QgsRasterDataProvider* rasterdata = nullptr;
	Ui::buildPyramid ui;
};
