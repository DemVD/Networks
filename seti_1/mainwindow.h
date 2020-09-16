#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTreeWidget>
#include "subwindowenterip.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    string saveToFile();
    void produceTree(IPClass &initIP, QTreeWidgetItem *R, byte_t targetMask = 0);
    void insert(QTreeWidgetItem *R, const IPClass &IP);

private slots:
    // defined slots
    void slotSubWinRet(vector<QString> vecQStr);

    // generated slots
    void on_actionNew_triggered();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_actionEdit_triggered();
    void on_actionAggregate_triggered();

private:
    Ui::MainWindow *ui; // main ui
    subWindowEnterIP *subWindow; // second window
    QTreeWidgetItem *root = new QTreeWidgetItem; // root
    IPClass InitialIP;
    QTreeWidgetItem *selectedItem = new QTreeWidgetItem; // selected item
    int selectedColumn;
    vector<QTreeWidgetItem> vecOfTopLevelItems; // top level items
    vector<QTreeWidgetItem> vecOfTreeIpItems; // tree of ips
    QTreeWidgetItem *treeForHostsROOT = new QTreeWidgetItem; // has the root of the bintree
    bool empty; // ui empty flag
};

#endif // MAINWINDOW_H
