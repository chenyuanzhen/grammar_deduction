#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    S -> AbB|Bc\nA -> aA|@\nB -> d|e
    ui->plainTextEdit_input->setPlainText("S -> AbB|Bc\nA -> Aa|@\nB -> d|e");
    ui->lineEdit_wait_deduction->setText("abd");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_confirm_clicked()
{
    QVector<QChar> can_not_end;
    QVector<QChar> can_not_reach;
    QString cache = this->ui->plainTextEdit_input->toPlainText();
    QTextStream textStream(&cache);
    grammar = new Grammar();
    while(!textStream.atEnd()){
        QString line = textStream.readLine();
        QChar non_terminal = line[0];
        // grammar只允许设置一次, 其余都视为无效
        grammar->set_start(non_terminal);
        QString deduction = line.mid(5);
        grammar->insert_grammar(non_terminal, deduction);
    }
    // 简化
    qDebug() << "简化: ";
    grammar->simplify(can_not_end, can_not_reach);
    qDebug() << grammar->print_all_grammer();

    // 去除直接左公因子
    qDebug() << "去除直接左公因子: ";
    grammar->turn_left_common_factor();
    qDebug() << grammar->print_all_grammer();

    // 去除直接与间接左递归
    qDebug() << "去除左递归: ";
    grammar->turn_left_recursion_to_right();
    qDebug() << grammar->print_all_grammer();

    // 简化
    qDebug() << "简化: ";
    grammar->simplify(can_not_end, can_not_reach);
    qDebug() << grammar->print_all_grammer();

    qDebug() << "提取左公因子: ";
    grammar->turn_left_common_factor();
    qDebug() << grammar->print_all_grammer();

    // 找first集合
    grammar->find_first();
    qDebug() << "first集合: ";
    qDebug() << grammar->print_first_set();
    // 简化
    grammar->simplify(can_not_end, can_not_reach);
    // 找follow集合
    grammar->find_follow();
    // 展示结果
    QString tmp;
    tmp += "不可终止: \n";
    for(auto& c : can_not_end){
        tmp.push_back(c);
        tmp.push_back(',');
    }
    tmp += "\n";
    tmp += "不可到达: \n";
    for(auto& c : can_not_reach){
        tmp.push_back(c);
        tmp.push_back(',');
    }
    ui->plainTextEdit_deducted_output_2->setPlainText(tmp);

    ui->plainTextEdit_deduction->setPlainText(grammar->print_all_grammer());
    ui->plainTextEdit_first->setPlainText(grammar->print_first_set());
    ui->plainTextEdit_follow->setPlainText(grammar->print_follow_set());
    has_init = true;
}

void MainWindow::on_pushButton_deducted_clicked()
{
    if(!has_init){
//        QMessageBox msgBox;
//        msgBox.setWindowTitle("title");
//        msgBox.setText("请先进行文法初始化");
//        msgBox.setStandardButtons(QMessageBox::Yes);
//        msgBox.addButton(QMessageBox::No);
//        msgBox.setDefaultButton(QMessageBox::No);
//        return
    }
    auto output = grammar->deduce(ui->lineEdit_wait_deduction->text());
    QString cache;
    cache += ui->lineEdit_wait_deduction->text();
    switch(output.second){
        case 0:
            cache += "推导失败:\n";
            break;
        case 1:
            cache += "推导成功:\n";
            break;
        case 2:
            cache += "推导存在二义性:\n";
            break;
    }

    for(auto& item: output.first){
        cache += item + "\n";
    }
    ui->plainTextEdit_deducted_output->setPlainText(cache);
}
