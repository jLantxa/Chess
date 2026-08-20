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

#include <gtest/gtest.h>

#include <memory>

#include "board.hpp"

class SanTest : public ::testing::Test {
 public:
  void SetUp() override { board = std::make_unique<chess::Board>(); }

 protected:
  std::unique_ptr<chess::Board> board;
};

TEST_F(SanTest, SimplePawnMove) {
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::WHITE), {4, 1});
  EXPECT_EQ(board->GetSanMove({{4, 1}, {4, 3}}), "e4");
}

TEST_F(SanTest, PawnCapture) {
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::WHITE), {3, 3});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {4, 4});
  EXPECT_EQ(board->GetSanMove({{3, 3}, {4, 4}}), "dxe5");
}

TEST_F(SanTest, KnightMove) {
  board->SetPiece(std::make_unique<chess::Knight>(chess::Colour::WHITE),
                  {6, 0});
  EXPECT_EQ(board->GetSanMove({{6, 0}, {5, 2}}), "Nf3");
}

TEST_F(SanTest, DisambiguationByFile) {
  // Two rooks on same rank with clear path to target
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::WHITE), {4, 4});
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::WHITE), {0, 0});
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::WHITE), {7, 0});
  // Both Ra1 and Rh1 can reach d1 along the first rank (king is on e4, not
  // blocking)
  EXPECT_EQ(board->GetSanMove({{0, 0}, {3, 0}}), "Rad1");
  EXPECT_EQ(board->GetSanMove({{7, 0}, {3, 0}}), "Rhd1");
}

TEST_F(SanTest, DisambiguationByRank) {
  // Two rooks on same file that can go to same square
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::WHITE), {0, 0});
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::WHITE), {0, 4});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {0, 2});
  // Ra1 can capture on a3, Ra5 can also go to a3
  EXPECT_EQ(board->GetSanMove({{0, 0}, {0, 2}}), "R1xa3");
}

TEST_F(SanTest, CastlingKingSide) {
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::WHITE), {4, 0});
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::WHITE), {7, 0});
  board->SetCastling(true, true, true, true);
  EXPECT_EQ(board->GetSanMove({{4, 0}, {6, 0}}), "O-O");
}

TEST_F(SanTest, CastlingQueenSide) {
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::WHITE), {4, 0});
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::WHITE), {0, 0});
  board->SetCastling(true, true, true, true);
  EXPECT_EQ(board->GetSanMove({{4, 0}, {2, 0}}), "O-O-O");
}

TEST_F(SanTest, PawnPromotion) {
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::WHITE), {4, 6});
  chess::Move move{{4, 6}, {4, 7}, true, chess::PieceType::QUEEN};
  EXPECT_EQ(board->GetSanMove(move), "e8=Q");
}

TEST_F(SanTest, PawnPromotionCapture) {
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::WHITE), {4, 6});
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::BLACK), {5, 7});
  chess::Move move{{4, 6}, {5, 7}, true, chess::PieceType::KNIGHT};
  EXPECT_EQ(board->GetSanMove(move), "exf8=N");
}

TEST_F(SanTest, CheckDetection) {
  // Black queen on e7, white king on e1, path clear -> check
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::WHITE), {4, 0});
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::BLACK), {4, 7});
  board->SetPiece(std::make_unique<chess::Queen>(chess::Colour::BLACK), {4, 6});
  // Qe7-e2+ gives check (no escape = check, not mate)
  EXPECT_EQ(board->GetSanMove({{4, 6}, {4, 1}}), "Qe2+");
}

TEST_F(SanTest, NoCheck) {
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::WHITE), {4, 0});
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::BLACK), {4, 7});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {3, 6});
  EXPECT_EQ(board->GetSanMove({{3, 6}, {3, 5}}), "d6");
}

TEST_F(SanTest, Checkmate) {
  // Scholar's mate final position: Qf7# (simplified)
  // White queen on h5, white bishop on c4, black king e8, black pawn f7
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::WHITE), {4, 0});
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::BLACK), {4, 7});
  board->SetPiece(std::make_unique<chess::Queen>(chess::Colour::WHITE), {7, 4});
  board->SetPiece(std::make_unique<chess::Bishop>(chess::Colour::WHITE),
                  {2, 3});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {5, 6});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {3, 6});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {6, 6});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {6, 5});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {5, 5});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {4, 6});
  // Qxf7# -- black king on e8, queen takes f7, bishop on c4 covers f7
  // King can't go to d8 (Q covers), f8 (Q covers), d7 (Q covers)...
  // Let me verify with a simpler checkmate: back rank mate
  board->Clear();
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::WHITE), {4, 0});
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::BLACK), {4, 7});
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::WHITE), {0, 6});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {3, 6});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {4, 6});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {5, 6});
  // Ra8# - back rank mate, pawns block king's escape
  EXPECT_EQ(board->GetSanMove({{0, 6}, {0, 7}}), "Ra8#");
}
