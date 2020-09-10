#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this); // обычные действия констр-ра
    subWindow = new subWindowEnterIP(); // иниц-я второго окна
    root = new BinTree;
    connect(subWindow, SIGNAL(signalSubWinRet(vector<QString>)),
            this, SLOT(slotSubWinRet(vector<QString>)));
}

MainWindow::~MainWindow(){
    delete ui;
}

string MainWindow::saveToFile(){
    return 0;
}

void MainWindow::slotSubWinRet(vector<QString> vecQStr){
    // POSSIBLE CHECK FOR IEEE in IPCALC CLASS
    // CREATE IPCLASS VAR
    IPClass IP;
    IP.setIP(vecQStr);
    qDebug()<<"IP:";
    qDebug()<<IP.getQStrIPAndMask();
    qDebug()<<"SUBNET ID:";
    qDebug()<<IP.getQStrSubNetID();
    qDebug()<<"BROADCAST:";
    qDebug()<<IP.getQStrBroadCast();
    qDebug()<<"HOSTS FREE:";
    qDebug()<<IP.AvailableHosts;
    qDebug()<<"SUBNETS FREE:";
    qDebug()<<IP.AvailableSubnets;
    qDebug()<<"Min IP:";
    qDebug()<<IP.getQStrMinIPAdress();
    qDebug()<<"Max IP:";
    qDebug()<<IP.getQStrMaxIPAdress();
    root->insertLeaf(root, IP);
}

void MainWindow::on_actionNew_triggered(){
    if(root->checkEmptyFlag()){ // tree empty
        subWindow->exec();
    }
    else{  // changes were made (tree exists)
        QMessageBox msgBox;
        msgBox.setWindowTitle("New file...");
        msgBox.setText("Save changes to current file?");
        msgBox.setStandardButtons(QMessageBox::Save);
        msgBox.setDefaultButton(QMessageBox::Save);
        msgBox.addButton(tr("Don't save"), QMessageBox::NoRole);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Warning);
        int userChoise = msgBox.exec();
        switch(userChoise){
        case QMessageBox::Save: // save was clicked
            return;
        case QMessageBox::Cancel: // cancel clicked
            return;
        case 0: // dont save clicked
            subWindow->exec();
            break;
        default:
            subWindow->exec();
            return;
        }
    }
}
