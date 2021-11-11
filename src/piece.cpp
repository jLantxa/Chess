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

[[nodiscard]] Colour Piece::GetColour() const { return m_colour; };

[[nodiscard]] PieceType Piece::GetType() const { return m_type; };

[[nodiscard]] uint8_t Piece::GetValue() const { return m_value; };

[[nodiscard]] bool Piece::IsCaptured() const { return m_is_captured; };

void Piece::SetCaptured(bool captured) { m_is_captured = captured; };

// PAWN
Pawn::Pawn(Colour colour) : Piece(colour, PieceType::PAWN, PAWN_VALUE) {}

[[nodiscard]] std::set<Move> Pawn::GetMoves(const Board&) const {
  // TODO: Implement pawn moves
  return {};
};

// KNIGHT
Knight::Knight(Colour colour)
    : Piece(colour, PieceType::KNIGHT, KNIGHT_VALUE) {}

[[nodiscard]] std::set<Move> Knight::GetMoves(const Board&) const {
  // TODO: Implement knight moves
  return {};
};

// BISHOP
Bishop::Bishop(Colour colour)
    : Piece(colour, PieceType::BISHOP, BISHOP_VALUE) {}

[[nodiscard]] std::set<Move> Bishop::GetMoves(const Board&) const {
  // TODO: Implement bishop moves
  return {};
};

// ROOK
Rook::Rook(Colour colour) : Piece(colour, PieceType::ROOK, ROOK_VALUE) {}

[[nodiscard]] std::set<Move> Rook::GetMoves(const Board&) const {
  // TODO: Implement rook moves
  return {};
};

// QUEEN
Queen::Queen(Colour colour) : Piece(colour, PieceType::QUEEN, QUEEN_VALUE) {}

[[nodiscard]] std::set<Move> Queen::GetMoves(const Board&) const {
  // TODO: Implement queen moves
  return {};
};

// KING
King::King(Colour colour) : Piece(colour, PieceType::KING, KING_VALUE) {}

[[nodiscard]] std::set<Move> King::GetMoves(const Board&) const {
  // TODO: Implement king moves
  return {};
};

}  // namespace chess
