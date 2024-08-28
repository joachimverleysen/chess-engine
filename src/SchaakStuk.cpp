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

pair<int, int> SchaakStuk::getPos() const {
    return position;
}


vector<pair<int, int>> SchaakStuk::validMoves(Game &game) {
    vector<pair<int,int>> result;

    for (auto p : mogelijke_zetten(game)) {
        if (testSelfCheck(game, p.first, p.second)) continue;
        result.push_back(p);

    }
    if (this->getNaam()==koning) {
        pair<int,int> kingPos = this->getPos();
        if (game.kingSideCastleIsValid(this->getKleur())) {
            pair<int,int> p(kingPos.first, kingPos.second+2);
            result.push_back(p);
        }
        if (game.queenSideCastleIsValid(this->getKleur())) {
            pair<int,int> p(kingPos.first, kingPos.second-2);
            result.push_back(p);
        }
    }

    return result;
}


bool SchaakStuk::testSelfCheck(Game &g, int r, int k) {
    pair<int,int> myPos = getPos();
    g.fakeMove(this, r, k);
    g.fakeMoveMade=true;
    if (g.schaak(getKleur())) {
        // UNDO FAKE MOVE:
        g.fakeMove(this, myPos.first, myPos.second);
        g.fakeMoveMade=false;
        g.tempCapturedPiece=nullptr;
        return true;
    }
    // UNDO FAKE MOVE:
    g.fakeMove(this, myPos.first, myPos.second);
    g.fakeMoveMade=false;
    g.tempCapturedPiece=nullptr;


    return false;
}
void SchaakStuk::setPos(pair<int, int> pos) {
    SchaakStuk::position = pos;
}

vector<pair<int, int>> SchaakStuk::attackingSquares(Game &game) {
    // Returns vector with postitions that are attacked by this piece
    vector<pair<int,int>> result;

    for (auto pos : mogelijke_zetten(game)) {
        if (game.getPiece(pos.first, pos.second) != nullptr) { // Opponent's piece in vision
            result.emplace_back(pos.first, pos.second); // Push position to result
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

    vector<pair<int, int>> result;

    pair<int,int> myPos(getPos());

    pair<int,int> diagL(myPos.first+1, myPos.second+1);
    pair<int,int> diagR(myPos.first+1, myPos.second-1);
    pair<int,int> front(myPos.first+1, myPos.second);
    pair<int,int> front_2(myPos.first+2, myPos.second);

    if (getKleur()==wit) {

        diagL.first = myPos.first - 1;
        diagL.second = myPos.second - 1;

        diagR.first = myPos.first - 1;
        diagR.second = myPos.second + 1;

        front.first = myPos.first - 1;

        front_2.first = myPos.first - 2;
    }
    if (diagL == game.enPassantSquare) result.push_back(diagL);
    if (diagR == game.enPassantSquare) result.push_back((diagR));

    if (game.getPiece(front.first, front.second) == nullptr) {
        result.push_back(front);

        if ((myPos.first == 1 && this->getKleur()==zwart) && game.getPiece(front_2.first, front_2.second) == nullptr) {
            result.push_back(front_2);
        }
        else if ((myPos.first == 6 && this->getKleur()==wit) &&game.getPiece(front_2.first, front_2.second)==nullptr) {
            result.push_back((front_2));
        }
    }

    if (inRange(diagL) && game.getPiece(diagL.first, diagL.second) != nullptr &&
            game.getPiece(diagL.first, diagL.second)->getKleur() != this->getKleur()) {
        result.push_back(diagL);
    }

    if (inRange(diagR) && game.getPiece(diagR.first, diagR.second) != nullptr &&
            game.getPiece(diagR.first, diagR.second)->getKleur() != this->getKleur()) {
        result.push_back(diagR);
    }


    return result;
}


vector<pair<int, int>> Loper::mogelijke_zetten(Game &game) {    // met hulp van ChatGPT
    vector<pair<int, int>> result;
    pair<int, int> myPos(getPos());



    for (int i = 1; myPos.first + i < 8 && myPos.second + i < 8; i++) {
        SchaakStuk* square = game.getPiece(myPos.first + i, myPos.second + i);
        if (square != nullptr){
            if (square->getKleur()!=this->getKleur()) {
                result.emplace_back(myPos.first + i, myPos.second + i);
                break;
            }

            else if (square->getKleur()==this->getKleur()) {
                break;
            }
        }

        result.emplace_back(myPos.first + i, myPos.second + i);
    }
    for (int i = 1; myPos.first + i < 8 && myPos.second - i >= 0; i++) {
        SchaakStuk* square = game.getPiece(myPos.first+i, myPos.second-i);
        if (square != nullptr){
            if (square->getKleur()!=this->getKleur()) {
                result.emplace_back(myPos.first + i, myPos.second - i);
                break;
            }

            else if (square->getKleur()==this->getKleur()) {
                break;
            }
        }
        result.emplace_back(myPos.first + i, myPos.second - i);
    }
    for (int i = 1; myPos.first - i >= 0 && myPos.second + i < 8; i++) {
        SchaakStuk* square = game.getPiece(myPos.first-i, myPos.second+i);
        if (square != nullptr){
            if (square->getKleur()!=this->getKleur()) {
                result.emplace_back(myPos.first - i, myPos.second + i);
                break;
            }

            else if (square->getKleur()==this->getKleur()) {
                break;
            }
        }
        result.emplace_back(myPos.first - i, myPos.second + i);
    }
    for (int i = 1; myPos.first - i >= 0 && myPos.second - i >= 0; i++) {
        SchaakStuk* square = game.getPiece(myPos.first-i, myPos.second-i);
        if (square != nullptr){
            if (square->getKleur()!=this->getKleur()) {
                result.emplace_back(myPos.first - i, myPos.second - i);
                break;
            }

            else if (square->getKleur()==this->getKleur()) {
                break;
            }
        }
        result.emplace_back(myPos.first - i, myPos.second - i);
    }
    return result;
}

vector<pair<int, int>> Toren::mogelijke_zetten(Game &game) {
    vector<pair<int, int>> result;
    pair<int, int> myPos(getPos());

    // Moving upwards
    for (int i = 1; myPos.first - i >= 0; i++) {
        SchaakStuk *square = game.getPiece(myPos.first - i, myPos.second);
        if (square != nullptr) {
            if (square->getKleur() != this->getKleur()) {
                result.emplace_back(myPos.first - i, myPos.second);
            }
            break;
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
            break;
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
            break;
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
            break;
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
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i != 0 || j != 0) {
                if (myPos.first+i<0 || myPos.first+i>7 || myPos.second+j<0 ||myPos.second+j>7) continue;
                pair<int,int> p(myPos.first+i, myPos.second+j);
                auto square = game.getPiece(p.first, p.second);
                if (square != nullptr) {
                    if (square->getKleur() == this->getKleur()) continue;

                }
                result.emplace_back(myPos.first+i, myPos.second+j);

            }
        }
    }

    return result;
}




vector<pair<int, int>> Paard::mogelijke_zetten(Game &game) {
    vector<pair<int, int>> result;
    pair<int, int> myPos(getPos());

    int moves[8][2] = {
            {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
            {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };

    for (int i = 0; i < 8; ++i) {
        int newX = myPos.first + moves[i][0];
        int newY = myPos.second + moves[i][1];

        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {
            SchaakStuk *square = game.getPiece(newX, newY);

            if (square == nullptr || square->getKleur() != this->getKleur()) {
                result.emplace_back(newX, newY);
            }
        }
    }

    return result;
}

vector<pair<int, int>> Koningin::mogelijke_zetten(Game &game) {
    vector<pair<int, int>> result;
    pair<int, int> myPos(getPos());


    // Moving upwards
    for (int i = 1; myPos.first - i >= 0; i++) {
        SchaakStuk *square = game.getPiece(myPos.first - i, myPos.second);
        if (square != nullptr) {
            if (square->getKleur() != this->getKleur()) {
                result.emplace_back(myPos.first - i, myPos.second);
            }
            break;
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
            break;
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
            break;
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
            break;
        } else {
            result.emplace_back(myPos.first, myPos.second - i);
        }
    }


    for (int i = 1; myPos.first + i < 8 && myPos.second + i < 8; i++) {
        SchaakStuk* square = game.getPiece(myPos.first + i, myPos.second + i);
        if (square != nullptr){
            if (square->getKleur()!=this->getKleur()) {
                result.emplace_back(myPos.first + i, myPos.second + i);
                break;
            }

            else if (square->getKleur()==this->getKleur()) {
                break;
            }
        }

        result.emplace_back(myPos.first + i, myPos.second + i);
    }
    for (int i = 1; myPos.first + i < 8 && myPos.second - i >= 0; i++) {
        SchaakStuk* square = game.getPiece(myPos.first+i, myPos.second-i);
        if (square != nullptr){
            if (square->getKleur()!=this->getKleur()) {
                result.emplace_back(myPos.first + i, myPos.second - i);
                break;
            }

            else if (square->getKleur()==this->getKleur()) {
                break;
            }
        }
        result.emplace_back(myPos.first + i, myPos.second - i);
    }
    for (int i = 1; myPos.first - i >= 0 && myPos.second + i < 8; i++) {
        SchaakStuk* square = game.getPiece(myPos.first-i, myPos.second+i);
        if (square != nullptr){
            if (square->getKleur()!=this->getKleur()) {
                result.emplace_back(myPos.first - i, myPos.second + i);
                break;
            }

            else if (square->getKleur()==this->getKleur()) {
                break;
            }
        }
        result.emplace_back(myPos.first - i, myPos.second + i);
    }
    for (int i = 1; myPos.first - i >= 0 && myPos.second - i >= 0; i++) {
        SchaakStuk* square = game.getPiece(myPos.first-i, myPos.second-i);
        if (square != nullptr){
            if (square->getKleur()!=this->getKleur()) {
                result.emplace_back(myPos.first - i, myPos.second - i);
                break;
            }

            else if (square->getKleur()==this->getKleur()) {
                break;
            }
        }
        result.emplace_back(myPos.first - i, myPos.second - i);
    }
    return result;
}
