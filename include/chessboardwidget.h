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

    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:


private:
    const QIcon WHITE_PAWN   = QIcon("res/icon/wp.svg");
    const QIcon WHITE_KNIGHT = QIcon("res/icon/wn.svg");
    const QIcon WHITE_BISHOP = QIcon("res/icon/wb.svg");
    const QIcon WHITE_ROOK   = QIcon("res/icon/wr.svg");
    const QIcon WHITE_QUEEN  = QIcon("res/icon/wq.svg");
    const QIcon WHITE_KING   = QIcon("res/icon/wk.svg");
    const QIcon BLACK_PAWN   = QIcon("res/icon/bp.svg");
    const QIcon BLACK_KNIGHT = QIcon("res/icon/bn.svg");
    const QIcon BLACK_BISHOP = QIcon("res/icon/bb.svg");
    const QIcon BLACK_ROOK   = QIcon("res/icon/br.svg");
    const QIcon BLACK_QUEEN  = QIcon("res/icon/bq.svg");
    const QIcon BLACK_KING   = QIcon("res/icon/bk.svg");

    ChessPalette m_palette;
    void SetPalette(const ChessPalette& palette);

    bool m_rotated = false;
    int m_board_size;
    int m_square_size;

    chess::Square* m_cur_sel_sq = nullptr;
    chess::Square* m_sel_src_sq = nullptr;
    chess::Square* m_sel_dst_sq = nullptr;
    std::set<chess::Square> m_hightlighted;

    static constexpr int MARGIN = 8;

    void GetRotatedCoordinates(uint8_t ax, uint8_t ay, uint8_t& bx, uint8_t& by, bool rotated) const;
    void GetGridCoordinates(int x, int y, uint8_t& u, uint8_t& v) const;
    chess::Square GetClickedSquare(int x, int y) const;
    bool IsOnBoard(int x, int y) const;

    void DrawBoard();
};

#endif  // _CHESS_INCLUDE_CHESS_H_
