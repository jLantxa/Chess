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

#include "chessboardwidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QRect>
#include <QRegularExpression>
#include <algorithm>
#include <cmath>
#include <functional>

#include "chess.hpp"
#include "resources.hpp"

ChessBoardWidget::ChessBoardWidget(QWidget* parent) : QWidget(parent) {
  SetColourPalette(GREEN_PALETTE);
  Reset();
}

void ChessBoardWidget::InitBoardFromFEN(chess::Board& board,
                                        const std::string& fen,
                                        chess::Colour& active,
                                        uint32_t& half_moves) const {
  board.Clear();
  const QStringList args = QString::fromStdString(fen).trimmed().split(" ");
  const QStringList& fen_lines = args[0].trimmed().split("/");
  for (uint8_t j = 0; j < 8; ++j) {
    uint8_t fi = 0;
    const QString& line = fen_lines[7 - j];
    for (const auto& qch : line) {
      char ch = qch.toLatin1();
      if (ch >= '0' && ch <= '9') {
        fi += (ch - '0');
      } else {
        const char lower = qch.toLower().toLatin1();
        const chess::Colour colour = (ch >= 'A' && ch <= 'Z')
                                         ? chess::Colour::WHITE
                                         : chess::Colour::BLACK;
        const chess::PieceType type = chess::GetPieceTypeFromChar(lower);
        if (lower == 'p' || lower == 'n' || lower == 'b' || lower == 'r' ||
            lower == 'q' || lower == 'k') {
          board.SetPiece(chess::Piece::Factory(type, colour), fi, j);
          fi++;
        } else {
          fi++;
        }
      }
    }
  }
  bool wkc = args[2].contains("K");
  bool wqc = args[2].contains("Q");
  bool bkc = args[2].contains("k");
  bool bqc = args[2].contains("q");
  board.SetCastling(wkc, wqc, bkc, bqc);

  active = (args[1] == "w") ? chess::Colour::WHITE : chess::Colour::BLACK;
  const uint32_t move_number = args[5].toUInt();
  half_moves = 2 * (move_number - 1);
  if (active == chess::Colour::BLACK) {
    half_moves++;
  }
}

void ChessBoardWidget::Reset() {
  m_score = 0;
  m_half_moves = 0;
  m_selected_square.reset();
  m_src_square.reset();
  m_last_move_src_square.reset();
  m_last_move_dst_square.reset();
  m_in_drag_mode = false;
  m_active_colour = chess::Colour::WHITE;
  m_start_fen = chess::STARTPOS_FEN;
  m_root_node = std::make_unique<chess::MoveNode>();
  m_current_node = m_root_node.get();
  m_move_index = 0;
}

static constexpr bool IsUpperCase(char ch) {
  return (ch >= 'A') && (ch <= 'Z');
}

static constexpr bool IsNumber(char ch) { return (ch >= '0') && (ch <= '9'); }

void ChessBoardWidget::SetPosition(const QString& fen_str) {
  const QStringList& args = fen_str.trimmed().split(" ");

  if (args[1] == 'w') {
    m_active_colour = chess::Colour::WHITE;
  } else if (args[1] == 'b') {
    m_active_colour = chess::Colour::BLACK;
  }

  m_board.Clear();

  const uint32_t move_number = args[5].toUInt();
  m_half_moves = 2 * (move_number - 1);
  if (m_active_colour == chess::Colour::BLACK) {
    m_half_moves++;
  }

  bool wkc = args[2].contains("K");
  bool wqc = args[2].contains("Q");
  bool bkc = args[2].contains("k");
  bool bqc = args[2].contains("q");
  m_board.SetCastling(wkc, wqc, bkc, bqc);

  const QStringList& lines = args[0].trimmed().split("/");
  uint8_t i, j;
  for (j = 0; j < 8; ++j) {
    i = 0;
    const QString& line = lines[7 - j];

    for (auto& qch : line) {
      char ch = qch.toLatin1();
      if (IsNumber(ch)) {
        const uint8_t num = ch - '0';
        i += num;
      } else {
        const char piece_char = qch.toLower().toLatin1();
        const chess::Colour colour =
            IsUpperCase(ch) ? chess::Colour::WHITE : chess::Colour::BLACK;
        const chess::PieceType type =
            chess::GetPieceTypeFromChar(qch.toLower().toLatin1());
        switch (piece_char) {
          case 'p':
          case 'n':
          case 'b':
          case 'r':
          case 'q':
          case 'k': {
            std::unique_ptr<chess::Piece> piece =
                chess::Piece::Factory(type, colour);
            m_board.SetPiece(std::move(piece), i, j);
            i++;
            break;
          }

          default:
            i++;
            break;
        }
      }
    }
  }

  repaint();
}

void ChessBoardWidget::Rotate() {
  m_side = chess::ToggleColour(m_side);
  repaint();
}

void ChessBoardWidget::SetSide(chess::Colour side) {
  m_side = side;
  repaint();
}

chess::Colour ChessBoardWidget::GetSide() const { return m_side; }

void ChessBoardWidget::SetActiveColour(chess::Colour colour) {
  m_active_colour = colour;
}

chess::Colour ChessBoardWidget::GetActiveColour() const {
  return m_active_colour;
}

void ChessBoardWidget::SetColourPalette(const ChessPalette& palette) {
  m_palette = palette;
  repaint();
}

uint32_t ChessBoardWidget::GetNumHalfMoves() const { return m_half_moves; }

void ChessBoardWidget::GetRotatedCoordinates(uint8_t ax, uint8_t ay,
                                             uint8_t& bx, uint8_t& by,
                                             chess::Colour side) const {
  if (side == chess::Colour::WHITE) {
    bx = ax;
    by = 7 - ay;
  } else {
    bx = 7 - ax;
    by = ay;
  }
}

void ChessBoardWidget::GetGridCoordinates(int x, int y, uint8_t& u,
                                          uint8_t& v) const {
  u = (x - (MARGIN + SCORE_BAR_WIDTH + SCORE_BAR_SPACING)) / m_square_size;
  v = (y - MARGIN) / m_square_size;
}

chess::Square ChessBoardWidget::GetClickedSquare(int x, int y) const {
  uint8_t i, j, u, v;
  GetGridCoordinates(x, y, u, v);
  GetRotatedCoordinates(u, v, i, j, m_side);
  return chess::Square{i, j};
}

bool ChessBoardWidget::IsOnBoard(int x, int y) const {
  const int board_x = MARGIN + SCORE_BAR_WIDTH + SCORE_BAR_SPACING;
  const int board_y = MARGIN;
  return (x > board_x) && (y > board_y) && (x < (board_x + m_board_size)) &&
         (y < (board_y + m_board_size));
}

void ChessBoardWidget::mousePressEvent(QMouseEvent* event) {
  HandleBoardMouseEvent(event);
}

void ChessBoardWidget::mouseReleaseEvent(QMouseEvent* event) {
  HandleBoardMouseEvent(event);
}

void ChessBoardWidget::HandleBoardMouseEvent(QMouseEvent* event) {
  if (!m_selectable) {
    return;
  }

  const QPointF position = event->position();
  const float x = position.x();
  const float y = position.y();

  const bool click_on_board = IsOnBoard(x, y);

  const auto event_type = event->type();
  const auto button = event->button();
  const bool left_press = (event_type == QMouseEvent::MouseButtonPress) &&
                          (button == Qt::LeftButton);
  const bool right_press = (event_type == QMouseEvent::MouseButtonPress) &&
                           (button == Qt::RightButton);
  const bool left_release = (event_type == QMouseEvent::MouseButtonRelease) &&
                            (button == Qt::LeftButton);

  if (!click_on_board && (left_press || right_press)) {
    m_selected_square.reset();
    return;
  }

  if (left_release && !click_on_board) {
    m_in_drag_mode = false;
    m_selected_square.reset();
  }

  m_mouse_position = position;
  auto square = GetClickedSquare(x, y);

  const chess::Piece* piece = m_board.PieceAt(square);
  const bool is_selectable_square =
      (piece != nullptr) && (piece->GetColour() == m_selectable_colour);

  if (left_press) {
    m_last_move_src_square.reset();
    m_last_move_dst_square.reset();

    if (m_src_square.has_value()) {
      if (m_src_square != square) {
        if (is_selectable_square) {
          m_in_drag_mode = true;
          m_selected_square = square;
          m_src_square = square;
        } else {
          chess::Move move{m_src_square.value(), square};
          if (DoMove(move)) {
            m_selected_square.reset();
            m_src_square.reset();
          }
        }
      } else {
        m_in_drag_mode = true;
        m_selected_square = square;
        m_src_square = square;
      }
    } else if (is_selectable_square) {
      m_in_drag_mode = true;
      m_selected_square = square;
      m_src_square = square;
    }
  } else if (left_release) {
    if (m_in_drag_mode) {
      if (m_selected_square.has_value() &&
          (square != m_selected_square.value())) {
        chess::Move move{m_selected_square.value(), square};
        DoMove(move);
        m_src_square.reset();
      }
    }

    m_selected_square.reset();
    m_in_drag_mode = false;
  }
  if (right_press) {
    if (m_in_drag_mode) {
      m_in_drag_mode = false;
    }
  }

  repaint();
}

void ChessBoardWidget::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);
  const QRect geometry = this->geometry();
  QFont coordinates_font;

  const float board_x_off = MARGIN + SCORE_BAR_WIDTH + SCORE_BAR_SPACING;
  const float board_y_off = MARGIN;
  const int board_available_width = geometry.width() - board_x_off;

  m_board_size =
      std::min(board_available_width, geometry.height()) - 2 * MARGIN;
  m_square_size = m_board_size / 8;

  coordinates_font.setWeight(QFont::Bold);
  coordinates_font.setPixelSize(m_square_size / 4);
  const int text_margin = 0.075f * m_square_size;
  QFontMetrics font_metrics(coordinates_font);

  painter.setFont(coordinates_font);

  // Board base (white colour)
  painter.fillRect(board_x_off, board_y_off, m_board_size, m_board_size,
                   m_palette.white_square);

  /* i, j -> Coordinates of the board
   * u, v -> Coordinates of the widget grid (accounting for rotation)
   */
  float x, y;
  uint8_t u, v;

  for (uint8_t i = 0; i < 8; ++i) {
    for (uint8_t j = 0; j < 8; ++j) {
      GetRotatedCoordinates(i, j, u, v, m_side);
      const bool is_selected_square =
          (m_src_square.has_value() && (m_src_square == chess::Square{i, j})) ||
          (m_last_move_src_square.has_value() &&
           (m_last_move_src_square == chess::Square{i, j})) ||
          (m_last_move_dst_square.has_value() &&
           (m_last_move_dst_square == chess::Square{i, j}));

      x = board_x_off + u * m_square_size;
      y = board_y_off + v * m_square_size;
      const QColor& square_colour =
          (((i + j) % 2) == 0) ? is_selected_square ? m_palette.black_highlight
                                                    : m_palette.black_square
          : is_selected_square ? m_palette.white_highlight
                               : m_palette.white_square;
      const QColor& text_colour = (((i + j) % 2) == 0) ? m_palette.white_square
                                                       : m_palette.black_square;

      // Square
      painter.fillRect(x, y, m_square_size, m_square_size, square_colour);

      // Coordinate
      if (v == 7) {
        painter.setPen(text_colour);
        painter.drawText(
            x + m_square_size - font_metrics.xHeight() - text_margin,
            y + m_square_size - text_margin, QString(chess::NumberToFile(i)));
      }
      if (u == 0) {
        painter.setPen(text_colour);
        painter.drawText(x + text_margin,
                         y + font_metrics.capHeight() + text_margin,
                         QString::number(j + 1));
      }

      // Pieces (except the one being dragged)
      if (m_in_drag_mode && is_selected_square) {
        continue;
      } else {
        x = board_x_off + u * m_square_size;
        y = board_y_off + v * m_square_size;
      }
      const chess::Piece* piece = m_board.PieceAt(i, j);
      if (piece != nullptr) {
        const uint8_t type_index = static_cast<uint8_t>(piece->GetType());
        const std::array<QIcon, 6>& icon_array =
            (piece->GetColour() == chess::Colour::WHITE)
                ? resources::WHITE_ICONS
                : resources::BLACK_ICONS;
        const QIcon piece_icon = icon_array[type_index];
        const int piece_icon_size = m_square_size;
        painter.drawPixmap(
            x, y, piece_icon_size, piece_icon_size,
            piece_icon.pixmap(QSize(piece_icon_size, piece_icon_size)));
      }
    }
    painter.setPen(m_palette.black_square);
    painter.drawRect(board_x_off, board_y_off, m_board_size, m_board_size);

    const bool in_mate = m_is_mate && (m_score == 0);
    if (m_score_enabled && !in_mate) {
      const float balance = GetBalance();
      const float white_height = m_board_size * balance;
      const float black_height = m_board_size - white_height;

      const float top_height =
          (m_side == chess::Colour::WHITE) ? black_height : white_height;
      const float bottom_height =
          (m_side == chess::Colour::WHITE) ? white_height : black_height;
      const QColor& top_colour = (m_side == chess::Colour::WHITE)
                                     ? m_palette.black_square
                                     : m_palette.white_square;
      const QColor& bottom_colour = (m_side == chess::Colour::WHITE)
                                        ? m_palette.white_square
                                        : m_palette.black_square;

      painter.fillRect(MARGIN, top_height + MARGIN, SCORE_BAR_WIDTH,
                       bottom_height, bottom_colour);
      painter.fillRect(MARGIN, MARGIN, SCORE_BAR_WIDTH, top_height, top_colour);

      painter.setPen(m_palette.black_square);
      painter.drawRect(MARGIN, MARGIN, SCORE_BAR_WIDTH,
                       white_height + black_height);
    }
  }

  // Dragged piece
  if (m_in_drag_mode && m_selected_square.has_value()) {
    const uint8_t i = m_selected_square->file;
    const uint8_t j = m_selected_square->rank;

    x = m_mouse_position.x() - m_square_size / 2;
    y = m_mouse_position.y() - m_square_size / 2;

    const float board_min_x = MARGIN + SCORE_BAR_WIDTH + SCORE_BAR_SPACING;
    const float board_min_y = MARGIN;
    const float board_max_x =
        MARGIN + SCORE_BAR_WIDTH + SCORE_BAR_SPACING + m_board_size;
    const float board_max_y = MARGIN + m_board_size;
    x = std::min(x, board_max_x - m_square_size / 2);
    y = std::min(y, board_max_y - m_square_size / 2);
    x = std::max(x, board_min_x - m_square_size / 2);
    y = std::max(y, board_min_y - m_square_size / 2);

    GetRotatedCoordinates(i, j, u, v, m_side);
    const chess::Piece* piece = m_board.PieceAt(i, j);
    if (piece != nullptr) {
      const uint8_t type_index = static_cast<uint8_t>(piece->GetType());
      const std::array<QIcon, 6>& icon_array =
          (piece->GetColour() == chess::Colour::WHITE) ? resources::WHITE_ICONS
                                                       : resources::BLACK_ICONS;
      const QIcon piece_icon = icon_array[type_index];
      const int piece_icon_size = m_square_size;
      painter.drawPixmap(
          x, y, piece_icon_size, piece_icon_size,
          piece_icon.pixmap(QSize(piece_icon_size, piece_icon_size)));
    }
  }
}

void ChessBoardWidget::mouseMoveEvent(QMouseEvent* event) {
  m_mouse_position = event->position();
  repaint();
}

float ChessBoardWidget::GetBalance() const {
  if (m_is_mate) {
    if (m_score > 0) {
      return 1.0f;
    } else {
      return 0.0f;
    }
  } else {
    return Transform(m_score);
  }
}

float ChessBoardWidget::Transform(float x) const {
  x /= 100.0f;  // Convert from centipawns

  constexpr float k = 2.0f;
  float value = 1.0f / (1.0f + exp(-x / k));

  // Limit the value
  constexpr float MAX = 0.95f;
  value = std::min(MAX, value);
  value = std::max(1 - MAX, value);
  return value;
}

void ChessBoardWidget::SetScore(int score, bool mate) {
  m_score = score;
  m_is_mate = mate;
  repaint();
}

void ChessBoardWidget::SetScoreEnabled(bool enabled) {
  m_score_enabled = enabled;
  repaint();
}

bool ChessBoardWidget::DoMove(const chess::Move& move) {
  if (m_current_node == nullptr) return false;
  if (!m_board.IsValidMove(move, m_active_colour)) {
    return false;
  }

  std::string san = m_board.GetSanMove(move);

  // Check if a child with this move already exists
  for (auto& child : m_current_node->children) {
    if (child->move == move) {
      // Move already exists in tree, navigate to it
      m_current_node = child.get();
      m_move_index++;
      m_board.DoMove(move);
      m_last_move_src_square = move.src;
      m_last_move_dst_square = move.dst;
      m_active_colour = chess::ToggleColour(m_active_colour);
      m_half_moves++;
      emit MoveDone(move);
      return true;
    }
  }

  // New move: create child node
  auto new_node = std::make_unique<chess::MoveNode>(move, san, m_current_node);
  m_current_node->children.push_back(std::move(new_node));
  m_current_node = m_current_node->children.back().get();
  m_move_index++;

  m_board.DoMove(move);
  m_last_move_src_square = move.src;
  m_last_move_dst_square = move.dst;
  m_active_colour = chess::ToggleColour(m_active_colour);
  m_half_moves++;

  emit MoveDone(move);
  return true;
}

std::string ChessBoardWidget::GetSanMove(const chess::Move& move) const {
  return m_board.GetSanMove(move);
}

const std::string& ChessBoardWidget::GetLastMoveSan() const {
  static const std::string empty;
  if (m_current_node == m_root_node.get()) return empty;
  return m_current_node->san;
}

void ChessBoardWidget::SetSelectableColour(const chess::Colour& colour) {
  m_selectable_colour = colour;
}

QString ChessBoardWidget::GetFEN() const {
  const QString board_pos =
      QString::fromStdString(m_board.GetPosition(m_active_colour));
  const QString half_moves_str = QString::number(m_half_moves);
  const QString full_move_str = QString::number(1 + (m_half_moves / 2));

  return board_pos + " " + half_moves_str + " " + full_move_str;
}

std::string ChessBoardWidget::GetStartFEN() const { return m_start_fen; }

void ChessBoardWidget::SetSelectable(bool selectable) {
  m_selectable = selectable;
}

void ChessBoardWidget::ReplayToCurrentNode() {
  if (m_current_node == nullptr) return;

  m_board.Clear();
  chess::Colour active;
  uint32_t half_moves;
  InitBoardFromFEN(m_board, m_start_fen, active, half_moves);

  // Collect path from root to current node
  std::vector<chess::Move> path;
  const chess::MoveNode* node = m_current_node;
  while (node != nullptr && node->parent != nullptr) {
    path.push_back(node->move);
    node = node->parent;
  }
  std::reverse(path.begin(), path.end());

  // Replay
  m_half_moves = 0;
  m_active_colour = active;
  for (const auto& move : path) {
    m_board.DoMove(move);
    m_active_colour = chess::ToggleColour(m_active_colour);
    m_half_moves++;
  }

  // Update last move squares
  m_last_move_src_square.reset();
  m_last_move_dst_square.reset();
  if (m_current_node->parent != nullptr) {
    m_last_move_src_square = m_current_node->move.src;
    m_last_move_dst_square = m_current_node->move.dst;
  }

  repaint();
}

void ChessBoardWidget::GoToStart() {
  if (m_root_node == nullptr) return;
  m_current_node = m_root_node.get();
  m_move_index = 0;
  ReplayToCurrentNode();
}

void ChessBoardWidget::GoBack() {
  if (m_current_node == nullptr || m_current_node->parent == nullptr) return;
  m_current_node = m_current_node->parent;
  m_move_index--;
  ReplayToCurrentNode();
}

void ChessBoardWidget::GoForward() {
  if (m_current_node == nullptr || m_current_node->children.empty()) return;
  m_current_node = m_current_node->children.front().get();
  m_move_index++;
  ReplayToCurrentNode();
}

void ChessBoardWidget::GoToEnd() {
  if (m_current_node == nullptr) return;
  while (!m_current_node->children.empty()) {
    m_current_node = m_current_node->children.front().get();
    m_move_index++;
  }
  ReplayToCurrentNode();
}

void ChessBoardWidget::GoToNode(const chess::MoveNode* target) {
  if (target == nullptr) return;

  // Collect path from root to target
  std::vector<chess::Move> path;
  const chess::MoveNode* node = target;
  while (node != nullptr && node->parent != nullptr) {
    path.push_back(node->move);
    node = node->parent;
  }
  std::reverse(path.begin(), path.end());

  // Set current node and replay
  m_current_node = const_cast<chess::MoveNode*>(target);
  m_move_index = path.size();
  ReplayToCurrentNode();
}

void ChessBoardWidget::GoToNodeByPath(const QString& path) {
  if (m_root_node == nullptr) return;

  const QStringList indices = path.split(',', Qt::SkipEmptyParts);
  chess::MoveNode* node = m_root_node.get();

  for (const auto& idx_str : indices) {
    int idx = idx_str.toInt();
    if (idx < 0 || idx >= static_cast<int>(node->children.size())) return;
    node = node->children[idx].get();
  }

  GoToNode(node);
}

bool ChessBoardWidget::CanGoBack() const {
  return m_current_node->parent != nullptr;
}

bool ChessBoardWidget::CanGoForward() const {
  return !m_current_node->children.empty();
}

size_t ChessBoardWidget::GetMoveIndex() const { return m_move_index; }

size_t ChessBoardWidget::GetTotalMoves() const {
  // Count total unique moves in the tree (main line depth)
  size_t total = 0;
  const chess::MoveNode* node = m_root_node.get();
  while (!node->children.empty()) {
    total++;
    node = node->children.front().get();
  }
  return total;
}

const chess::MoveNode* ChessBoardWidget::GetRootNode() const {
  return m_root_node.get();
}

const chess::MoveNode* ChessBoardWidget::GetCurrentNode() const {
  return m_current_node;
}

const std::vector<chess::Move> ChessBoardWidget::GetMainLineMoves() const {
  std::vector<chess::Move> moves;
  const chess::MoveNode* node = m_root_node.get();
  while (!node->children.empty()) {
    node = node->children.front().get();
    moves.push_back(node->move);
  }
  return moves;
}

const std::vector<std::string> ChessBoardWidget::GetMainLineSan() const {
  std::vector<std::string> sans;
  const chess::MoveNode* node = m_root_node.get();
  while (!node->children.empty()) {
    node = node->children.front().get();
    sans.push_back(node->san);
  }
  return sans;
}

QStringList ChessBoardWidget::ConvertUciToSan(
    const QStringList& uci_moves) const {
  if (m_current_node == nullptr) return {};

  QStringList san_moves;
  chess::Board temp_board;
  chess::Colour active;
  uint32_t half_moves;
  InitBoardFromFEN(temp_board, m_start_fen, active, half_moves);

  // Replay path from root to current node (not the full main line)
  std::vector<chess::Move> path;
  const chess::MoveNode* node = m_current_node;
  while (node != nullptr && node->parent != nullptr) {
    path.push_back(node->move);
    node = node->parent;
  }
  std::reverse(path.begin(), path.end());
  for (const auto& move : path) {
    temp_board.DoMove(move);
    active = chess::ToggleColour(active);
  }

  // Convert each UCI move to SAN
  for (const auto& uci : uci_moves) {
    chess::Move move = chess::UCIToMove(uci.toStdString());
    if (!temp_board.IsValidMove(move, active)) {
      break;
    }
    std::string san = temp_board.GetSanMove(move);
    san_moves.push_back(QString::fromStdString(san));
    temp_board.DoMove(move);
    active = chess::ToggleColour(active);
  }

  return san_moves;
}

QString ChessBoardWidget::GetPGN() const {
  QString pgn;
  int move_num = 1;

  chess::Colour active = chess::Colour::WHITE;

  // Walk the main line
  const auto sans = GetMainLineSan();
  for (size_t i = 0; i < sans.size(); ++i) {
    if (active == chess::Colour::WHITE) {
      pgn += QString::number(move_num) + ". ";
    }
    pgn += QString::fromStdString(sans[i]) + " ";
    active = chess::ToggleColour(active);
    if (active == chess::Colour::WHITE) {
      move_num++;
    }
  }

  return pgn.trimmed();
}

bool ChessBoardWidget::ImportPGN(const QString& pgn) {
  // Extract move text: skip headers
  QString move_text;
  bool in_headers = false;
  for (const auto& line : pgn.split('\n')) {
    QString trimmed = line.trimmed();
    if (trimmed.startsWith('[')) {
      in_headers = true;
      continue;
    }
    if (in_headers && trimmed.isEmpty()) {
      in_headers = false;
      continue;
    }
    if (in_headers) continue;
    move_text += " " + trimmed;
  }

  // Reset
  m_start_fen = chess::STARTPOS_FEN;
  m_root_node = std::make_unique<chess::MoveNode>();
  m_current_node = m_root_node.get();
  m_move_index = 0;
  m_half_moves = 0;
  m_active_colour = chess::Colour::WHITE;
  InitBoardFromFEN(m_board, m_start_fen, m_active_colour, m_half_moves);

  // Parse with recursive variation support
  int pos = 0;
  auto skipWhitespace = [&]() {
    while (pos < move_text.size() && move_text[pos].isSpace()) pos++;
  };
  auto parseMoveToken = [&]() -> QString {
    skipWhitespace();
    if (pos >= move_text.size()) return {};
    int start = pos;
    while (pos < move_text.size() && !move_text[pos].isSpace() &&
           move_text[pos] != '(' && move_text[pos] != ')' &&
           move_text[pos] != '{' && move_text[pos] != '}' &&
           move_text[pos] != ';')
      pos++;
    return move_text.mid(start, pos - start);
  };

  // Recursive PGN parser
  std::function<void(chess::MoveNode*)> parseVariation =
      [&](chess::MoveNode* parent_node) {
        chess::MoveNode* cur = parent_node;
        chess::Colour turn =
            (parent_node == m_root_node.get())
                ? m_active_colour
                : chess::ToggleColour(parent_node->move.src.file == 0 &&
                                              parent_node->move.src.rank == 0
                                          ? chess::Colour::WHITE
                                          : chess::Colour::WHITE);

        // Determine turn from board state by counting depth
        {
          int depth = 0;
          const chess::MoveNode* n = parent_node;
          while (n->parent != nullptr) {
            depth++;
            n = n->parent;
          }
          chess::Colour start_active = m_active_colour;
          turn = start_active;
          for (int i = 0; i < depth; ++i) {
            turn = chess::ToggleColour(turn);
          }
        }

        while (true) {
          skipWhitespace();
          if (pos >= move_text.size()) break;
          QChar c = move_text[pos];

          if (c == ';') {
            // Line comment to end of line
            while (pos < move_text.size() && move_text[pos] != '\n') pos++;
            continue;
          }
          if (c == '{') {
            // Block comment
            pos++;
            while (pos < move_text.size() && move_text[pos] != '}') pos++;
            if (pos < move_text.size()) pos++;
            continue;
          }
          if (c == '(') {
            pos++;
            // Variation: parse from current node's parent context
            // The variation branches off from `cur`'s last move
            if (cur->parent != nullptr) {
              parseVariation(cur->parent);
            } else {
              parseVariation(cur);
            }
            skipWhitespace();
            if (pos < move_text.size() && move_text[pos] == ')') pos++;
            continue;
          }
          if (c == ')') break;

          QString token = parseMoveToken();
          if (token.isEmpty()) continue;

          // Skip result tokens
          if (token == "1-0" || token == "0-1" || token == "1/2-1/2" ||
              token == "*")
            continue;

          // Strip move numbers from token (e.g. "1." "1..." "12.")
          QString san_token = token;
          // Remove leading digits and dots
          while (!san_token.isEmpty() &&
                 (san_token[0].isDigit() || san_token[0] == '.' ||
                  san_token[0] == '-'))
            san_token.remove(0, 1);
          if (san_token.isEmpty()) continue;

          std::optional<chess::Move> move =
              m_board.ParseSanMove(san_token.toStdString());
          if (!move.has_value()) break;

          std::string san = m_board.GetSanMove(move.value());
          auto new_node =
              std::make_unique<chess::MoveNode>(move.value(), san, cur);
          chess::MoveNode* child = new_node.get();
          cur->children.push_back(std::move(new_node));

          m_board.DoMove(move.value());
          m_active_colour = chess::ToggleColour(m_active_colour);
          m_half_moves++;

          cur = child;
          turn = chess::ToggleColour(turn);
        }
      };

  parseVariation(m_root_node.get());

  // Navigate to end of main line
  m_current_node = m_root_node.get();
  m_move_index = 0;
  GoToEnd();

  return m_root_node != nullptr && !m_root_node->children.empty();
}
