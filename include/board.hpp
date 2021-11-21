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

#ifndef _CHESS_INCLUDE_BOARD_H_
#define _CHESS_INCLUDE_BOARD_H_

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "chess.hpp"
#include "piece.hpp"

namespace chess {

class Piece;

class Board {
 public:
  Board() = default;
  Board(const Board& b);

  [[nodiscard]] const Piece* PieceAt(uint8_t i, uint8_t j) const;
  [[nodiscard]] const Piece* PieceAt(const chess::Square& square) const;

  void ClearPieceAt(uint8_t i, uint8_t j);
  void ClearPieceAt(const chess::Square& square);
  void Clear();

  void SetPiece(std::unique_ptr<Piece> piece, uint8_t i, uint8_t j);
  void SetPiece(std::unique_ptr<Piece> piece, const chess::Square& square);

  void DoMove(const Move& move);

  [[nodiscard]] std::string GetPosition(const Colour& active_colour) const;

  void SetCastling(bool wkc, bool wqc, bool bkc, bool bqc);

  [[nodiscard]] std::vector<Move> GetMovesFrom(uint8_t i, uint8_t j) const;
  [[nodiscard]] std::vector<Move> GetMovesFrom(const Square& square) const;

  /**
   * @brief Check if player is in check.
   *
   * @param colour Colour
   * @return true The player is in check.
   * @return false  The player is not in check.
   */
  [[nodiscard]] bool IsInCheck(chess::Colour colour) const;

  /**
   * @brief A piece in a give square can be captured by the other colour.
   *
   * @param square Square to check
   * @return true An existing piece can be captured.
   * @return false No piece can be captured in the given square.
   */
  [[nodiscard]] bool CanBeCaptured(const Square& square) const;

  [[nodiscard]] Board AfterMove(const Move& move) const;

 private:
  std::unique_ptr<Piece> m_board[8][8];

  std::optional<Square> m_en_passant;

  bool m_wkc = true;
  bool m_wqc = true;
  bool m_bkc = true;
  bool m_bqc = true;

  bool CanWKC() const;
  bool CanWQC() const;
  bool CanBKC() const;
  bool CanBQC() const;
};

}  // namespace chess

#endif  // _CHESS_INCLUDE_BOARD_H_
