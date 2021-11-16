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

#include "piece.hpp"

namespace chess {

Piece::Piece(Colour colour, PieceType type, uint8_t value)
    : m_colour(colour), m_type(type), m_value(value) {}

[[nodiscard]] Colour Piece::GetColour() const { return m_colour; }

[[nodiscard]] PieceType Piece::GetType() const { return m_type; }

[[nodiscard]] uint8_t Piece::GetValue() const { return m_value; }

[[nodiscard]] bool Piece::IsCaptured() const { return m_is_captured; }

[[nodiscard]] Square Piece::GetSquare() const { return m_square; }

void Piece::SetSquare(const Square& square) { m_square = square; }

void Piece::SetCaptured(bool captured) { m_is_captured = captured; }

// PAWN
Pawn::Pawn(Colour colour) : Piece(colour, PieceType::PAWN, PAWN_VALUE) {}

[[nodiscard]] char Pawn::GetFenChar() const {
  if (m_colour == Colour::WHITE) {
    return 'P';
  } else {
    return 'p';
  }
}

[[nodiscard]] std::vector<Move> Pawn::GetMoves(const Board& board) const {
  std::vector<Move> moves;

  uint8_t i = m_square.file;
  uint8_t j = m_square.rank;

  bool has_moved = ((m_colour == Colour::WHITE) && (m_square.rank != 1)) ||
                   ((m_colour == Colour::BLACK) && (m_square.rank != 6));

  const auto square_front = (m_colour == Colour::WHITE)
                                ? Square{i, static_cast<uint8_t>(j + 1)}
                                : Square{i, static_cast<uint8_t>(j - 1)};
  const auto square_front_two = (m_colour == Colour::WHITE)
                                    ? Square{i, static_cast<uint8_t>(j + 2)}
                                    : Square{i, static_cast<uint8_t>(j - 2)};
  const auto square_left =
      (m_colour == Colour::WHITE)
          ? Square{static_cast<uint8_t>(i - 1), static_cast<uint8_t>(j + 1)}
          : Square{static_cast<uint8_t>(i + 1), static_cast<uint8_t>(j - 1)};
  const auto square_right =
      (m_colour == Colour::WHITE)
          ? Square{static_cast<uint8_t>(i + 1), static_cast<uint8_t>(j + 1)}
          : Square{static_cast<uint8_t>(i - 1), static_cast<uint8_t>(j - 1)};

  const auto* piece_front = board.PieceAt(square_front);
  const auto* piece_front_two = board.PieceAt(square_front_two);
  const auto* piece_left = board.PieceAt(square_left);
  const auto* piece_right = board.PieceAt(square_right);

  if (piece_front == nullptr) {
    moves.push_back(Move{m_square, square_front});
  }
  if (!has_moved && (piece_front == nullptr) && (piece_front_two == nullptr)) {
    moves.push_back(Move{m_square, square_front_two});
  }
  if (piece_left != nullptr && (piece_left->GetColour() != m_colour)) {
    moves.push_back(Move{m_square, square_left});
  }
  if (piece_right != nullptr && (piece_right->GetColour() != m_colour)) {
    moves.push_back(Move{m_square, square_right});
  }

  return moves;
};

// KNIGHT
Knight::Knight(Colour colour)
    : Piece(colour, PieceType::KNIGHT, KNIGHT_VALUE) {}

[[nodiscard]] char Knight::GetFenChar() const {
  if (m_colour == Colour::WHITE) {
    return 'N';
  } else {
    return 'n';
  }
}

[[nodiscard]] std::vector<Move> Knight::GetMoves(const Board& board) const {
  std::vector<Move> moves;

  uint8_t i = m_square.file;
  uint8_t j = m_square.rank;

  const std::vector<Square> dsts{
      {static_cast<uint8_t>(i + 1), static_cast<uint8_t>(j + 2)},
      {static_cast<uint8_t>(i + 2), static_cast<uint8_t>(j + 1)},
      {static_cast<uint8_t>(i + 2), static_cast<uint8_t>(j - 1)},
      {static_cast<uint8_t>(i + 1), static_cast<uint8_t>(j - 2)},
      {static_cast<uint8_t>(i - 1), static_cast<uint8_t>(j + 2)},
      {static_cast<uint8_t>(i - 2), static_cast<uint8_t>(j + 1)},
      {static_cast<uint8_t>(i - 2), static_cast<uint8_t>(j - 1)},
      {static_cast<uint8_t>(i - 1), static_cast<uint8_t>(j - 2)}};

  for (const auto& dst_square : dsts) {
    const auto* dst_piece = board.PieceAt(dst_square);
    if (dst_piece == nullptr || (dst_piece->GetColour() != m_colour)) {
      moves.push_back({m_square, dst_square});
    }
  }

  return moves;
};

// BISHOP
Bishop::Bishop(Colour colour)
    : Piece(colour, PieceType::BISHOP, BISHOP_VALUE) {}

[[nodiscard]] char Bishop::GetFenChar() const {
  if (m_colour == Colour::WHITE) {
    return 'B';
  } else {
    return 'b';
  }
}

[[nodiscard]] std::vector<Move> Bishop::GetMoves(const Board&) const {
  // TODO: Implement bishop moves
  return {};
};

// ROOK
Rook::Rook(Colour colour) : Piece(colour, PieceType::ROOK, ROOK_VALUE) {}

[[nodiscard]] char Rook::GetFenChar() const {
  if (m_colour == Colour::WHITE) {
    return 'R';
  } else {
    return 'r';
  }
}

[[nodiscard]] std::vector<Move> Rook::GetMoves(const Board&) const {
  // TODO: Implement rook moves
  return {};
};

// QUEEN
Queen::Queen(Colour colour) : Piece(colour, PieceType::QUEEN, QUEEN_VALUE) {}

[[nodiscard]] char Queen::GetFenChar() const {
  if (m_colour == Colour::WHITE) {
    return 'Q';
  } else {
    return 'q';
  }
}

[[nodiscard]] std::vector<Move> Queen::GetMoves(const Board&) const {
  // TODO: Implement queen moves
  return {};
};

// KING
King::King(Colour colour) : Piece(colour, PieceType::KING, KING_VALUE) {}

[[nodiscard]] char King::GetFenChar() const {
  if (m_colour == Colour::WHITE) {
    return 'K';
  } else {
    return 'k';
  }
}

[[nodiscard]] std::vector<Move> King::GetMoves(const Board&) const {
  // TODO: Implement king moves
  return {};
};

}  // namespace chess
