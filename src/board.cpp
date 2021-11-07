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

#include "board.hpp"

namespace chess {

void Board::SetPiece(std::unique_ptr<Piece> piece, uint8_t i, uint8_t j) {
    ClearPieceAt(i, j);
    m_board[i][j] = std::move(piece);
}

const Piece* Board::PieceAt(uint8_t i, uint8_t j) const {
    if ((i > 7) || (j > 7)) {
        return nullptr;
    }

    return m_board[i][j].get();
}

void Board::ClearPieceAt(uint8_t i, uint8_t j) {
    if (m_board[i][j].get() != nullptr) {
        m_board[i][j].reset();
    }
}

void Board::DoMove(const Move& move) {
    auto& src = m_board[move.src.rank][move.src.file];
    auto& dst = m_board[move.dst.rank][move.dst.file];
    dst.swap(src);
    src.reset();
}

void Board::Clear() {
    for (uint8_t i; i < 8; ++i) {
        for (uint8_t j; j < 8; ++j) {
            ClearPieceAt(i, j);
        }
    }
}

}  // namespace chess
