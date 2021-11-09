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

#ifndef _CHESS_INCLUDE_UCI_ENGINE_HPP_
#define _CHESS_INCLUDE_UCI_ENGINE_HPP_

#include <QDebug>
#include <QProcess>
#include <QString>

class UCIEngine : public QObject {
  Q_OBJECT;

 public:
  UCIEngine();

  /**
   * @brief A struct that contains the best move found by the engine
   * and a suggested move to ponder.
   */
  struct BestMove {
    QString bestmove;
    QString ponder;
  };

  /**
   * @brief A struct seach information
   */
  struct DepthInfo {
    uint8_t line_id;
    uint8_t depth;
    QStringList pv;
    bool mate_counter = false;
    int score;
  };

  /** Start engine process. */
  void Init(const QString& command);

  /** Close engine process. */
  void Close();

  /** Reset engine process. */
  void Reset();

  /**
   * @brief Send stop command.
   */
  void Stop();

  /**
   * @brief Write a raw command to the engine.
   * @param str QString without \n ending.
   */
  void Write(const QString& str);

  /**
   * @brief Start a new hame and tell the engine.
   */
  void NewGame();

  /**
   * @brief Set a position using FEN format.
   * @param fen FEN string describing the position.
   */
  void SetPosition(const QString& fen);

  /**
   * @brief Set a position using a chain of moves from the standard position.
   * @param moves A QStringList containing the current line.
   */
  void SetPositionFromMoves(const QStringList& moves);

  /**
   * @brief Set number of lines.
   * @param num_lines Number of lines (principal variations) to return.
   */
  void SetNumLines(uint8_t num_lines);

  /**
   * @brief Search using a maximum depth.
   * @param depth Maximum depth.
   */
  void SearchWithDepth(uint8_t depth);

  /**
   * @brief Search using a timeout.
   * @param msec Maximum search time in milliseconds.
   */
  void SearchWithTime(uint32_t msec);

  /**
   * @brief Search forever. The search can be stopped at any time by sending a
   * stop message.
   */
  void SearchInfinite();

  /**
   * @brief Set number of threads.
   * @param num_threads Number of CPU threads to use.
   */
  void SetNumThreads(uint16_t num_threads);

 public slots:
  /**
   * @brief The engine process has started.
   */
  void OnStart();

  /**
   * @brief There is data available in the engine's standard output.
   */
  void OnReadyReadStdout();

 signals:
  void BestMoveAvailable(const UCIEngine::BestMove& best_move);
  void DepthInfoAvailable(const UCIEngine::DepthInfo& depth_info);

 private:
  QProcess m_engine_process;

  void ConnectProcessSignals();

  void ParseText(const QString& text);
  bool ParseInfo(const QStringList& args);
  bool ParseBestMove(const QStringList& args);
  UCIEngine::DepthInfo ParseDepthInfo(const QStringList& args);
};

#endif  // _CHESS_INCLUDE_UCI_ENGINE_HPP_
