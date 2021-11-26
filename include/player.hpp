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

#ifndef _CHESS_INCLUDE_PLAYER_HPP_
#define _CHESS_INCLUDE_PLAYER_HPP_

#include "chess.hpp"
#include "chessboardwidget.h"
#include "uciengine.hpp"

class Player {
 public:
  Player(chess::Colour colour, ChessBoardWidget* board_widget);
  virtual void Prompt(const chess::Move&) = 0;

 protected:
  chess::Colour m_colour;
  ChessBoardWidget* m_board_widget;
};

class LocalPlayer final : public Player {
 public:
  LocalPlayer(chess::Colour colour, ChessBoardWidget* board_widget);
  inline void Prompt(const chess::Move&) override;
};

class EnginePlayer final : public Player {
 public:
  EnginePlayer(chess::Colour colour, ChessBoardWidget* board_widget,
               UCIEngine* engine);
  inline void Prompt(const chess::Move&) override;

 private:
  UCIEngine* m_engine;
};

#endif  // _CHESS_INCLUDE_PLAYER_HPP_
