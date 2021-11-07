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

#ifndef _CHESS_INCLUDE_CHESS_H_
#define _CHESS_INCLUDE_CHESS_H_

#include <cstdint>

#include <set>

#include <QIcon>
#include <QWidget>

#include "board.hpp"
#include "chess.hpp"

struct ChessPalette {
    QColor white_square;
    QColor black_square;
    QColor white_highlight;
    QColor black_highlight;
    QColor highlight_important;
};

class ChessBoardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoardWidget(QWidget *parent = nullptr);

    void Set(const QString& fen_str);

    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:


private:
    const std::array<QIcon, 6> WHITE_ICONS {
        QIcon("res/icon/wp.svg"),
        QIcon("res/icon/wn.svg"),
        QIcon("res/icon/wb.svg"),
        QIcon("res/icon/wr.svg"),
        QIcon("res/icon/wq.svg"),
        QIcon("res/icon/wk.svg")
    };

    const std::array<QIcon, 6> BLACK_ICONS {
        QIcon("res/icon/bp.svg"),
        QIcon("res/icon/bn.svg"),
        QIcon("res/icon/bb.svg"),
        QIcon("res/icon/br.svg"),
        QIcon("res/icon/bq.svg"),
        QIcon("res/icon/bk.svg")
    };

    ChessPalette m_palette;
    void SetPalette(const ChessPalette& palette);

    chess::Board m_board;

    chess::Colour m_side = chess::Colour::WHITE;
    int m_board_size;
    int m_square_size;

    chess::Square* m_cur_sel_sq = nullptr;
    chess::Square* m_sel_src_sq = nullptr;
    chess::Square* m_sel_dst_sq = nullptr;
    std::set<chess::Square> m_hightlighted;

    static constexpr int MARGIN = 8;

    void GetRotatedCoordinates(uint8_t ax, uint8_t ay, uint8_t& bx, uint8_t& by, chess::Colour side) const;
    void GetGridCoordinates(int x, int y, uint8_t& u, uint8_t& v) const;
    chess::Square GetClickedSquare(int x, int y) const;
    bool IsOnBoard(int x, int y) const;

    void DrawBoard();
};

#endif  // _CHESS_INCLUDE_CHESS_H_