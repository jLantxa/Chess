#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>

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
    void on_sbDepth_valueChanged(int depth);
    void on_sbLines_valueChanged(int num_lines);
    void on_sbThreads_valueChanged(int arg1);

    void on_chInfinite_toggled(bool checked);

private:
    Ui::MainWindow *ui;

    UCIEngine m_engine;
    const char* DEFAULT_ENGINE_CMD = "stockfish";

    uint8_t m_depth;

    uint32_t m_move_number;
    bool m_white_moves;
    std::vector<UCIEngine::DepthInfo> m_depth_info;
    UCIEngine::BestMove m_best_move;
    QStringList m_moves_list;

    void RestartSearch();

    void UpdateLineInfo();

    void SetNumLines(uint8_t num_lines);
    void SetDepth(uint8_t depth);

};
#endif // MAINWINDOW_H
