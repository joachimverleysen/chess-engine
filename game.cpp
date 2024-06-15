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
    bool kingside = true;   // is it a kingside castle? (if not -> queenside)
    if (king_target_pos.second < kingPos.second)  kingside=false;
    // note: castling should already be valid, so we are not checking again

    // move rook:
    SchaakStuk* rook = nullptr; // initialise local var: = the castling rook
    for (auto p : getActivePieces()) {
        pair<int,int> q = p->getPos();
        if (p->getKleur()!=king->getKleur() || p->getNaam()!=toren) continue;
        // if it is a kingside castle, we are looking for the rook to the right of the king and vice versa
        if (kingside && q.second<kingPos.second) continue; // wrong rook
        if (!kingside && q.second>kingPos.second) continue; // wrong rook
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

    // check if the move is En Passant
    bool ep = true;
    if (s->getNaam()!=pion) ep = false;
    if (enPassantSquare.first == -1) ep = false;    // ep not possible since no available ep square
    if (myPos.second==k) ep = false;    // ep is a diagonal move so column indices must be different
    if (getPiece(r, k)!=nullptr) ep = false; // EP means the pawn should go to an empty square
    if (targetPos != enPassantSquare) ep = false;
    if (enPassantTargetPos.first == -1) ep = false;   // invalid enPassantTargetPos (-1 = default value)
    if (ep) {
        setPiece(targetPos.first, targetPos.second, s); // move the pawn to epSquare
        s->setPos(targetPos);   // update pawn's position
        setPiece(myPos.first, myPos.second, nullptr);   // clear original square
        setPiece(enPassantTargetPos.first, enPassantTargetPos.second, nullptr); // Remove captured enPassantTargetPos pawn

        // reset en passant vars
        enPassantTargetPos = pair<int,int>(-1, -1);    // default value
        enPassantSquare = pair<int,int>(-1,-1);

        pair<SchaakStuk*, pair<int, int>> castling_rook(nullptr, pair<int, int>(-1, -1));
        castling_rook_stack.push_back(castling_rook);
        return true;
    }
    // check if the move is castling:
    if (s->getNaam()==koning &&
    abs(myPos.second-k)>1) {
        auto castling_rook = getCastlingRook(targetPos, s);
        pair<SchaakStuk*, pair<int, int>> castling_rook_pair(castling_rook,
                                                        castling_rook->getPos());
        castling_rook_stack.push_back(castling_rook_pair);// check if distance between king and selectedPos is more then 1 ->castle
        if (k>myPos.second && kCastleValid(s->getKleur())) executeCastle(s->getKleur(), targetPos);
        else if (k<myPos.second && qCastleValid(s->getKleur())) executeCastle(s->getKleur(), targetPos);

        return true;
    }
    pair<SchaakStuk*, pair<int, int>> castling_rook(nullptr,pair<int, int>(-1, -1));
    castling_rook_stack.push_back(castling_rook);

    // Controleer of de opgegeven positie binnen het schaakbord ligt
    if (r < 0 || r > 7 || k < 0 || k > 7 ) {
        return false; // Ongeldige positie
    }
    // Controleer of de opgegeven positie een geldige zet is voor het schaakstuk
    vector<pair<int, int>> zetten = s->mogelijke_zetten(*this);
    auto it = find(zetten.begin(), zetten.end(), targetPos);

    if (it != zetten.end()) {
        // stack undoStack.captured_piece
        // if there is a capture, the captured piece will be pushed. if not, a nullptr will be pushed

        // Geldige positie voor de zet

        // Verplaats het schaakstuk naar het nieuwe veld
        setPiece(r,k,s);

        // Wis het originele veld
        setPiece(myPos.first, myPos.second, nullptr);

        // Werk de positie van het schaakstuk bij
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
        // EP square is the square behind the pawn
        if (whiteToMove()) enPassantSquare = pair<int,int>(selectionPos.first - 1, selectionPos.second);
        else if (!whiteToMove()) enPassantSquare = pair<int,int>(selectionPos.first + 1, selectionPos.second);
        enPassantTargetPos=pair<int,int>(selected->getPos().first, selected->getPos().second);
        SchaakStuk* epTargetPiece = getPiece(enPassantTargetPos.first, enPassantTargetPos.second);

        // make sure the enPassantTargetPos will be marked as 'piece threat' (see bottom of func)
        // check if this pawn landed next to an enemy pawn -> this pawn is threatened because of ep
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

        // initialise threateneds vector with its default values
        // this is for the 'piece threat' tile marks to work ()
    }
}

void Game::aiChoses() { // func where ai choses a piece + position to move
    vector<SchaakStuk*> shuffled = getActivePieces();

    //shuffle vector - Met hulp van Chat GPT
    random_device rd;
    default_random_engine rng(rd());    // initialise random number generator

    // Willekeurig schudden van de vector
    std::shuffle(shuffled.begin(), shuffled.end(), rng);
    // shuffled now contains the active pieces but shuffled

    if (whiteToMove()) return;   // It should be black's whiteToMove
    // This function allows to play against AI player. AI is black

    // Choose a move. Priority = checkmate, check, capture, random
    SchaakStuk* movingPiece = nullptr;   // piece that will be chosen to move
    pair<int,int> myPos(-1,-1); // will be set to original pos of moving piece
    bool found = false;
    // Check if checkmate can be delivered:
    std::shuffle(shuffled.begin(), shuffled.end(), rng);    // shuffle the pieces-vector

    for (auto p : shuffled) {
        if (found) break;
        if (p->getKleur()==wit) continue;
        myPos = p->getPos();
        for (auto z : p->validMoves(*this)) {
            if (found) break;
            aiFakeMove(p, z.first, z.second);  // execute the FAKE MOVE (GUI won't be updated)


            aiFakeMoveMade=true;
//            if (schaakmat(wit)) {
//                aiTargetPos = z;
//                found = true;
//                movingPiece = p;
//
//            }
            // Undo the fake move
            aiFakeMove(p, myPos.first, myPos.second); // move the piece back to myPos
            aiFakeMoveMade=false; // fake move is undone -> var back to false
            tempPiece_2=nullptr; // resetting purposes
        }
    }

    if (!found) {
        // Check if check is possible
        std::shuffle(shuffled.begin(), shuffled.end(), rng);    // shuffle the pieces-vector

        for (auto p: shuffled) {
            if (found) break;
            if (p->getKleur()==wit) continue;
            myPos = p->getPos();
            for (auto z: p->validMoves(*this)) {
                if (found) break;
                aiFakeMove(p, z.first, z.second);  // execute the FAKE MOVE (GUI won't be updated)
                aiFakeMoveMade = true;
                if (schaak(wit)) {
                    aiTargetPos = z;
                    found = true;
                    movingPiece = p;

                }
                // Undo the fake move
                aiFakeMove(p, myPos.first, myPos.second); // move the piece back to myPos
                aiFakeMoveMade = false; // fake move is undone -> var back to false
                tempPiece_2 = nullptr; // resetting purposes
            }
        }
    }


    if (!found) {
        // Check if a capture is possible
        std::shuffle(shuffled.begin(), shuffled.end(), rng);    // shuffle the pieces-vector

        for (auto p : shuffled) {
            if (found) break;
            if (p->getKleur()==wit) continue;
            myPos = p->getPos();
            for (auto z : p->validMoves(*this)) {
                if (found) break;
                if (getPiece(z.first, z.second) != nullptr) {  // move involves a capture
                    aiTargetPos = z; found = true;
                    movingPiece = p;

                }
            }
        }
    }

    if (!found) {
        // If above failed, just do a random move
        std::shuffle(shuffled.begin(), shuffled.end(), rng);    // shuffle the pieces-vector

        for (auto p : shuffled) {
            if (found) break;
            if (p->getKleur()==wit) continue;
            myPos = p->getPos();
            for (auto z : p->validMoves(*this)) {
                if (found) break;
                aiTargetPos = z;
                found = true;    // Take the first move available
                movingPiece = p;
            }
        }
    }

    if (found) {
        // movingPiece shouldn't be nullptr
        aiSelection = movingPiece;
    }
}

void Game::aiMoves() {
    pair<int,int> myPos = aiSelection->getPos();
    undoStack.captured_piece.push_back(getPiece(aiTargetPos.first, aiTargetPos.second));    // update undo-stack

    setPiece(aiTargetPos.first, aiTargetPos.second, aiSelection);

    aiSelection->setPos(aiTargetPos);
    setPiece(myPos.first, myPos.second, nullptr);




}
//todo: undo castle + en passant

void Game::executeCastle(zw kleur, pair<int, int> pos) {
    // Don't call this func if castling is not valid
    // position will be a the square to to places from the king's left (if queenside) or right (kingside)


        // Controleer of de opgegeven positie binnen het schaakbord ligt
    if (pos.first < 0 || pos.first > 7 || pos.second < 0 || pos.second > 7 ) {
        return; // Ongeldige positie
    }

    auto kingPos = findKing(kleur);
    bool kingside = true;   // is it a kingside castle? (if not -> queenside)
    if (pos.second < kingPos.second)  kingside=false;

    // note: castling should already be valid, so we are not checking again

        // Controleer of de opgegeven positie een geldige zet is voor het schaakstuk
    SchaakStuk* king = getPiece(kingPos.first, kingPos.second); // put king piece in local var
    vector<pair<int, int>> zetten = king->validMoves(*this);
    auto it = find(zetten.begin(), zetten.end(), pos);

    // move the king
    if (it != zetten.end()) {
        // Geldige positie voor de zet

        // Verplaats het schaakstuk naar het nieuwe veld
        setPiece(pos.first,pos.second,king);

        // Wis het originele veld
        setPiece(kingPos.first, kingPos.second, nullptr);

        // Werk de positie van het schaakstuk bij
        king->setPos(pos);

    }

    // move rook:
    SchaakStuk* rook = nullptr; // initialise local var: = the castling rook
    for (auto p : getActivePieces()) {
        pair<int,int> q = p->getPos();
        if (p->getKleur()!=kleur || p->getNaam()!=toren) continue;
        // if it is a kingside castle, we are looking for the rook to the right of the king and vice versa
        if (kingside && q.second<kingPos.second) continue; // wrong rook
        if (!kingside && q.second>kingPos.second) continue; // wrong rook
        rook = p;
    }
    pair<int,int> rookPos = rook->getPos();
    castling_rook = {rook, rookPos};

    // Verplaats het schaakstuk naar het nieuwe veld
    pair<int,int> newPos(-1,-1); // initilise local var

    if (kingside) {
        newPos= pair<int,int>(rookPos.first,rookPos.second-2);
        setPiece(rookPos.first,rookPos.second-2,rook);  // move the rook 2 squares towards king (jump over)
    }
    else if (!kingside) {   // queenside castle
        newPos= pair<int,int>(rookPos.first,rookPos.second+3);
        setPiece(rookPos.first,rookPos.second+3,rook);  // move the rook 3 squares towards king (jump over)
    }

    // Wis het originele veld
    setPiece(rookPos.first, rookPos.second, nullptr);

    // Werk de positie van het schaakstuk bij
    if (newPos.first != -1) rook->setPos(newPos);

}

void Game::executeEP(zw kleur, pair<int, int> pos) {    // executes an en passant move
    // don't call this func if ep is not valid
}

// moves a piece unconditially (even if invalid move) to a square
// it is to check if a certain move doesn't result in a self-check
bool Game::fakeMove(SchaakStuk* s, int r, int k) {
// Houdt geen rekening met de mogelijkeZetten()
// will be called for every possible move in mogelijkeZetten()

        if (s == nullptr) {
            return false; // Onmogelijk om een nul-pointer te verplaatsen
        }

        // Controleer of de opgegeven positie binnen het schaakbord ligt
        if (r < 0 || r > 7 || k < 0 || k > 7 ) {
            return false; // Ongeldige positie (buiten bord)
        }

        // We don't check if the move is within mogelijke_zetten
        auto myPos = s->getPos();
        pair<int, int> targetPos(r, k);

        // Geldige positie voor de zet

        // Verplaats het schaakstuk naar het nieuwe veld
        if (getPiece(r,k)!=nullptr) tempPiece= getPiece(r,k); // save this piece to restore it in the "undo fake move"
        setPiece(r,k,s);
        // Wis het originele veld
        setPiece(myPos.first, myPos.second, nullptr);

        // If this is the "undo" of the fake move AND a piece was captured in the fake move, we must restore this piece (=tempPiece)
        // how do we know if there is a piece to restore? -> if tempPiece != nullptr
        if (tempPiece != nullptr && fakeMoveMade) setPiece(myPos.first, myPos.second, tempPiece);   // restore tempPiece
        // Werk de positie van het schaakstuk bij
        s->setPos(targetPos);

        return true;

}
// Help function
// Checks if a certain move will put the moving player himself in check = selfcheck (not legal)


bool Game::aiFakeMove(SchaakStuk* s, int r, int k) {
// seperate function because of overwriting problems of variable tempPiece
// func stays the same except for some var substitutions

    if (s == nullptr) {
        return false; // Onmogelijk om een nul-pointer te verplaatsen
    }

    // Controleer of de opgegeven positie binnen het schaakbord ligt
    if (r < 0 || r > 7 || k < 0 || k > 7 ) {
        return false; // Ongeldige positie (buiten bord)
    }

    // We don't check if the move is within mogelijke_zetten
    auto myPos = s->getPos();
    pair<int, int> targetPos(r, k);

    // Geldige positie voor de zet

    // Verplaats het schaakstuk naar het nieuwe veld
    if (getPiece(r,k)!=nullptr) tempPiece_2= getPiece(r,k); // save this piece to restore it in the "undo fake move"
    setPiece(r,k,s);
    // Wis het originele veld
    setPiece(myPos.first, myPos.second, nullptr);

    // If this is the "undo" of the fake move AND a piece was captured in the fake move, we must restore this piece (=tempPiece_2)
    // how do we know if there is a piece to restore? -> if tempPiece_2 != nullptr
    if (tempPiece_2 != nullptr && aiFakeMoveMade) setPiece(myPos.first, myPos.second, tempPiece_2);   // restore tempPiece_2
    // Werk de positie van het schaakstuk bij
    s->setPos(targetPos);

    return true;

}

// Geeft true als kleur schaak staat
bool Game::schaak(zw kleur) {
    // Vind de positie van de koning van de gegeven kleur
    pair<int, int> kingPos = findKing(kleur);

    // Controleer of de koning wordt aangevallen door een tegenstander
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

    // Als de koning niet wordt gevonden, geef een ongeldige positie terug
    pair<int,int> p(-1, -1);
    return p;
}

// Geeft true als kleur schaakmat staat
bool Game::schaakmat(zw kleur) {
    vector<pair<int, int>> legalMoves;   // will contain all the positions that are valid for one of the pieces to go
    // checkmate(kleur) is true if kleur is in check AND kleur has no legal moves
    if (!schaak(kleur)) return false;

    // check if kleur has any legal moves
    for (auto p: getActivePieces()) {  // loop through all active pieces
        if (p->getKleur() != kleur) continue;
        vector<pair<int,int>> g = p->validMoves(*this); // legal moves of piece
        legalMoves.insert(legalMoves.end(), g.begin(), g.end()); // insert moves to the vector
    }
    if (!legalMoves.empty()) return false;
    return true;
}

// Geeft true als kleur pat staat
// (pat = geen geldige zet mogelijk, maar kleur staat niet schaak;
// dit resulteert in een gelijkspel)
bool Game::pat(zw kleur) {
    vector<pair<int, int>> legalMoves;   // will contain all the positions that are valid for one of the pieces to go

    // stalemate(kleur) is true if kleur is NOT in check AND kleur has no legal moves
    if (schaak(kleur)) return false;

    // check if kleur has any legal moves
    for (auto p: getActivePieces()) {  // loop through all active pieces
        if (p->getKleur() != kleur) continue;
        vector<pair<int,int>> g = p->validMoves(*this); // legal moves of piece
        legalMoves.insert(legalMoves.end(), g.begin(), g.end()); // insert moves to the vector
    }
    if (!legalMoves.empty()) return false;
    return true;
    return false;
}

// Geeft een pointer naar het schaakstuk dat op rij r, kolom k staat
// Als er geen schaakstuk staat op deze positie, geef nullptr terug
SchaakStuk* Game::getPiece(int r, int k) const {
    SchaakStuk* p = schaakbord[r][k];
    return p;
}

// Zet het schaakstuk waar s naar verwijst neer op rij r, kolom k.
// Als er al een schaakstuk staat, wordt het overschreven.
// Bewaar in jouw datastructuur de *pointer* naar het schaakstuk,
// niet het schaakstuk zelf.
void Game::setPiece(int r, int k, SchaakStuk* s)
{
    schaakbord[r][k] = s;
}

vector<SchaakStuk *> Game::getActivePieces() const {
    // Returns vector containing all the pieces still on the board
    vector<SchaakStuk*> result;
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            if (getPiece(i, j) == nullptr) continue;
            result.push_back(getPiece(i, j));

        }
    }
    return result;
};

bool Game::kCastleValid(zw kleur) {
    /*
     * returns whether a KINGSIDE castle is valid for 'kleur'
     */

    // Castle is not possible if your king has already moved:
    if (kleur==zwart && blackKingMoved) return false;
    if (kleur==wit && whiteKingMoved) return false;
    if (schaak(kleur)) return false;    // can't castle when in check
    pair<int,int> kingPos = findKing(kleur);

    // Check whether squares between king and rook are empty (kingside!)
    bool valid = true;
    for (int i=1; i<=2; i++) {  // 2 squares to check between king and rook
        pair<int,int> square(kingPos.first, kingPos.second+i); // column index increases towards kingside
        if (getPiece(square.first, square.second)!=nullptr) return false;

        // Check if one of the squares between king and rook is under enemy control
        // In that case, castle is also not possible
        zw k = wit;
        if (kleur==wit) k = zwart;  // set k to the enemy's color
        vector<pair<int,int>> v = controlledSquares(k);
        auto it = find(v.begin(), v.end(), square); // search for square in enemy's controlled squares
        if (it != v.end()) return false;     // square  under enemy control -> castle not valid
    }
    return true;
}

void Game::promote(int r, int k) {
    // for convenience, promotion will always be a queen.
    zw color = getPiece(r, k)->getKleur();
    pair<int,int> newPos(r, k);
    Koningin* q = new Koningin(color, newPos);
    setPiece(r, k, q);
}


bool Game::qCastleValid(zw kleur) {
    /*
     * returns whether a QUEENSIDE castle is valid for 'kleur'
     */

    // Castle is not possible if your king has already moved:
    if (kleur==zwart && blackKingMoved) return false;
    if (kleur==wit && whiteKingMoved) return false;
    if (schaak(kleur)) return false;    // can't castle when in check
    pair<int,int> kingPos = findKing(kleur);

    // Check whether squares between king and rook are empty (queenside!)
    for (int i=1; i<=3; i++) {  // 3 squares to check between king and rook
        pair<int,int> square(kingPos.first, kingPos.second-i); // column index decreases towards queenside
        if (getPiece(square.first, square.second)!=nullptr) return false;

        // Check if one of the squares between king and rook is under enemy control
        // In that case, castle is also not possible
        zw k = wit;
        if (kleur==wit) k = zwart;  // set k to the enemy's color
        vector<pair<int,int>> v = controlledSquares(k);
        auto it = find(v.begin(), v.end(), square); // search for square in enemy's controlled squares
        if (it != v.end()) return false;     // square  under enemy control -> castle not valid
    }
    return true;
}

vector<pair<int,int>> Game::kingControls(zw kleur) const {
    // Hulpfunction that returns the positions that the king of color kleur controls
    // We more or less copy the mogelijke_zetten function, but this avoids circular dependency
    vector<pair<int, int>> result;
    bool kingFound=false;
    pair<int,int> kingPos(-1,-1); // invalid default position
    for (auto p : getActivePieces()) {
        if (p->getKleur()!=kleur || p->getNaam()!=koning) continue;
        kingPos = p->getPos();
        kingFound=true;
    }
    if (!kingFound) return result; // return empty vector (should never be the case)

    // Aangrenzende posities
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i != 0 || j != 0) {     // cant be both zero (mypos)
                pair<int,int> p(kingPos.first+i,kingPos.second+j);
                result.push_back(p);
            }
        }
    }
    return result;

}
vector<pair<int, int>> Game::controlledSquares(zw kleur) {  // returns vector with positions that 'kleur' controls
    vector<pair<int,int>> result;
//    Check the attacking squares of all the pieces from 'kleur'
    vector<SchaakStuk*> activePieces = getActivePieces();

    if (activePieces.empty()) {
        // voorkom nullptr toegang of onverwachte fouten
        return result;
    }
    Game g = *this;
    for (auto p : getActivePieces()) {
        if (p->getKleur()!=kleur) continue;
        if (p->getNaam()==pion && p->getKleur()==zwart) {
            pair<int,int> pawnPos = p->getPos();
            pair<int,int> diagL(pawnPos.first + 1, pawnPos.second + 1); // square in front left
            pair<int,int> diagR(pawnPos.first + 1, pawnPos.second - 1); // square in front right
            result.push_back(diagL); result.push_back(diagR);
            if (!p->testSelfCheck(g, diagL.first, diagL.second)) result.push_back(diagL); // insert square (if valid)
            if (!p->testSelfCheck(g, diagR.first, diagR.second)) result.push_back(diagR);

        }
        else if (p->getNaam()==pion && p->getKleur()==wit) {
            pair<int,int> pawnPos = p->getPos();
            pair<int,int> diagL(pawnPos.first - 1, pawnPos.second + 1); // square in front left
            pair<int,int> diagR(pawnPos.first - 1, pawnPos.second - 1); // square in front right
            result.push_back(diagL); result.push_back(diagR);
            if (!p->testSelfCheck(g, diagL.first, diagL.second)) result.push_back(diagL); // insert square (if valid)
            if (!p->testSelfCheck(g, diagR.first, diagR.second)) result.push_back(diagR);

        }
        else if (p->getNaam()==koning) { // We need to do this separately to avoid segfault (circular dependency?)
            vector<pair<int,int>> v = kingControls(kleur);
            result.insert(result.end(), v.begin(), v.end()); // add all the piece's attacking squares to result
        } else {  // Not a king, not a pawn -> can safely use validMoves()
            vector<pair<int,int>> v = p->validMoves(*this);
            result.insert(result.end(), v.begin(), v.end()); // add all the piece's attacking squares to result
       }
    }
    return result;
}

vector<pair<int, int>> Game::piecesInVision(zw kleur) {
    vector<pair<int,int>> result;
    // returns vector of the pieces that can be taken by any piece of 'kleur'
    // Loop through controlled_squares en check for every position if there is a piece on that square

    for (auto piece : getActivePieces()) {
        if (piece->getKleur()!=kleur) continue;
        for (auto pos : piece->validMoves(*this)) {
            if (getPiece(pos.first, pos.second) == nullptr) continue;   // empty square
            result.push_back(pos);  // enemy piece
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


