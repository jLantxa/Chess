# Chess
A chess game and analysis tool built with Qt. It supports chess engines compatible with the UCI protocol, such as Stockfish.

**Currently implemented features:**
- Communication with Stockfish in multithread, multi PV mode.
- Loading a position for analysis using Forsyth–Edwards Notation.

# How to build
## Using Qt Creator
You can directly import the project into Qt Creator.

## Makefile
If you prefer not to use Qt Creator, you can use the ``Makefile``. Just make sure to point to your **qmake** binary using the ``QMAKE`` variable.
