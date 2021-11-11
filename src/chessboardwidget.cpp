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
#include <algorithm>
#include <cmath>
#include <functional>

#include "chess.hpp"
#include "resources.hpp"

ChessBoardWidget::ChessBoardWidget(QWidget* parent) : QWidget(parent) {
  SetColourPalette(GREEN_PALETTE);
  Reset();
}

void ChessBoardWidget::Reset() {
  SetPosition(QString::fromStdString(chess::STARTPOS_FEN));
  m_score = 0;
}

static bool IsUpperCase(char ch) {
  if ((ch >= 'A') && (ch <= 'Z')) {
    return true;
  }
  return false;
}

static bool IsNumber(char ch) {
  if ((ch >= '0') && (ch <= '9')) {
    return true;
  }
  return false;
}

void ChessBoardWidget::SetPosition(const QString& fen_str) {
  const QStringList& args = fen_str.trimmed().split(" ");

  if (args[1] == 'w') {
    m_playing_colour = chess::Colour::WHITE;
  } else if (args[1] == 'b') {
    m_playing_colour = chess::Colour::BLACK;
  }

  m_board.Clear();

  const uint32_t move_number = args[5].toUInt();
  m_half_moves = 2 * (move_number - 1);
  if (m_playing_colour == chess::Colour::BLACK) {
    m_half_moves++;
  }

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
        const chess::Colour colour =
            IsUpperCase(ch) ? chess::Colour::WHITE : chess::Colour::BLACK;
        switch (qch.toLower().toLatin1()) {
          case 'p':
            m_board.SetPiece(std::make_unique<chess::Pawn>(colour), i, j);
            i++;
            break;

          case 'n':
            m_board.SetPiece(std::make_unique<chess::Knight>(colour), i, j);
            i++;
            break;

          case 'b':
            m_board.SetPiece(std::make_unique<chess::Bishop>(colour), i, j);
            i++;
            break;

          case 'r':
            m_board.SetPiece(std::make_unique<chess::Rook>(colour), i, j);
            i++;
            break;

          case 'q':
            m_board.SetPiece(std::make_unique<chess::Queen>(colour), i, j);
            i++;
            break;

          case 'k':
            m_board.SetPiece(std::make_unique<chess::King>(colour), i, j);
            i++;
            break;

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
  if (m_side == chess::Colour::WHITE) {
    SetSide(chess::Colour::BLACK);
  } else {
    SetSide(chess::Colour::WHITE);
  }
}

void ChessBoardWidget::SetSide(chess::Colour side) {
  m_side = side;
  repaint();
}

chess::Colour ChessBoardWidget::GetSide() { return m_side; }

void ChessBoardWidget::SetPlayingColour(chess::Colour colour) {
  m_playing_colour = colour;
}

chess::Colour ChessBoardWidget::GetPlayingColour() const {
  return m_playing_colour;
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
  const bool inside = (x > board_x) && (y > board_y) &&
                      (x < (board_x + m_board_size)) &&
                      (y < (board_y + m_board_size));

  if (inside) {
    return true;
  }

  return false;
}

void ChessBoardWidget::mousePressEvent(QMouseEvent* event) {
  HandleBoardMouseEvent(event);
}

void ChessBoardWidget::mouseReleaseEvent(QMouseEvent* event) {
  HandleBoardMouseEvent(event);
  event->type();
}

bool ChessBoardWidget::HandleBoardMouseEvent(QMouseEvent* event) {
  const QPoint position = event->pos();
  const int x = position.x();
  const int y = position.y();

  const bool click_on_board = IsOnBoard(x, y);

  const auto event_type = event->type();
  const auto button = event->button();
  const bool left_press = (event_type == QMouseEvent::MouseButtonPress) &&
                          (button == Qt::LeftButton);
  const bool right_press = (event_type == QMouseEvent::MouseButtonPress) &&
                           (button == Qt::RightButton);
  const bool left_release = (event_type == QMouseEvent::MouseButtonRelease) &&
                            (button == Qt::LeftButton);
  //  const bool right_release = (event_type == QMouseEvent::MouseButtonRelease)
  //  &&
  //                             (button == Qt::RightButton);

  if (!click_on_board && (left_press || right_press)) {
    m_selected_square.reset();
    return false;
  }

  if (left_release && !click_on_board) {
    m_in_drag_mode = false;
  }

  m_mouse_position = event->position();
  auto square = GetClickedSquare(x, y);

  if (left_press) {
    if (m_src_square.has_value() && (m_src_square != square)) {
      chess::Move move{m_src_square.value(), square};
      if (DoMove(move)) {
        m_selected_square.reset();
        m_src_square.reset();
      }
    } else if (m_board.PieceAt(square) != nullptr) {
      m_in_drag_mode = true;
      m_selected_square = square;
    }
  } else if (left_release && click_on_board) {
    if (m_in_drag_mode) {
      if (square != m_selected_square.value()) {
        chess::Move move{m_selected_square.value(), square};
        DoMove(move);
      } else {
        m_src_square = m_selected_square;
      }
    }

    m_selected_square.reset();
    m_in_drag_mode = false;
  }

  repaint();
  return true;
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
          m_src_square.has_value() && (m_src_square == chess::Square{i, j});

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
  // TODO: Refactor piece draw
  if (m_in_drag_mode && m_src_square.has_value()) {
    const uint8_t i = m_src_square->file;
    const uint8_t j = m_src_square->rank;

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
  if (IsValidMove(move)) {
    m_board.DoMove(move);
    // TODO: Get FEN, update engine and update status
    return true;
  } else {
    return false;
  }
}

bool ChessBoardWidget::IsValidMove(const chess::Move& move) const {
  // TODO: Use real implementation once move generation is complete
  (void)move;
  return true;

  // const chess::Piece* src_piece = m_board.PieceAt(move.src);
  // if (src_piece == nullptr) {
  //   return false;
  // }

  // const auto valid_moves = src_piece->GetMoves(m_board);
  // for (auto& valid_move : valid_moves) {
  //   if (valid_move == move) {
  //     return true;
  //   }
  // }

  // return false;
}
