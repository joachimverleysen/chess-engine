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

public:
    SchaakStuk* schaakbord[8][8];
    Game();
    ~Game();

    bool playAgainstAI = false;

    int moveCount=0;
    bool whiteToMove() const;

    zw colorToMove() const;
    bool move(SchaakStuk* s,int r, int k);

    pair<int,int> findKing(zw kleur) const;
    bool schaak(zw kleur);
    bool schaakmat(zw kleur);
    bool pat(zw kleur);
    void setStartBord();
    vector<pair<int,int>> controlledSquares(zw kleur);

    SchaakStuk* getPiece(int r, int k) const;
    void setPiece(int r, int k, SchaakStuk* s);
    vector<SchaakStuk*> getActivePieces() const;
    bool fakeMove(SchaakStuk *s, int r, int k);
    vector<pair<int, int>> kingControls(zw kleur) const;
    bool whiteKingMoved=false;
    bool blackKingMoved=false;

    pair<SchaakStuk*, pair<int, int>> castling_rook;


    GameStack undoStack;
    GameStack redoStack;

    vector<pair<SchaakStuk*, pair<int, int>>> castling_rook_stack;

    pair<int,int> enPassantSquare = pair<int,int>(-1,-1);
    pair<int,int> enPassantTargetPos = pair<int,int>(-1, -1);
    vector<pair<SchaakStuk*, pair<int, int>>> rd_castling_rook_stack;

    bool fakeMoveMade=false;

    SchaakStuk* tempCapturedPiece=nullptr;

    bool aiFakeMoveMade=false;
    SchaakStuk* tempPiece_2 = nullptr;


    bool kingSideCastleIsValid(zw kleur);

    void executeCastle(zw kleur, pair<int, int> pos);

    bool queenSideCastleIsValid(zw kleur);

    vector<pair<int, int>> piecesInVision(zw kleur);

    void promote(int r, int k);

    SchaakStuk *getCastlingRook(pair<int, int> king_target_pos, SchaakStuk *king);

    void updateEnPassantTarget(pair<int, int> clickedPos, pair<int, int> myPosition, SchaakStuk *selected,
                               pair<int, int> selectionPos);
};

#endif //SCHAKEN_GAME_H


