#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <vector>

#include <QDialog>
#include <QVariant>

#include "chessboardwidget.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

signals:
    void PaletteChanged(const ChessBoardWidget::ChessPalette& palette);

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

#endif // SETTINGSDIALOG_H
