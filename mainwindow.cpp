#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QThread>

MainWindow::MainWindow(QWidget *parent)
:   QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_depth(18),
    m_move_number(1),
    m_white_moves(true)
{
    m_engine.Init(DEFAULT_ENGINE_CMD);

    ui->setupUi(this);

    ui->sbDepth->setValue(m_depth);
    SetNumLines(1);
    ui->sbLines->setValue(1);

    const int max_num_threads = QThread::idealThreadCount();
    const int initial_threads = max_num_threads / 4;
    ui->sbThreads->setMaximum(max_num_threads);
    ui->sbThreads->setToolTip(QString("Maximum number of threads: " + QString::number(max_num_threads)));
    m_engine.SetNumThreads(initial_threads);
    ui->sbThreads->setValue(initial_threads);

    connect(&m_engine, &UCIEngine::BestMoveAvailable, this, &MainWindow::OnBestMoveAvailable);
    connect(&m_engine, &UCIEngine::DepthInfoAvailable, this, &MainWindow::OnDepthInfoAvailable);
}

MainWindow::~MainWindow() {
    delete ui;
    m_engine.Close();
}

void MainWindow::OnBestMoveAvailable(UCIEngine::BestMove best_move) {
    m_best_move = best_move;
    UpdateLineInfo();
}

void MainWindow::OnDepthInfoAvailable(UCIEngine::DepthInfo depth_info) {
    // MultiPV starts at index 1
    const uint8_t line_id = depth_info.line_id - 1;
    m_depth_info[line_id] = depth_info;
    UpdateLineInfo();
}

void MainWindow::SetNumLines(uint8_t num_lines) {
    m_engine.SetNumLines(num_lines);
    m_depth_info.clear();
    m_depth_info.resize(num_lines);
    RestartSearch();
}

void MainWindow::SetDepth(uint8_t depth) {
    m_depth = depth;
    RestartSearch();
}

void MainWindow::on_bEngineOn_toggled(bool checked) {
    if (checked) {
        if (ui->chInfinite->isChecked()) {
            m_engine.SearchInfinite();
        } else {
            m_engine.SearchWithDepth(m_depth);
        }
    } else {
        m_engine.Stop();
    }
}

void MainWindow::on_sbDepth_valueChanged(int depth) {
    SetDepth(depth);
}


void MainWindow::on_sbLines_valueChanged(int num_lines) {
    SetNumLines(num_lines);
}

void MainWindow::RestartSearch() {
    if (ui->bEngineOn->isChecked()) {
        m_engine.Stop();
        m_engine.SearchWithDepth(m_depth);
    }
}

void MainWindow::UpdateLineInfo() {
    ui->teLines->clear();

    if (!m_best_move.bestmove.isEmpty()) {
        ui->teLines->append("<b>Best:</b> " + m_best_move.bestmove + "<br>");
    }

    for (auto info : m_depth_info) {
        QString sign = (info.score > 0)? "+" : "";
        QStringList move_chain;

        if (!m_white_moves) {
            info.pv.push_front("...");
        }

        for (uint32_t i = 0; i < info.pv.length()/2; ++i) {
            move_chain.push_back(QString::number(m_move_number + i) + ". " + info.pv[2*i] + " " + info.pv[2*i + 1]);
        }

        QString score_str = "<b>[" + sign + QString::number(static_cast<float>(info.score)/100) + "]</b>";
        ui->teLines->append(score_str + " " + move_chain.join("  ") + "<br>");
    }
}


void MainWindow::on_sbThreads_valueChanged(int threads) {
    m_engine.SetNumThreads(threads);
}


void MainWindow::on_chInfinite_toggled(bool checked) {
    ui->lDepth->setEnabled(!checked);
    ui->sbDepth->setEnabled(!checked);

    if (!checked) {
        m_engine.Stop();
    }
}

