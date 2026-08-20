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

#include <QColor>
#include <QFont>
#include <QInputDialog>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QThread>

#include "resources.hpp"
#include "settingsdialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  m_board = ui->boardWidget;
  m_settings_dialog = std::make_unique<SettingsDialog>();
  connect(m_settings_dialog.get(), &SettingsDialog::PaletteChanged, this,
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
  setWindowTitle(WINDOW_TITLE);
  ui->bEngineOn->setPalette(QColor(Qt::red));

  ui->bRotateBoard->setIcon(resources::ROTATE_ICON);
  ui->bSettings->setIcon(resources::SETTINGS_ICON);
  ui->bEngineOn->setIcon(resources::ENGINE_ICON);
  ui->bDownload->setIcon(resources::DOWNLOAD_ICON);

  m_board->SetActiveColour(chess::Colour::WHITE);
  m_board->SetSelectableColour(chess::Colour::WHITE);

  m_engine.Init(DEFAULT_ENGINE_CMD);
  SetEngineEnabled(false);

  m_restart_timer.setSingleShot(true);
  m_restart_timer.setInterval(50);
  connect(&m_restart_timer, &QTimer::timeout, this,
          &MainWindow::DoRestartSearch);

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

void MainWindow::NewGame() {
  m_board->Reset();
  ResetPosition(QString::fromUtf8(chess::STARTPOS_FEN));
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

void MainWindow::RestartSearch() { m_restart_timer.start(); }

void MainWindow::DoRestartSearch() {
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

  UpdateMoveList();

  QString fen_str = m_board->GetFEN();
  m_engine.SetPosition(fen_str);
  RestartSearch();
}

void MainWindow::on_bDownload_clicked() {
  const QString fen_str = m_board->GetFEN();
  const QString pgn_str = m_board->GetPGN();
  QString text = "FEN:\n" + fen_str + "\n\nPGN:\n" + pgn_str;
  ShowMsgBox("Position", text);
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
  if (m_board == nullptr) return;

  QSignalBlocker blocker(ui->teLines);
  ui->teLines->clear();
  const auto lines = m_engine.GetLines();
  const auto colour = m_board->GetActiveColour();

  m_engine_lines_data.clear();

  for (uint32_t i = 0; i < lines.size(); ++i) {
    auto& info = lines[i];

    if (m_show_lines) {
      m_engine_lines_data.push_back(info.pv);

      const QStringList san_moves = m_board->ConvertUciToSan(info.pv);

      // Format: [score] move move move ...
      const int score = GetColourScore(colour, info.score);

      QString score_str;
      if (!info.mate_counter) {
        score_str = GetSignedScoreStr(score);
      } else {
        QString signed_mate_str = (score >= 0) ? "M" : "-M";
        score_str = signed_mate_str + QString::number(abs(score));
      }

      QStringList parts;
      parts << ("[" + score_str + "]");

      const uint32_t starting_half_move_num = m_board->GetNumHalfMoves();
      for (int j = 0; j < san_moves.length(); ++j) {
        const uint32_t half_move_number = (starting_half_move_num + j);
        if ((half_move_number % 2) == 0) {
          parts << (QString::number(1 + (half_move_number / 2)) + "." +
                    san_moves[j]);
        } else {
          if (j == 0) {
            parts << (QString::number(1 + (half_move_number / 2)) + "..." +
                      san_moves[j]);
          } else {
            parts << san_moves[j];
          }
        }
      }

      auto* item = new QListWidgetItem(parts.join(" "));
      QFont mono("Monospace");
      mono.setStyleHint(QFont::TypeWriter);
      item->setFont(mono);
      item->setData(Qt::UserRole, static_cast<int>(i));
      ui->teLines->addItem(item);

      if (i == 0) {
        m_board->SetScore(score, info.mate_counter);
      }
    }
  }
}

static void BuildLineHtml(const chess::MoveNode* node,
                          const chess::MoveNode* current, const QString& path,
                          int& move_num, chess::Colour& turn, QString& html) {
  if (node->children.empty()) return;

  const chess::MoveNode* main_child = node->children.front().get();
  const QString child_path = path.isEmpty() ? "0" : path + ",0";

  if (turn == chess::Colour::WHITE) {
    html += QString::number(move_num) + ". ";
  }
  if (main_child == current) {
    html += "<b><span style=\"background-color:#B4D2FF;\"> " +
            QString::fromStdString(main_child->san) + " </span></b>";
  } else {
    html += "<a href=\"move:" + child_path +
            "\" style=\"text-decoration:none;color:inherit;\">" +
            QString::fromStdString(main_child->san) + "</a>";
  }
  html += " ";

  turn = chess::ToggleColour(turn);
  const int var_num_base = move_num;
  if (turn == chess::Colour::WHITE) move_num++;

  // Render variations (siblings that branch from this position)
  if (node->children.size() > 1) {
    html += "<span style=\"color:gray;\">(";
    for (size_t i = 1; i < node->children.size(); ++i) {
      const chess::MoveNode* var = node->children[i].get();
      const QString var_path =
          path.isEmpty() ? QString::number(i) : path + "," + QString::number(i);
      chess::Colour var_turn = chess::ToggleColour(turn);
      int var_num = var_num_base;
      if (var_turn == chess::Colour::WHITE) {
        html += QString::number(var_num) + ". ";
      }
      if (var == current) {
        html += "<b><span style=\"background-color:#B4D2FF;\"> " +
                QString::fromStdString(var->san) + " </span></b>";
      } else {
        html += "<a href=\"move:" + var_path +
                "\" style=\"text-decoration:none;color:inherit;\">" +
                QString::fromStdString(var->san) + "</a>";
      }
      html += " ";
      var_turn = chess::ToggleColour(var_turn);
      if (var_turn == chess::Colour::WHITE) var_num++;
      BuildLineHtml(var, current, var_path, var_num, var_turn, html);
      if (i < node->children.size() - 1) html += " ";
    }
    html += ")</span> ";
  }

  BuildLineHtml(main_child, current, child_path, move_num, turn, html);
}

void MainWindow::UpdateMoveList() {
  const auto* root = m_board->GetRootNode();
  if (root == nullptr) return;

  const auto* current = m_board->GetCurrentNode();

  QSignalBlocker blocker(ui->teMoves);

  chess::Colour turn;
  int move_num;
  {
    const QStringList args =
        QString::fromStdString(m_board->GetStartFEN()).trimmed().split(" ");
    turn = (args[1] == "w") ? chess::Colour::WHITE : chess::Colour::BLACK;
    move_num = args[5].toInt();
  }

  QString html;
  BuildLineHtml(root, current, QString(), move_num, turn, html);

  if (html.isEmpty()) {
    html = "<span style=\"color:gray;\">No moves yet</span>";
  }

  ui->teMoves->setHtml(html);
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

void MainWindow::on_sbLines_valueChanged(int value) {
  const int num_lines = value;
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

void MainWindow::on_bGoStart_clicked() {
  m_board->GoToStart();
  m_board->SetSelectableColour(m_board->GetActiveColour());
  UpdateMoveList();
  const QString fen = m_board->GetFEN();
  m_engine.SetPosition(fen);
  RestartSearch();
}

void MainWindow::on_bGoBack_clicked() {
  m_board->GoBack();
  m_board->SetSelectableColour(m_board->GetActiveColour());
  UpdateMoveList();
  const QString fen = m_board->GetFEN();
  m_engine.SetPosition(fen);
  RestartSearch();
}

void MainWindow::on_bGoForward_clicked() {
  m_board->GoForward();
  m_board->SetSelectableColour(m_board->GetActiveColour());
  UpdateMoveList();
  const QString fen = m_board->GetFEN();
  m_engine.SetPosition(fen);
  RestartSearch();
}

void MainWindow::on_bGoEnd_clicked() {
  m_board->GoToEnd();
  m_board->SetSelectableColour(m_board->GetActiveColour());
  UpdateMoveList();
  const QString fen = m_board->GetFEN();
  m_engine.SetPosition(fen);
  RestartSearch();
}

void MainWindow::on_actionLoad_PGN_triggered() {
  QInputDialog* dialog = new QInputDialog(this);
  dialog->setWindowTitle("Import PGN");
  dialog->setLabelText("Paste PGN:");
  dialog->resize(500, 300);
  dialog->exec();

  QString pgn = dialog->textValue();
  if (pgn.isEmpty()) return;

  const bool ok = m_board->ImportPGN(pgn);
  if (!ok) {
    ShowMsgBox("Error", "Could not parse PGN.");
    return;
  }

  m_board->SetSelectableColour(m_board->GetActiveColour());
  const QString fen = m_board->GetFEN();
  m_engine.SetPosition(fen);
  RestartSearch();
  UpdateMoveList();
}

void MainWindow::on_teMoves_anchorClicked(const QUrl& url) {
  if (url.scheme() != "move") return;

  const QString path = url.path();

  disconnect(ui->teMoves, &QTextBrowser::anchorClicked, this,
             &MainWindow::on_teMoves_anchorClicked);

  m_board->GoToNodeByPath(path);
  m_board->SetSelectableColour(m_board->GetActiveColour());
  UpdateMoveList();
  const QString fen = m_board->GetFEN();
  m_engine.SetPosition(fen);
  RestartSearch();

  connect(ui->teMoves, &QTextBrowser::anchorClicked, this,
          &MainWindow::on_teMoves_anchorClicked);
}

void MainWindow::on_teLines_itemClicked(QListWidgetItem* item) {
  if (item == nullptr) return;

  const int line_index = item->data(Qt::UserRole).toInt();
  if (line_index < 0 ||
      line_index >= static_cast<int>(m_engine_lines_data.size()))
    return;

  const QStringList& uci_moves = m_engine_lines_data[line_index];
  if (uci_moves.isEmpty()) return;

  chess::Move move = chess::UCIToMove(uci_moves.first().toStdString());
  m_board->DoMove(move);
}
