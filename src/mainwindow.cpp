/*
 * Copyright (C) 2021  Javier Lancha Vázquez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <QThread>

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
:   QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_depth(18),
    m_move_number(1),
    m_white_moves(true)
{
    m_engine.Init(DEFAULT_ENGINE_CMD);

    ui->setupUi(this);

    setWindowTitle(WINDOW_TITLE);

    ui->sbDepth->setValue(m_depth);
    SetNumLines(1);
    ui->sbLines->setValue(1);

    const int max_num_threads = QThread::idealThreadCount();
    const int initial_threads = max_num_threads / 4;
    ui->sbThreads->setMaximum(max_num_threads);
    ui->sbThreads->setToolTip(QString("Maximum number of threads: " +
                                      QString::number(max_num_threads)));
    m_engine.SetNumThreads(initial_threads);
    ui->sbThreads->setValue(initial_threads);

    connect(&m_engine, &UCIEngine::DepthInfoAvailable, this, &MainWindow::OnDepthInfoAvailable);
}

MainWindow::~MainWindow() {
    delete ui;
    m_engine.Close();
}

void MainWindow::OnDepthInfoAvailable(const UCIEngine::DepthInfo& info) {
    const uint32_t line_id = info.line_id;
    if (line_id > m_depth_infos.size()) {
        return;
    }

    m_num_received_lines = line_id;
    m_depth_infos[line_id - 1] = info;  // Lines start counting at 1

    // Black has negative score
    if (!m_white_moves) {
        m_depth_infos[line_id - 1].score *= -1;
    }

    UpdateLineInfo();
}

void MainWindow::SetNumLines(uint8_t num_lines) {
    m_depth_infos.clear();
    m_depth_infos.resize(num_lines);
    m_engine.SetNumLines(num_lines);
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

void MainWindow::RestartSearch() {
    if (ui->bEngineOn->isChecked()) {
        m_engine.Stop();
        m_engine.SearchWithDepth(m_depth);
    }
}

void MainWindow::UpdateLineInfo() {
    ui->teLines->clear();
    for (uint32_t i = 0; i < m_num_received_lines; ++i) {
        auto& info = m_depth_infos[i];
        QStringList move_str_chain;

        /* If black plays, the first move in the sequence belongs to black, and
         * we must omit white's move:
         * n... <black> instead of n. <white> <black>
         */
        uint8_t first_white_move = (m_white_moves)? 0 : 1;
        for (int i = 0; i < info.pv.length(); ++i) {
            if ((i % 2) == first_white_move) {
                move_str_chain.push_back(QString::number(m_move_number + i) + ". " + info.pv[i]);
            } else {
                if ((i != 0) || m_white_moves) {
                    move_str_chain.push_back(info.pv[i]);
                } else {
                    move_str_chain.push_back(QString::number(m_move_number + i) + "... " + info.pv[i]);
                }

            }
        }

        QString score_str;
        if (!info.mate_counter) {
            score_str = "<b>[" + GetSignedScore(info.score) + "]</b>";
        } else {
            score_str = "<b>[M" + QString::number(info.score) + "]</b>";
        }

        ui->teLines->append(score_str + " " + move_str_chain.join(" ") + "<br>");
    }
}

void MainWindow::UpdateMoveList() {
    ui->teMoves->clear();
    const int length = m_moves_list.length();
    QString moves_str;
    for (int i = 0; i < length; ++i) {
        const QString&  move = m_moves_list[i];
        if (i % 2 == 0) {
            moves_str += QString::number(i/2 + 1) + ". " + "<b>" + move + "</b> ";
        } else {
            moves_str += "<b>" + move + "</b> ";
        }
    }

    ui->teMoves->setHtml(moves_str);
}

void MainWindow::on_chInfinite_toggled(bool checked) {
    ui->lDepth->setEnabled(!checked);
    ui->sbDepth->setEnabled(!checked);

    if (!checked) {
        m_engine.Stop();
    }
}

void MainWindow::on_sbThreads_editingFinished() {
    const int threads = ui->sbThreads->value();
    m_engine.SetNumThreads(threads);
}


void MainWindow::on_sbLines_editingFinished() {
    const int num_lines = ui->sbLines->value();
    SetNumLines(num_lines);
}


void MainWindow::on_sbDepth_editingFinished() {
    const int depth = ui->sbDepth->value();
    SetDepth(depth);
}


void MainWindow::on_bSetPosition_clicked() {
    m_moves_list.push_back(ui->lePosition->text().trimmed());
    ui->lePosition->clear();
    m_white_moves = ((m_moves_list.length() % 2) == 0);
    m_move_number = (m_moves_list.length() / 2) + 1;
    m_engine.SetPositionFromMoves(m_moves_list);

    UpdateMoveList();
    RestartSearch();
}

void MainWindow::on_bPrevMove_clicked() {
    if (!m_moves_list.isEmpty()) {
        m_moves_list.pop_back();
    }

    m_white_moves = ((m_moves_list.length() % 2) == 0);
    m_move_number = (m_moves_list.length() / 2) + 1;

    m_engine.SetPositionFromMoves(m_moves_list);

    UpdateMoveList();
    RestartSearch();
}
