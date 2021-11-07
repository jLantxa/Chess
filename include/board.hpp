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

#include "chess.hpp"
#include "piece.hpp"

namespace chess {

class Board {
public:
    const Piece* PieceAt(uint8_t i, uint8_t j) const;
    void ClearPieceAt(uint8_t i, uint8_t j);
    void Clear();
    void SetPiece(std::unique_ptr<Piece> piece, uint8_t i, uint8_t j);
    void DoMove(const Move& move);

private:
    std::unique_ptr<Piece> m_board[8][8];

    bool white_queen_side_castle = true;
    bool white_king_side_castle  = true;
    bool black_queen_side_castle = true;
    bool black_king_side_castle  = true;
};

}  // namespace chess

#endif  // _CHESS_INCLUDE_BOARD_H_
