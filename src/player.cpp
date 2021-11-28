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

#include "player.hpp"

#include <unistd.h>

#include <thread>

#include "chess.hpp"

Player::Player(chess::Colour colour, ChessBoardWidget* board_widget)
    : m_colour(colour), m_board_widget(board_widget) {}

LocalPlayer::LocalPlayer(chess::Colour colour, ChessBoardWidget* board_widget)
    : Player(colour, board_widget) {}

inline void LocalPlayer::Prompt(const chess::Move&) {}

EnginePlayer::EnginePlayer(chess::Colour colour, ChessBoardWidget* board_widget,
                           UCIEngine* engine)
    : Player(colour, board_widget), m_engine(engine) {}

inline void EnginePlayer::Prompt(const chess::Move&) {}
