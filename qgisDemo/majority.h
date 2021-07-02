#pragma once

#include <QWidget>
#include "ui_majority.h"
#include"qgsmapcanvas.h"

class majority : public QWidget
{
	Q_OBJECT

public:
	majority(QgsMapCanvas*& canvas, QWidget *parent = Q_NULLPTR);
	void freshui1();
public slots:
	void savepath();
	void returntext();
	void fresh(QgsMapCanvas*& can);
signals:
	void start(QList<QString>);
private:
	Ui::majority ui;
	QgsMapCanvas* m_canvas = nullptr;
	//QList<QgsMapLayer*> m_layerlist;
	QList<QString> name;
};
