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

#ifndef _CHESS_INCLUDE_RESOURCES_H_
#define _CHESS_INCLUDE_RESOURCES_H_

#include <QIcon>
#include <array>

namespace resources {

const std::array<QIcon, 6> WHITE_ICONS{
    QIcon("res/icon/wp.svg"), QIcon("res/icon/wn.svg"),
    QIcon("res/icon/wb.svg"), QIcon("res/icon/wr.svg"),
    QIcon("res/icon/wq.svg"), QIcon("res/icon/wk.svg")};

const std::array<QIcon, 6> BLACK_ICONS{
    QIcon("res/icon/bp.svg"), QIcon("res/icon/bn.svg"),
    QIcon("res/icon/bb.svg"), QIcon("res/icon/br.svg"),
    QIcon("res/icon/bq.svg"), QIcon("res/icon/bk.svg")};

const QIcon ROTATE_ICON = QIcon("res/icon/rotate.svg");
const QIcon SETTINGS_ICON = QIcon("res/icon/settings.svg");
const QIcon ENGINE_ICON = QIcon("res/icon/power.svg");
const QIcon DOWNLOAD_ICON = QIcon("res/icon/download.svg");

}  // namespace resources

#endif  // _CHESS_INCLUDE_RESOURCES_H_
