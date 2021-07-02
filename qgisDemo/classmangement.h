#pragma once

#include <QWidget>
#include "ui_classmangement.h"
#include"qgsmapcanvas.h"
class classmangement : public QWidget
{
	Q_OBJECT

public:
	classmangement(QgsMapCanvas *& canvas,QWidget *parent = Q_NULLPTR);
	void freshui1();
signals:
	void start(QList<QString>);
private:
	Ui::classmangement ui;
	QgsMapCanvas* m_canvas;
	//QList<QgsMapLayer*> m_layerlist;
	QList<QString> name;
private slots:
	void star_classify();
	void findtext();
	void outputtiff();
	void classifytype(QString text);
};
