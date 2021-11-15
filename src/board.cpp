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

#include <sstream>

namespace chess {

void Board::SetPiece(std::unique_ptr<Piece> piece, uint8_t i, uint8_t j) {
  ClearPieceAt(i, j);
  m_board[i][j] = std::move(piece);
  m_board[i][j]->SetSquare({i, j});
}

void Board::SetPiece(std::unique_ptr<Piece> piece,
                     const chess::Square& square) {
  const uint8_t i = square.file;
  const uint8_t j = square.rank;
  SetPiece(std::move(piece), i, j);
}

const Piece* Board::PieceAt(uint8_t i, uint8_t j) const {
  if ((i > 7) || (j > 7)) {
    return nullptr;
  }

  return m_board[i][j].get();
}

const Piece* Board::PieceAt(const chess::Square& square) const {
  const uint8_t i = square.file;
  const uint8_t j = square.rank;
  return PieceAt(i, j);
}

void Board::ClearPieceAt(uint8_t i, uint8_t j) {
  if (m_board[i][j].get() != nullptr) {
    m_board[i][j].reset();
  }
}

void Board::ClearPieceAt(const chess::Square& square) {
  const uint8_t i = square.file;
  const uint8_t j = square.rank;
  ClearPieceAt(i, j);
}

void Board::DoMove(const Move& move) {
  auto& src = m_board[move.src.file][move.src.rank];
  auto& dst = m_board[move.dst.file][move.dst.rank];
  dst.swap(src);
  dst->SetSquare(move.dst);
  src.reset();
}

void Board::Clear() {
  for (uint8_t i = 0; i < 8; ++i) {
    for (uint8_t j = 0; j < 8; ++j) {
      ClearPieceAt(i, j);
    }
  }
}

void Board::SetCastling(bool wkc, bool wqc, bool bkc, bool bqc) {
  m_wkc = wkc;
  m_wqc = wqc;
  m_bkc = bkc;
  m_bqc = bqc;
}

std::string Board::GetPosition(const Colour& active_colour) const {
  std::stringstream ss;

  uint8_t empty_count;
  for (uint8_t j = 0; j < 8; ++j) {
    empty_count = 0;
    for (uint8_t i = 0; i < 8; ++i) {
      auto piece = m_board[i][7 - j].get();
      if (piece == nullptr) {
        empty_count++;
      } else {
        if (empty_count > 0) {
          ss << std::to_string(empty_count);
          empty_count = 0;
        }
        ss << std::string{piece->GetFenChar()};
      }
    }

    if (empty_count > 0) {
      ss << std::to_string(empty_count);
    }
    if (j != 7) {
      ss << "/";
    } else {
      ss << " ";
    }
  }

  ss << ((active_colour == Colour::WHITE) ? "w " : "b ");

  const bool some_castling_possible =
      CanWKC() || CanWQC() || CanBKC() || CanBQC();

  if (some_castling_possible) {
    ss << (CanWKC() ? "K" : "");
    ss << (CanWQC() ? "Q" : "");
    ss << (CanBKC() ? "k" : "");
    ss << (CanBQC() ? "q" : "");
    ss << " ";
  } else {
    ss << "- ";
  }

  ss << ((m_en_passant.has_value()) ? SquareToString(m_en_passant.value())
                                    : "- ");

  return ss.str();
}

bool Board::CanWKC() const {
  // TODO: Implement
  return m_wkc;
}

bool Board::CanWQC() const {
  // TODO: Implement
  return m_wqc;
}

bool Board::CanBKC() const {
  // TODO: Implement
  return m_bkc;
}

bool Board::CanBQC() const {
  // TODO: Implement
  return m_bqc;
}

std::vector<Move> Board::GetMovesFrom(uint8_t i, uint8_t j) const {
  const Square square{i, j};
  return GetMovesFrom(square);
}

std::vector<Move> Board::GetMovesFrom(const Square& square) const {
  // TODO: Handle en passant moves.
  const Piece* src_piece = PieceAt(square);
  if (src_piece == nullptr) {
    return {};
  }

  return src_piece->GetMoves(*this);
}

}  // namespace chess
