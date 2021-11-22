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

#include "chess.hpp"

namespace chess {

bool Square::operator==(const Square& other) const {
  return (file == other.file) && (rank == other.rank);
}

bool Square::operator!=(const Square& other) const { return !(*this == other); }

bool Move::operator==(const Move& other) const {
  return (src == other.src) && (dst == other.dst);
}

bool Move::operator!=(const Move& other) const { return !(*this == other); }

void ToggleColour(Colour* colour) {
  if (*colour == Colour::WHITE) {
    *colour = Colour::BLACK;
  } else if (*colour == Colour::BLACK) {
    *colour = Colour::WHITE;
  }
}

uint8_t FileToNumber(char file) {
  file = tolower(file);
  const bool invalid_file = (file < 'a') || (file > 'h');
  if (invalid_file) {
    return 0;
  }

  return (file - 'a');
}

char NumberToFile(uint8_t number) {
  if (number > 7) {
    return '-';
  }

  return FILES[number];
}

std::string SquareToString(const Square& square) {
  const char file_char = NumberToFile(square.file);
  const std::string str =
      std::string{file_char} + (std::to_string(square.rank + 1));

  return str;
}

Square StringToSquare(const std::string& str) {
  uint8_t file = FileToNumber(str[0]);
  uint8_t rank = str[1] - '1';
  Square square{file, rank};

  return square;
}

std::string MoveToUCI(const Move& move) {
  const std::string src = SquareToString(move.src);
  const std::string dst = SquareToString(move.dst);
  const std::string str = src + dst;

  return str;
}

Move UCIToMove(const std::string& uci) {
  if (uci.size() < 4) {
    return Move{{0, 0}, {0, 0}};
  }

  Square src = StringToSquare(uci.substr(0, 2));
  Square dst = StringToSquare(uci.substr(2, 4));
  Move move{src, dst};

  return move;
}

bool IsValidSquare(const Square& square) {
  return ((square.file < 8) && (square.rank < 8));
}

bool IsMoveInBoard(const Move& move) {
  return (IsValidSquare(move.src) && IsValidSquare(move.dst));
}

Square GetSquareInDirection(Direction direction, uint8_t n,
                            const Square& square) {
  switch (direction) {
    case Direction::UP:
      return {static_cast<uint8_t>(square.file),
              static_cast<uint8_t>(square.rank + n)};
      break;
    case Direction::DOWN:
      return {static_cast<uint8_t>(square.file),
              static_cast<uint8_t>(square.rank - n)};
      break;
    case Direction::LEFT:
      return {static_cast<uint8_t>(square.file - n),
              static_cast<uint8_t>(square.rank)};
      break;
    case Direction::RIGHT:
      return {static_cast<uint8_t>(square.file + n),
              static_cast<uint8_t>(square.rank)};
      break;
    default:
      return {};
  }
}

}  // namespace chess
