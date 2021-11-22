# Chess
A chess game and analysis tool built with Qt 6. It supports chess engines compatible with the UCI protocol, such as Stockfish.

**Currently implemented:**
- Communication with Stockfish in multithread, multi PV mode.
  - Can configure number of lines, depth and number of CPU threads.
- Can load a position for analysis using Forsyth–Edwards Notation.
- Board widget.
  - Move generation is in progress; (missing castles, en passant and promotion).
  - Evaluation bar.
- Colour themes.

**To-Do:**
- Complete move generation.
- Play against engine.
- Undo moves.
- Move tree (main line and variations).
- Add sound effects.
- Load and save games in PGN format.
- Network play.


# How to build
## Dependencies
* Qt 6.2.1

## Using Qt Creator
You can directly import the project into Qt Creator.

## Makefile
If you prefer not to use Qt Creator, you can use the ``Makefile``. Just make sure to point to your **qmake** binary using the ``QMAKE`` variable:
```export QMAKE=<path to qmake>```

# Integration with chess engines
All communication with the chess engine occurrs via the ``QProcess`` class. ``QProcess`` provides a duplex communication channel with a child process using standard input/output. The UCI (Universal Chess Interface) establishes the commands and syntax to communicate with a chess engine. At the moment, this app only uses ``Stockfish``, as the process command is hardcoded. In the future it should be trivial to allow the user to specify path to any chess engine program, provided that this engine is compatible with the UCI protocol.
