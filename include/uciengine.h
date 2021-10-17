#ifndef UCIENGINE_H
#define UCIENGINE_H

#include <QDebug>
#include <QProcess>
#include <QString>

class UCIEngine : public QObject {
    Q_OBJECT;

public:
    UCIEngine();

    struct BestMove {
        QString bestmove;
        QString ponder;
    };

    struct DepthInfo {
        uint8_t line_id;
        uint8_t depth;
        QStringList pv;
        bool mate_counter = false;
        int score;
    };

    /** Start engine process */
    void Init(const QString& command);

    /** Close engine process */
    void Close();

    /** Reset engine process */
    void Reset();

    /**
     * @brief Send stop command.
     */
    void Stop();

    /**
     * @brief Write a raw command to the engine
     * @param str QString without \n ending
     */
    void Write(const QString& str);

    void NewGame();
    void SetPosition(const QString& fen);
    void SetPositionFromMoves(const QStringList& moves);

    /**
     * @brief Set number of lines
     * @param num_lines Number of lines (principal variations) to return
     */
    void SetNumLines(uint8_t num_lines);

    /**
     * @brief Search using a maximum depth
     * @param depth Maximum depth
     */
    void SearchWithDepth(uint8_t depth);

    /**
     * @brief Search using a timeout
     * @param msec Maximum search time in milliseconds
     */
    void SearchWithTime(uint32_t msec);

    /**
     * @brief Search forever. The search can be stopped at any time by
     * sending a stop message.
     */
    void SearchInfinite();

    /**
     * @brief SetNumThreads
     * @param num_threads Number of CPU threads to use
     */
    void SetNumThreads(uint16_t num_threads);

public slots:
    /**
     * @brief The engine process has started
     */
    void OnStart();

    /**
     * @brief There is data available in the engine's standard outptu
     */
    void OnReadyReadStdout();

signals:
    void BestMoveAvailable(BestMove best_move);
    void DepthInfoAvailable(DepthInfo depth_info);

private:
    QProcess m_engine_process;

    void ConnectProcessSignals();

    void ParseText(const QString &text);
    void ParseInfo(const QStringList& args);
    void ParseDepthInfo(const QStringList& args);
    void ParseBestMove(const QStringList& args);
};

#endif // UCIENGINE_H
