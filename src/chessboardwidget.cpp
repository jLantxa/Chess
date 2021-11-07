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

#include <algorithm>
#include <functional>

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRect>

#include "chess.hpp"
#include "chessboardwidget.h"

ChessBoardWidget::ChessBoardWidget(QWidget* parent) : QWidget(parent) {
    static constexpr ChessPalette GREEN_PALETTE {
        .white_square = QColor(238,238,210),
        .black_square = QColor(118,150,86),
        .white_highlight = QColor(),
        .black_highlight = QColor(),
        .highlight_important = QColor(255, 113, 74),
    };

    SetPalette(GREEN_PALETTE);
}

void ChessBoardWidget::Set(const QString& fen_str) {
    // TODO
    (void) fen_str;
}

void ChessBoardWidget::SetPalette(const ChessPalette& palette) {
    m_palette = palette;
}

void ChessBoardWidget::GetRotatedCoordinates(uint8_t ax, uint8_t ay, uint8_t& bx, uint8_t& by, chess::Colour side) const {
    if (side == chess::Colour::WHITE) {
        bx = ax;
        by = 7-ay;
    } else {
        bx = 7-ax;
        by = ay;
    }
}

void ChessBoardWidget::GetGridCoordinates(int x, int y, uint8_t& u, uint8_t& v) const {
    u = (x-MARGIN) / m_square_size;
    v = (y-MARGIN) / m_square_size;
}

chess::Square ChessBoardWidget::GetClickedSquare(int x, int y) const {
    uint8_t i, j, u, v;
    GetGridCoordinates(x, y, u, v);
    GetRotatedCoordinates(u, v, i, j, m_side);
    return chess::Square{i, j};
}

bool ChessBoardWidget::IsOnBoard(int x, int y) const {
    const bool inside = (x > MARGIN) &&
                        (y > MARGIN) &&
                        (x < MARGIN + 8*m_square_size) &&
                        (y < MARGIN + 8*m_square_size);

    if (inside) {
        return true;
    }

    return false;
}

void ChessBoardWidget::paintEvent(QPaintEvent*) {
    DrawBoard();
}

void ChessBoardWidget::mousePressEvent(QMouseEvent* event) {
    const QPoint position = event->pos();
    const int x = position.x();
    const int y = position.y();
    if (!IsOnBoard(x, y)) {
        return;
    }

    const Qt::MouseButton button = event->button();
    const chess::Square square = GetClickedSquare(x, y);

    (void) square;

    switch (button) {
        default:
            return;

        case Qt::LeftButton:
            break;

        case Qt::RightButton:
            break;
    }
}

void ChessBoardWidget::mouseReleaseEvent(QMouseEvent* event) {
    const QPoint position = event->pos();
    const int x = position.x();
    const int y = position.y();
    if (!IsOnBoard(x, y)) {
        return;
    }

    const Qt::MouseButton button = event->button();
    const chess::Square square = GetClickedSquare(position.x(), position.y());

    (void) square;

    switch (button) {
        default:
            return;

        case Qt::LeftButton:
            break;

        case Qt::RightButton:
            break;
    }
}

void ChessBoardWidget::DrawBoard() {
    QPainter painter(this);
    const QRect geometry = this->geometry();
    QFont font;

    m_board_size = std::min(geometry.width(), geometry.height()) - 2*MARGIN;
    m_square_size = m_board_size/8;

    font.setWeight(QFont::Bold);
    font.setPixelSize(m_square_size/4);
    const int text_margin = 3;
    QFontMetrics font_metrics(font);

    painter.setFont(font);

    /* i, j -> Coordinates of the board
     * u, v -> Coordinates of the widget grid (accounting for rotation)
     */
    uint8_t u, v;
    for (uint8_t i = 0; i < 8; ++i) {
        for (uint8_t j = 0; j < 8; ++j) {
            GetRotatedCoordinates(i, j, u, v, m_side);
            const int x = MARGIN + u*m_square_size;
            const int y = MARGIN + v*m_square_size;
            const QColor& square_colour = (((i+j) % 2) == 0)?
                                          m_palette.black_square : m_palette.white_square;
            const QColor& text_colour = (((i+j) % 2) == 0)?
                                        m_palette.white_square : m_palette.black_square;

            // Square
            painter.fillRect(x, y, m_square_size, m_square_size, square_colour);

            // Coordinate
            if (v == 7) {

                painter.setPen(text_colour);
                painter.drawText(x + m_square_size - font_metrics.xHeight() - text_margin,
                                 y + m_square_size - text_margin,
                                 QString(chess::NumberToFile(i)));
            }
            if (u == 0) {
                painter.setPen(text_colour);
                painter.drawText(x + text_margin,
                                 y + font_metrics.capHeight() + text_margin,
                                 QString::number(j+1));
            }

            // Piece
            const chess::Piece* piece = m_board.PieceAt(i, j);
            if (piece != nullptr) {
                const uint8_t type_index = static_cast<uint8_t>(piece->GetType());
                const std::array<QIcon, 6>& icon_array = (piece->GetColour() == chess::Colour::WHITE)?
                                          WHITE_ICONS : BLACK_ICONS;
                const QIcon icon = icon_array[type_index];
                painter.drawPixmap(x, y,
                                   m_square_size, m_square_size,
                                   icon.pixmap(QSize(m_square_size, m_square_size)));
            }
        }
    }
}
