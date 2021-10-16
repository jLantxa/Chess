#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
:   QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_engine.Init(DEFAULT_ENGINE_CMD);
}

MainWindow::~MainWindow() {
    delete ui;
    m_engine.Close();
}
