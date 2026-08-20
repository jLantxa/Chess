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

void Board::ClearPieceAt(uint8_t i, uint8_t j) { m_board[i][j].reset(); }

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
  // Handle special moves first
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
  } else if (MoveIsPawnPromotion(move)) {
    MoveForPromotion(move);
  } else if (MoveIsEnPassant(move)) {
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

void Board::MoveForPromotion(const Move& move) {
  auto& src = m_board[move.src.file][move.src.rank];
  auto& dst = m_board[move.dst.file][move.dst.rank];

  const auto* src_piece = PieceAt(move.src);
  const PieceType type = move.promotion_type;
  const Colour colour = src_piece->GetColour();

  // Reset both pieces
  src.reset();
  dst.reset();

  // Set promoted piece at dst square
  std::unique_ptr<Piece> promoted_piece = Piece::Factory(type, colour);
  SetPiece(std::move(promoted_piece), move.dst);

  dst->SetSquare(move.dst);
  dst->SetMoved(true);
  UpdateCastles();
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

  // Squares the king passes through and lands on must not be attacked
  constexpr std::array<Square, 2> path{Square{5, 0}, {6, 0}};
  for (const auto& sq : path) {
    if (CanBeCaptured(sq)) {
      return false;
    }
  }

  // Squares between king and rook must be empty
  const auto* f1 = PieceAt(5, 0);
  const auto* g1 = PieceAt(6, 0);
  if (f1 != nullptr || g1 != nullptr) {
    return false;
  }

  return true;
}

[[nodiscard]] bool Board::CanWQC() const {
  if (!m_wqc || IsInCheck(Colour::WHITE)) {
    return false;
  }

  // Squares the king passes through and lands on must not be attacked
  constexpr std::array<Square, 3> path{Square{3, 0}, {2, 0}, {1, 0}};
  for (const auto& sq : path) {
    if (CanBeCaptured(sq)) {
      return false;
    }
  }

  // Squares between king and rook must be empty
  const auto* b1 = PieceAt(1, 0);
  const auto* c1 = PieceAt(2, 0);
  const auto* d1 = PieceAt(3, 0);
  if (b1 != nullptr || c1 != nullptr || d1 != nullptr) {
    return false;
  }

  return true;
}

[[nodiscard]] bool Board::CanBKC() const {
  if (!m_bkc || IsInCheck(Colour::BLACK)) {
    return false;
  }

  // Squares the king passes through and lands on must not be attacked
  constexpr std::array<Square, 2> path{Square{5, 7}, {6, 7}};
  for (const auto& sq : path) {
    if (CanBeCaptured(sq)) {
      return false;
    }
  }

  // Squares between king and rook must be empty
  const auto* f8 = PieceAt(5, 7);
  const auto* g8 = PieceAt(6, 7);
  if (f8 != nullptr || g8 != nullptr) {
    return false;
  }

  return true;
}

[[nodiscard]] bool Board::CanBQC() const {
  if (!m_bqc || IsInCheck(Colour::BLACK)) {
    return false;
  }

  // Squares the king passes through and lands on must not be attacked
  constexpr std::array<Square, 3> path{Square{3, 7}, {2, 7}, {1, 7}};
  for (const auto& sq : path) {
    if (CanBeCaptured(sq)) {
      return false;
    }
  }

  // Squares between king and rook must be empty
  const auto* b8 = PieceAt(1, 7);
  const auto* c8 = PieceAt(2, 7);
  const auto* d8 = PieceAt(3, 7);
  if (b8 != nullptr || c8 != nullptr || d8 != nullptr) {
    return false;
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

const std::optional<Square>& Board::GetWhiteKing() const {
  return m_white_king_square;
}

const std::optional<Square>& Board::GetBlackKing() const {
  return m_black_king_square;
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

[[nodiscard]] bool Board::MoveIsPawnPromotion(const Move& move) const {
  if (!move.is_pawn_promotion) {
    return false;
  }

  const bool advance_to_last_rank =
      ((move.src.rank == 6) && (move.dst.rank == 7)) ||
      ((move.src.rank == 1) && (move.dst.rank == 0));

  return advance_to_last_rank;
}

[[nodiscard]] bool Board::MoveIsEnPassant(const Move& move) const {
  (void)move;
  return false;
}

std::string Board::GetSanMove(const Move& move) const {
  // Castling
  if (MoveIsWKC(move) || MoveIsBKC(move)) {
    return "O-O";
  }
  if (MoveIsWQC(move) || MoveIsBQC(move)) {
    return "O-O-O";
  }

  const auto* src_piece = PieceAt(move.src);
  if (src_piece == nullptr) {
    return {};
  }

  const auto piece_type = src_piece->GetType();
  const bool is_capture = (PieceAt(move.dst) != nullptr);
  std::string san;

  if (piece_type == PieceType::PAWN) {
    if (is_capture) {
      san += NumberToFile(move.src.file);
      san += 'x';
    }
    san += SquareToString(move.dst);
    if (move.is_pawn_promotion) {
      san += '=';
      san += static_cast<char>(
          std::toupper(PIECE_CHARS[static_cast<uint8_t>(move.promotion_type)]));
    }
  } else {
    san += static_cast<char>(
        std::toupper(PIECE_CHARS[static_cast<uint8_t>(piece_type)]));

    // Disambiguation: check if another piece of same type & colour can reach
    // dst
    bool need_file_disambig = false;
    bool need_rank_disambig = false;
    for (uint8_t fi = 0; fi < 8; ++fi) {
      for (uint8_t ri = 0; ri < 8; ++ri) {
        const Square sq{fi, ri};
        if (sq == move.src) continue;
        const auto* other = PieceAt(sq);
        if (other == nullptr || other->GetColour() != src_piece->GetColour() ||
            other->GetType() != piece_type) {
          continue;
        }
        const auto other_moves =
            other->GetMoves(*this, Piece::FLAG_EXCLUDE_CASTLES);
        for (const auto& m : other_moves) {
          if (m.dst == move.dst) {
            if (fi != move.src.file) {
              need_file_disambig = true;
            } else {
              need_rank_disambig = true;
            }
          }
        }
      }
    }
    if (need_file_disambig) {
      san += NumberToFile(move.src.file);
    }
    if (need_rank_disambig) {
      san += static_cast<char>('1' + move.src.rank);
    }

    if (is_capture) {
      san += 'x';
    }
    san += SquareToString(move.dst);
  }

  // Promotion suffix (handled above inline for pawns)
  if (piece_type != PieceType::PAWN && move.is_pawn_promotion) {
    san += '=';
    san += static_cast<char>(
        std::toupper(PIECE_CHARS[static_cast<uint8_t>(move.promotion_type)]));
  }

  // Check / checkmate detection
  Board future_board(*this);
  future_board.DoMove(move);
  const auto opponent_colour =
      (src_piece->GetColour() == Colour::WHITE) ? Colour::BLACK : Colour::WHITE;
  if (future_board.IsInCheck(opponent_colour)) {
    // Check if opponent has any legal moves (checkmate)
    bool has_legal_moves = false;
    for (uint8_t fi = 0; fi < 8 && !has_legal_moves; ++fi) {
      for (uint8_t ri = 0; ri < 8 && !has_legal_moves; ++ri) {
        const Square sq{fi, ri};
        const auto* piece = future_board.PieceAt(sq);
        if (piece == nullptr || piece->GetColour() != opponent_colour) continue;
        const auto moves = future_board.GetMovesFrom(sq);
        for (const auto& m : moves) {
          if (future_board.IsValidMove(m, opponent_colour)) {
            has_legal_moves = true;
            break;
          }
        }
      }
    }
    san += has_legal_moves ? '+' : '#';
  }

  return san;
}

std::optional<Move> Board::ParseSanMove(const std::string& san) const {
  if (san.empty()) return std::nullopt;

  // Castling
  if (san == "O-O" || san == "0-0") {
    const auto* king =
        PieceAt(4, (IsInCheck(Colour::WHITE) || PieceAt(4, 0)) ? 0 : 7);
    if (!king || king->GetType() != PieceType::KING) {
      for (uint8_t r : {0u, 7u}) {
        const auto* k = PieceAt(4, r);
        if (k && k->GetType() == PieceType::KING &&
            k->GetColour() == Colour::WHITE) {
          return WHITE_KING_CASTLE;
        }
        if (k && k->GetType() == PieceType::KING &&
            k->GetColour() == Colour::BLACK) {
          return BLACK_KING_CASTLE;
        }
      }
      return std::nullopt;
    }
    return (king->GetColour() == Colour::WHITE) ? WHITE_KING_CASTLE
                                                : BLACK_KING_CASTLE;
  }
  if (san == "O-O-O" || san == "0-0-0") {
    for (uint8_t r : {0u, 7u}) {
      const auto* k = PieceAt(4, r);
      if (k && k->GetType() == PieceType::KING) {
        return (k->GetColour() == Colour::WHITE) ? WHITE_QUEEN_CASTLE
                                                 : BLACK_QUEEN_CASTLE;
      }
    }
    return std::nullopt;
  }

  std::string s = san;
  // Strip check/mate symbols
  while (!s.empty() && (s.back() == '+' || s.back() == '#')) {
    s.pop_back();
  }

  // Check for promotion
  PieceType promotion_type = PieceType::QUEEN;
  bool is_promotion = false;
  auto eq_pos = s.find('=');
  if (eq_pos != std::string::npos) {
    is_promotion = true;
    char promo_char = std::tolower(s[eq_pos + 1]);
    promotion_type = GetPieceTypeFromChar(promo_char);
    s = s.substr(0, eq_pos);
  }

  // Determine piece type
  PieceType piece_type = PieceType::PAWN;
  if (!s.empty() && std::isupper(static_cast<unsigned char>(s[0]))) {
    piece_type = GetPieceTypeFromChar(s[0]);
    s = s.substr(1);
  }

  // Remove capture symbol
  auto x_pos = s.find('x');
  if (x_pos != std::string::npos) {
    s.erase(x_pos, 1);
  }

  if (s.size() < 2) return std::nullopt;

  Square dst = StringToSquare(s.substr(s.size() - 2));

  // Determine source square by finding the matching piece
  for (uint8_t fi = 0; fi < 8; ++fi) {
    for (uint8_t ri = 0; ri < 8; ++ri) {
      const Square sq{fi, ri};
      const auto* piece = PieceAt(sq);
      if (piece == nullptr) continue;
      if (piece->GetType() != piece_type) continue;

      auto moves = piece->GetMoves(*this, Piece::FLAG_EXCLUDE_CASTLES);
      for (const auto& m : moves) {
        if (m.dst != dst) continue;
        if (!IsValidMove(m, piece->GetColour())) continue;

        // Check disambiguation chars (remaining chars in s after removing dst)
        std::string disambig = s.substr(0, s.size() - 2);
        bool file_match =
            disambig.empty() ||
            (std::isalpha(static_cast<unsigned char>(disambig[0])) &&
             disambig[0] == NumberToFile(fi));
        bool rank_match =
            disambig.empty() ||
            (std::isdigit(static_cast<unsigned char>(disambig.back())) &&
             disambig.back() == ('1' + ri));

        if (file_match && rank_match) {
          Move move{sq, dst, is_promotion, promotion_type};
          return move;
        }
      }
    }
  }

  return std::nullopt;
}

}  // namespace chess
