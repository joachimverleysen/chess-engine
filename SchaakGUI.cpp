//
// Created by toonc on 12/17/2021.
//

#include "SchaakGUI.h"
#include "guicode/message.h"
#include "guicode/fileIO.h"
#include "game.h"
#include <iostream>
#include <algorithm>
#include <iostream>
using std::cout;


// Constructor
SchaakGUI::SchaakGUI():ChessWindow(nullptr) {
    g.setStartBord();
    update();
}

// Update de inhoud van de grafische weergave van het schaakbord (scene)
// en maak het consistent met de game state in variabele g.
void SchaakGUI::update() {
    clearBoard();

//     Loop through board matrix and call setItem() for every piece
for (int i=0; i<8; i++) {
    for (int j=0; j<8; j++) {
        if (g.schaakbord[i][j] != nullptr) {
            setItem(i, j, g.getPiece(i, j));
        }
        else if (g.schaakbord[i][j] == nullptr) {
//            setItem(i, j, nullptr);
        }
    }
}
}


// Deze functie wordt opgeroepen telkens er op het schaakbord
// geklikt wordt. x,y geeft de positie aan waar er geklikt
// werd; r is de 0-based rij, k de 0-based kolom
void SchaakGUI::clicked(int r, int k) {

    // Wat hier staat is slechts een voorbeeldje dat wordt afgespeeld ter illustratie.
    // Jouw code zal er helemaal anders uitzien en zal enkel de aanpassing in de spelpositie maken en er voor
    // zorgen dat de visualisatie (al dan niet via update) aangepast wordt.

    // Volgende schaakstukken worden aangemaakt om het voorbeeld te illustreren.
    // In jouw geval zullen de stukken uit game g komen
    pair<int, int> clickedPos(r, k);
    SchaakStuk *clickedItem = g.getPiece(r, k);     // can be a piece or a nullptr (empty square)
//    if (clickedPiece!=nullptr) {
//        g.move(clickedPiece, 5, 4);     // para 's' CAN'T be a nullptr !!!
//
//    }
//    update();


//     Check if a valid piece (own color) was selected




    if (!pieceSelected) {
        if (clickedItem != nullptr) {
            if (clickedItem->getKleur() == wit && g.turn ||         // White's turn and white piece selected
                clickedItem->getKleur() == zwart && !g.turn) {   // Black's turn and black piece selected
                pieceSelected = true;
                selectedPiece = clickedItem;
                selectionPos.first = r;
                selectionPos.second = k;
                std::cout<< "PIECE SELECTED"<<'\n';

                // SET TILE/PIECE MARKING
                // selection mark
                setTileSelect(r,k,true);    // mark selected piece
                vector<pair<int, int>> validMoves = selectedPiece->geldige_zetten(g);

                // 'valid position' + 'danger zone' marks
                zw kl = zwart;
                if (selectedPiece->getKleur()==zwart) kl=wit;   // make sure "kl" (kleur) is the enemy's color
                vector<pair<int,int>> dangerZone = g.controlledSquares(kl);  // squares that the enemy controls
                for (auto p : validMoves) {
                    if (displayMoves()) setTileFocus(p.first, p.second, true);  // mark each valid position
                    setPieceThreat(p.first, p.second, false); // remove these marks during a selection
                    auto it = std::find(dangerZone.begin(), dangerZone.end(), p);
                    if (it != dangerZone.end() && displayThreats()) setTileThreat(p.first, p.second, true);     // mark this square as danger zone

                    // note: displayThreats() returns whether this box is ticked in GUI
                    // same for displayMoves, displayKills
                }


            }
        }

        return;
    }

    // pieceSelected must be true at this point (first click done)
    // selectedPiece must be a valid piece (own color) at this point
    // the second click can be: 1) the target position   OR   2) the current selection -> unselect
    pair<int,int> myPos = selectedPiece->getPos(); // original position of the selected piece
    if (selectionPos == clickedPos) {     // The current selection is clicked again -> unselect
        pieceSelected = false;  cout <<"UNSELECTED\n";
        vector<pair<int,int>> validMoves = selectedPiece->geldige_zetten(g);
        setTileSelect(r,k,false);
        for (auto p : validMoves) {
            setTileFocus(p.first, p.second, false);  // remove the 'valid position marks'
            setTileThreat(p.first, p.second, false); // remove tile threat marks
        }

        // mark the piece threats again
        zw oppkleur = wit; if (selectedPiece->getKleur()==zwart) oppkleur = zwart;
        vector<pair<int,int>> threatenedPieces = g.piecesInVision(oppkleur);
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                setPieceThreat(i, j, false);    // reset previous marks

                // Set current marks pieces that can be captured by kleur
                // Note: displayKills() returns whether this box is ticked in GUI
                if (g.turn && displayKills()) {   // It is white's turn
                    for (auto p : threatenedPieces) {
                        setPieceThreat(p.first, p.second, true);
                    }
                }
                else if (!g.turn && displayKills()) {     // it is black's turn
                    for (auto p : threatenedPieces) {
                        setPieceThreat(p.first, p.second, true);
                    }
                }
            }

        }
        update(); // update gui to show marking

        return;
        // No need to reset selectionPos
    }
    vector<pair<int, int>> validMoves = selectedPiece->geldige_zetten(g);
    // set tile focus

    // check if the clickedPos is a valid move
    auto it = std::find(validMoves.begin(), validMoves.end(), clickedPos);
    if (it != validMoves.end()) {       // Valid position
        g.move(selectedPiece, clickedPos.first, clickedPos.second);

        g.ud_lastMovingPiece.push_back(selectedPiece);  // purpose = undo func
        g.ud_prevPos.push_back(selectionPos);   // original pos of last moving piece

        // empty the Redo-Stacks
        g.rd_prevPos.clear();
        g.rd_lastMovingPiece.clear();


        // check for PROMOTION

        if (selectedPiece->getNaam()==pion &&
                (clickedPos.first==0 || clickedPos.first==7))
            g.promote(clickedPos.first, clickedPos.second);



        // note: at this point, selectedPiece is still the piece that just moved

        // Check for En Passant:
        // We check if a pawn just moved 2 squares. In that case we set epSquare to the pos behind the pawn
        // In the following turn of the opponent, any pawn that stands next to pawn that previously moved (2 squares) can capture EP

        zw oppkleur = wit; if (selectedPiece->getKleur()==wit) oppkleur = zwart;
        vector<pair<int,int>> threatenedPieces = g.piecesInVision(oppkleur);
        if (selectedPiece->getNaam()==pion &&
            abs(clickedPos.first - myPos.first)==2) {     // The pawn just moved 2 squares
            // EP square is the square behind the pawn
            if (g.turn) g.enPassantSquare = pair<int,int>(selectionPos.first-1, selectionPos.second);   // if white's turn
            else if (!g.turn)  g.enPassantSquare = pair<int,int>(selectionPos.first+1, selectionPos.second);    // if black's turn
            g.epTarget=pair<int,int>(clickedPos.first, clickedPos.second);
            SchaakStuk* epTargetPiece = g.getPiece(g.epTarget.first, g.epTarget.second);

            // make sure the epTarget will be marked as 'piece threat' (see bottom of func)
            // check if this pawn landed next to an enemy pawn -> this pawn is threatened because of ep
            SchaakStuk* lSquare = g.getPiece(g.epTarget.first, g.epTarget.second-1);    // square to the left of epTarget
            SchaakStuk* rSquare = g.getPiece(g.epTarget.first, g.epTarget.second+1);    // square to the right of epTarget

            if (lSquare != nullptr &&
            lSquare->getNaam()==pion &&
            lSquare->getKleur() != epTargetPiece->getKleur())   // enemy pawn to the left -> ep possible
                 {threatenedPieces.push_back(g.epTarget);}
            if (rSquare != nullptr &&
                rSquare->getNaam()==pion &&
                rSquare->getKleur() != epTargetPiece->getKleur())   // enemy pawn to the right -> ep possible
            {threatenedPieces.push_back(g.epTarget);}

        }
        else {
            g.enPassantSquare = pair<int, int>(-1, -1); // reset

            // initialise threatenedPieces vector with its default values
            // this is for the 'piece threat' tile marks to work ()
        }
        if (selectedPiece->getNaam()==koning && selectedPiece->getKleur()==zwart) g.bKingHasMoved=true;
        else if (selectedPiece->getNaam()==koning && selectedPiece->getKleur()==wit) g.wKingHasMoved=true;
        // remove tile colors
        setTileSelect(selectionPos.first, selectionPos.second, false);  // remove selection color of selected piece
        for (auto p : validMoves) {
            setTileFocus(p.first, p.second, false);  // remove the 'valid position marks'
            setTileThreat(p.first, p.second, false); // remove threat marks
        }



        g.moveCount++;        // increment movecount
        g.turn = (g.moveCount%2==0);    // update turn status (true if white's turn, false for black)
        pieceSelected=false;    // 'reset' this function.
        // next time this function is called, it will be the opponent's turn
        if (g.schaak(wit)) cout << "WHITE IN CHECK\n";
        if (g.schaak(zwart)) cout << "BLACK IN CHECK\n";
        if (g.schaakmat(wit)) cout << "BLACK WON! (checkmate)\n";
        if (g.schaakmat(zwart)) cout << "White WON! (checkmate)\n";
        if (g.pat(zwart)) cout << "TIE! (black in stalemate)\n";
        if (g.pat(wit)) cout << "TIE! (white in stalemate)\n";

        // Set piece threat marks (pieces that player can capture)
        // these marks are visible any time there is NO selection going on
        // the marks shown depend on who's turn it is.


        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                setPieceThreat(i, j, false);    // reset previous marks

                // Set current marks pieces that can be captured by kleur
                // Note: displayKills() returns whether this box is ticked in GUI
                if (g.turn && displayKills()) {   // It is white's turn
                    for (auto p : threatenedPieces) {
                        setPieceThreat(p.first, p.second, true);
                    }
                }
                else if (!g.turn && displayKills()) {     // it is black's turn
                    for (auto p : threatenedPieces) {
                        setPieceThreat(p.first, p.second, true);
                    }
                }
            }

        }
        // todo: segfault
        //todo: const schaakstuk??
//        if (!g.turn) g.aiMoves();

        update();

        return;
    }
}



void SchaakGUI::newGame()
{

}


void SchaakGUI::save() {
    QFile file;
    if (openFileToWrite(file)) {
        QDataStream out(&file);
        out << QString("Rb") << QString("Hb") << QString("Bb") << QString("Qb") << QString("Kb") << QString("Bb") << QString("Hb") << QString("Rb");
        for  (int i=0;i<8;i++) {
            out << QString("Pb");
        }
        for  (int r=3;r<7;r++) {
            for (int k=0;k<8;k++) {
                out << QString(".");
            }
        }
        for  (int i=0;i<8;i++) {
            out << QString("Pw");
        }
        out << QString("Rw") << QString("Hw") << QString("Bw") << QString("Qw") << QString("Kw") << QString("Bw") << QString("Hw") << QString("Rw");
    }
}

void SchaakGUI::open() {
    QFile file;
    if (openFileToRead(file)) {
        try {
            QDataStream in(&file);
            QString debugstring;
            for (int r=0;r<8;r++) {
                for (int k=0;k<8;k++) {
                    QString piece;
                    in >> piece;
                    debugstring += "\t" + piece;
                    if (in.status()!=QDataStream::Ok) {
                        throw QString("Ongeldig formaat");
                    }
                }
                debugstring += "\n";
            }
            message(debugstring);
        } catch (QString& Q) {
            message(Q);
        }
    }
    update();
}


void SchaakGUI::undo() {
    message("UNDO");

    if (g.ud_prevPos.empty()) return;   // Undo-stacks empty -> = starting pos -> no undo possible

    pair<int,int> originalPos = g.ud_prevPos.back();    // target pos of this undo
    SchaakStuk* s = g.ud_lastMovingPiece.back();
    pair<int,int> myPos(s->getPos());

    // update Redo-stacks
    g.rd_lastMovingPiece.push_back(s);
    g.rd_prevPos.push_back(myPos);

    g.setPiece(originalPos.first, originalPos.second, s);   // move last moving piece back
    s->setPos(originalPos);
    if (g.ud_capturedPiece.back() != nullptr) g.setPiece(myPos.first, myPos.second, g.ud_capturedPiece.back());   // restore captured piece
    else g.setPiece(myPos.first, myPos.second, nullptr);  // clear square again if there was no capture
    update();

    // pop the Undo-Stacks
    // todo: make this 1 stack?
    g.ud_capturedPiece.pop_back();
    g.ud_prevPos.pop_back();
    g.ud_lastMovingPiece.pop_back();

    if (g.turn) g.turn = false; else g.turn = true;     // switch who's turn it is
    g.moveCount--;  // decrement movecount
}

void SchaakGUI::redo() {
    message("REDO");

    if (g.rd_lastMovingPiece.empty()) return; // Redo-stacks are empty


    // Note: No pieces to restore in an undo. (Just like regular moves)
    pair<int,int> originalPos = g.rd_prevPos.back();    // target pos of this redo
    SchaakStuk* s = g.rd_lastMovingPiece.back();
    pair<int,int> myPos(s->getPos());

    g.setPiece(originalPos.first, originalPos.second, s);
    s->setPos(originalPos);
    g.setPiece(myPos.first, myPos.second, nullptr);  // clear original square
    update();

    g.rd_prevPos.pop_back();
    g.rd_lastMovingPiece.pop_back();
    if (g.turn) g.turn = false; else g.turn = true;     // switch who's turn it is
    g.moveCount++; // increment movecount
}


void SchaakGUI::visualizationChange() {
    QString visstring = QString(displayMoves()?"T":"F")+(displayKills()?"T":"F")+(displayThreats()?"T":"F");
    message(QString("Nieuwe settings : ")+visstring);
}









