#ifndef SUBWINDOWENTERIP_H
#define SUBWINDOWENTERIP_H

#include <QDialog>
#include "ipcalc.h"

using namespace std;

namespace Ui {
class subWindowEnterIP;
}

class subWindowEnterIP : public QDialog{
    Q_OBJECT

public:
    explicit subWindowEnterIP(QWidget *parent = nullptr);
    ~subWindowEnterIP();

private slots:
    void on_buttonBox_accepted();
    void on_spinBox_editingFinished();

signals:
    void signalSubWinRet(vector<QString> vecQStr);

private:
    Ui::subWindowEnterIP *ui;
};

#endif // SUBWINDOWENTERIP_H
