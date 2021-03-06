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

#include "mainwindow.hpp"

#include <unistd.h>

#include <QColor>
#include <QInputDialog>
#include <QMessageBox>
#include <QThread>

#include "resources.hpp"
#include "settingsdialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  m_board = ui->boardWidget;
  m_settings_dialog = new SettingsDialog;
  connect(m_settings_dialog, &SettingsDialog::PaletteChanged, this,
          &MainWindow::SetBoardPalette);

  Init();
  connect(&m_engine, &UCIEngine::DepthInfoAvailable, this,
          &MainWindow::OnDepthInfoAvailable);
  connect(m_board, &ChessBoardWidget::MoveDone, this, &MainWindow::OnMoveDone);
}

MainWindow::~MainWindow() {
  delete ui;
  m_engine.Close();
}

void MainWindow::Init() {
  // GUI defaults
  setWindowTitle(WINDOW_TITLE);
  ui->bEngineOn->setPalette(QColor(Qt::red));

  ui->bRotateBoard->setIcon(resources::ROTATE_ICON);
  ui->bSettings->setIcon(resources::SETTINGS_ICON);
  ui->bEngineOn->setIcon(resources::ENGINE_ICON);
  ui->bDownload->setIcon(resources::DOWNLOAD_ICON);

  m_board->SetActiveColour(chess::Colour::WHITE);
  m_board->SetSelectableColour(chess::Colour::WHITE);

  // Engine defaults
  m_engine.Init(DEFAULT_ENGINE_CMD);
  SetEngineEnabled(false);

  SetDepth(20);

  ui->sbDepth->setValue(m_depth);
  SetNumLines(1);

  ui->sbLines->setValue(1);

  const int max_num_threads = QThread::idealThreadCount();
  const int initial_threads = max_num_threads / 4;
  ui->sbThreads->setMaximum(max_num_threads);
  ui->sbThreads->setToolTip(
      QString("Number of CPU threads to use.\nMaximum number of threads: " +
              QString::number(max_num_threads)));
  m_engine.SetNumThreads(initial_threads);
  ui->sbThreads->setValue(initial_threads);

  NewGame();
}

uint32_t MainWindow::CurrentMoveNumber() const {
  const uint32_t half_moves = m_board->GetNumHalfMoves();
  return 1 + ((half_moves + m_moves_list.length()) / 2);
}

void MainWindow::NewGame() {
  m_board->Reset();
  m_moves_list.clear();
  ResetPosition(QString::fromStdString(chess::STARTPOS_FEN));
  m_engine.NewGame();
  m_board->SetSelectableColour(chess::Colour::WHITE);
}

void MainWindow::SetNumLines(uint8_t num_lines) {
  num_lines = std::max(static_cast<uint8_t>(1U), num_lines);
  m_engine.SetNumLines(num_lines);
  RestartSearch();
}

void MainWindow::SetDepth(uint8_t depth) {
  m_depth = depth;
  RestartSearch();
}

void MainWindow::RestartSearch() {
  if (ui->bEngineOn->isChecked()) {
    m_engine.Stop();
    const bool infinite_search = ui->chInfinite->isChecked();
    if (infinite_search) {
      m_engine.SearchInfinite();
    } else {
      m_engine.SearchWithDepth(m_depth);
    }
  }
}

bool MainWindow::ResetPosition(const QString& fen_str) {
  if (fen_str.isEmpty()) {
    return false;
  }

  const char* FEN_SEPARATOR = " ";
  QStringList args = fen_str.trimmed().split(FEN_SEPARATOR);
  for (auto& arg : args) {
    arg = arg.trimmed();
  }

  if (args.length() != 6) {
    return false;
  }

  m_board->SetPosition(fen_str);
  const auto active_colour = m_board->GetActiveColour();
  m_board->SetSelectableColour(active_colour);

  m_moves_list.clear();
  ui->teMoves->clear();

  m_engine.SetPosition(fen_str);
  RestartSearch();

  return true;
}

void MainWindow::OnMoveDone(const chess::Move& move) {
  (void)move;

  const auto active_colour = m_board->GetActiveColour();
  auto& next_player = GetNextPlayer(active_colour);
  m_board->SetSelectableColour(active_colour);
  next_player->Prompt(move);

  m_moves_list.push_back(QString::fromStdString(chess::MoveToUCI(move)));
  UpdateMoveList();

  QString fen_str = m_board->GetFEN();
  m_engine.SetPosition(fen_str);
  RestartSearch();
}

void MainWindow::on_bDownload_clicked() {
  const QString fen_str = m_board->GetFEN();
  ShowMsgBox("Position", fen_str);
}

void MainWindow::ShowMsgBox(const QString& title, const QString& text) {
  QMessageBox msg_box(this);
  msg_box.setWindowTitle(title);
  msg_box.setText(text);
  msg_box.exec();
}

QString MainWindow::GetSignedScoreStr(int cp_score) const {
  QString sign = (cp_score > 0) ? "+" : "";
  return sign + QString::number(static_cast<float>(cp_score) / 100, 'f', 2);
}

int MainWindow::GetColourScore(chess::Colour colour, int score) const {
  if (colour == chess::Colour::WHITE) {
    return score;
  } else {
    return -score;
  }
}

void MainWindow::OnDepthInfoAvailable() {
  ui->teLines->clear();
  std::vector<UCIEngine::DepthInfo> lines = m_engine.GetLines();
  const auto colour = m_board->GetActiveColour();

  for (uint32_t i = 0; i < lines.size(); ++i) {
    auto& info = lines[i];
    QStringList move_str_chain;

    // Show info in widget
    if (m_show_lines) {
      /* If black plays, the first move in the sequence belongs to black, and
       * we must omit white's move:
       * n... <black> instead of n. <white> <black>
       */
      const uint32_t starting_half_move_num = m_board->GetNumHalfMoves();
      for (int i = 0; i < info.pv.length(); ++i) {
        const uint32_t half_move_number = (starting_half_move_num + i);
        if ((half_move_number % 2) == 0) {  // White move
          move_str_chain.push_back(QString::number(1 + (half_move_number / 2)) +
                                   ". " + info.pv[i]);
        } else {  // Black move
          if (i == 0) {
            move_str_chain.push_back(
                QString::number(1 + (half_move_number / 2)) + "... " +
                info.pv[i]);
          } else {
            move_str_chain.push_back(info.pv[i]);
          }
        }
      }

      const int score = GetColourScore(colour, info.score);

      QString score_str;
      if (!info.mate_counter) {
        score_str = "<b>[" + GetSignedScoreStr(score) + "]</b>";
      } else {
        QString signed_mate_str = (score >= 0) ? "M" : "-M";
        score_str =
            "<b>[" + signed_mate_str + QString::number(abs(score)) + "]</b>";
      }
      ui->teLines->append(score_str + " " + move_str_chain.join(" ") + "<br>");

      // Send score to board widget
      if (i == 0) {
        m_board->SetScore(score, info.mate_counter);
      }
    }
  }
}

void MainWindow::UpdateMoveList() {
  ui->teMoves->clear();
  const int length = m_moves_list.size();
  QString moves_str;
  const uint32_t starting_half_move_num =
      m_board->GetNumHalfMoves() - m_moves_list.size();
  for (int i = 0; i < length; ++i) {
    const QString& move = m_moves_list[i];
    const uint32_t half_moves = (starting_half_move_num + (i + 1));
    if ((half_moves % 2) != 0) {
      moves_str +=
          QString::number(1 + (half_moves / 2)) + ". " + "<b>" + move + "</b> ";
    } else {
      moves_str += "<b>" + move + "</b> ";
    }
  }

  ui->teMoves->setHtml(moves_str);
}

void MainWindow::SetEngineEnabled(bool enabled) {
  if (enabled) {
    ui->bEngineOn->setPalette(QColor(Qt::green));
    const bool infinite_search = ui->chInfinite->isChecked();
    if (infinite_search) {
      m_engine.SearchInfinite();
    } else {
      m_engine.SearchWithDepth(m_depth);
    }
  } else {
    ui->bEngineOn->setPalette(QColor(Qt::red));
    m_engine.Stop();
    ui->teLines->clear();
  }

  m_board->SetScoreEnabled(enabled);
  SetEngineControlsEnabled(enabled);
}

void MainWindow::SetEngineControlsEnabled(bool enabled) {
  ui->lDepth->setVisible(enabled);
  ui->sbDepth->setVisible(enabled);
  ui->chInfinite->setVisible(enabled);
  ui->lLines->setVisible(enabled);
  ui->sbLines->setVisible(enabled);
  ui->lThreads->setVisible(enabled);
  ui->sbThreads->setVisible(enabled);
  ui->sbDepth->setVisible(enabled);
  ui->teLines->setVisible(enabled);
}

std::unique_ptr<Player>& MainWindow::GetNextPlayer(chess::Colour colour) {
  return ((colour == chess::Colour::WHITE) ? m_white_player : m_black_player);
}

void MainWindow::on_bEngineOn_toggled(bool checked) {
  SetEngineEnabled(checked);
}

void MainWindow::on_chInfinite_toggled(bool checked) {
  ui->lDepth->setEnabled(!checked);
  ui->sbDepth->setEnabled(!checked);
  RestartSearch();
}

void MainWindow::on_sbThreads_editingFinished() {
  const int threads = ui->sbThreads->value();
  m_engine.SetNumThreads(threads);
}

void MainWindow::on_sbLines_editingFinished() {
  const int num_lines = ui->sbLines->value();
  m_show_lines = (num_lines > 0);
  ui->teLines->setVisible(m_show_lines);
  SetNumLines(num_lines);
}

void MainWindow::on_sbDepth_editingFinished() {
  const int depth = ui->sbDepth->value();
  SetDepth(depth);
}

void MainWindow::on_actionSet_FEN_position_triggered() {
  QInputDialog* dialog = new QInputDialog(this);
  dialog->setWindowTitle("Set position");
  dialog->setLabelText("FEN position:");
  dialog->resize(450, 300);
  dialog->exec();

  QString fen_str = dialog->textValue();
  const bool position_ok = ResetPosition(fen_str.trimmed());
  if (!position_ok) {
    ShowMsgBox("Error", "Could not set position.");
  }
}

void MainWindow::on_actionNew_game_triggered() { NewGame(); }

void MainWindow::on_actionExit_triggered() { QCoreApplication::quit(); }

void MainWindow::on_bRotateBoard_clicked() { m_board->Rotate(); }

void MainWindow::on_actionSettings_triggered() { m_settings_dialog->exec(); }

void MainWindow::SetBoardPalette(
    const ChessBoardWidget::ChessPalette& palette) {
  m_board->SetColourPalette(palette);
}

void MainWindow::on_bSettings_clicked() { m_settings_dialog->exec(); }

void MainWindow::on_actionRestart_triggered() {
  m_engine.Reset();
  const QString fen = m_board->GetFEN();
  m_engine.SetPosition(fen);
  RestartSearch();
}
