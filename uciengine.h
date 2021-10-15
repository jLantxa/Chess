#ifndef UCIENGINE_H
#define UCIENGINE_H

#include <QDebug>
#include <QProcess>
#include <QString>

class UCIEngine : public QObject {
    Q_OBJECT;

public:
    UCIEngine();

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

    /**
     * @brief Set number of lines
     * @param num_lines Number of lines (principal variations) to return
     */
    void SetNumLines(uint8_t num_lines);

    void GoDepth(uint8_t depth);
    void GoInfinite();

    /**
     * @brief SetNumThreads
     * @param num_threads Number of CPU threads to use
     */
    void SetNumThreads(uint16_t num_threads);

public slots:
    void OnStart();
    void OnReadyReadStdout();

private:
    QProcess m_engine_process;

    void ConnectProcessSignals();
};

#endif // UCIENGINE_H
