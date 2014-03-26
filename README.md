Third Year Chess Project
========================

Chess AI created for my third year final project.

Dependencies
------------

* [GTKMM](http://www.gtkmm.org/) 2.4+
* [Boost](http://www.boost.org/)

How it works
------------

This program uses minimax search with alpha-beta pruning to traverse all possible moves up to a certain depth (e.g. thinking 3 moves ahead). Once this depth has been reached, a number of heuristics are used to evaluate the position:

* __Material worth__ - Number of pieces left on either side. A queen is worth more than a rook, which is worth more than a pawn, etc.
* __Position bonuses__ - Each type of piece gets bonus evaluation points based on their position on the board. For example, bishops receive a bonus for being in the centre of the board, where they have more influence over the board.
* __Pawn structure__ - Players' evaluations are penalised by having [passed, isolated, doubled and backward pawns](http://en.wikipedia.org/wiki/Outline_of_chess#Pawn_structure).
* __Central threat__ - Evaluation bonuses given for being able to attack the centre of the board. This dissuades the other player from gaining control of the important centre.
