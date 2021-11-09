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

#include "settingsdialog.h"

#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::SettingsDialog) {
  ui->setupUi(this);

  for (auto& item : PALETTES) {
    ui->cbPalette->addItem(item.first, item.second);
  }
}

SettingsDialog::~SettingsDialog() { delete ui; }

void SettingsDialog::on_buttonBox_accepted() {
  ChessBoardWidget::ChessPalette palette =
      ui->cbPalette->currentData().value<ChessBoardWidget::ChessPalette>();
  emit PaletteChanged(palette);
}
