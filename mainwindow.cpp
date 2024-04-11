#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  file_path = "R:\\dtb";
  ui->setupUi(this);

}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::Readfile()
{
  QFile file(file_path);
  if(file.open(QIODevice::ReadWrite))
    {
      ui->label_filesize->setText(QString::number(file.size()));
      QByteArray readdata = file.readAll();
      if(!readdata.contains("gpu-opp-table_v2"))
        {
          QMessageBox::critical(this,"错误","文件无效");
        }
      QList<QByteArray> opplist;
      QList<uchar> opplist_index;
      QList<uchar> opplist_voltage;
      int index=0;
      while ((index = readdata.indexOf("gpu-opp-table_v2", index)) != -1)
        {
          if(readdata.mid(index,51).endsWith("operating-points-v2"))
            {
              qDebug() << "Found gpu-opp-table_v2 at index position " << index;
              qDebug() << "Found psci at index position " << readdata.indexOf("psci",index + 73);
              opplist_index.append(index + 72);//到这里是真正频率点
              opplist.append(readdata.mid(index + 72,readdata.indexOf("psci",index + 73)- index - 72));
//              QLabel *label = new QLabel(ui->centralwidget);
//              ui->verticalLayout_freq->addWidget(label);
//              label->setText("111111");
            }
          ++index;
        }
      for(uint8_t i = 0;i<opplist.size();i++)
        {
          ui->textBrowser->insertPlainText(opplist[i].toHex());
          ui->textBrowser->insertPlainText("\n");
        }
//      qDebug()<<opplist;
    }
}


void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::on_pushButton_2_clicked()
{

  Readfile();
}

void MainWindow::on_pushButton_3_clicked()
{
  file_path = QFileDialog::getOpenFileName(this);
  ui->lineEdit_path->setText(file_path);
}
