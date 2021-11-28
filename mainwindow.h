#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextStream>
#include <QMessageBox>
#include "grammar.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_confirm_clicked();

    void on_pushButton_deducted_clicked();

private:
    bool has_init = false;
    Ui::MainWindow *ui;
    Grammar *grammar;
};
#endif // MAINWINDOW_H
