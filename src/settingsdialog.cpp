#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    for (auto& item : PALETTES) {
        ui->cbPalette->addItem(item.first, item.second);
    }
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_accepted() {
    ChessBoardWidget::ChessPalette palette = ui->cbPalette->currentData().value<ChessBoardWidget::ChessPalette>();
    emit PaletteChanged(palette);
}

