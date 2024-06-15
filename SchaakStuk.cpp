//  Student: Joachim Verleysen
//  Rolnummer: 20231444
//  Opmerkingen: (bvb aanpassingen van de opgave)
//
#include <iostream>
#include "SchaakStuk.h"
#include "game.h"
#include <utility>
#include <algorithm>
using std::cout; using std::vector; using std::pair; using std::find;

// Schaakstuk methods
pair<int, int> SchaakStuk::getPos() const {
    // The object on which this method is called may not be a nullptr !!!
    return position;
}

// checks for every 'mogelijke zet' if the move is also legal
// (that means: not putting yourself in check or ignoring check)
vector<pair<int, int>> SchaakStuk::validMoves(Game &game) {
    vector<pair<int,int>> result;
    // We will loop through every possible move and execute the move (without updating GUI)
    // We will check if the move results in the moving player being in check
    // In that case, the move is not legal and will be undone. it will not be added to the legal moves
    for (auto p : mogelijke_zetten(game)) {
        if (testSelfCheck(game, p.first, p.second)) continue; // test if the move results in a self check (illegal)
        result.push_back(p);   // position is legal -> add to result

    }

    // Add castle moves if valid:
    if (this->getNaam()==koning) {
        pair<int,int> kingPos = this->getPos();
        //kingside castle
        if (game.kCastleValid(this->getKleur())) {  // kingside castle valid
            pair<int,int> p(kingPos.first, kingPos.second+2);
            result.push_back(p);    // add square that is 2 places to the king's right
        }
        //queenside castle
        if (game.qCastleValid(this->getKleur())) {  // kingside castle valid
            pair<int,int> p(kingPos.first, kingPos.second-2);
            result.push_back(p);    // add square that is 2 places to the king's right
        }
    }

    return result;
}


bool SchaakStuk::testSelfCheck(Game &g, int r, int k) {
    pair<int,int> myPos = getPos(); // original position
    g.fakeMove(this, r, k);  // execute the FAKE MOVE (GUI won't be updated)
    g.fakeMoveMade=true;
    // if a piece was captured in the fake move, it will be stored in tempPiece to restore it in the undo.
    if (g.schaak(getKleur())) {  // player tried to put himself in check
        // UNDO FAKE MOVE:
        g.fakeMove(this, myPos.first, myPos.second); // move the piece back to myPos
        g.fakeMoveMade=false; // fake move is undone -> var back to false
        g.tempPiece=nullptr; // resetting purposes
        return true;
        // pieceSelected remains true since there hasn't been made a valid move
    }
    // UNDO FAKE MOVE:
    g.fakeMove(this, myPos.first, myPos.second); // move the piece back to myPos
    g.fakeMoveMade=false; // fake move is undone -> var back to false
    g.tempPiece=nullptr; // resetting purposes


    return false;
    // Note: movecount (int) and turn (bool) are not updated here
}
void SchaakStuk::setPos(pair<int, int> pos) {
    SchaakStuk::position = pos;
}

vector<pair<int, int>> SchaakStuk::attackingSquares(Game &game) {
    // Returns vector with postitions that are attacked by this piece
    vector<pair<int,int>> result;

    for (auto po : mogelijke_zetten(game)) { // Loop through the squares that are in vision of the piece
        if (game.getPiece(po.first, po.second) != nullptr) { // Opponent's piece in vision
            result.emplace_back(po.first, po.second); // Push position to result
        }
    }

    return result;
}

pieceName SchaakStuk::getNaam() const {
    return naam;
}

bool inRange(pair<int, int> &pos) {
    return pos.first>=0 && pos.first<8 &&
    pos.second >=0 && pos.second <8;
}
vector<pair<int, int>> Pion::mogelijke_zetten(Game &game) {
    // ignores check, checkmate...

    vector<pair<int, int>> result;

    pair<int,int> myPos(getPos());      // Initialize var with position

    // initialize positions for black
    pair<int,int> diagL(myPos.first+1, myPos.second+1); // square in front left
    pair<int,int> diagR(myPos.first+1, myPos.second-1); // square in front right
    pair<int,int> front(myPos.first+1, myPos.second);   // square in front
    pair<int,int> front_2(myPos.first+2, myPos.second);   // 2 squares in front

    // redefine the positions if it is white's turn
    if (getKleur()==wit) {

        diagL.first = myPos.first - 1;
        diagL.second = myPos.second - 1;

        diagR.first = myPos.first - 1;
        diagR.second = myPos.second + 1;

        front.first = myPos.first - 1;

        front_2.first = myPos.first - 2;
    }
    // Check for En Passant
    if (diagL == game.enPassantSquare) result.push_back(diagL);
    if (diagR == game.enPassantSquare) result.push_back((diagR));
    // Check if the square in front is empty
    if (game.getPiece(front.first, front.second) == nullptr) {
        result.push_back(front);

        // If on starting position, check if the 2 squares in front are also empty
        if ((myPos.first == 1 && this->getKleur()==zwart) && game.getPiece(front_2.first, front_2.second) == nullptr) {
            result.push_back(front_2);
        }
        else if ((myPos.first == 6 && this->getKleur()==wit) &&game.getPiece(front_2.first, front_2.second)==nullptr) {
            result.push_back((front_2));
        }
    }


    // Check if the diagonally left square has an opponent's piece
    if (inRange(diagL) && game.getPiece(diagL.first, diagL.second) != nullptr &&
            game.getPiece(diagL.first, diagL.second)->getKleur() != this->getKleur()) {
        result.push_back(diagL);
    }

    // Check if the diagonally right square has an opponent's piece
    if (inRange(diagR) && game.getPiece(diagR.first, diagR.second) != nullptr &&
            game.getPiece(diagR.first, diagR.second)->getKleur() != this->getKleur()) {
        result.push_back(diagR);
    }
//
//    // Check for en passant
//    if (game.getEnPassantSquare().first == myPos.first && abs(game.getEnPassantSquare().second - myPos.second) == 1) {
//        // En passant square is next to the current pawn
//        result.emplace_back(game.getEnPassantSquare().first + (getKleur() == zwart ? 1 : -1), game.getEnPassantSquare().second);
//    }

    return result;
}


vector<pair<int, int>> Loper::mogelijke_zetten(Game &game) {    // met hulp van ChatGPT
    vector<pair<int, int>> result;
    pair<int, int> myPos(getPos());


    // The bishop moves diagonally.
    // We will use 4 loops for the 4 different directions:
        // (x+1,y+1) ; (x-1,y-1) ; (x-1,y+1) ; (x+1,y-1)
    // When we encounter a non-empty square, we will check first if the piece is of the opposite color -> capture possible


    for (int i = 1; myPos.first + i < 8 && myPos.second + i < 8; i++) {
        SchaakStuk* square = game.getPiece(myPos.first + i, myPos.second + i);
        if (square != nullptr){   // square not empty
            if (square->getKleur()!=this->getKleur()) {     // piece is from opponent -> can be captured
                result.emplace_back(myPos.first + i, myPos.second + i);
                break; // no further moves available in this direction
            }

            else if (square->getKleur()==this->getKleur()) {       // same-colored piece: can't be captured!
                // same-colored piece stands in the bishop's way. No further moves in this direction are available
                break;  // jump out of this loop as no more available moves
            }
        }

        result.emplace_back(myPos.first + i, myPos.second + i);
    }
    for (int i = 1; myPos.first + i < 8 && myPos.second - i >= 0; i++) {
        SchaakStuk* square = game.getPiece(myPos.first+i, myPos.second-i);
        if (square != nullptr){   // square not empty
            if (square->getKleur()!=this->getKleur()) {     // piece is from opponent -> can be captured
                result.emplace_back(myPos.first + i, myPos.second - i);
                break; // no further moves available in this direction
            }

            else if (square->getKleur()==this->getKleur()) {       // same-colored piece: can't be captured!
                // same-colored piece stands in the bishop's way. No further moves in this direction are available
                break;  // jump out of this loop as no more available moves
            }
        }
        result.emplace_back(myPos.first + i, myPos.second - i);
    }
    for (int i = 1; myPos.first - i >= 0 && myPos.second + i < 8; i++) {
        SchaakStuk* square = game.getPiece(myPos.first-i, myPos.second+i);
        if (square != nullptr){   // square not empty
            if (square->getKleur()!=this->getKleur()) {     // piece is from opponent -> can be captured
                result.emplace_back(myPos.first - i, myPos.second + i);
                break; // no further moves available in this direction
            }

            else if (square->getKleur()==this->getKleur()) {       // same-colored piece: can't be captured!
                // same-colored piece stands in the bishop's way. No further moves in this direction are available
                break;  // jump out of this loop as no more available moves in this direction
            }
        }
        result.emplace_back(myPos.first - i, myPos.second + i);
    }
    for (int i = 1; myPos.first - i >= 0 && myPos.second - i >= 0; i++) {
        SchaakStuk* square = game.getPiece(myPos.first-i, myPos.second-i);
        if (square != nullptr){   // square not empty
            if (square->getKleur()!=this->getKleur()) {     // piece is from opponent -> can be captured
                result.emplace_back(myPos.first - i, myPos.second - i);
                break; // no further moves available in this direction
            }

            else if (square->getKleur()==this->getKleur()) {       // same-colored piece: can't be captured!
                // same-colored piece stands in the bishop's way. No further moves in this direction are available
                break;  // jump out of this loop as no more available moves
            }
        }
        // if the square IS empty, the bishop can move here:
        result.emplace_back(myPos.first - i, myPos.second - i);
    }
    return result;
}

vector<pair<int, int>> Toren::mogelijke_zetten(Game &game) {
    vector<pair<int, int>> result;
    pair<int, int> myPos(getPos());

    // The rook moves in straight lines.
    // We will use 4 loops for the 4 different directions:
    // (x,y+1) ; (x+1,y) ; (x,y-1) ; (x-1,y)

    // Moving upwards
    for (int i = 1; myPos.first - i >= 0; i++) {
        SchaakStuk *square = game.getPiece(myPos.first - i, myPos.second);
        if (square != nullptr) {
            if (square->getKleur() != this->getKleur()) {
                result.emplace_back(myPos.first - i, myPos.second);
            }
            break; // Stop further moves in this direction
        } else {
            result.emplace_back(myPos.first - i, myPos.second);
        }
    }

    // Moving downwards
    for (int i = 1; myPos.first + i < 8; i++) {
        SchaakStuk *square = game.getPiece(myPos.first + i, myPos.second);
        if (square != nullptr) {
            if (square->getKleur() != this->getKleur()) {
                result.emplace_back(myPos.first + i, myPos.second);
            }
            break; // Stop further moves in this direction
        } else {
            result.emplace_back(myPos.first + i, myPos.second);
        }
    }

    // Moving to the right
    for (int i = 1; myPos.second + i < 8; i++) {
        SchaakStuk *square = game.getPiece(myPos.first, myPos.second + i);
        if (square != nullptr) {
            if (square->getKleur() != this->getKleur()) {
                result.emplace_back(myPos.first, myPos.second + i);
            }
            break; // Stop further moves in this direction
        } else {
            result.emplace_back(myPos.first, myPos.second + i);
        }
    }

    // Moving to the left
    for (int i = 1; myPos.second - i >= 0; i++) {
        SchaakStuk *square = game.getPiece(myPos.first, myPos.second - i);
        if (square != nullptr) {
            if (square->getKleur() != this->getKleur()) {
                result.emplace_back(myPos.first, myPos.second - i);
            }
            break; // Stop further moves in this direction
        } else {
            result.emplace_back(myPos.first, myPos.second - i);
        }
    }

    return result;
}
vector<pair<int, int>> Koning::mogelijke_zetten(Game &game) {       // met hulp van ChatGPT
    vector<pair<int, int>> result;
    pair<int, int> myPos(getPos());

    // check for castle options:

    // Aangrenzende posities
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i != 0 || j != 0) {     // cant be both zero (mypos)
                if (myPos.first+i<0 || myPos.first+i>7 || myPos.second+j<0 ||myPos.second+j>7) continue;
                pair<int,int> p(myPos.first+i, myPos.second+j); // possible valid move
                bool valid=true;    // checks if a position is a valid move
                // check if a same-colored piece blocks the way
                auto square = game.getPiece(p.first, p.second);
                if (square != nullptr) { // same colored piece
                    // if square is a king, we handle this with hulpfunction kingControls
                    // we must skip it here to avoid segfault:
//                    if (square->getNaam()==koning) {valid=false; continue;} // skip the 'king' cases to avoid segfault
                    if (square->getKleur() == this->getKleur()) continue;  // cant capture own color

                }


                // at this point, position p is an empty square or an enemy piece

                // make sure king doesn't walk into check
                result.emplace_back(myPos.first+i, myPos.second+j);

            }
        }
    }

    return result;
}




vector<pair<int, int>> Paard::mogelijke_zetten(Game &game) {
    vector<pair<int, int>> result;
    pair<int, int> myPos(getPos());

    // The knight moves (L-shape)
    int moves[8][2] = {     // Store the moves as arrays in an array
            {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
            {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };

    for (int i = 0; i < 8; ++i) {   // We will loop through the array and check every move
        int newX = myPos.first + moves[i][0];
        int newY = myPos.second + moves[i][1];

        // Check if the new position is within the board boundaries
        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
            SchaakStuk *square = game.getPiece(newX, newY);

            // Check if the square is empty or has an opponent's piece
            if (square == nullptr || square->getKleur() != this->getKleur()) {
                result.emplace_back(newX, newY);
            }
            // Otherwise, the square has a piece of the same color, so it can't be captured
        }
    }

    return result;
}

vector<pair<int, int>> Koningin::mogelijke_zetten(Game &game) {
    vector<pair<int, int>> result;
    pair<int, int> myPos(getPos());

    // We simply combine the rook+bishop method to implement the queen

    // Moving upwards
    for (int i = 1; myPos.first - i >= 0; i++) {
        SchaakStuk *square = game.getPiece(myPos.first - i, myPos.second);
        if (square != nullptr) {
            if (square->getKleur() != this->getKleur()) {
                result.emplace_back(myPos.first - i, myPos.second);
            }
            break; // Stop further moves in this direction
        } else {
            result.emplace_back(myPos.first - i, myPos.second);
        }
    }

    // Moving downwards
    for (int i = 1; myPos.first + i < 8; i++) {
        SchaakStuk *square = game.getPiece(myPos.first + i, myPos.second);
        if (square != nullptr) {
            if (square->getKleur() != this->getKleur()) {
                result.emplace_back(myPos.first + i, myPos.second);
            }
            break; // Stop further moves in this direction
        } else {
            result.emplace_back(myPos.first + i, myPos.second);
        }
    }

    // Moving to the right
    for (int i = 1; myPos.second + i < 8; i++) {
        SchaakStuk *square = game.getPiece(myPos.first, myPos.second + i);
        if (square != nullptr) {
            if (square->getKleur() != this->getKleur()) {
                result.emplace_back(myPos.first, myPos.second + i);
            }
            break; // Stop further moves in this direction
        } else {
            result.emplace_back(myPos.first, myPos.second + i);
        }
    }

    // Moving to the left
    for (int i = 1; myPos.second - i >= 0; i++) {
        SchaakStuk *square = game.getPiece(myPos.first, myPos.second - i);
        if (square != nullptr) {
            if (square->getKleur() != this->getKleur()) {
                result.emplace_back(myPos.first, myPos.second - i);
            }
            break; // Stop further moves in this direction
        } else {
            result.emplace_back(myPos.first, myPos.second - i);
        }
    }


    for (int i = 1; myPos.first + i < 8 && myPos.second + i < 8; i++) {
        SchaakStuk* square = game.getPiece(myPos.first + i, myPos.second + i);
        if (square != nullptr){   // square not empty
            if (square->getKleur()!=this->getKleur()) {     // piece is from opponent -> can be captured
                result.emplace_back(myPos.first + i, myPos.second + i);
                break; // no further moves available in this direction
            }

            else if (square->getKleur()==this->getKleur()) {       // same-colored piece: can't be captured!
                // same-colored piece stands in the  way. No further moves in this direction are available
                break;  // jump out of this loop as no more available moves
            }
        }

        result.emplace_back(myPos.first + i, myPos.second + i);
    }
    for (int i = 1; myPos.first + i < 8 && myPos.second - i >= 0; i++) {
        SchaakStuk* square = game.getPiece(myPos.first+i, myPos.second-i);
        if (square != nullptr){   // square not empty
            if (square->getKleur()!=this->getKleur()) {     // piece is from opponent -> can be captured
                result.emplace_back(myPos.first + i, myPos.second - i);
                break; // no further moves available in this direction
            }

            else if (square->getKleur()==this->getKleur()) {       // same-colored piece: can't be captured!
                // same-colored piece stands in the way. No further moves in this direction are available
                break;  // jump out of this loop as no more available moves
            }
        }
        result.emplace_back(myPos.first + i, myPos.second - i);
    }
    for (int i = 1; myPos.first - i >= 0 && myPos.second + i < 8; i++) {
        SchaakStuk* square = game.getPiece(myPos.first-i, myPos.second+i);
        if (square != nullptr){   // square not empty
            if (square->getKleur()!=this->getKleur()) {     // piece is from opponent -> can be captured
                result.emplace_back(myPos.first - i, myPos.second + i);
                break; // no further moves available in this direction
            }

            else if (square->getKleur()==this->getKleur()) {       // same-colored piece: can't be captured!
                // same-colored piece stands in the way. No further moves in this direction are available
                break;  // jump out of this loop as no more available moves in this direction
            }
        }
        result.emplace_back(myPos.first - i, myPos.second + i);
    }
    for (int i = 1; myPos.first - i >= 0 && myPos.second - i >= 0; i++) {
        SchaakStuk* square = game.getPiece(myPos.first-i, myPos.second-i);
        if (square != nullptr){   // square not empty
            if (square->getKleur()!=this->getKleur()) {     // piece is from opponent -> can be captured
                result.emplace_back(myPos.first - i, myPos.second - i);
                break; // no further moves available in this direction
            }

            else if (square->getKleur()==this->getKleur()) {       // same-colored piece: can't be captured!
                // same-colored piece stands in the way. No further moves in this direction are available
                break;  // jump out of this loop as no more available moves
            }
        }
        // if the square IS empty, the bishop can move here:
        result.emplace_back(myPos.first - i, myPos.second - i);
    }
    return result;
}


