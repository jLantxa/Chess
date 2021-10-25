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

void ToggleColour(Colour* colour) {
     if (*colour == Colour::WHITE) {
        *colour = Colour::BLACK;
    } else if (*colour == Colour::BLACK) {
        *colour = Colour::WHITE;
    }
}

static uint8_t FileToNumber(char file, bool* ok) {
    file = tolower(file);
    const bool invalid_file = (file < 'a') || (file > 'h');
    if (invalid_file) {
        *ok = false;
        return 0;
    }

    *ok = true;
    return (file - 'a');
}

static char NumberToFile(uint8_t number, bool* ok) {
    if (number > 8) {
        *ok = false;
        return FILES[0];
    }

    *ok = true;
    return FILES[number];
}

std::string SquareToString(const Square& square, bool* ok) {
    if ((square.file > 8) || (square.rank > 8)) {
        *ok = false;
        return "a0";
    }

    bool file_ok;
    const char file_char = NumberToFile(square.file, &file_ok);
    *ok = file_ok;

    const std::string str = std::string{file_char} + std::to_string(square.rank);
    return str;
}

Square StringToSquare(const std::string& str, bool* ok) {
    bool file_ok;
    bool rank_ok;
    uint8_t file = FileToNumber(str[0], &file_ok);
    uint8_t rank = str[1] - '0';
    rank_ok = rank < 10;

    Square square {file, rank};
    *ok = (file_ok && rank_ok);
    return square;
}

std::string MoveToUCI(const Move& move, bool* ok) {
    bool src_ok;
    bool dst_ok;
    const std::string src = SquareToString(move.src, &src_ok);
    const std::string dst = SquareToString(move.dst, &dst_ok);

    const std::string str = src + dst;
    *ok = (src_ok && dst_ok);
    return str;
}

Move UCIToMove(const std::string& uci, bool* ok) {
    if (uci.size() < 4) {
        *ok = false;
        return Move{{0, 0}, {0, 0}};
    }

    bool src_ok;
    bool dst_ok;
    Square src = StringToSquare(uci.substr(0, 2), &src_ok);
    Square dst = StringToSquare(uci.substr(2, 4), &dst_ok);

    Move move {src, dst};
    *ok = (src_ok && dst_ok);
    return move;
}

}  // namespace chess
