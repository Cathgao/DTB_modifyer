#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
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
  QList<uint16_t> voltage_children_list;
  while ((index = data.indexOf("opp-", index)) != -1)
  {
    pointdata = data.mid(index, 60);

    uint16_t voltage = pointdata.mid(50, 2).toHex().toUInt(nullptr, 16);
    voltage_children_list.append(voltage);
    // 增加频率标签
    label[opp_index] = new QLabel(ui->centralwidget);
    label[opp_index]->setText(pointdata.mid(4, 9));
    ui->verticalLayout_freq->addWidget(label[opp_index]);
    ui->textBrowser->insertPlainText(pointdata.mid(4, 9) + " ");

    // 增加16进制电压输入
    lineedit_vol_hex[opp_index] = new QLineEdit(ui->centralwidget);
    lineedit_vol_hex[opp_index]->setText(QString::number(voltage, 16).rightJustified(4, '0'));
    lineedit_vol_hex[opp_index]->setObjectName("lineedit_vol_hex_" + QString::number(opp_index));
    lineedit_vol_hex[opp_index]->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9a-f]+$")));
    ui->verticalLayout_vol_hex->addWidget(lineedit_vol_hex[opp_index]);
    ui->textBrowser->insertPlainText("0x" + QString::number(voltage, 16).rightJustified(4, '0') + "\n");

    // 增加10进制电压输入
    lineedit_vol_dec[opp_index] = new QLineEdit(ui->centralwidget);
    lineedit_vol_dec[opp_index]->setText(QString::number(voltage, 10));
    lineedit_vol_dec[opp_index]->setObjectName("lineedit_vol_dec_" + QString::number(opp_index));
    lineedit_vol_dec[opp_index]->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]+$")));
    ui->verticalLayout_vol_dec->addWidget(lineedit_vol_dec[opp_index]);

    connect(lineedit_vol_hex[opp_index], &QLineEdit::textChanged, this, &MainWindow::on_vol_edited);
    connect(lineedit_vol_dec[opp_index], &QLineEdit::textChanged, this, &MainWindow::on_vol_edited);

    index++;
    opp_index++;
  }
  voltage_list.append(voltage_children_list);
}

void MainWindow::Readfile()
{
  int index = 0;
  while ((index = file_data.indexOf("gpu-opp-table_v2", index)) != -1)
  {
    if (file_data.mid(index, 51).endsWith("operating-points-v2"))
    {
      qDebug() << "Found gpu-opp-table_v2 at index position " << index;
      qDebug() << "Found psci at index position " << file_data.indexOf("psci", index + 73);
      // 到这里是真正频率点起始地址
      opplist_index.append(index + 72);
      // 读取opp-<freq>到psci前的字节
      oppdata.append(file_data.mid(index + 72, file_data.indexOf("psci", index + 73) - index - 72));
      ui->textBrowser->insertPlainText("list found in addr 0x" + QString::number(index + 72, 16) + "\n");
    }
    ++index;
  }
  for (uint8_t i = 0; i < oppdata.size(); i++)
  {
    Read_opp_list(oppdata[i]);
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
  QFile file(file_path);
  if (file.open(QIODevice::ReadWrite))
  {
    ui->label_filesize->setText(QString::number(file.size()));
    file_data = file.readAll();
    file.close();
    if (!file_data.contains("gpu-opp-table_v2"))
    {
      QMessageBox::critical(this, "错误", "文件无效");
      return;
    }
  }
  ui->pushButton_open->setEnabled(0);
  ui->pushButton_write->setEnabled(1);
  ui->pushButton_3->setEnabled(0);
  Readfile();
}

void MainWindow::on_vol_edited()
{
  QLineEdit *lineedit = qobject_cast<QLineEdit *>(sender());
  QString text = lineedit->text();
  QString name = sender()->objectName();
  qDebug() << "text:" << text;
  qDebug() << "objectName:" << name << "\n";
  QStringList tmp = name.split("_");
  int index = tmp.last().toUInt();
  if (name.startsWith("lineedit_vol_hex"))
  {
    qDebug() << "hex:" + text << "dec:" << text.toUInt(nullptr, 16);
    lineedit_vol_dec[index]->setText(QString::number(text.toUInt(nullptr, 16)));
  }
  else if (name.startsWith("lineedit_vol_dec"))
  {
    qDebug() << "dec:" + text << "hex:" << QString::number(text.toInt(), 16).toLower().rightJustified(4, '0');
    lineedit_vol_hex[index]->setText(QString::number(text.toInt(), 16).toLower().rightJustified(4, '0'));
  }
}

void MainWindow::on_pushButton_write_clicked()
{
  QString voltage_list_str;
  uint16_t label_index = 0;
  // 频率表层
  for (uint16_t voltage_list_index = 0; voltage_list_index != voltage_list.size(); ++voltage_list_index)
  {
    // 频率点层
    for (uint16_t voltage_index = 0; voltage_index != voltage_list[voltage_list_index].size(); ++voltage_index)
    {
      QByteArray new_voltage_hex = QByteArray::fromHex(lineedit_vol_hex[label_index]->text().toUtf8());
      uint16_t new_voltage = new_voltage_hex.toHex().toUShort(nullptr, 16);
      // 16bit电压间隔121bytes，替换2bytes电压值
      oppdata[voltage_list_index].replace(50 + 60 * voltage_index, 2, new_voltage_hex);
      voltage_list_str.append("频率：" + label[label_index]->text());
      voltage_list_str.append(" 原始电压(Hex):" + QString::number(voltage_list[voltage_list_index][voltage_index], 16).rightJustified(4, '0'));
      voltage_list_str.append(" 修改后(Hex):" + QString::number(new_voltage, 16).rightJustified(4, '0'));
      voltage_list_str.append(" 变化(Dec):" + QString::number(int32_t(new_voltage - voltage_list[voltage_list_index][voltage_index])) + "\n");
      label_index++;
    }
    // 电压表数据块更新完成，分别更新到文件块中
    file_data.replace(opplist_index[voltage_list_index], oppdata[voltage_list_index].size(), oppdata[voltage_list_index]);
  }
  // 全部电压表更新完成，文件块写入到新文件中
  QFileInfo fileinfo(file_path);
  QString path = fileinfo.path();
  QFile newfile(path + "dtb_new");
  if (newfile.open(QIODevice::WriteOnly))
  {
    newfile.write(file_data);
    newfile.close();
    voltage_list_str.append("修改成功，新文件已保存至" + newfile.fileName());
    QMessageBox::information(this, "修改成功", voltage_list_str);
    //    QMessageBox::information(this, "成功", "已保存至" + newfile.fileName());
  }
  else
  {
    QMessageBox::critical(this, "错误", "写入到" + newfile.fileName() + "失败");
  }
}
