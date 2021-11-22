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

#ifndef _CHESS_INCLUDE_PIECE_HPP_
#define _CHESS_INCLUDE_PIECE_HPP_

#include <vector>

#include "board.hpp"
#include "chess.hpp"

namespace chess {

class Board;

class Piece {
 public:
  Piece(Colour colour, PieceType type, uint8_t value);

  static constexpr uint32_t FLAG_EXCLUDE_CASTLES = (1 << 0);

  [[nodiscard]] Colour GetColour() const;
  [[nodiscard]] PieceType GetType() const;
  [[nodiscard]] uint8_t GetValue() const;
  [[nodiscard]] Square GetSquare() const;
  void SetSquare(const Square& square);
  [[nodiscard]] bool HasMoved() const;
  void SetMoved(bool moved);

  [[nodiscard]] virtual std::unique_ptr<Piece> Clone() const = 0;

  [[nodiscard]] virtual std::vector<Move> GetMoves(const Board& board,
                                                   uint32_t flag = 0) const = 0;
  [[nodiscard]] virtual char GetFenChar() const = 0;

 protected:
  const Colour m_colour;
  const PieceType m_type;
  Square m_square;
  const uint8_t m_value;
  bool m_has_moved = false;

  [[nodiscard]] std::vector<Move> GetSlidingMoves(const Board& board,
                                                  Direction direction) const;
};

class Pawn final : public Piece {
 public:
  Pawn(Colour colour);
  [[nodiscard]] std::unique_ptr<Piece> Clone() const override;
  [[nodiscard]] char GetFenChar() const override;
  [[nodiscard]] std::vector<Move> GetMoves(const Board& board,
                                           uint32_t flag = 0) const override;
};

class Knight final : public Piece {
 public:
  Knight(Colour colour);
  [[nodiscard]] std::unique_ptr<Piece> Clone() const override;
  [[nodiscard]] char GetFenChar() const override;
  [[nodiscard]] std::vector<Move> GetMoves(const Board& board,
                                           uint32_t flag = 0) const override;
};

class Bishop final : public Piece {
 public:
  Bishop(Colour colour);
  [[nodiscard]] std::unique_ptr<Piece> Clone() const override;
  [[nodiscard]] char GetFenChar() const override;
  [[nodiscard]] std::vector<Move> GetMoves(const Board& board,
                                           uint32_t flag = 0) const override;
};

class Rook final : public Piece {
 public:
  Rook(Colour colour);
  [[nodiscard]] std::unique_ptr<Piece> Clone() const override;
  [[nodiscard]] char GetFenChar() const override;
  [[nodiscard]] std::vector<Move> GetMoves(const Board& board,
                                           uint32_t flag = 0) const override;
};

class Queen final : public Piece {
 public:
  Queen(Colour colour);
  [[nodiscard]] std::unique_ptr<Piece> Clone() const override;
  [[nodiscard]] char GetFenChar() const override;
  [[nodiscard]] std::vector<Move> GetMoves(const Board& board,
                                           uint32_t flag = 0) const override;
};

class King final : public Piece {
 public:
  King(Colour colour);
  [[nodiscard]] std::unique_ptr<Piece> Clone() const override;
  [[nodiscard]] char GetFenChar() const override;
  [[nodiscard]] std::vector<Move> GetMoves(const Board& board,
                                           uint32_t flag = 0) const override;
};

}  // namespace chess

#endif  // _CHESS_INCLUDE_PIECE_HPP_
