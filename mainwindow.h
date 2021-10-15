#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "uciengine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    UCIEngine m_engine;
    const char* DEFAULT_ENGINE_CMD = "stockfish";

};
#endif // MAINWINDOW_H
