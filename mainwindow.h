#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QLabel>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui
{
  class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  void Readfile();
  void Read_opp_list(QByteArray data);
  QLabel *label[20];
  uint opp_index = 0;
  QLineEdit *lineedit_vol_hex[20];
  QLineEdit *lineedit_vol_dec[20];

private slots:
  void on_pushButton_clicked();

  void on_pushButton_3_clicked();

  void on_pushButton_open_clicked();

  void on_vol_edited();

  void on_pushButton_write_clicked();

private:
  Ui::MainWindow *ui;
  QByteArray file_data;
  QString file_path;
  uint16_t file_size;
  QList<QByteArray> oppdata;           // 保存频率表的二进制块
  QList<uint32_t> opplist_index;       // 保存频率表二进制块的地址
  QList<QList<uint16_t>> voltage_list; // 保存提取的16bit电压表
};
#endif // MAINWINDOW_H
