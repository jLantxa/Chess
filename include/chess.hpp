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

#ifndef _CHESS_INCLUDE_CHESS_HPP_
#define _CHESS_INCLUDE_CHESS_HPP_

#include <array>
#include <cstdint>
#include <string>

namespace chess {

enum class Colour { WHITE, BLACK };

enum class PieceType { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

constexpr uint8_t PAWN_VALUE = 1U;
constexpr uint8_t KNIGHT_VALUE = 3U;
constexpr uint8_t BISHOP_VALUE = 3U;
constexpr uint8_t ROOK_VALUE = 5U;
constexpr uint8_t QUEEN_VALUE = 9U;
constexpr uint8_t KING_VALUE = UINT8_MAX;

/**
 * @brief A square in a chess board.
 * A file is a column.
 * A rank is a row.
 */
struct Square {
  uint8_t file;
  uint8_t rank;

  bool operator==(const Square& other) const;
  bool operator!=(const Square& other) const;
};

/**
 * @brief A direction of movement in the chess board.
 */
enum class Direction {
  LEFT,
  UP,
  RIGHT,
  DOWN,
  UP_LEFT,
  UP_RIGHT,
  DOWN_LEFT,
  DOWN_RIGHT
};

/**
 * @brief Get the Square in a given direction from a departing square.
 *
 * @param direction Direction of movement.
 * @param n Steps to advance.
 * @param square Departing square.
 * @return Square Destination.
 */
Square GetSquareInDirection(Direction direction, uint8_t n,
                            const Square& square);

/**
 * @brief A move in a game of chess. It is defined by a source and destination
 * square.
 */
struct Move {
  Square src;
  Square dst;

  bool operator==(const Move& other) const;
  bool operator!=(const Move& other) const;
};

constexpr Move WHITE_KING_CASTLE{{4, 0}, {6, 0}};
constexpr Move WHITE_QUEEN_CASTLE{{4, 0}, {2, 0}};
constexpr Move BLACK_KING_CASTLE{{4, 7}, {6, 7}};
constexpr Move BLACK_QUEEN_CASTLE{{4, 7}, {2, 7}};
constexpr Move WHITE_KING_ROOK_CASTLE{{7, 0}, {5, 0}};
constexpr Move WHITE_QUEEN_ROOK_CASTLE{{0, 0}, {3, 0}};
constexpr Move BLACK_KING_ROOK_CASTLE{{7, 7}, {5, 7}};
constexpr Move BLACK_QUEEN_ROOK_CASTLE{{0, 7}, {3, 7}};

const std::string STARTPOS_FEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

constexpr std::array<char, 8> FILES{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

/**
 * @brief Converts a file index into its corresponding letter.
 */
uint8_t FileToNumber(char file);

/**
 * @brief Converts a file letter into its corresponding index 0-7.
 */
char NumberToFile(uint8_t number);

[[nodiscard]] std::string SquareToString(const Square& square);
[[nodiscard]] Square StringToSquare(const std::string& str);

/**
 * @brief Converts a move into a string representation
 */
[[nodiscard]] std::string MoveToUCI(const Move& move);

/**
 * @brief Converts a move string into a Move object.
 */
[[nodiscard]] Move UCIToMove(const std::string& uci);

/**
 * @brief Toggle colour
 */
void ToggleColour(Colour* colour);

/**
 * @brief Returns true if the given square is contained in the 8x8 chess board.
 */
bool IsValidSquare(const Square& square);

/**
 * @brief Returns true if the given move is contained in the 8x8 chess board.
 */
bool IsMoveInBoard(const Move& move);

}  // namespace chess

#endif  // _CHESS_INCLUDE_CHESS_HPP_
