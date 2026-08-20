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

#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QTimer>
#include <memory>
#include <vector>

#include "board.hpp"
#include "chess.hpp"
#include "chessboardwidget.h"
#include "player.hpp"
#include "settingsdialog.h"
#include "uciengine.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 public slots:
  void OnDepthInfoAvailable();
  void OnMoveDone(const chess::Move& move);

 private slots:
  void SetBoardPalette(const ChessBoardWidget::ChessPalette& palette);

  void on_bEngineOn_toggled(bool checked);
  void on_chInfinite_toggled(bool checked);
  void on_sbThreads_editingFinished();
  void on_sbLines_valueChanged(int value);
  void on_sbDepth_editingFinished();
  void on_actionSet_FEN_position_triggered();
  void on_actionNew_game_triggered();
  void on_actionExit_triggered();
  void on_bRotateBoard_clicked();
  void on_actionSettings_triggered();
  void on_bSettings_clicked();
  void on_actionRestart_triggered();
  void on_bDownload_clicked();
  void on_bGoStart_clicked();
  void on_bGoBack_clicked();
  void on_bGoForward_clicked();
  void on_bGoEnd_clicked();
  void on_actionLoad_PGN_triggered();

  void on_teMoves_anchorClicked(const QUrl& url);
  void on_teLines_itemClicked(QListWidgetItem* item);

 private:
  Ui::MainWindow* ui;

  friend class EnginePlayer;

  void Init();

  const char* WINDOW_TITLE = "Chess";

  UCIEngine m_engine;
  const char* DEFAULT_ENGINE_CMD = "stockfish";

  uint8_t m_depth = 20;

  bool m_show_lines = true;

  std::unique_ptr<Player> m_white_player =
      std::make_unique<LocalPlayer>(chess::Colour::WHITE, m_board);
  std::unique_ptr<Player> m_black_player =
      std::make_unique<LocalPlayer>(chess::Colour::BLACK, m_board);

  ChessBoardWidget* m_board;

  std::unique_ptr<SettingsDialog> m_settings_dialog;

  /** Engine lines data: UCI move strings for each line */
  std::vector<QStringList> m_engine_lines_data;

  void NewGame();
  void RestartSearch();
  void ScheduleRestartSearch();
  void DoRestartSearch();
  QTimer m_restart_timer;
  void UpdateMoveList();
  void SetEngineEnabled(bool enabled);
  void SetNumLines(uint8_t num_lines);
  void SetDepth(uint8_t depth);
  bool ResetPosition(const QString& fen_str);
  QString GetSignedScoreStr(int cp_score) const;
  int GetColourScore(chess::Colour colour, int score) const;
  void ShowMsgBox(const QString& title, const QString& text);
  void SetEngineControlsEnabled(bool enabled);
  std::unique_ptr<Player>& GetNextPlayer(chess::Colour colour);
};
#endif  // _CHESS_INCLUDE_MAINWINDOW_HPP_
