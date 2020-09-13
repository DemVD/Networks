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
    void produceTree(IPClass initialIP);
    void insert(QTreeWidgetItem *R, const IPClass &IP);

private slots:
    void slotSubWinRet(vector<QString> vecQStr);
    void on_actionNew_triggered();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow *ui;
    subWindowEnterIP *subWindow; // second window
    QTreeWidgetItem *root = new QTreeWidgetItem;
    bool empty;
};

#endif // MAINWINDOW_H
