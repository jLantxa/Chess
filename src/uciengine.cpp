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

#include "uciengine.hpp"

#include <algorithm>

static const char* SEPARATOR = " ";
static const char* LINE_SEPARATOR = "\n";

UCIEngine::UCIEngine() { ConnectProcessSignals(); }

void UCIEngine::ConnectProcessSignals() {
  connect(&m_engine_process, &QProcess::readyReadStandardOutput, this,
          &UCIEngine::OnReadyReadStdout);
  connect(&m_engine_process, &QProcess::started, this, &UCIEngine::OnStart);
}

void UCIEngine::Init(const QString& command) {
  m_engine_process.setProgram(command);
  m_engine_process.start();
}

void UCIEngine::Close() { m_engine_process.close(); }

void UCIEngine::Reset() {
  m_engine_process.close();
  m_engine_process.start();
}

void UCIEngine::Write(const QString& str) {
  QString cmd = str + "\n";
  m_engine_process.write(cmd.toStdString().c_str());
}

void UCIEngine::NewGame() {
  Write("ucinewgame");
  Write("position startpos");
}

void UCIEngine::SetPosition(const QString& fen) {
  m_best_move.reset();
  Write("position fen " + fen);
}

void UCIEngine::SetPositionFromMoves(const QStringList& moves) {
  m_best_move.reset();
  Write("position startpos moves " + moves.join(SEPARATOR));
}

void UCIEngine::Stop() { Write("stop"); }

void UCIEngine::SetNumLines(uint8_t num_lines) {
  m_lines.clear();
  m_lines.resize(num_lines);
  Write("setoption name MultiPV value " + QString::number(num_lines));
}

void UCIEngine::SetNumThreads(uint16_t num_threads) {
  Write("setoption name Threads value " + QString::number(num_threads));
}

void UCIEngine::SearchWithDepth(uint8_t depth) {
  Write("go depth " + QString::number(depth));
}

void UCIEngine::SearchWithTime(uint32_t msec) {
  Write("go movetime " + QString::number(msec));
}

void UCIEngine::SearchInfinite() { Write("go infinite"); }

void UCIEngine::OnStart() {}

void UCIEngine::OnReadyReadStdout() {
  while (m_engine_process.canReadLine()) {
    const QString text = m_engine_process.readAllStandardOutput();
    ParseText(text.trimmed());
  }
}

void UCIEngine::ParseText(const QString& text) {
  if (text.isEmpty()) {
    return;
  }

  const QStringList lines = text.split(LINE_SEPARATOR);
  uint8_t max_line = 0;

  {
    std::lock_guard<std::mutex> info_mutex(m_info_mutex);

    for (auto& line : lines) {
      const QStringList args = line.trimmed().split(SEPARATOR);

      if (args[0] == "bestmove") {
        m_best_move = ParseBestMove(args);
      } else if (args[0] == "info") {
        /* Ignore currmove messages.
         * Upperbound and lowerbound messages are for engine debug and we ignore
         * those messages.
         */
        const bool ignore_message = args.contains("currmove") ||
                                    args.contains("upperbound") ||
                                    args.contains("lowerbound");
        if (ignore_message) {
          continue;
        }

        const DepthInfo info = ParseDepthInfo(args);
        const auto line_id = info.line_id;
        m_lines[line_id - 1] = info;
        max_line = std::max(max_line, line_id);
      } else {
        continue;
      }
    }
  }
  if (max_line == m_lines.size()) {
    emit DepthInfoAvailable();
  }
  if (m_best_move.has_value()) {
    emit BestMoveAvailable();
  }
}

UCIEngine::DepthInfo UCIEngine::ParseDepthInfo(const QStringList& args) {
  DepthInfo depth_info;

  depth_info.depth = args[2].toUInt();

  if (args.contains("multipv")) {
    const auto line_id_ix = args.indexOf("multipv");
    depth_info.line_id = args[line_id_ix + 1].toUInt();
  } else {
    depth_info.line_id = 1;
  }

  const auto score_ix = args.indexOf("score");
  depth_info.mate_counter = (args[score_ix + 1] == "mate");
  depth_info.score = args[score_ix + 2].toInt();

  if (args.contains("pv")) {
    const auto pv_ix = args.indexOf("pv");
    for (int i = pv_ix + 1; i < args.length(); ++i) {
      depth_info.pv.push_back(args[i]);
    }
  }

  return depth_info;
}

UCIEngine::BestMove UCIEngine::ParseBestMove(const QStringList& args) {
  BestMove best_move;
  best_move.bestmove = args[1];
  if (args.contains("ponder")) {
    const auto ponder_ix = args.indexOf("ponder") + 1;
    best_move.ponder = args[ponder_ix];
  }

  return best_move;
}

std::optional<UCIEngine::BestMove> UCIEngine::GetBestMove() {
  std::lock_guard<std::mutex> mutex(m_info_mutex);
  return m_best_move;
}

std::vector<UCIEngine::DepthInfo> UCIEngine::GetLines() {
  std::lock_guard<std::mutex> mutex(m_info_mutex);
  return m_lines;
}
