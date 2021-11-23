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

#include <QDebug>
#include <algorithm>
#include <sstream>

namespace chess {

Board::Board(const Board& original) {
  for (uint8_t i = 0; i < 8; ++i) {
    for (uint8_t j = 0; j < 8; ++j) {
      if (original.m_board[i][j]) {
        m_board[i][j] = original.m_board[i][j]->Clone();
      }
    }
  }

  m_en_passant = original.m_en_passant;
  m_white_king_square = original.m_white_king_square;
  m_black_king_square = original.m_black_king_square;

  m_wkc = original.m_wkc;
  m_wqc = original.m_wqc;
  m_bkc = original.m_bkc;
  m_bqc = original.m_bqc;
}

void Board::SetPiece(std::unique_ptr<Piece> piece, uint8_t i, uint8_t j) {
  ClearPieceAt(i, j);
  m_board[i][j] = std::move(piece);
  m_board[i][j]->SetSquare({i, j});
  SaveSquareIfKing(m_board[i][j].get());
}

void Board::SetPiece(std::unique_ptr<Piece> piece,
                     const chess::Square& square) {
  const uint8_t i = square.file;
  const uint8_t j = square.rank;
  SetPiece(std::move(piece), i, j);
}

const Piece* Board::PieceAt(uint8_t i, uint8_t j) const {
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

void Board::SaveSquareIfKing(Piece* const piece) {
  if (piece == nullptr) {
    return;
  }

  const auto type = piece->GetType();
  const auto colour = piece->GetColour();
  if (type == PieceType::KING) {
    if (colour == Colour::WHITE) {
      m_white_king_square = piece->GetSquare();
    } else {
      m_black_king_square = piece->GetSquare();
    }
  }
}

void Board::DoMove(const Move& move) {
  if (MoveIsWKC(move)) {
    MovePieces(move);
    MovePieces(WHITE_KING_ROOK_CASTLE);
  } else if (MoveIsWQC(move)) {
    MovePieces(move);
    MovePieces(WHITE_QUEEN_ROOK_CASTLE);
  } else if (MoveIsBKC(move)) {
    MovePieces(move);
    MovePieces(BLACK_KING_ROOK_CASTLE);
  } else if (MoveIsBQC(move)) {
    MovePieces(move);
    MovePieces(BLACK_QUEEN_ROOK_CASTLE);
  } else {
    MovePieces(move);
  }
}

void Board::MovePieces(const Move& move) {
  auto& src = m_board[move.src.file][move.src.rank];
  auto& dst = m_board[move.dst.file][move.dst.rank];

  dst.swap(src);
  dst->SetSquare(move.dst);
  dst->SetMoved(true);
  SaveSquareIfKing(dst.get());
  UpdateCastles();
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

  const bool some_castling_possible = m_wkc || m_wqc || m_bkc || m_bqc;

  if (some_castling_possible) {
    ss << (m_wkc ? "K" : "");
    ss << (m_wqc ? "Q" : "");
    ss << (m_bkc ? "k" : "");
    ss << (m_bqc ? "q" : "");
    ss << " ";
  } else {
    ss << "- ";
  }

  ss << ((m_en_passant.has_value()) ? SquareToString(m_en_passant.value())
                                    : "-");

  return ss.str();
}

[[nodiscard]] bool Board::CanWKC() const {
  if (!m_wkc || IsInCheck(Colour::WHITE)) {
    return false;
  }

  constexpr std::array<Square, 3> path{Square{5, 0}, {6, 0}, {7, 0}};
  for (const auto& sq : path) {
    if (CanBeCaptured(sq)) {
      return false;
    }
  }

  return true;
}

[[nodiscard]] bool Board::CanWQC() const {
  if (!m_wqc || IsInCheck(Colour::WHITE)) {
    return false;
  }

  constexpr std::array<Square, 5> path{Square{3, 0}, {2, 0}, {1, 0}, {0, 0}};
  for (const auto& sq : path) {
    if (CanBeCaptured(sq)) {
      return false;
    }
  }

  return true;
}

[[nodiscard]] bool Board::CanBKC() const {
  if (!m_bkc || IsInCheck(Colour::BLACK)) {
    return false;
  }

  constexpr std::array<Square, 3> path{Square{5, 7}, {6, 7}, {7, 7}};
  for (const auto& sq : path) {
    if (CanBeCaptured(sq)) {
      return false;
    }
  }

  return true;
}

[[nodiscard]] bool Board::CanBQC() const {
  if (!m_bqc || IsInCheck(Colour::BLACK)) {
    return false;
  }

  constexpr std::array<Square, 5> path{Square{3, 7}, {2, 7}, {1, 7}, {0, 7}};
  for (const auto& sq : path) {
    if (CanBeCaptured(sq)) {
      return false;
    }
  }

  return true;
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

bool Board::IsValidMove(const Move& move, Colour active_colour) const {
  const chess::Piece* src_piece = PieceAt(move.src);
  if (src_piece == nullptr) {
    return false;
  }

  // Can only make moves that don't put yourself in check or prevent being in
  // check
  auto future_board = AfterMove(move);
  const bool will_be_in_check = future_board.IsInCheck(active_colour);
  if (will_be_in_check) {
    return false;
  }

  const auto valid_moves = GetMovesFrom(move.src);
  for (auto& valid_move : valid_moves) {
    if (valid_move == move) {
      return true;
    }
  }

  return false;
}

[[nodiscard]] bool Board::CanBeCaptured(const Square& square) const {
  if (!IsValidSquare(square)) {
    return false;
  }

  const auto* piece_in_check = PieceAt(square);

  for (uint8_t i = 0; i < 8; ++i) {
    for (uint8_t j = 0; j < 8; ++j) {
      const Square src_square{i, j};
      const auto* src_piece = PieceAt(src_square);
      if ((piece_in_check == nullptr) || (src_piece == nullptr)) {
        continue;
      } else if (src_piece->GetColour() == piece_in_check->GetColour()) {
        continue;
      }

      const auto moves =
          src_piece->GetMoves(*this, Piece::FLAG_EXCLUDE_CASTLES);
      for (auto& move : moves) {
        if (move.dst == square) {
          return true;
        }
      }
    }
  }

  return false;
}

[[nodiscard]] bool Board::IsInCheck(chess::Colour colour) const {
  const auto king_square =
      (colour == Colour::WHITE) ? m_white_king_square : m_black_king_square;
  if (!king_square.has_value()) {
    return false;
  }

  return CanBeCaptured(king_square.value());
}

[[nodiscard]] Board Board::AfterMove(const Move& move) const {
  Board future_board(*this);
  future_board.DoMove(move);
  return future_board;
}

void Board::UpdateCastles() {
  if (m_white_king_square.has_value()) {
    const auto* white_king = PieceAt(m_white_king_square.value());
    const auto* a1 = PieceAt(0, 0);
    const auto* h1 = PieceAt(7, 0);

    const bool king_not_moved = (white_king != nullptr) &&
                                (!white_king->HasMoved()) &&
                                (white_king->GetSquare() == Square{4, 0});
    const bool a1_not_moved = (a1 != nullptr) && !a1->HasMoved() &&
                              (a1->GetType() == PieceType::ROOK) &&
                              (a1->GetColour() == Colour::WHITE);
    const bool h1_not_moved = (h1 != nullptr) && !h1->HasMoved() &&
                              (h1->GetType() == PieceType::ROOK) &&
                              (h1->GetColour() == Colour::WHITE);

    m_wkc = king_not_moved && h1_not_moved;
    m_wqc = king_not_moved && a1_not_moved;
  }

  if (m_black_king_square.has_value()) {
    const auto* black_king = PieceAt(m_black_king_square.value());
    const auto* a8 = PieceAt(0, 7);
    const auto* h8 = PieceAt(7, 7);

    const bool king_not_moved = (black_king != nullptr) &&
                                (!black_king->HasMoved()) &&
                                (black_king->GetSquare() == Square{4, 7});
    const bool a8_not_moved = (a8 != nullptr) && !a8->HasMoved() &&
                              (a8->GetType() == PieceType::ROOK) &&
                              (a8->GetColour() == Colour::BLACK);
    const bool h8_not_moved = (h8 != nullptr) && !h8->HasMoved() &&
                              (h8->GetType() == PieceType::ROOK) &&
                              (h8->GetColour() == Colour::BLACK);

    m_bkc = king_not_moved && h8_not_moved;
    m_bqc = king_not_moved && a8_not_moved;
  }
}

[[nodiscard]] bool Board::MoveIsWKC(const Move& move) const {
  const auto* piece = PieceAt(move.src);
  const bool is_white_king_moving = (piece != nullptr) &&
                                    (piece->GetType() == PieceType::KING) &&
                                    (piece->GetColour() == Colour::WHITE);
  return (is_white_king_moving && (move == WHITE_KING_CASTLE));
}

[[nodiscard]] bool Board::MoveIsWQC(const Move& move) const {
  const auto* piece = PieceAt(move.src);
  const bool is_white_king_moving = (piece != nullptr) &&
                                    (piece->GetType() == PieceType::KING) &&
                                    (piece->GetColour() == Colour::WHITE);
  return (is_white_king_moving && (move == WHITE_QUEEN_CASTLE));
}

[[nodiscard]] bool Board::MoveIsBKC(const Move& move) const {
  const auto* piece = PieceAt(move.src);
  const bool is_black_king_moving = (piece != nullptr) &&
                                    (piece->GetType() == PieceType::KING) &&
                                    (piece->GetColour() == Colour::BLACK);
  return (is_black_king_moving && (move == BLACK_KING_CASTLE));
}

[[nodiscard]] bool Board::MoveIsBQC(const Move& move) const {
  const auto* piece = PieceAt(move.src);
  const bool is_black_king_moving = (piece != nullptr) &&
                                    (piece->GetType() == PieceType::KING) &&
                                    (piece->GetColour() == Colour::BLACK);
  return (is_black_king_moving && (move == BLACK_QUEEN_CASTLE));
}

}  // namespace chess
