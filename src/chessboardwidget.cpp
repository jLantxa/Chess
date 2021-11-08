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

#include <cmath>

#include <algorithm>
#include <functional>

#include <QMouseEvent>
#include <QPainter>
#include <QRect>

#include "chess.hpp"
#include "chessboardwidget.h"

const QString ChessBoardWidget::STARTPOS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const std::array<QIcon, 6> ChessBoardWidget::WHITE_ICONS {
    QIcon("res/icon/wp.svg"),
    QIcon("res/icon/wn.svg"),
    QIcon("res/icon/wb.svg"),
    QIcon("res/icon/wr.svg"),
    QIcon("res/icon/wq.svg"),
    QIcon("res/icon/wk.svg")
};

const std::array<QIcon, 6> ChessBoardWidget::BLACK_ICONS {
    QIcon("res/icon/bp.svg"),
    QIcon("res/icon/bn.svg"),
    QIcon("res/icon/bb.svg"),
    QIcon("res/icon/br.svg"),
    QIcon("res/icon/bq.svg"),
    QIcon("res/icon/bk.svg")
};

ChessBoardWidget::ChessBoardWidget(QWidget* parent) : QWidget(parent) {
    SetColourPalette(GREEN_PALETTE);

    Reset();
}

void ChessBoardWidget::Reset() {
    SetPosition(STARTPOS_FEN);
    m_score = 0;
}

static bool IsUpperCase(char ch) {
    if ((ch >= 'A') && (ch <= 'Z')) {
        return true;
    }
    return false;
}

static bool IsNumber(char ch) {
    if ((ch >= '0') && (ch <= '9')) {
        return true;
    }
    return false;
}

void ChessBoardWidget::SetPosition(const QString& fen_str) {
    const QStringList& args = fen_str.trimmed().split(" ");

    if (args[1] == 'w') {
        m_playing_colour = chess::Colour::WHITE;
    } else if (args[1] == 'b') {
        m_playing_colour = chess::Colour::BLACK;
    }

    m_board.Clear();

    const QStringList& lines = args[0].trimmed().split("/");
    uint8_t i, j;
    for (j = 0; j < 8; ++j) {
        i = 0;
        const QString& line = lines[7-j];

        for (auto& qch : line) {
            char ch = qch.toLatin1();
            if (IsNumber(ch)) {
                const uint8_t num = ch - '0';
                i += num;
            } else {
                const chess::Colour colour = IsUpperCase(ch)? chess::Colour::WHITE : chess::Colour::BLACK;
                switch (qch.toLower().toLatin1()) {
                case 'p':
                    m_board.SetPiece(std::make_unique<chess::Pawn>(colour), i, j);
                    i++;
                    break;

                case 'n':
                    m_board.SetPiece(std::make_unique<chess::Knight>(colour), i, j);
                    i++;
                    break;

                case 'b':
                    m_board.SetPiece(std::make_unique<chess::Bishop>(colour), i, j);
                    i++;
                    break;

                case 'r':
                    m_board.SetPiece(std::make_unique<chess::Rook>(colour), i, j);
                    i++;
                    break;

                case 'q':
                    m_board.SetPiece(std::make_unique<chess::Queen>(colour), i, j);
                    i++;
                    break;

                case 'k':
                    m_board.SetPiece(std::make_unique<chess::King>(colour), i, j);
                    i++;
                    break;

                default:
                    i++;
                    break;
                }
            }
        }
    }

    repaint();
}

void ChessBoardWidget::Rotate() {
    if (m_side == chess::Colour::WHITE) {
        SetSide(chess::Colour::BLACK);
    } else {
        SetSide(chess::Colour::WHITE);
    }
}

void ChessBoardWidget::SetSide(chess::Colour side) {
    m_side = side;
    repaint();
}

chess::Colour ChessBoardWidget::GetSide() {
    return m_side;
}

void ChessBoardWidget::SetPlayingColour(chess::Colour colour) {
    m_playing_colour = colour;
}

chess::Colour ChessBoardWidget::GetPlayingColour() const {
    return m_playing_colour;
}

void ChessBoardWidget::SetColourPalette(const ChessPalette& palette) {
    m_palette = palette;
    repaint();
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

void ChessBoardWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::TextAntialiasing);
    const QRect geometry = this->geometry();
    QFont coordinates_font;

    constexpr int SCORE_BAR_WIDTH = 20;
    constexpr int SCORE_BAR_SPACING = 10;
    const int board_x_off = SCORE_BAR_WIDTH + SCORE_BAR_SPACING;
    const int board_available_width = geometry.width() - board_x_off;

    m_board_size = std::min(board_available_width, geometry.height()) - 2*MARGIN;
    m_square_size = m_board_size / 8;

    coordinates_font.setWeight(QFont::Bold);
    coordinates_font.setPixelSize(m_square_size/4);
    const int text_margin = 0.075f * m_square_size;
    QFontMetrics font_metrics(coordinates_font);

    painter.setFont(coordinates_font);

    /* i, j -> Coordinates of the board
     * u, v -> Coordinates of the widget grid (accounting for rotation)
     */
    uint8_t u, v;
    for (uint8_t i = 0; i < 8; ++i) {
        for (uint8_t j = 0; j < 8; ++j) {
            GetRotatedCoordinates(i, j, u, v, m_side);
            const float x = MARGIN + board_x_off + u*m_square_size;
            const float y = MARGIN + v*m_square_size;
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
                const QIcon piece_icon = icon_array[type_index];
                const int piece_icon_size = m_square_size;
                painter.drawPixmap(x,
                                   y,
                                   piece_icon_size, piece_icon_size,
                                   piece_icon.pixmap(QSize(piece_icon_size, piece_icon_size)));
            }
        }
    }

    if (m_score_enabled) {
        const float balance = GetBalance();
        const int white_height = m_board_size * balance;
        const int black_height = m_board_size - white_height;

        if (m_side == chess::Colour::WHITE) {
            painter.fillRect(MARGIN, MARGIN,
                             SCORE_BAR_WIDTH, black_height,
                             m_palette.black_square);
            painter.fillRect(MARGIN, black_height + MARGIN,
                             SCORE_BAR_WIDTH, white_height,
                             m_palette.white_square);
        } else {
            painter.fillRect(MARGIN, MARGIN,
                             SCORE_BAR_WIDTH, white_height,
                             m_palette.white_square);
            painter.fillRect(MARGIN, white_height + MARGIN,
                             SCORE_BAR_WIDTH, black_height,
                             m_palette.black_square);
        }

        painter.setPen(QPen(m_palette.highlight_important, 3));
        painter.drawLine(MARGIN, MARGIN + m_board_size/2, SCORE_BAR_WIDTH + MARGIN, m_board_size/2 + MARGIN);
        painter.setPen(Qt::black);
        painter.drawRect(MARGIN, MARGIN, SCORE_BAR_WIDTH, m_board_size);
    }
}

float ChessBoardWidget::GetBalance() const {
    if (m_is_mate) {
        if (m_score > 0) {
            return 1.0f;
        } else {
            return 0.0f;
        }
    } else {
        return Transform(m_score);
    }
}

float ChessBoardWidget::Transform(float x) const {
    x /= 100.0f;  // Convert from centipawns

    constexpr float k = 2.0f;
    float value = 1.0f / (1.0f + exp(-x/k));

    // Limit the value
    constexpr float MAX = 0.95f;
    value = std::min(MAX, value);
    value = std::max(1 - MAX, value);
    return value;
}

void ChessBoardWidget::SetScore(int score, bool mate) {
    m_score = score;
    m_is_mate = mate;
    repaint();
}

void ChessBoardWidget::SetScoreEnabled(bool enabled) {
    m_score_enabled = enabled;
    repaint();
}
