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
