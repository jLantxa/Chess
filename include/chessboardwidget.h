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

#ifndef _CHESS_INCLUDE_CHESS_H_
#define _CHESS_INCLUDE_CHESS_H_

#include <QIcon>
#include <QWidget>
#include <cstdint>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "board.hpp"
#include "chess.hpp"

class ChessBoardWidget : public QWidget {
  Q_OBJECT
 public:
  explicit ChessBoardWidget(QWidget* parent = nullptr);

  void Reset();

  void SetPosition(const QString& fen_str);

  void Rotate();
  void SetSide(chess::Colour side);
  chess::Colour GetSide() const;

  void SetActiveColour(chess::Colour colour);
  chess::Colour GetActiveColour() const;
  void SetSelectableColour(const chess::Colour& colour);
  void SetSelectable(bool selectable);

  void paintEvent(QPaintEvent*);
  bool hasHeightForWidth() const override { return true; }
  int heightForWidth(int width) const override { return width; }
  void mouseMoveEvent(QMouseEvent*);
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);

  void HandleBoardMouseEvent(QMouseEvent*);

  struct ChessPalette {
    QColor white_square;
    QColor black_square;
    QColor white_highlight;
    QColor black_highlight;
    QColor highlight_important;
  };

  static constexpr ChessPalette GREEN_PALETTE{
      .white_square = QColor(238, 238, 210),
      .black_square = QColor(118, 150, 86),
      .white_highlight = QColor(246, 255, 115),
      .black_highlight = QColor(190, 201, 24),
      .highlight_important = QColor(255, 113, 74),
  };

  static constexpr ChessPalette RED_PALETTE{
      .white_square = QColor(255, 191, 191),
      .black_square = QColor(125, 26, 26),
      .white_highlight = QColor(246, 255, 115),
      .black_highlight = QColor(190, 201, 24),
      .highlight_important = QColor(255, 113, 74),
  };

  static constexpr ChessPalette PURPLE_PALETTE{
      .white_square = QColor(246, 217, 255),
      .black_square = QColor(170, 84, 255),
      .white_highlight = QColor(246, 255, 115),
      .black_highlight = QColor(190, 201, 24),
      .highlight_important = QColor(255, 113, 74),
  };

  static constexpr ChessPalette BLUE_PALETTE{
      .white_square = QColor(217, 232, 255),
      .black_square = QColor(76, 149, 252),
      .white_highlight = QColor(246, 255, 115),
      .black_highlight = QColor(190, 201, 24),
      .highlight_important = QColor(255, 113, 74),
  };

  static constexpr ChessPalette YELLOW_PALETTE{
      .white_square = QColor(255, 232, 168),
      .black_square = QColor(230, 179, 41),
      .white_highlight = QColor(246, 255, 115),
      .black_highlight = QColor(190, 201, 24),
      .highlight_important = QColor(255, 113, 74),
  };

  void SetScoreEnabled(bool enabled);
  void SetScore(int score, bool mate);

  void SetColourPalette(const ChessPalette& palette);

  uint32_t GetNumHalfMoves() const;

  bool DoMove(const chess::Move& move);

  [[nodiscard]] std::string GetSanMove(const chess::Move& move) const;
  [[nodiscard]] const std::string& GetLastMoveSan() const;

  // Move tree navigation
  void GoToStart();
  void GoBack();
  void GoForward();
  void GoToEnd();
  void GoToNode(const chess::MoveNode* target);
  void GoToNodeByPath(const QString& path);
  [[nodiscard]] bool CanGoBack() const;
  [[nodiscard]] bool CanGoForward() const;

  // Move history / PGN
  [[nodiscard]] QString GetFEN() const;
  [[nodiscard]] QString GetPGN() const;
  [[nodiscard]] std::string GetStartFEN() const;
  [[nodiscard]] bool ImportPGN(const QString& pgn);
  [[nodiscard]] size_t GetMoveIndex() const;
  [[nodiscard]] size_t GetTotalMoves() const;
  [[nodiscard]] const std::vector<chess::Move> GetMainLineMoves() const;
  [[nodiscard]] const std::vector<std::string> GetMainLineSan() const;
  [[nodiscard]] QStringList ConvertUciToSan(const QStringList& uci_moves) const;
  [[nodiscard]] const chess::MoveNode* GetRootNode() const;
  [[nodiscard]] const chess::MoveNode* GetCurrentNode() const;

 signals:
  void MoveDone(const chess::Move& move);

 private:
  static constexpr int SCORE_BAR_WIDTH = 10;
  static constexpr int SCORE_BAR_SPACING = 15;

  ChessPalette m_palette;

  chess::Board m_board;

  /** Number of half moves from starting position. */
  uint32_t m_half_moves = 0;

  chess::Colour m_active_colour = chess::Colour::WHITE;
  bool m_selectable = true;
  chess::Colour m_selectable_colour = m_active_colour;
  chess::Colour m_side = m_active_colour;
  float m_board_size = 50;
  float m_square_size = m_board_size / 8;

  bool m_in_drag_mode = false;
  std::optional<chess::Square> m_selected_square;
  std::optional<chess::Square> m_src_square;
  std::optional<chess::Square> m_last_move_src_square;
  std::optional<chess::Square> m_last_move_dst_square;
  QPointF m_mouse_position;

  int m_score = 0;
  bool m_is_mate = false;
  bool m_score_enabled = false;

  static constexpr int MARGIN = 5;

  void GetRotatedCoordinates(uint8_t ax, uint8_t ay, uint8_t& bx, uint8_t& by,
                             chess::Colour side) const;
  void GetGridCoordinates(int x, int y, uint8_t& u, uint8_t& v) const;
  chess::Square GetClickedSquare(int x, int y) const;
  bool IsOnBoard(int x, int y) const;

  float GetBalance() const;
  float Transform(float x) const;

  // Move tree state
  std::string m_start_fen = std::string(chess::STARTPOS_FEN);
  std::unique_ptr<chess::MoveNode> m_root_node;
  chess::MoveNode* m_current_node = nullptr;
  size_t m_move_index = 0;

  void ReplayToCurrentNode();
  void InitBoardFromFEN(chess::Board& board, const std::string& fen,
                        chess::Colour& active, uint32_t& half_moves) const;
};

#endif  // _CHESS_INCLUDE_CHESS_H_
