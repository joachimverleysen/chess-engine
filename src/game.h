//  Student:
//  Rolnummer:
//  Opmerkingen: (bvb aanpassingen van de opgave)
//

#ifndef SCHAAK_GAME_H
#define SCHAAK_GAME_H

#include <utility>

#include "SchaakStuk.h"

class SchaakStuk;

struct CastlingRook {
    SchaakStuk* piece;
    pair<int, int> position;
    CastlingRook() {piece=nullptr; position = pair<int,int>(-1,-1);}
    CastlingRook(SchaakStuk* piece, pair<int, int> position) : piece(piece), position(std::move(position)){};
};
struct GameStack {
    vector<SchaakStuk*> moving_piece;
    vector<SchaakStuk*> captured_piece;
    vector<pair<int, int>> previous_position;
    vector<CastlingRook> castling_rook;


    void push(SchaakStuk* last, SchaakStuk* captured, pair<int, int> previous_pos) {
        moving_piece.push_back(last);
        captured_piece.push_back(captured);
        previous_position.push_back(previous_pos);
    }
    void push(SchaakStuk* last, SchaakStuk* captured, pair<int, int> previous_pos, CastlingRook& rook) {
        moving_piece.push_back(last);
        captured_piece.push_back(captured);
        previous_position.push_back(previous_pos);
        castling_rook.push_back(rook);
    }
    void pushCastlingRook(CastlingRook& rook) {
        castling_rook.push_back(rook);
    }
    void popCastlingRook() {
        castling_rook.pop_back();
    }


    void pop() {
        moving_piece.pop_back();
        captured_piece.pop_back();
        previous_position.pop_back();
    }

};

class Game {

public:
    SchaakStuk* schaakbord[8][8];
    Game();
    ~Game();

    int moveCount=0;
    bool isCastleMove_=false;
    CastlingRook castlingRook;
    bool whiteToMove() const;

    zw colorToMove() const;
    bool move(SchaakStuk* piece, int row, int col);

    pair<int,int> findKing(zw kleur) const;
    bool schaak(zw kleur) const;
    bool schaakmat(zw kleur);
    bool pat(zw kleur);
    void setStartBord();
    vector<pair<int,int>> controlledSquares(zw kleur) const;

    SchaakStuk* getPiece(int r, int k) const;
    void setPiece(int r, int k, SchaakStuk* s);
    vector<SchaakStuk*> getActivePieces() const;
    bool fakeMove(SchaakStuk *s, int r, int k);
    vector<pair<int, int>> kingControls(zw kleur) const;

    int firstWhiteKingMove=-1;
    int firstBlackKingMove=-1;

    GameStack undoStack;
    GameStack redoStack;

    vector<pair<SchaakStuk*, pair<int, int>>> castling_rook_stack;

    pair<int,int> enPassantSquare = pair<int,int>();
    pair<int,int> enPassantTargetPos = pair<int,int>(-1, -1);
    vector<pair<SchaakStuk*, pair<int, int>>> rd_castling_rook_stack;

    bool fakeMoveMade=false;

    SchaakStuk* tempCapturedPiece=nullptr;


    bool kingSideCastleIsValid(zw kleur) const;

    void executeCastle(zw kleur, pair<int, int> king_target_pos);

    bool queenSideCastleIsValid(zw kleur) const;

    vector<pair<int, int>> piecesInVision(zw kleur);

    void promote(int r, int k);

    SchaakStuk *getCastlingRook(pair<int, int> king_target_pos, SchaakStuk *king) const;

    void updateEnPassantTarget(pair<int, int> clickedPos, pair<int, int> myPosition, SchaakStuk *selected);

    void movePiece(SchaakStuk *piece, int row, int col);

    bool isEnPassantMove(SchaakStuk *piece, pair<int, int> target_position) const;

    static bool isCastleMove(SchaakStuk *piece, pair<int, int> target_position) ;

    void executeEnPassant(SchaakStuk *piece, int row, int col);
};

#endif //SCHAKEN_GAME_H