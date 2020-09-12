#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this); // обычные действия констр-ра
    subWindow = new subWindowEnterIP(); // иниц-я второго окна
    empty = true;
    connect(subWindow, SIGNAL(signalSubWinRet(vector<QString>)),
            this, SLOT(slotSubWinRet(vector<QString>)));
}

MainWindow::~MainWindow(){
    delete ui;
}

string MainWindow::saveToFile(){

}

void MainWindow::slotSubWinRet(vector<QString> vecQStr){
    // POSSIBLE CHECK FOR RFC in IPCALC CLASS
    // Create IP var, calc its data, insert into widget
    empty = false;
    IPClass IP;
    IP.setIP(vecQStr);
    IP.calcIPData();
    insert(IP);
    if(IP.getUserInputHosts() <= IP.getAvailableHosts()){ // subnetting is possible
        qDebug()<<"USER INP: "<<IP.getUserInputHosts()<<"IP AVAIL HOSTS: "<<IP.getAvailableHosts();
        IP.produceAdressForHosts();
        qDebug()<<IP.getQStrIPAndMask();
        ui->statusBar->showMessage("OK.",6000);
    }
    else{ // subnetting inpossible
        ui->statusBar->showMessage("Ошибка. Кол-во хостов вне допустимого промежутка для данного CIDR IP.",6000);

    }
}

void MainWindow::insert(const IPClass &IP){
    QTreeWidgetItem *item = new QTreeWidgetItem; // TOP item
    item->setText(0, IP.getQStrIPAndMask()); // set col 0 text
    ui->treeWidget->addTopLevelItem(item); // ADD TOP LEVEL item

    // FIRST ROW DATA
    QTreeWidgetItem *childRow1 = new QTreeWidgetItem; // CHILD item - first row
    childRow1->setText(0, "IP:");
    childRow1->setText(1, IP.getQStrIP());
    item->addChild(childRow1); // add  this first row data (CHILD ITEM) to TOP item
    // SECOND ROW DATA
    QTreeWidgetItem *childRow2 = new QTreeWidgetItem; // CHILD item - second row
    childRow2->setText(0, "Subnet ID:");
    childRow2->setText(1, IP.getQStrSubNetID());
    item->addChild(childRow2);
    // ROW 3
    QTreeWidgetItem *childRow3 = new QTreeWidgetItem;
    childRow3->setText(0, "Broadcast:");
    childRow3->setText(1, IP.getQStrBroadCast());
    item->addChild(childRow3);
    // ROW 4
    QTreeWidgetItem *childRow4 = new QTreeWidgetItem;
    childRow4->setText(0, "Min IP:");
    childRow4->setText(1, IP.getQStrMinIPAdress());
    item->addChild(childRow4);
    // ROW 5
    QTreeWidgetItem *childRow5 = new QTreeWidgetItem;
    childRow5->setText(0, "Max IP:");
    childRow5->setText(1, IP.getQStrMaxIPAdress());
    item->addChild(childRow5);
    // ROW 6
    QTreeWidgetItem *childRow6 = new QTreeWidgetItem;
    childRow6->setText(0, "Hosts:");
    childRow6->setText(1, IP.getQStrAvailableHosts());
    item->addChild(childRow6);
    // ROW 7
    QTreeWidgetItem *childRow7 = new QTreeWidgetItem;
    childRow7->setText(0, "Subnets:");
    childRow7->setText(1, IP.getQStrAvailableSubnets());
    item->addChild(childRow7);
    // ROW 8
    QTreeWidgetItem *childRow8 = new QTreeWidgetItem;
    childRow8->setText(0, "Subnet Range List:");
    //item->addChild(childRow8);
    vector<vector<byte_t>> subNets = IP.getVectOfIPNetsInRange();
    for(unsigned i=0;i<subNets.size()-1;i+=2){
        QTreeWidgetItem *ch = new QTreeWidgetItem;
        ch->setText(0, IP.convVecToQStr(subNets[i]));
        ch->setText(1, IP.convVecToQStr(subNets[i+1]));
        childRow8->addChild(ch);
    }
    item->addChild(childRow8);

    /*
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, "Stolbes 0");
    item->setText(1, "Stolbes 1");
    item->setText(2, "Stolbes 2");
    item->setText(3, "Stolbes 3");
    item->setText(4, "Stolbes 4");

    item->setBackground(0, Qt::red);
    item->setBackground(1, Qt::blue);
    item->setBackground(2, Qt::yellow);
    item->setBackground(3, Qt::green);
    item->setBackground(4, Qt::gray);

    item->setForeground(0, QColor(255,255,255,255));
    item->setForeground(1, QColor(255,255,255,255));
    item->setForeground(2, QColor(200,0,100,255));
    item->setForeground(3, QColor(0,0,0,255));
    item->setForeground(4, QColor(255,255,255,255));

    ui->treeWidget->setColumnCount(5);
    ui->treeWidget->addTopLevelItem(item);

    // must be filled like item1
    QTreeWidgetItem *item2 = new QTreeWidgetItem;
    ui->treeWidget->addTopLevelItem(item2);

    QTreeWidgetItem *child0 = new QTreeWidgetItem;
    QTreeWidgetItem *child1 = new QTreeWidgetItem;
    QTreeWidgetItem *child2 = new QTreeWidgetItem;
    QTreeWidgetItem *child3 = new QTreeWidgetItem;

    child0->setText(0, "Text child 0 stolbes 0");
    child1->setText(1, "Text child 1 stolbes 1");
    child2->setText(2, "Text child 2 stolbes 2");
    child3->setText(3, "Text child 3 stolbes 3");

    item->addChild(child0);
    item->addChild(child1);
    item->addChild(child2);
    item->addChild(child3);
    */
}

void MainWindow::on_actionNew_triggered(){
    if(empty){ // empty widget
        ui->treeWidget->clear();
        subWindow->exec();
    }
    else{  // changes were made (widget not empty)
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
            ui->treeWidget->clear();
            return;
        case QMessageBox::Cancel: // cancel clicked
            return;
        case 0: // dont save clicked
            ui->treeWidget->clear();
            subWindow->exec();
            break;
        default:
            ui->treeWidget->clear();
            subWindow->exec();
            return;
        }
    }
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column){
    qDebug()<<"CLICK GOT YEETED!";
    qDebug()<<column;
    qDebug()<<item->text(column);
}
