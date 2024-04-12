#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExpValidator>


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

void MainWindow::Read_opp_list(QByteArray data)
{
  int index = 0;
  QByteArray pointdata;
  while ((index = data.indexOf("opp-", index)) != -1)
    {
      pointdata = data.mid(index,60);

      //增加频率标签
      label[opp_index] = new QLabel(ui->centralwidget);
      label[opp_index]->setText(pointdata.mid(4,9));
      ui->verticalLayout_freq->addWidget(label[opp_index]);
      ui->textBrowser->insertPlainText(pointdata.mid(4,9) + " ");

      //增加16进制电压输入
      lineedit_vol_hex[opp_index] = new QLineEdit(ui->centralwidget);
      lineedit_vol_hex[opp_index]->setText(pointdata.mid(50,2).toHex());
      lineedit_vol_hex[opp_index]->setObjectName("lineedit_vol_hex_" + QString::number(opp_index));
      lineedit_vol_hex[opp_index]->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9a-f]+$")));
      ui->verticalLayout_vol_hex->addWidget(lineedit_vol_hex[opp_index]);
      ui->textBrowser->insertPlainText("0x" + pointdata.mid(50,2).toHex() + "\n");

      //增加10进制电压输入
      lineedit_vol_dec[opp_index] = new QLineEdit(ui->centralwidget);
      lineedit_vol_dec[opp_index]->setText(QString::number(pointdata.mid(50,2).toHex().toInt(nullptr, 16)));
      lineedit_vol_dec[opp_index]->setObjectName("lineedit_vol_dec_" + QString::number(opp_index));
      lineedit_vol_dec[opp_index]->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]+$")));
      qDebug()<<pointdata.mid(50,2).toHex().toInt(nullptr, 16);
      ui->verticalLayout_vol_dec->addWidget(lineedit_vol_dec[opp_index]);

      connect(lineedit_vol_hex[opp_index],&QLineEdit::textChanged,this,&MainWindow::on_vol_edited);
      connect(lineedit_vol_dec[opp_index],&QLineEdit::textChanged,this,&MainWindow::on_vol_edited);

      index++;
      opp_index++;
    }

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
          return;
        }
      ui->pushButton_open->setEnabled(0);
      int index=0;
      while ((index = readdata.indexOf("gpu-opp-table_v2", index)) != -1)
        {
          if(readdata.mid(index,51).endsWith("operating-points-v2"))
            {
              qDebug() << "Found gpu-opp-table_v2 at index position " << index;
              qDebug() << "Found psci at index position " << readdata.indexOf("psci",index + 73);
              opplist_index.append(index + 72);//到这里是真正频率点
              opplist.append(readdata.mid(index + 72,readdata.indexOf("psci",index + 73)- index - 72)); //读取opp-<freq>到psci前的字节

              ui->textBrowser->insertPlainText("list found in addr 0x" + QString::number(index +72 , 16) + "\n");
            }
          ++index;
        }
      for(uint8_t i = 0;i<opplist.size();i++)
        {
          Read_opp_list(opplist[i]);
        }
    }
}


void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::on_pushButton_3_clicked()
{
  file_path = QFileDialog::getOpenFileName(this);
  ui->lineEdit_path->setText(file_path);
}

void MainWindow::on_pushButton_open_clicked()
{
      Readfile();
}

void MainWindow::on_vol_edited()
{
  QLineEdit *lineedit = qobject_cast<QLineEdit*>(sender());
  QString text = lineedit->text();
  QString name = sender()->objectName();
  qDebug()<<"text:"<<text;
  qDebug()<<"objectName:"<<name<<"\n";
  QStringList tmp = name.split("_");
  int index = tmp.last().toUInt();
  if(name.startsWith("lineedit_vol_hex"))
    {
      qDebug()<<"hex:" + text<<"dec:"<<text.toUInt(nullptr,16);
      lineedit_vol_dec[index]->setText(QString::number(text.toUInt(nullptr,16)));
    }
  else if(name.startsWith("lineedit_vol_dec"))
    {
      qDebug()<<"dec:" + text<<"hex:"<<QString::number(text.toInt(),16).toLower().rightJustified(4, '0');
      lineedit_vol_hex[index]->setText(QString::number(text.toInt(),16).toLower().rightJustified(4, '0'));
    }

}
