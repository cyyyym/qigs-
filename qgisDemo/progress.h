#pragma once

#include <QWidget>
#include "ui_progress.h"

class progress : public QWidget
{
	Q_OBJECT

public:
	progress(QWidget *parent = Q_NULLPTR);
	void setprogressvalue(int i);
	void setmessage(QString& text);
	void qingkongtext();
	void deleteprogress();
	~progress();

private:
	Ui::progress ui;
};
