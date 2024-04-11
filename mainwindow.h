#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QLabel>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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

  void on_lineEdit_path_textChanged(const QString &arg1);

private:
  Ui::MainWindow *ui;
  QString file_path;
  uint16_t file_size;
};
#endif // MAINWINDOW_H
