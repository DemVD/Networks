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
    void produceTree(IPClass &initialIP);
    void insert(QTreeWidgetItem *R, const IPClass &IP);

private slots:
    // defined slots
    void slotSubWinRet(vector<QString> vecQStr);

    // generated slots
    void on_actionNew_triggered();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_actionEdit_triggered();
    void on_actionAggregate_triggered();
    void on_actiondisaggregate_triggered();

private:
    Ui::MainWindow *ui; // main ui
    subWindowEnterIP *subWindow; // second window
    QTreeWidgetItem *root = new QTreeWidgetItem; // root
    QTreeWidgetItem *selectedItem = new QTreeWidgetItem; // selected item
    vector<QTreeWidgetItem> *vecOfTopLevelItems = new vector<QTreeWidgetItem>;
    QTreeWidgetItem *treeForHostsROOT = new QTreeWidgetItem; // has the root of the bintree
    bool empty; // ui empty flag
};

#endif // MAINWINDOW_H
