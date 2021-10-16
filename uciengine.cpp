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

void UCIEngine::Write(const QString& str) {
    QString cmd = str + "\n";
    m_engine_process.write(cmd.toStdString().c_str());
}

void UCIEngine::NewGame() {
    Write("ucinewgame");
}

void UCIEngine::SetPosition(const QString &fen) {
    Write("position fen " + fen);
}

void UCIEngine::SetPositionFromMoves(const QStringList &moves) {
    const char* SEPARATOR = " ";
    Write("position startpos moves " + moves.join(SEPARATOR));
}

void UCIEngine::Stop() {
    Write("stop");
}

void UCIEngine::SetNumLines(uint8_t num_lines) {
    Write("setoption name MultiPV value " + QString::number(num_lines));
    qDebug() << "Set MultiPV to " << QString::number(num_lines);
}

void UCIEngine::SetNumThreads(uint16_t num_threads) {
    Write("setoption name Threads value " + QString::number(num_threads));
    qDebug() << "Set threads to " << QString::number(num_threads);
}

void UCIEngine::SearchWithDepth(uint8_t depth) {
    Write("go depth " + QString::number(depth));
}

void UCIEngine::SearchWithTime(uint32_t msec) {
    Write("go movetime" + QString::number(msec));
}

void UCIEngine::SearchInfinite() {
    Write("go infinite");
}

void UCIEngine::OnStart() {
    qInfo() << "Engine process started";
}

void UCIEngine::OnReadyReadStdout() {
    while (m_engine_process.canReadLine()) {
        const QString text = m_engine_process.readLine();
        ParseText(text);
    }
}

void UCIEngine::ParseText(const QString& text) {
    const char* SPLITTER = " ";
    QStringList args = text.trimmed().split(SPLITTER, QString::SplitBehavior::SkipEmptyParts);

    if (args.length() == 0) {
        return;
    }

    QString& type = args[0];
    if (type == "info") {
        ParseInfo(args);
    }
    else if (type == "bestmove") {
        ParseBestMove(args);
    }
}

void UCIEngine::ParseInfo(const QStringList& args) {
    if (args[1] == "depth") {
        /* Ignore currmove messages
         * Upperbound and lowerbound messages are for engine debug and we ignore those messages
         */
        const bool ignore_message =
                args.contains("currmove")   ||
                args.contains("upperbound") ||
                args.contains("lowerbound");

        if (ignore_message) {
            return;
        }

        ParseDepthInfo(args);
    }
}

void UCIEngine::ParseDepthInfo(const QStringList& args) {
    DepthInfo depth_info;
    depth_info.depth = args[2].toUInt();

    const auto line_id_ix = args.indexOf("multipv");
    depth_info.line_id = args[line_id_ix + 1].toUInt();

    const auto score_ix = args.indexOf("score");
    if (args[score_ix+1] == "cp") {
        depth_info.score = args[score_ix+2].toInt();
    }

    const auto pv_ix = args.indexOf("pv");
    for (int i = pv_ix+1; i < args.length(); ++i) {
        depth_info.pv.push_back(args[i]);
    }

    qDebug() << "Depth " << depth_info.depth <<
                ", line " << depth_info.line_id <<
                ", score " << depth_info.score <<
                ", pv " << depth_info.pv.join(" ");

    emit DepthInfoAvailable(depth_info);
}

void UCIEngine::ParseBestMove(const QStringList& args) {
    if (args.length() != 4) {
        return;
    }
    BestMove best_move = {args[1], args[3]};
    qDebug() << "Best move " << best_move.bestmove << ", ponder " << best_move.ponder;
    emit BestMoveAvailable(best_move);
}
