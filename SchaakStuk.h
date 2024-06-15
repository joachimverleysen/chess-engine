//  Student:
//  Rolnummer:
//  Opmerkingen: (bvb aanpassingen van de opgave)

#ifndef SCHAKEN_SCHAAKSTUK_H
#define SCHAKEN_SCHAAKSTUK_H
#include <guicode/chessboard.h>
#include <utility>
#include <vector>
#include <string>
using std::pair; using std::vector; using std::string;

class Game;

enum zw{zwart,wit};
enum pieceName {
    pion,
    toren,
    loper,
    paard,
    koning,
    koningin
};

class SchaakStuk {  // Abstract class
public:
    SchaakStuk(zw kleur, pair<int,int> pos=pair<int,int>(-1,-1), pieceName naam=pion) : kleur(kleur) , position(pos), naam(naam){};       // para 'position' initialised with default value (-1,-1)
    virtual ~SchaakStuk() {};

     virtual vector<pair<int,int>> mogelijke_zetten(Game &game) = 0; // pure virtual

    virtual Piece piece() const=0;      // Verander deze functie niet!
                                        // Deze functie wordt gebruikt door
                                        // setItem(x,y,SchaakStuk*) van
                                        // SchaakGUI

    zw getKleur() const { return kleur; }

    pair<int, int> getPos() const;

    void setPos(pair<int,int>);
    pair<int, int> position;

    vector<pair<int,int>> attackingSquares(Game &game) ;

    virtual pieceName getNaam() const;


    bool testSelfCheck(Game &g, int r, int k);

    vector<pair<int, int>> validMoves(Game &game);


private:
    zw kleur;
    pieceName naam;


};

class Pion:public SchaakStuk {
public:

    // Wanneer er een Pion-object zou aangemaakt worden zonder een position mee te geven, dan zal de base class constructor de defaultwaarde (-1,-1) toekennen.
    Pion(zw kleur, pair<int,int> pos = pair<int,int>(), pieceName naam=pion) : SchaakStuk(kleur, pos, naam) {}
    ~Pion() {};

    virtual Piece piece() const override {  // don't change this
        return Piece(Piece::Pawn,getKleur()==wit?Piece::White:Piece::Black);
    }

    vector<pair<int,int>> mogelijke_zetten(Game &game);

private:
    pieceName naam = pion;
};

class Toren : public SchaakStuk {
public:
    Toren(zw kleur, pair<int, int> pos = pair<int, int>(), pieceName naam=toren) : SchaakStuk(kleur, pos, naam) {}
    ~Toren() {};
    Piece piece() const override {
        return Piece(Piece::Rook, getKleur() == wit ? Piece::White : Piece::Black);
    }

    vector<pair<int, int>> mogelijke_zetten(Game &game);

private:
    pieceName naam = toren;
};

class Paard : public SchaakStuk {
public:
    Paard(zw kleur, pair<int, int> pos = pair<int, int>(), pieceName naam=paard) : SchaakStuk(kleur, pos, naam) {}
    ~Paard() {};
    Piece piece() const {
        return Piece(Piece::Knight, getKleur() == wit ? Piece::White : Piece::Black);
    }

    vector<pair<int, int>> mogelijke_zetten(Game &game);

private:
    pieceName naam = paard;
};

class Loper : public SchaakStuk {
public:
    Loper(zw kleur, pair<int, int> pos = pair<int, int>(), pieceName naam=loper) : SchaakStuk(kleur, pos, naam) {}
    ~Loper() {};
    Piece piece() const override {
        return Piece(Piece::Bishop, getKleur() == wit ? Piece::White : Piece::Black);
    }

    vector<pair<int, int>> mogelijke_zetten(Game &game);

private:
    pieceName naam = loper;
};

class Koning : public SchaakStuk {
public:
    Koning(zw kleur, pair<int, int> pos = pair<int, int>(), pieceName naam=koning) : SchaakStuk(kleur, pos, naam) {}
    ~Koning() {};
    Piece piece() const override {
        return Piece(Piece::King, getKleur() == wit ? Piece::White : Piece::Black);
    }

    vector<pair<int, int>> mogelijke_zetten(Game &game);

    bool inCheck= false;

private:
    pieceName naam = koning;

};

class Koningin : public SchaakStuk {
public:
    Koningin(zw kleur, pair<int, int> pos = pair<int, int>(), pieceName naam=koningin) : SchaakStuk(kleur, pos, naam) {}
    ~Koningin() {};
    Piece piece() const override {
        return Piece(Piece::Queen, getKleur() == wit ? Piece::White : Piece::Black);
    }

    vector<pair<int, int>> mogelijke_zetten(Game &game);

private:
    pieceName naam = koningin;

};

#endif //SCHAKEN_SCHAAKSTUK_H
