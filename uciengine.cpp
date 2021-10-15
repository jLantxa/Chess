#include "uciengine.h"

UCIEngine::UCIEngine() {
    ConnectProcessSignals();
}

void UCIEngine::ConnectProcessSignals() {
    connect(&m_engine_process, SIGNAL(readyReadStandardOutput()), this, SLOT(OnReadyReadStdout()));
    connect(&m_engine_process, SIGNAL(started()), this, SLOT(OnStart()));
}

void UCIEngine::Init(const QString& command) {
    m_engine_process.setProgram(command);
    m_engine_process.start();
}

void UCIEngine::Close() {
    m_engine_process.close();
}

void UCIEngine::Reset() {
    m_engine_process.reset();
}


void UCIEngine::Stop() {
    Write("stop");
}

void UCIEngine::SetNumLines(uint8_t num_lines) {
    Write("setoption name MultiPV value " + QString::number(num_lines));
}

void UCIEngine::SetNumThreads(uint16_t num_threads) {
    Write("setoption name Threads value " + QString::number(num_threads));
}

void UCIEngine::GoDepth(uint8_t depth) {
    Write("go depth " + QString::number(depth));
}

void UCIEngine::GoInfinite() {
    Write("go infinite");
}

void UCIEngine::OnStart() {
    qInfo() << "Engine process started";
}

void UCIEngine::OnReadyReadStdout() {
    qDebug() << m_engine_process.readAllStandardOutput();
}

void UCIEngine::Write(const QString& str) {
    QString cmd = str + "\n";
    m_engine_process.write(cmd.toStdString().c_str());
}
