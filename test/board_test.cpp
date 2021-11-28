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

#include <gtest/gtest.h>

#include <memory>
#include <optional>

class BoardTest : public ::testing::Test {
 public:
  void SetUp() override { board = std::make_unique<chess::Board>(); }

 protected:
  std::unique_ptr<chess::Board> board;
};

TEST_F(BoardTest, GetFen) {
  const std::string start_pos =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";

  for (uint8_t i = 0; i < 8; ++i) {
    board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::WHITE),
                    {i, 1});
    board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK),
                    {i, 6});
  }
  board->SetPiece(std::make_unique<chess::Knight>(chess::Colour::WHITE),
                  {1, 0});
  board->SetPiece(std::make_unique<chess::Knight>(chess::Colour::WHITE),
                  {6, 0});
  board->SetPiece(std::make_unique<chess::Knight>(chess::Colour::BLACK),
                  {1, 7});
  board->SetPiece(std::make_unique<chess::Knight>(chess::Colour::BLACK),
                  {6, 7});
  board->SetPiece(std::make_unique<chess::Bishop>(chess::Colour::WHITE),
                  {2, 0});
  board->SetPiece(std::make_unique<chess::Bishop>(chess::Colour::WHITE),
                  {5, 0});
  board->SetPiece(std::make_unique<chess::Bishop>(chess::Colour::BLACK),
                  {2, 7});
  board->SetPiece(std::make_unique<chess::Bishop>(chess::Colour::BLACK),
                  {5, 7});
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::WHITE), {0, 0});
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::WHITE), {7, 0});
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::BLACK), {0, 7});
  board->SetPiece(std::make_unique<chess::Rook>(chess::Colour::BLACK), {7, 7});
  board->SetPiece(std::make_unique<chess::Queen>(chess::Colour::WHITE), {3, 0});
  board->SetPiece(std::make_unique<chess::Queen>(chess::Colour::BLACK), {3, 7});
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::WHITE), {4, 0});
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::BLACK), {4, 7});

  ASSERT_EQ(start_pos, board->GetPosition(chess::Colour::WHITE));
}

TEST_F(BoardTest, CanBeCaptured) {
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::WHITE), {2, 3});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::WHITE), {3, 3});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {4, 4});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {5, 4});

  EXPECT_FALSE(board->CanBeCaptured({2, 3}));
  EXPECT_TRUE(board->CanBeCaptured({3, 3}));
  EXPECT_TRUE(board->CanBeCaptured({4, 4}));
  EXPECT_FALSE(board->CanBeCaptured({5, 4}));
}

TEST_F(BoardTest, IsInCheck) {
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::WHITE), {2, 3});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::WHITE), {3, 3});
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::BLACK), {4, 4});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {5, 4});

  EXPECT_FALSE(board->IsInCheck(chess::Colour::WHITE));
  EXPECT_TRUE(board->IsInCheck(chess::Colour::BLACK));
}

TEST_F(BoardTest, KeepTrackOfTheKings) {
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::WHITE), {0, 0});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::WHITE), {7, 1});
  board->SetPiece(std::make_unique<chess::King>(chess::Colour::BLACK), {7, 7});
  board->SetPiece(std::make_unique<chess::Pawn>(chess::Colour::BLACK), {1, 7});

  const std::optional<chess::Square>& white_king_square = board->GetWhiteKing();
  const std::optional<chess::Square>& black_king_square = board->GetBlackKing();
  ASSERT_TRUE(white_king_square.has_value());
  EXPECT_EQ(white_king_square.value(), (chess::Square{0, 0}));
  ASSERT_TRUE(black_king_square.has_value());
  EXPECT_EQ(black_king_square.value(), (chess::Square{7, 7}));

  board->DoMove({{0, 0}, {1, 0}});
  ASSERT_TRUE(white_king_square.has_value());
  EXPECT_EQ(white_king_square.value(), (chess::Square{1, 0}));

  board->DoMove({{7, 7}, {7, 6}});
  ASSERT_TRUE(black_king_square.has_value());
  EXPECT_EQ(black_king_square.value(), (chess::Square{7, 6}));
}
