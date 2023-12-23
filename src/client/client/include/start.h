//
// Created by 10315 on 2023/12/19.
//

#ifndef KINGDOM_CARD_START_H
#define KINGDOM_CARD_START_H

#include <QMainWindow>
#include "../ui/ui_startwindow.h"
#include "../../communicator/communicator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class StartWindow : public QMainWindow
{
Q_OBJECT

public:
    QString address;
    unsigned port;

    StartWindow(QWidget *parent = nullptr);
    ~StartWindow();

private:
    Ui::StartWindow *ui;

public slots:
    void GameStart();
    void ShutDown(const BasicMessage &message);
};

#endif //KINGDOM_CARD_START_H
