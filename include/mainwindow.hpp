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

#ifndef _CHESS_INCLUDE_MAINWINDOW_HPP_
#define _CHESS_INCLUDE_MAINWINDOW_HPP_

#include <vector>

#include <QLabel>
#include <QMainWindow>

#include "uciengine.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void OnDepthInfoAvailable(const UCIEngine::DepthInfo& depth_info);

private slots:
    void on_bEngineOn_toggled(bool checked);
    void on_chInfinite_toggled(bool checked);
    void on_sbThreads_editingFinished();
    void on_sbLines_editingFinished();
    void on_sbDepth_editingFinished();
    void on_bSetPosition_clicked();
    void on_bPrevMove_clicked();

    void on_actionSet_FEN_position_triggered();

private:
    Ui::MainWindow *ui;

    const char* WINDOW_TITLE = "Chess";

    UCIEngine m_engine;
    const char* DEFAULT_ENGINE_CMD = "stockfish";

    uint8_t m_depth;

    QStringList m_moves_list;
    uint32_t m_start_move_number;
    bool m_white_moves;

    std::vector<UCIEngine::DepthInfo> m_depth_infos;
    uint32_t m_num_received_lines = 0;

    uint32_t CurrentMoveNumber() const;

    void RestartSearch();

    void UpdateLineInfo();
    void UpdateMoveList();

    void SetNumLines(uint8_t num_lines);
    void SetDepth(uint8_t depth);

    bool SetPosition(const QString& fen_str);

    QString GetSignedScore(int cp_score) const {
        QString sign = (cp_score > 0)? "+" : "";
        return sign + QString::number(static_cast<float>(cp_score) / 100, 'f', 2);
    }

    void ShowMsgBox(const QString& title, const QString& text);
};
#endif  // _CHESS_INCLUDE_MAINWINDOW_HPP_
