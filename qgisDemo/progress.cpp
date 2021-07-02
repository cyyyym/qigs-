#include "progress.h"

progress::progress(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.progressBar->setRange(0, 100);
	ui.progressBar->setMinimum(0);
	ui.progressBar->setValue(0);
}
void progress::setprogressvalue(int i)
{
	ui.progressBar->setValue(i);
}
void progress::setmessage(QString& text)
{
	ui.textBrowser->insertPlainText(text);
}
void progress::qingkongtext()
{
	ui.textBrowser->clear();
}
void progress::deleteprogress()
{
	ui.progressBar->reset();
}
progress::~progress()
{
}
