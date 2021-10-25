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

#include "chess.hpp"

namespace chess {

class Board;

class Piece {
public:
    Piece(Colour colour, PieceType type);

    inline [[nodiscard]] Colour GetColour() const;
    inline [[nodiscard]] PieceType GetType() const;
    inline [[nodiscard]] uint8_t GetValue() const;
    inline [[nodiscard]] bool IsCaptured() const;
    void SetCaptured(bool captured);
    [[nodiscard]] std::vector<Move> GetMoves(const Board& board) const;

protected:
    const Colour m_colour;
    const PieceType m_type;
    bool m_is_captured;
}

}  // namespace chess

#endif  // _CHESS_INCLUDE_PIECE_HPP_
