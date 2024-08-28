//  Student:
//  Rolnummer:
//  Opmerkingen: (bvb aanpassingen van de opgave)
//

#include "game.h"
#include <algorithm>
#include <iostream>
#include <random>     // voor std::default_random_engine en std::random_device

#include "SchaakStuk.h"
using namespace std;

Game::Game() {

}


Game::~Game() {}

// Zet het bord klaar; voeg de stukken op de jusite plaats toe
void Game::setStartBord() {
    // Initialize board matrix with nullpointers
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            schaakbord[i][j] = nullptr;
        }
    }

    // Note: [0][0] is the top left square

    // Pionnen
    for (int i = 0; i < 8; i++) {

        schaakbord[1][i] = new Pion(zwart, pair<int,int>(1,i));
        schaakbord[6][i] = new Pion(wit, pair<int,int>(6,i));
    }
// Torens
    schaakbord[0][0] = new Toren(zwart, pair<int,int>(0,0));
    schaakbord[0][7] = new Toren(zwart, pair<int,int>(0,7));
    schaakbord[7][0] = new Toren(wit, pair<int,int>(7,0));
    schaakbord[7][7] = new Toren(wit, pair<int,int>(7,7));

// Paarden
    schaakbord[0][1] = new Paard(zwart, pair<int,int>(0,1));
    schaakbord[0][6] = new Paard(zwart, pair<int,int>(0,6));
    schaakbord[7][1] = new Paard(wit, pair<int,int>(7,1));
    schaakbord[7][6] = new Paard(wit, pair<int,int>(7,6));

// Lopers
    schaakbord[0][2] = new Loper(zwart, pair<int,int>(0,2));
    schaakbord[0][5] = new Loper(zwart, pair<int,int>(0,5));
    schaakbord[7][2] = new Loper(wit, pair<int,int>(7,2));
    schaakbord[7][5] = new Loper(wit, pair<int,int>(7,5));

// Koninginnen
    schaakbord[0][3] = new Koningin(zwart, pair<int,int>(0,3));
    schaakbord[7][3] = new Koningin(wit, pair<int,int>(7,3));

// Koningen
    schaakbord[0][4] = new Koning(zwart, pair<int,int>(0,4));
    schaakbord[7][4] = new Koning(wit, pair<int,int>(7,4));
    // Initialise piece positions


}

SchaakStuk* Game::getCastlingRook(pair<int, int> king_target_pos, SchaakStuk* king) {
    auto kingPos = king->getPos();
    bool kingside = true;
    if (king_target_pos.second < kingPos.second)  kingside=false;

    SchaakStuk* rook = nullptr;
    for (auto p : getActivePieces()) {
        pair<int,int> q = p->getPos();
        if (p->getKleur()!=king->getKleur() || p->getNaam()!=toren) continue;
        if (kingside && q.second<kingPos.second) continue;
        if (!kingside && q.second>kingPos.second) continue;
        rook = p;
    }
    return rook;
}

// Verplaats stuk s naar positie (r,k)
// Als deze move niet mogelijk is, wordt false teruggegeven
// en verandert er niets aan het schaakbord.
// Anders wordt de move uitgevoerd en wordt true teruggegeven

bool Game::move(SchaakStuk* s, int r, int k) {
    auto myPos = s->getPos();
    pair<int, int> targetPos(r, k);

    bool enpassant = true;
    if (s->getNaam()!=pion) enpassant = false;
    if (enPassantSquare.first == -1) enpassant = false;
    if (myPos.second==k) enpassant = false;
    if (getPiece(r, k)!=nullptr) enpassant = false;
    if (targetPos != enPassantSquare) enpassant = false;
    if (enPassantTargetPos.first == -1) enpassant = false;
    if (enpassant) {
        setPiece(targetPos.first, targetPos.second, s);
        s->setPos(targetPos);
        setPiece(myPos.first, myPos.second, nullptr);
        setPiece(enPassantTargetPos.first, enPassantTargetPos.second, nullptr);

        enPassantTargetPos = pair<int,int>(-1, -1);
        enPassantSquare = pair<int,int>(-1,-1);

        pair<SchaakStuk*, pair<int, int>> castling_rook_piece(nullptr, pair<int, int>(-1, -1));
        return true;
    }

    isCastleMove=false;
    if (s->getNaam()==koning &&
    abs(myPos.second-k)>1) {
        auto castling_rook_piece = getCastlingRook(targetPos, s);
        pair<SchaakStuk*, pair<int, int>> castling_rook_pair(castling_rook_piece,
                                                             castling_rook_piece->getPos());
        if (k>myPos.second && kingSideCastleIsValid(s->getKleur()) ||
                k<myPos.second && queenSideCastleIsValid(s->getKleur())) {
            castlingRook.piece = castling_rook_piece;
            castlingRook.position = castling_rook_piece->getPos();
            executeCastle(s->getKleur(), targetPos);
            isCastleMove=true;

        }

        return true;
    }
    pair<SchaakStuk*, pair<int, int>> castling_rook(nullptr,pair<int, int>(-1, -1));

    if (r < 0 || r > 7 || k < 0 || k > 7 ) {
        return false;
    }
    vector<pair<int, int>> zetten = s->mogelijke_zetten(*this);
    auto it = find(zetten.begin(), zetten.end(), targetPos);

    if (it != zetten.end()) {

        setPiece(r,k,s);


        setPiece(myPos.first, myPos.second, nullptr);

        s->setPos(targetPos);


        return true;
    }
    return false;
}


void Game::updateEnPassantTarget(pair<int, int> clickedPos, pair<int, int> myPosition, SchaakStuk* selected,
                                 pair<int, int> selectionPos) {
    zw oppkleur = wit; if (selected->getKleur()==wit) oppkleur = zwart;
    vector<pair<int,int>> threateneds = piecesInVision(oppkleur);
    if (selected->getNaam()==pion &&
        abs(clickedPos.first - myPosition.first)==2) {
        if (whiteToMove()) enPassantSquare = pair<int,int>(clickedPos.first - 1, clickedPos.second);
        else if (!whiteToMove()) enPassantSquare = pair<int,int>(clickedPos.first + 1, clickedPos.second);
        enPassantTargetPos=pair<int,int>(selected->getPos().first, selected->getPos().second);
        SchaakStuk* epTargetPiece = getPiece(enPassantTargetPos.first, enPassantTargetPos.second);


        SchaakStuk* leftSquare = getPiece(enPassantTargetPos.first, enPassantTargetPos.second - 1);    // square to the left of enPassantTargetPos
        SchaakStuk* rightSquare = getPiece(enPassantTargetPos.first, enPassantTargetPos.second + 1);    // square to the right of enPassantTargetPos

        if (leftSquare != nullptr &&
            leftSquare->getNaam() == pion &&
            leftSquare->getKleur() != epTargetPiece->getKleur()) {
            threateneds.push_back(enPassantTargetPos);}
        if (rightSquare != nullptr &&
            rightSquare->getNaam() == pion &&
            rightSquare->getKleur() != epTargetPiece->getKleur()) {
            threateneds.push_back(enPassantTargetPos);}


    }
    else {
        enPassantSquare = pair<int, int>(-1, -1); // reset
    }
}

//todo: undo castle + en passant

void Game::executeCastle(zw kleur, pair<int, int> pos) {
    if (pos.first < 0 || pos.first > 7 || pos.second < 0 || pos.second > 7 ) {
        return;
    }

    auto kingPos = findKing(kleur);
    bool kingside = true;
    if (pos.second < kingPos.second)  kingside=false;

    SchaakStuk* king = getPiece(kingPos.first, kingPos.second);
    vector<pair<int, int>> zetten = king->validMoves(*this);
    auto it = find(zetten.begin(), zetten.end(), pos);

    if (it != zetten.end()) {
        setPiece(pos.first,pos.second,king);

        setPiece(kingPos.first, kingPos.second, nullptr);

        king->setPos(pos);

    }

    SchaakStuk* rook = nullptr;
    for (auto p : getActivePieces()) {
        pair<int,int> q = p->getPos();
        if (p->getKleur()!=kleur || p->getNaam()!=toren) continue;
        if (kingside && q.second<kingPos.second) continue;
        if (!kingside && q.second>kingPos.second) continue;
        rook = p;
    }
    pair<int,int> rookPos = rook->getPos();
    castling_rook = {rook, rookPos};

    pair<int,int> newPos(-1,-1);
    if (kingside) {
        newPos= pair<int,int>(rookPos.first,rookPos.second-2);
        setPiece(rookPos.first,rookPos.second-2,rook);
    }
    else if (!kingside) {
        newPos= pair<int,int>(rookPos.first,rookPos.second+3);
        setPiece(rookPos.first,rookPos.second+3,rook);
    }


    setPiece(rookPos.first, rookPos.second, nullptr);


    if (newPos.first != -1) rook->setPos(newPos);

}


bool Game::fakeMove(SchaakStuk* s, int r, int k) {
        if (s == nullptr) {
            return false;
        }

        if (r < 0 || r > 7 || k < 0 || k > 7 ) {
            return false;
        }

        auto myPos = s->getPos();
        pair<int, int> targetPos(r, k);

        if (getPiece(r,k)!=nullptr) tempCapturedPiece= getPiece(r, k);
        setPiece(r,k,s);
        setPiece(myPos.first, myPos.second, nullptr);


        if (tempCapturedPiece != nullptr && fakeMoveMade) setPiece(myPos.first, myPos.second, tempCapturedPiece);
        s->setPos(targetPos);

        return true;

}


bool Game::schaak(zw kleur) {
    pair<int, int> kingPos = findKing(kleur);

    for (auto piece : getActivePieces()) {
        if (piece == nullptr || piece->getKleur() == kleur) continue;

        vector<pair<int, int>> attackingSquares = piece->attackingSquares(*this);
        if (find(attackingSquares.begin(), attackingSquares.end(), kingPos) != attackingSquares.end()) {
            return true;
        }
    }

    return false;
}
pair<int, int> Game::findKing(zw kleur) const {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            SchaakStuk* piece = getPiece(i, j);
            if (piece != nullptr && piece->getNaam() == koning && piece->getKleur() == kleur) {
                pair<int,int> result(i, j);
                return result;
            }
        }
    }

    pair<int,int> p(-1, -1);
    return p;
}

bool Game::schaakmat(zw kleur) {
    vector<pair<int, int>> legalMoves;
    if (!schaak(kleur)) return false;

    for (auto p: getActivePieces()) {
        if (p->getKleur() != kleur) continue;
        vector<pair<int,int>> g = p->validMoves(*this);
        legalMoves.insert(legalMoves.end(), g.begin(), g.end());
    }
    if (!legalMoves.empty()) return false;
    return true;
}


bool Game::pat(zw kleur) {
    vector<pair<int, int>> legalMoves;
    if (schaak(kleur)) return false;


    for (auto p: getActivePieces()) {
        if (p->getKleur() != kleur) continue;
        vector<pair<int,int>> g = p->validMoves(*this);
        legalMoves.insert(legalMoves.end(), g.begin(), g.end());
    }
    if (!legalMoves.empty()) return false;
    return true;
}


SchaakStuk* Game::getPiece(int r, int k) const {
    SchaakStuk* p = schaakbord[r][k];
    return p;
}


void Game::setPiece(int r, int k, SchaakStuk* s)
{
    schaakbord[r][k] = s;
}

vector<SchaakStuk *> Game::getActivePieces() const {
    vector<SchaakStuk*> result;
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            if (getPiece(i, j) == nullptr) continue;
            result.push_back(getPiece(i, j));

        }
    }
    return result;
};

bool Game::kingSideCastleIsValid(zw kleur) {
    if (kleur==zwart && firstBlackKingMove!=-1) return false;
    if (kleur==wit && firstWhiteKingMove!=-1) return false;
    if (schaak(kleur)) return false;
    pair<int,int> kingPos = findKing(kleur);

    for (int i=1; i<=2; i++) {
        pair<int,int> square(kingPos.first, kingPos.second+i);
        if (getPiece(square.first, square.second)!=nullptr) return false;


        zw k = wit;
        if (kleur==wit) k = zwart;
        vector<pair<int,int>> v = controlledSquares(k);
        auto it = find(v.begin(), v.end(), square);
        if (it != v.end()) return false;
    }
    return true;
}

void Game::promote(int r, int k) {
    zw color = getPiece(r, k)->getKleur();
    pair<int,int> newPos(r, k);
    Koningin* q = new Koningin(color, newPos);
    setPiece(r, k, q);
}


bool Game::queenSideCastleIsValid(zw kleur) {


    if (kleur==zwart && blackKingMoved) return false;
    if (kleur==wit && whiteKingMoved) return false;
    if (schaak(kleur)) return false;
    pair<int,int> kingPos = findKing(kleur);

    for (int i=1; i<=3; i++) {
        pair<int,int> square(kingPos.first, kingPos.second-i);
        if (getPiece(square.first, square.second)!=nullptr) return false;


        zw enemyColor = wit;
        if (kleur==wit) enemyColor = zwart;
        vector<pair<int,int>> controlled = controlledSquares(enemyColor);
        auto it = find(controlled.begin(), controlled.end(), square);
        if (it != controlled.end()) return false;
    }
    return true;
}

vector<pair<int,int>> Game::kingControls(zw kleur) const {
    vector<pair<int, int>> result;
    bool kingFound=false;
    pair<int,int> kingPos(-1,-1);
    for (auto p : getActivePieces()) {
        if (p->getKleur()!=kleur || p->getNaam()!=koning) continue;
        kingPos = p->getPos();
        kingFound=true;
    }
    if (!kingFound) return result;


    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i != 0 || j != 0) {
                pair<int,int> p(kingPos.first+i,kingPos.second+j);
                result.push_back(p);
            }
        }
    }
    return result;

}
vector<pair<int, int>> Game::controlledSquares(zw kleur) {
    vector<pair<int,int>> result;
    vector<SchaakStuk*> activePieces = getActivePieces();

    if (activePieces.empty()) {
        return result;
    }
    Game g = *this;
    for (auto p : getActivePieces()) {
        if (p->getKleur()!=kleur) continue;
        if (p->getNaam()==pion && p->getKleur()==zwart) {
            pair<int,int> pawnPos = p->getPos();
            pair<int,int> diagL(pawnPos.first + 1, pawnPos.second + 1);
            pair<int,int> diagR(pawnPos.first + 1, pawnPos.second - 1);
            result.push_back(diagL); result.push_back(diagR);
            if (!p->testSelfCheck(g, diagL.first, diagL.second)) result.push_back(diagL);
            if (!p->testSelfCheck(g, diagR.first, diagR.second)) result.push_back(diagR);

        }
        else if (p->getNaam()==pion && p->getKleur()==wit) {
            pair<int,int> pawnPos = p->getPos();
            pair<int,int> diagL(pawnPos.first - 1, pawnPos.second + 1);
            pair<int,int> diagR(pawnPos.first - 1, pawnPos.second - 1);
            result.push_back(diagL); result.push_back(diagR);
            if (!p->testSelfCheck(g, diagL.first, diagL.second)) result.push_back(diagL);
            if (!p->testSelfCheck(g, diagR.first, diagR.second)) result.push_back(diagR);

        }
        else if (p->getNaam()==koning) {
            vector<pair<int,int>> v = kingControls(kleur);
            result.insert(result.end(), v.begin(), v.end());
        } else {
            vector<pair<int,int>> v = p->validMoves(*this);
            result.insert(result.end(), v.begin(), v.end());
       }
    }
    return result;
}

vector<pair<int, int>> Game::piecesInVision(zw kleur) {
    vector<pair<int,int>> result;

    for (auto piece : getActivePieces()) {
        if (piece->getKleur()!=kleur) continue;
        for (auto pos : piece->validMoves(*this)) {
            if (getPiece(pos.first, pos.second) == nullptr) continue;
            result.push_back(pos);
        }
    }
    return result;
}

zw Game::colorToMove() const {
    if (whiteToMove()) return wit;
    return zwart;
}

bool Game::whiteToMove() const {
    return (moveCount%2==0);
}


