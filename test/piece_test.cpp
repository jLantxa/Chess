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

#include "piece.hpp"

#include <gtest/gtest.h>

TEST(PieceTest, PieceInfo) {
  chess::Pawn wpawn(chess::Colour::WHITE);
  chess::Pawn bpawn(chess::Colour::BLACK);
  EXPECT_EQ(wpawn.GetColour(), chess::Colour::WHITE);
  EXPECT_EQ(wpawn.GetType(), chess::PieceType::PAWN);
  EXPECT_EQ(wpawn.GetValue(), chess::PAWN_VALUE);
  EXPECT_EQ(bpawn.GetColour(), chess::Colour::BLACK);
  EXPECT_EQ(bpawn.GetType(), chess::PieceType::PAWN);
  EXPECT_EQ(bpawn.GetValue(), chess::PAWN_VALUE);
  EXPECT_EQ(chess::PAWN_VALUE, 1);

  chess::Knight knight(chess::Colour::WHITE);
  EXPECT_EQ(knight.GetType(), chess::PieceType::KNIGHT);
  EXPECT_EQ(knight.GetValue(), chess::KNIGHT_VALUE);
  EXPECT_EQ(chess::KNIGHT_VALUE, 3);

  chess::Bishop bishop(chess::Colour::WHITE);
  EXPECT_EQ(bishop.GetType(), chess::PieceType::BISHOP);
  EXPECT_EQ(bishop.GetValue(), chess::BISHOP_VALUE);
  EXPECT_EQ(chess::BISHOP_VALUE, 3);

  chess::Rook rook(chess::Colour::WHITE);
  EXPECT_EQ(rook.GetType(), chess::PieceType::ROOK);
  EXPECT_EQ(rook.GetValue(), chess::ROOK_VALUE);
  EXPECT_EQ(chess::ROOK_VALUE, 5);

  chess::Queen queen(chess::Colour::WHITE);
  EXPECT_EQ(queen.GetType(), chess::PieceType::QUEEN);
  EXPECT_EQ(queen.GetValue(), chess::QUEEN_VALUE);
  EXPECT_EQ(chess::QUEEN_VALUE, 9);

  chess::King king(chess::Colour::WHITE);
  EXPECT_EQ(king.GetType(), chess::PieceType::KING);
  EXPECT_EQ(king.GetValue(), chess::KING_VALUE);
  EXPECT_EQ(chess::KING_VALUE, 255);
}
