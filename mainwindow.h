#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

#include <QLabel>
#include <QMainWindow>

#include "uciengine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void OnBestMoveAvailable(UCIEngine::BestMove best_move);
    void OnDepthInfoAvailable(UCIEngine::DepthInfo depth_info);

private slots:
    void on_bEngineOn_toggled(bool checked);
    void on_chInfinite_toggled(bool checked);
    void on_sbThreads_editingFinished();
    void on_sbLines_editingFinished();
    void on_sbDepth_editingFinished();

    void on_bSetPosition_clicked();

private:
    Ui::MainWindow *ui;

    const char* WINDOW_TITLE = "Chess";

    UCIEngine m_engine;
    const char* DEFAULT_ENGINE_CMD = "./stockfish";

    uint8_t m_depth;

    uint32_t m_move_number;
    bool m_white_moves;
    std::vector<UCIEngine::DepthInfo> m_depth_info;
    UCIEngine::BestMove m_best_move;
    QStringList m_moves_list;

    void RestartSearch();

    void UpdateLineInfo();
    void UpdateMoveList();

    void SetNumLines(uint8_t num_lines);
    void SetDepth(uint8_t depth);

    QString GetSignedScore(int cp_score) const {
        QString sign = (cp_score > 0)? "+" : "";
        return sign + QString::number(static_cast<float>(cp_score) / 100, 'f', 2);
    }

};
#endif // MAINWINDOW_H
