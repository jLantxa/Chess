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

void ChessBoardWidget::SetPalette(const ChessPalette& palette) {
    m_palette = palette;
}

void ChessBoardWidget::paintEvent(QPaintEvent*) {
    DrawBoard();
    DrawPieces();
}

void ChessBoardWidget::mousePressEvent(QMouseEvent* event) {
    (void) event;
}

void ChessBoardWidget::mouseReleaseEvent(QMouseEvent* event) {
    (void) event;
}

void ChessBoardWidget::DrawBoard() {
    QPainter painter(this);
    const QRect geometry = this->geometry();
    QFont font;

    const int board_size = std::min(geometry.width(), geometry.height()) - 2*MARGIN;
    const int sq_size = board_size/8;

    font.setWeight(QFont::Bold);
    font.setPixelSize(sq_size/4);
    const int text_margin = 3;
    QFontMetrics font_metrics(font);

    painter.setFont(font);

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            const int x = MARGIN + i*sq_size;
            const int y = MARGIN + j*sq_size;
            const QColor& square_colour = (((i+j) % 2) == 0)?
                                  m_palette.white_square : m_palette.black_square;

            painter.fillRect(x, y, sq_size, sq_size, square_colour);

            if (j == 7) {
                const QColor& text_colour = (((i+j) % 2) != 0)?
                    m_palette.white_square : m_palette.black_square;

                painter.setPen(text_colour);
                painter.drawText(x + sq_size - font_metrics.xHeight() - text_margin,
                                 y + sq_size - text_margin,
                                 QString(chess::NumberToFile(i)));
            }
            if (i == 0) {
                const QColor& text_colour = (((i+j) % 2) != 0)?
                    m_palette.white_square : m_palette.black_square;

                painter.setPen(text_colour);
                painter.drawText(x + text_margin,
                                 y + font_metrics.capHeight() + text_margin,
                                 QString::number(8-j));
            }
        }
    }
}

void ChessBoardWidget::DrawPieces() {

}
