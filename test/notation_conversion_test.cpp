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

#include "chess.hpp"

TEST(NotationConversion, SquareToString) {
  EXPECT_EQ(chess::SquareToString({0, 0}), "a1");
  EXPECT_EQ(chess::SquareToString({0, 1}), "a2");
  EXPECT_EQ(chess::SquareToString({0, 7}), "a8");
  EXPECT_EQ(chess::SquareToString({1, 0}), "b1");
  EXPECT_EQ(chess::SquareToString({2, 1}), "c2");
  EXPECT_EQ(chess::SquareToString({7, 7}), "h8");
  EXPECT_EQ(chess::SquareToString({3, 3}), "d4");
  EXPECT_EQ(chess::SquareToString({4, 4}), "e5");
  EXPECT_EQ(chess::SquareToString({5, 5}), "f6");
  EXPECT_EQ(chess::SquareToString({6, 6}), "g7");
  EXPECT_EQ(chess::SquareToString({8, 6}), "-7");
}

TEST(NotationConversion, StringToSquare) {
  EXPECT_EQ(chess::StringToSquare("a1"), (chess::Square{0, 0}));
  EXPECT_EQ(chess::StringToSquare("a2"), (chess::Square{0, 1}));
  EXPECT_EQ(chess::StringToSquare("a8"), (chess::Square{0, 7}));
  EXPECT_EQ(chess::StringToSquare("b1"), (chess::Square{1, 0}));
  EXPECT_EQ(chess::StringToSquare("b2"), (chess::Square{1, 1}));
  EXPECT_EQ(chess::StringToSquare("b8"), (chess::Square{1, 7}));
  EXPECT_EQ(chess::StringToSquare("h1"), (chess::Square{7, 0}));
  EXPECT_EQ(chess::StringToSquare("h2"), (chess::Square{7, 1}));
  EXPECT_EQ(chess::StringToSquare("h8"), (chess::Square{7, 7}));
  EXPECT_EQ(chess::StringToSquare("c3"), (chess::Square{2, 2}));
  EXPECT_EQ(chess::StringToSquare("d4"), (chess::Square{3, 3}));
  EXPECT_EQ(chess::StringToSquare("e5"), (chess::Square{4, 4}));
  EXPECT_EQ(chess::StringToSquare("f6"), (chess::Square{5, 5}));
  EXPECT_EQ(chess::StringToSquare("g7"), (chess::Square{6, 6}));

  // Upper case
  EXPECT_EQ(chess::StringToSquare("G7"), (chess::Square{6, 6}));
}

TEST(NotationConversion, MoveToUCI) {
  EXPECT_EQ(chess::MoveToUCI(chess::Move{{3, 1}, {3, 3}}), "d2d4");
  EXPECT_EQ(chess::MoveToUCI(chess::Move{{4, 1}, {4, 3}}), "e2e4");
  EXPECT_EQ(chess::MoveToUCI(chess::Move{{6, 0}, {5, 2}}), "g1f3");
  EXPECT_EQ(chess::MoveToUCI(chess::Move{{4, 0}, {6, 0}}), "e1g1");

  // Pawn promotion
  EXPECT_EQ(chess::MoveToUCI(
                chess::Move{{4, 6}, {4, 7}, true, chess::PieceType::KNIGHT}),
            "e7e8n");
}

TEST(NotationConversion, UCIToMove) {
  EXPECT_EQ(chess::UCIToMove("d2d4"), (chess::Move{{3, 1}, {3, 3}}));
  EXPECT_EQ(chess::UCIToMove("e2e4"), (chess::Move{{4, 1}, {4, 3}}));
  EXPECT_EQ(chess::UCIToMove("g1f3"), (chess::Move{{6, 0}, {5, 2}}));
  EXPECT_EQ(chess::UCIToMove("e1g1"), (chess::Move{{4, 0}, {6, 0}}));

  // Upper case
  EXPECT_EQ(chess::UCIToMove("E1G1"), (chess::Move{{4, 0}, {6, 0}}));
  // Mixed case
  EXPECT_EQ(chess::UCIToMove("E1g1"), (chess::Move{{4, 0}, {6, 0}}));
  EXPECT_EQ(chess::UCIToMove("e1G1"), (chess::Move{{4, 0}, {6, 0}}));

  // Pawn promotion
  EXPECT_EQ(chess::UCIToMove("a7a8q"),
            (chess::Move{{0, 6}, {0, 7}, true, chess::PieceType::QUEEN}));
  EXPECT_EQ(chess::UCIToMove("a7a8r"),
            (chess::Move{{0, 6}, {0, 7}, true, chess::PieceType::ROOK}));
}
