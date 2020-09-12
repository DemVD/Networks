#include "subwindowenterip.h"
#include "ui_subwindowenterip.h"

subWindowEnterIP::subWindowEnterIP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::subWindowEnterIP){
    ui->setupUi(this);
    ui->spinBox->clear(); // чистим содержимое окон ввода
    ui->spinBox_2->clear();
    ui->spinBox_3->clear();
    ui->spinBox_4->clear();
    ui->spinBox_5->clear();
    ui->spinBox_6->clear();
    ui->spinBox->setFocus(); // фокус (мигающий курсор)
    ui->buttonBox->setEnabled(false);
}

subWindowEnterIP::~subWindowEnterIP(){
    delete ui;
}

void subWindowEnterIP::on_buttonBox_accepted(){
    vector<QString> vecQStr; // vec to return with values
    vecQStr.push_back(QString::number(ui->spinBox->value())); // 1st octet
    vecQStr.push_back(QString::number(ui->spinBox_2->value())); // 2nd octet
    vecQStr.push_back(QString::number(ui->spinBox_3->value())); // 3rd octet
    vecQStr.push_back(QString::number(ui->spinBox_4->value())); // 4th octet
    vecQStr.push_back(QString::number(ui->spinBox_5->value())); // mask
    vecQStr.push_back(QString::number(ui->spinBox_6->value())); // HOSTS!
    emit(signalSubWinRet(vecQStr)); // emit signal for main win to catch
    this->close();
}

// проверка завершения ввода данных
void subWindowEnterIP::on_spinBox_editingFinished(){
    ui->buttonBox->setEnabled(true); // включить кнопки ОК и Отмена
}
