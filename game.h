//  Student:
//  Rolnummer:
//  Opmerkingen: (bvb aanpassingen van de opgave)
//

#ifndef SCHAAK_GAME_H
#define SCHAAK_GAME_H

#include "SchaakStuk.h"

class SchaakStuk;

struct GameStack {
    vector<SchaakStuk*> last_piece;
    vector<SchaakStuk*> captured_piece;
    vector<pair<int, int>> previous_position;

    void push(SchaakStuk* last, SchaakStuk* captured, pair<int, int> previous_pos) {
        last_piece.push_back(last);
        captured_piece.push_back(captured);
        previous_position.push_back(previous_pos);
    }

    void pop() {
        last_piece.pop_back();
        captured_piece.pop_back();
        previous_position.pop_back();
    }

    void clear() {
        last_piece.clear();
        captured_piece.clear();
        previous_position.clear();
    }
};

class Game {
// variabelen om de status van het spel/bord te bewaren

public:
    SchaakStuk* schaakbord[8][8];
    Game();
    ~Game();

    bool playAgainstAI = false;

    int moveCount=0;
    bool whiteToMove() const;     // true for White, false for Black

    zw colorToMove() const;
    bool move(SchaakStuk* s,int r, int k); // Verplaats stuk s naar rij r en kolom k

    pair<int,int> findKing(zw kleur) const;
    bool schaak(zw kleur);
    bool schaakmat(zw kleur);
    bool pat(zw kleur);
    void setStartBord();
    vector<pair<int,int>> controlledSquares(zw kleur); // returns vector with positions that 'kleur' controls

    SchaakStuk* getPiece(int r, int k) const;
    void setPiece(int r, int k, SchaakStuk* s);
    vector<SchaakStuk*> getActivePieces() const;
    bool fakeMove(SchaakStuk *s, int r, int k);
    vector<pair<int, int>> kingControls(zw kleur) const;  // squares that the king controls
    bool whiteKingMoved=false;   // white king has moved?
    bool blackKingMoved=false;   // black king has moved?

    pair<SchaakStuk*, pair<int, int>> castling_rook;        // + the original posistion of the rook

    // AI specific vars:
    SchaakStuk* aiSelection = nullptr;
    pair<int,int> aiTargetPos = pair<int,int>(-1,-1);

    GameStack undoStack;
    GameStack redoStack;

    vector<pair<SchaakStuk*, pair<int, int>>> castling_rook_stack;  // rook+original_pos, ELSE nullptr + (-1, -1)

    pair<int,int> enPassantSquare = pair<int,int>(-1,-1);   // initialise with invalid pos
    pair<int,int> enPassantTargetPos = pair<int,int>(-1, -1); // The pawn that is a target for en passant
    vector<pair<SchaakStuk*, pair<int, int>>> rd_castling_rook_stack;  // rook+original_pos, ELSE nullptr + (-1, -1)


    // squares that a pawn controls


    bool fakeMoveMade=false; // Indicates if a fake move has been made. This means the fake move must be made undone!
    // the reason for the var above is for the fake move (undo?) to work properly -> see fakeMove()
    // purpose of fake moves: to test if the move results in a self-check (illegal)

    SchaakStuk* tempPiece=nullptr;
    // purpose of this var: make sure that a fake move doesn't actually capture pieces.
    // the tempPiece is the piece that is captured within a fake move, and will be restored in the 'undo'

    bool aiFakeMoveMade=false;
    SchaakStuk* tempPiece_2 = nullptr;  // temp piece used in the aiFakeMoveMade
    // We use a separate var because of overwriting issues

    // Hier zet jij jouw datastructuur neer om het bord te bewaren ...

    bool kCastleValid(zw kleur);

    void executeCastle(zw kleur, pair<int, int> pos);

    bool qCastleValid(zw kleur);

    vector<pair<int, int>> piecesInVision(zw kleur);

    void executeEP(zw kleur, pair<int, int> pos);

    void promote(int r, int k);

    void aiMoves();

    void aiChoses();

    bool aiFakeMove(SchaakStuk *s, int r, int k);

    SchaakStuk *getCastlingRook(pair<int, int> king_target_pos, SchaakStuk *king);

    void updateEnPassantTarget(pair<int, int> clickedPos, pair<int, int> myPosition, SchaakStuk *selected,
                               pair<int, int> selectionPos);
};

#endif //SCHAKEN_GAME_H


