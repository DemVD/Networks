#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "bintree.h"
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

private slots:
    void slotSubWinRet(vector<QString> vecQStr);
    void on_actionNew_triggered();

private:
    Ui::MainWindow *ui;
    subWindowEnterIP *subWindow; // второе окно
    BinTree* root; // tree root
};

#endif // MAINWINDOW_H
