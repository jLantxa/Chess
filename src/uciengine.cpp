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

#include <array>
#include <functional>

#include "uciengine.hpp"

static const char* SEPARATOR = " ";

UCIEngine::UCIEngine() {
    ConnectProcessSignals();
}

void UCIEngine::ConnectProcessSignals() {
    connect(&m_engine_process, &QProcess::readyReadStandardOutput,
            this, &UCIEngine::OnReadyReadStdout);
    connect(&m_engine_process, &QProcess::started,
            this, &UCIEngine::OnStart);
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
    Write("go movetime " + QString::number(msec));
}

void UCIEngine::SearchInfinite() {
    Write("go infinite");
}

void UCIEngine::OnStart() {
    qInfo() << "Engine process started";
}

void UCIEngine::OnReadyReadStdout() {
    while (m_engine_process.canReadLine()) {
        const QString line = m_engine_process.readLine();
        ParseText(line.trimmed());
    }
}

void UCIEngine::ParseText(const QString& text) {
    static const std::array<std::function<bool(const QStringList&)>, 2> PARSE_FUNCTIONS = {
        std::bind(&UCIEngine::ParseInfo, this, std::placeholders::_1),
        std::bind(&UCIEngine::ParseBestMove, this, std::placeholders::_1),
    };

    if (text.isEmpty()) {
        return;
    }

    const QStringList args = text.split(SEPARATOR);
    if (args.length() == 0) {
        return;
    }

    for (auto parse_function : PARSE_FUNCTIONS) {
        if (parse_function(args) == true) {
            break;
        }
    }
}

bool UCIEngine::ParseInfo(const QStringList& args) {
    if (args[0] != "info") {
        return false;
    }

    /* Ignore currmove messages.
     * Upperbound and lowerbound messages are for engine debug and we ignore
     * those messages.
     */
    const bool ignore_message = args.contains("currmove")   ||
                                args.contains("upperbound") ||
                                args.contains("lowerbound");
    if (ignore_message) {
        return false;
    }

    if (args[1] != "depth") {
        return false;
    }

    const DepthInfo info = ParseDepthInfo(args);
    emit DepthInfoAvailable(info);

    return true;
}

UCIEngine::DepthInfo UCIEngine::ParseDepthInfo(const QStringList& args) {
    DepthInfo depth_info;

    depth_info.depth = args[2].toUInt();

    if (args.contains("multipv")) {
        const auto line_id_ix = args.indexOf("multipv");
        depth_info.line_id = args[line_id_ix + 1].toUInt();
    } else {
        depth_info.line_id = 1;
    }

    const auto score_ix = args.indexOf("score");
    depth_info.mate_counter = (args[score_ix+1] == "mate");
    depth_info.score = args[score_ix + 2].toInt();

    if (args.contains("pv")) {
        const auto pv_ix = args.indexOf("pv");
        for (int i = pv_ix+1; i < args.length(); ++i) {
            depth_info.pv.push_back(args[i]);
        }
    }

    return depth_info;
}

bool UCIEngine::ParseBestMove(const QStringList& args) {
    if (args[0] != "bestmove") {
        return false;
    }

    BestMove best_move;
    best_move.bestmove = args[1];
    if (args.contains("ponder")) {
        const auto ponder_ix = args.indexOf("ponder") + 1;
        best_move.ponder = args[ponder_ix];
    }

    emit BestMoveAvailable(best_move);
    return true;
}
