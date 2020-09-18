#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this); // обычные действия констр-ра
    subWindow = new subWindowEnterIP(); // иниц-я второго окна
    //QTreeWidgetItem *root = new QTreeWidgetItem;
    empty = true;
    connect(subWindow, SIGNAL(signalSubWinRet(vector<QString>)),
            this, SLOT(slotSubWinRet(vector<QString>)));
    // a heavy vector with RFC banned ips, exists only in InitialIP.
    InitialIP.setRFCVect();
}

MainWindow::~MainWindow(){
    delete ui;
}

string MainWindow::saveToFile(){

}

void MainWindow::produceTree(IPClass &initIP, QTreeWidgetItem *R, byte_t targetMask){
    IPClass targetIP = initIP; // copy of initial IP
    if(targetMask == 0){
        targetIP.produceAdressForHosts(); // now it has the target mask (bottom level leaf)
    }
    else{ // target mask specified
        targetIP.produceAdressForHosts();
        targetIP.setMask(targetMask);
        initIP.setMask(root->text(0));
    }
    vector<IPClass> ipClassVect = initIP.produceOneLevelBranch();

    // now generate pair for level in tree
    // initialIP has the starting mask
    // tempIpObj has the target IP

    QTreeWidgetItem *childRow9 = new QTreeWidgetItem;
    childRow9->setText(0, "Tree for Hosts:");
    childRow9->setText(1, initIP.getQStrUserInputHosts());
    childRow9->setBackground(0, QColor(224,224,224,255));
    childRow9->setBackground(1, QColor(224,224,224,255));
    R->addChild(childRow9);

    while( (initIP.getMask() != targetIP.getMask())){ // while not target mask
        if(initIP.getMask()>31) break;
        QTreeWidgetItem *subChildRow1 = new QTreeWidgetItem;
        insert(subChildRow1, ipClassVect[0]);
        insert(subChildRow1, ipClassVect[1]);
        subChildRow1->setText(0, ipClassVect[0].getQStrIPAndMask());
        subChildRow1->setText(1, ipClassVect[1].getQStrIPAndMask());
        if(initIP.getIsRightSon()){ // our IP is in right son
            subChildRow1->setBackground(0, QColor(192,192,192,255));
            subChildRow1->setBackground(1, QColor(128,255,128,255));
            vecOfTreeIpItems.push_back(*subChildRow1);
        }
        else{ // our IP is in left son
            subChildRow1->setBackground(0, QColor(128,255,128,255));
            subChildRow1->setBackground(1, QColor(192,192,192,255));
            vecOfTreeIpItems.push_back(*subChildRow1);
        }
        childRow9->addChild(subChildRow1);
        initIP.setMask( initIP.getMask()+1 );
        ipClassVect = initIP.produceOneLevelBranch();
    }

    treeForHostsROOT = childRow9;
}

void MainWindow::slotSubWinRet(vector<QString> vecQStr){
    // Create IP var, calc its data, insert into widget
    if(!empty){ // second func call means we're editing
        delete root;
        root = new QTreeWidgetItem;
        ui->treeWidget->clear();
    }
    InitialIP.setIP(vecQStr);
    InitialIP.calcIPData();

    // CHECK FOR RFC in IPCALC CLASS
    QStringList QStrL_ForbiddenIpRange;
    IPClass ip1, ip2;
    for(auto elem:*InitialIP.getVecPairs_IpRfc()){
        QStrL_ForbiddenIpRange = elem[0].split(' '); // gives a vec[2] with ip range
        ip1.setIP(QStrL_ForbiddenIpRange[0]);
        ip1.setMask(QStrL_ForbiddenIpRange[0]);
        ip2.setIP(QStrL_ForbiddenIpRange[1]);
        ip2.setMask(QStrL_ForbiddenIpRange[1]);
        if(InitialIP.getMask()==ip1.getMask()){ // Initial mask == forbidden mask
            if( (InitialIP>=ip1)&&(InitialIP<=ip2) ){ // initial ip in range of forbidden ips
                ui->statusBar->showMessage("Forbidden IP, "+elem[1],30000);
                return;
            }
        }
    }

    empty = false;
    root->setBackground(0, QColor(224,224,224,255));
    root->setBackground(1, QColor(224,224,224,255));
    root->setText(1,"Initial IP");
    ui->treeWidget->addTopLevelItem(root);
    insert(root, InitialIP);
    if(InitialIP.getUserInputHosts() <= InitialIP.getAvailableHosts()){ // subnetting is possible
        //qDebug()<<"USER INP: "<<IP.getUserInputHosts()<<"IP AVAIL HOSTS: "<<IP.getAvailableHosts();
        produceTree(InitialIP, root);
        ui->statusBar->showMessage("OK.",6000);
    }
    else{ // subnetting inpossible
        ui->statusBar->showMessage("Ошибка. Кол-во хостов вне допустимого промежутка для данного CIDR IP.",6000);

    }
}

void MainWindow::insert(QTreeWidgetItem *R, const IPClass &IP){
    // TOP item
    R->setText(0, IP.getQStrIPAndMask()); // set col 0 text
    R->setBackground(0, QColor(224,224,224,255));
    R->setBackground(1, QColor(220,220,220,255));
    //ui->treeWidget->addTopLevelItem(R); // ADD TOP LEVEL item

    // FIRST ROW DATA
    QTreeWidgetItem *childRow1 = new QTreeWidgetItem; // CHILD item - first row
    childRow1->setText(0, "IP:");
    childRow1->setText(1, IP.getQStrIP());
    childRow1->setBackground(0, QColor(230,230,230,255));
    childRow1->setBackground(1, QColor(230,230,230,255));
    R->addChild(childRow1); // add  this first row data (CHILD ITEM) to TOP item
    // SECOND ROW DATA
    QTreeWidgetItem *childRow2 = new QTreeWidgetItem; // CHILD item - second row
    childRow2->setText(0, "Subnet ID:");
    childRow2->setText(1, IP.getQStrSubNetID());
    childRow2->setBackground(0, QColor(245,245,245,255));
    childRow2->setBackground(1, QColor(245,245,245,255));
    R->addChild(childRow2);
    // ROW 3
    QTreeWidgetItem *childRow3 = new QTreeWidgetItem;
    childRow3->setText(0, "Broadcast:");
    childRow3->setText(1, IP.getQStrBroadCast());
    childRow3->setBackground(0, QColor(230,230,230,255));
    childRow3->setBackground(1, QColor(230,230,230,255));
    R->addChild(childRow3);
    // ROW 4
    QTreeWidgetItem *childRow4 = new QTreeWidgetItem;
    childRow4->setText(0, "Min IP:");
    childRow4->setText(1, IP.getQStrMinIPAdress());
    childRow4->setBackground(0, QColor(245,245,245,255));
    childRow4->setBackground(1, QColor(245,245,245,255));
    R->addChild(childRow4);
    // ROW 5
    QTreeWidgetItem *childRow5 = new QTreeWidgetItem;
    childRow5->setText(0, "Max IP:");
    childRow5->setText(1, IP.getQStrMaxIPAdress());
    childRow5->setBackground(0, QColor(230,230,230,255));
    childRow5->setBackground(1, QColor(230,230,230,255));
    R->addChild(childRow5);
    // ROW 6
    QTreeWidgetItem *childRow6 = new QTreeWidgetItem;
    childRow6->setText(0, "Hosts:");
    childRow6->setText(1, IP.getQStrAvailableHosts());
    childRow6->setBackground(0, QColor(245,245,245,255));
    childRow6->setBackground(1, QColor(245,245,245,255));
    R->addChild(childRow6);
    // ROW 7
    QTreeWidgetItem *childRow7 = new QTreeWidgetItem;
    childRow7->setText(0, "Subnets:");
    childRow7->setText(1, IP.getQStrAvailableSubnets());
    childRow7->setBackground(0, QColor(230,230,230,255));
    childRow7->setBackground(1, QColor(230,230,230,255));
    R->addChild(childRow7);
    // ROW 8
    QTreeWidgetItem *childRow8 = new QTreeWidgetItem;
    childRow8->setText(0, "Subnet Range List:");
    childRow8->setText(1, "ID - BC");
    childRow8->setBackground(0, QColor(224,224,224,255));
    childRow8->setBackground(1, QColor(224,224,224,255));
    //item->addChild(childRow8);
    vector<vector<byte_t>> subNets = IP.getVectOfIPNetsInRange();
    for(unsigned i=0;i<subNets.size()-1;i+=2){
        QTreeWidgetItem *ch = new QTreeWidgetItem;
        ch->setText(0, IP.convVecToQStr(subNets[i]));
        ch->setText(1, IP.convVecToQStr(subNets[i+1]));
        ch->setBackground(0, QColor(192,192,192,255));
        ch->setBackground(1, QColor(182,182,182,255));
        childRow8->addChild(ch);
    }
    R->addChild(childRow8);
    /*// ROW 9 (separator)
    QTreeWidgetItem *childRow9 = new QTreeWidgetItem;
    childRow9->setBackground(0, QColor(245,245,245,255));
    childRow9->setBackground(1, QColor(245,245,245,255));
    R->addChild(childRow9);*/
}

void MainWindow::on_actionNew_triggered(){
    if(empty){ // empty widget
        ui->treeWidget->clear();
        ui->treeWidget->setColumnCount(2);
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
            ui->treeWidget->setColumnCount(2);
            return;
        case QMessageBox::Cancel: // cancel clicked
            return;
        case 0: // dont save clicked
            ui->treeWidget->clear();
            root = new QTreeWidgetItem;
            ui->treeWidget->setColumnCount(2);
            subWindow->exec();
            break;
        default:
            ui->treeWidget->clear();
            root = new QTreeWidgetItem;
            ui->treeWidget->setColumnCount(2);
            subWindow->exec();
            return;
        }
    }
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column){
    selectedItem = item;
    selectedColumn = column;
}

void MainWindow::on_actionEdit_triggered(){
    // Diff between top and bottom items is that a top-level item has no parent()
    // It is used to know if an item is top level or not.
    if(!selectedItem->parent()){
        if(selectedItem == root){
            subWindow->exec();
        }
    }
    else{
        ui->statusBar->showMessage("Можно редактировать только Initial IP",3000);
    }
    //ui->treeWidget->editItem(selectedItem);
}

void MainWindow::on_actionAggregate_triggered(){
    if(selectedItem->parent()){
        if(selectedItem->parent()->text(0) == treeForHostsROOT->text(0)){
            QMessageBox msgBox;
            msgBox.setWindowTitle("Aggregate");
            msgBox.setText("Aggregate subnets under selected subnet?");
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.setDefaultButton(QMessageBox::Yes);
            msgBox.addButton(QMessageBox::No);
            msgBox.setIcon(QMessageBox::Question);
            int userChoise = msgBox.exec();
            switch(userChoise){
            case QMessageBox::Yes:{ // yes was clicked
                QTreeWidgetItem *newTopLvlItem = new QTreeWidgetItem;
                IPClass newIP; // create new ip, fill it with calculated data
                newIP.setIP(selectedItem->text(selectedColumn));
                newIP.setMask(selectedItem->text(selectedColumn));
                newIP.calcIPData();

                newTopLvlItem->setText(1, "Aggregated"); // add new top level item
                ui->treeWidget->addTopLevelItem(newTopLvlItem);
                insert(newTopLvlItem, newIP); // insert basic details
                newIP.setUserInputHosts(InitialIP.getUserInputHosts()); // to make the same bintree
                produceTree(newIP, newTopLvlItem); // build the aggregated lower part of the tree
                vecOfTopLevelItems.push_back(*newTopLvlItem);

                if(selectedItem->parent()->parent()->text(1) == "Initial IP"){
                    treeForHostsROOT = selectedItem->parent();
                }
                return;
            }
            case QMessageBox::No: // cancel clicked
                return;
            default:
                return;
            }
        }
        for(QTreeWidgetItem elem:vecOfTopLevelItems){
            if(selectedItem->parent()->parent()->text(0) == elem.text(0)){
                QMessageBox msgBox;
                msgBox.setWindowTitle("Aggregate");
                msgBox.setText("Aggregate subnets under selected subnet?");
                msgBox.setStandardButtons(QMessageBox::Yes);
                msgBox.setDefaultButton(QMessageBox::Yes);
                msgBox.addButton(QMessageBox::No);
                msgBox.setIcon(QMessageBox::Question);
                int userChoise = msgBox.exec();
                switch(userChoise){
                case QMessageBox::Yes:{ // yes was clicked
                    QTreeWidgetItem *newTopLvlItem = new QTreeWidgetItem;
                    IPClass newIP; // create new ip, fill it with calculated data
                    newIP.setIP(selectedItem->text(selectedColumn));
                    newIP.setMask(selectedItem->text(selectedColumn));
                    newIP.calcIPData();

                    newTopLvlItem->setText(1, "Aggregated"); // add new top level item
                    ui->treeWidget->addTopLevelItem(newTopLvlItem);
                    insert(newTopLvlItem, newIP); // insert basic details
                    newIP.setUserInputHosts(InitialIP.getUserInputHosts()); // to make the same bintree
                    produceTree(newIP, newTopLvlItem); // build the aggregated lower part of the tree
                    vecOfTopLevelItems.push_back(*newTopLvlItem);
                    return;
                }
                case QMessageBox::No: // cancel clicked
                    return;
                default:
                    return;
                }
            }
        }
        ui->statusBar->showMessage("Можно редактировать только Initial IP или Aggregated IP.",3000);
    }
}


void MainWindow::on_actionDelete_triggered(){
    if(selectedItem->parent() == treeForHostsROOT){
        ui->statusBar->showMessage("Удалять можно только Aggregated IP.",3000);
    }
    for(unsigned i=0;i<vecOfTopLevelItems.size();i++){
        if(selectedItem->text(0) == vecOfTopLevelItems[i].text(0)){
            QMessageBox msgBox;
            msgBox.setWindowTitle("Delete");
            msgBox.setText("Delete selected subnet?");
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.setDefaultButton(QMessageBox::Yes);
            msgBox.addButton(QMessageBox::No);
            msgBox.setIcon(QMessageBox::Question);
            int userChoise = msgBox.exec();
            switch(userChoise){
            case QMessageBox::Yes:{ // yes was clicked
                vector<QTreeWidgetItem> tempVec;
                for(auto elem:vecOfTopLevelItems){
                    if(elem.text(0) != selectedItem->text(0)){
                        tempVec.push_back(elem);
                    }
                }
                delete ui->treeWidget->takeTopLevelItem(i+1);
                vecOfTopLevelItems = tempVec;
                return;
            }
            case QMessageBox::No: // cancel clicked
                return;
            default:
                return;
            }
        }
    }
    ui->statusBar->showMessage("Удалять можно только Aggregated subnets.",3000);
}
