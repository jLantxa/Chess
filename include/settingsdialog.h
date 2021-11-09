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

#ifndef _CHESS_INCLUDE_SETTINGS_DIALOG_H_
#define _CHESS_INCLUDE_SETTINGS_DIALOG_H_

#include <QDialog>
#include <QVariant>
#include <vector>

#include "chessboardwidget.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog {
  Q_OBJECT

 public:
  explicit SettingsDialog(QWidget *parent = nullptr);
  ~SettingsDialog();

 signals:
  void PaletteChanged(const ChessBoardWidget::ChessPalette &palette);

 private slots:
  void on_buttonBox_accepted();

 private:
  Ui::SettingsDialog *ui;

  std::vector<std::pair<QString, QVariant>> PALETTES = {
      {"Green", QVariant::fromValue(ChessBoardWidget::GREEN_PALETTE)},
      {"Blue", QVariant::fromValue(ChessBoardWidget::BLUE_PALETTE)},
      {"Yellow", QVariant::fromValue(ChessBoardWidget::YELLOW_PALETTE)},
      {"Red", QVariant::fromValue(ChessBoardWidget::RED_PALETTE)},
      {"Purple", QVariant::fromValue(ChessBoardWidget::PURPLE_PALETTE)},
  };
};

#endif  // _CHESS_INCLUDE_SETTINGS_DIALOG_H_
