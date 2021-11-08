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

#include "board.hpp"
#include "chess.hpp"
#include "chessboardwidget.h"
#include "settingsdialog.h"
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
    /**
     * @brief Depth search info available from the engine
     * @param depth_info
     */
    void OnDepthInfoAvailable(const UCIEngine::DepthInfo& depth_info);

private slots:
    void SetBoardPalette(const ChessBoardWidget::ChessPalette& palette);

    void on_bEngineOn_toggled(bool checked);
    void on_chInfinite_toggled(bool checked);
    void on_sbThreads_editingFinished();
    void on_sbLines_editingFinished();
    void on_sbDepth_editingFinished();
    void on_actionSet_FEN_position_triggered();
    void on_actionNew_game_triggered();
    void on_actionExit_triggered();
    void on_bRotateBoard_clicked();
    void on_actionSettings_triggered();
    void on_bSettings_clicked();

private:
    Ui::MainWindow *ui;

    /**
     * @brief Initialise window.
     */
    void Init();

    const char* WINDOW_TITLE = "Chess";

    UCIEngine m_engine;
    const char* DEFAULT_ENGINE_CMD = "stockfish";

    /** Engine search depth. */
    uint8_t m_depth;

    /** List of moves from starting position. */
    QStringList m_moves_list;

    /** Number of half moves from starting position. */
    uint32_t m_start_half_moves;

    std::vector<UCIEngine::DepthInfo> m_depth_infos;
    uint32_t m_num_received_lines = 0;

    ChessBoardWidget* m_board;

    SettingsDialog* m_settings_dialog;

    /** Current move number as in game. First move is 1. */
    uint32_t CurrentMoveNumber() const;

    void NewGame();

    /**
     * @brief Restart engine search.
     */
    void RestartSearch();

    void UpdateLineInfo();
    void UpdateMoveList();

    void SetNumLines(uint8_t num_lines);

    /**
     * @brief Set engine search depth.
     * @param depth Search depth.
     */
    void SetDepth(uint8_t depth);

    /**
     * @brief Set position using a FEN string.
     * @param fen_str FEN string.
     * @return true if the position is valid.
     */
    bool SetPosition(const QString& fen_str);

    /**
     * @brief Get a score string with a + or - sign.
     * @param cp_score Score is centipawns.
     * @return A score string with a + or - sign.
     */
    QString GetSignedScore(int cp_score) const {
        QString sign = (cp_score > 0)? "+" : "";
        return sign + QString::number(static_cast<float>(cp_score) / 100, 'f', 2);
    }

    /**
     * @brief Display a message box.
     * @param title Window title.
     * @param text Message.
     */
    void ShowMsgBox(const QString& title, const QString& text);
};
#endif  // _CHESS_INCLUDE_MAINWINDOW_HPP_
