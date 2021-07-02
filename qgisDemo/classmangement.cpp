#include "classmangement.h"
#include"qgsmapcanvas.h"
#include"qtextcodec.h"
#include"qfiledialog.h"
#include"qmessagebox.h"
classmangement::classmangement (QgsMapCanvas*& canvas, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_canvas = canvas;
	//m_layerlist = m_canvas->layers();
	freshui1();
}
void classmangement::classifytype(QString text)
{
	QTextCodec* codec = QTextCodec::codecForName("GBK");//�޸�������
	if (text == codec->toUnicode("���ɭ�ַ���"))
	{
		ui.label_3->setText(codec->toUnicode("ÿ����ѵ����������"));
		ui.lineEdit->setText("0.6");
		ui.label_6->setText(codec->toUnicode("Ҷ�ӽڵ���С������"));
		ui.lineEdit_3->setText("50");
		ui.label_6->show();
		ui.label_7->show();
		ui.label_8->show();
		ui.label_7->setText(codec->toUnicode("��ѡ���԰ٷֱ�"));
		ui.lineEdit_4->setText("0.6");
		ui.label_8->setText(codec->toUnicode("����������"));
		ui.lineEdit_5->setText("20");
		ui.lineEdit_5->show();
		ui.lineEdit_3->show();
		ui.lineEdit_4->show();
		ui.toolButton->hide();
		//ui.toolButton_2->hide();
	}
	if (text == codec->toUnicode("�����Ȼ����"))
	{
		ui.label_3->setText(codec->toUnicode("������Ϣ���"));
		ui.label_6->hide();
		ui.label_7->hide();
		ui.label_8->hide();
		ui.lineEdit->clear();
		ui.lineEdit_5->hide();
		ui.lineEdit_3->hide();
		ui.lineEdit_4->hide();
		ui.toolButton->show();
	}
}
void classmangement::freshui1()
{
	ui.comboBox->clear();
	ui.comboBox_3->clear();
	foreach(QgsMapLayer* index, m_canvas->layers())
	{
		if (index->type() == QgsMapLayerType::RasterLayer)
		{
			ui.comboBox->addItem(index->name());
		}
		else if (index->type() == QgsMapLayerType::VectorLayer)
		{
			ui.comboBox_2->addItem(index->name());
		}
	}
	QTextCodec* codec = QTextCodec::codecForName("GBK");//�޸�������
	ui.comboBox_3->addItem(codec->toUnicode("�����Ȼ����"));
	ui.comboBox_3->addItem(codec->toUnicode("���ɭ�ַ���"));
}
void classmangement::star_classify()
{
	this->close();
	QTextCodec* codec = QTextCodec::codecForName("GBK");//�޸�������
	QString text = ui.comboBox_3->currentText();
	if (text == codec->toUnicode("���ɭ�ַ���"))
	{
		name.clear();
		name.append(ui.comboBox->currentText());
		name.append(ui.comboBox_2->currentText());
		name.append(ui.lineEdit->text());
		name.append(ui.lineEdit_3->text());
		name.append(ui.lineEdit_4->text());
		name.append(ui.lineEdit_5->text());
		name.append(ui.lineEdit_2->text());
		name.append(ui.comboBox_3->currentText());
		emit start(name);
		
	}
	if (text == codec->toUnicode("�����Ȼ����"))
	{
		name.clear();
		name.append(ui.comboBox->currentText());
		name.append(ui.comboBox_2->currentText());
		name.append(ui.lineEdit->text());
		name.append(ui.lineEdit_2->text());
		name.append(ui.comboBox_3->currentText());
		emit start(name);
	}
	
}
void classmangement::findtext()
{
	QString fileName = QFileDialog::getSaveFileName(0, tr("save"), "", tr(" (*.txt)"));
	if (fileName.isNull())
	{
		QMessageBox::information(NULL, "Information", QString::fromLocal8Bit("�ļ�����ʧ��"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		return;
	}
	else
	{
		ui.lineEdit->setText(fileName);
	}
}
void classmangement::outputtiff()
{
	QString fileName = QFileDialog::getSaveFileName(0, tr("save"), "", tr(" (*.tiff)"));
	if (fileName.isNull())
	{
		QMessageBox::information(NULL, "Information", QString::fromLocal8Bit("�ļ�����ʧ��"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		return;
	}
	else
	{
		ui.lineEdit_2->setText(fileName);
	}
}