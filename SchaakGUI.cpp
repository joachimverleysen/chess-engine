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

void SchaakGUI::update() {
    clearBoard();

    for (int i=0; i<8; i++) {
        for (int j=0; j<8; j++) {
            if (g.schaakbord[i][j] != nullptr) {
                setItem(i, j, g.getPiece(i, j));
            }

        }
    }
}

void SchaakGUI::doPieceThreatMarking() {
    if (!displayThreats()) return;
    zw colorToMark = wit;
    if (!g.whiteToMove()) colorToMark = zwart;
    vector<pair<int,int>> threatenedPieces = g.piecesInVision(colorToMark);
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            for (auto p : threatenedPieces) {
                setPieceThreat(p.first, p.second, true);
            }

        }

    }
}

void SchaakGUI::updateMarking() {
    zw oppkleur = wit; if (selectedPiece->getKleur()==zwart) oppkleur = zwart;
    vector<pair<int,int>> threatenedPieces = g.piecesInVision(oppkleur);
    if (!pieceSelected) {
        doPieceThreatMarking();
        return;
    }
    setTileSelect(selectedPiece->getPos().first, selectedPiece->getPos().second,true);
    vector<pair<int, int>> validMoves = selectedPiece->validMoves(g);

    // 'valid position' + 'danger zone' marks
    zw color = zwart;
    if (selectedPiece->getKleur()==zwart) color=wit;
    vector<pair<int,int>> dangerZone = g.controlledSquares(color);
    for (auto p : validMoves) {
        if (displayMoves()) setTileFocus(p.first, p.second, true);
        setPieceThreat(p.first, p.second, false);
        auto it = std::find(dangerZone.begin(), dangerZone.end(), p);
        if (it != dangerZone.end() && displayThreats()) setTileThreat(p.first, p.second, true);

    }
}

void SchaakGUI::clicked(int r, int k) {
    if (g.schaakmat(wit) || g.schaakmat(zwart) || g.pat(wit) || g.pat(zwart)) return;
    pair<int, int> clickedPos(r, k);
    clickedItem = g.getPiece(r, k);

    if (!pieceSelected && clickedItem == nullptr)
        return;
    if (!pieceSelected && clickedItem->getKleur() != g.colorToMove())
        return;

    else if (!pieceSelected) {
        pieceSelected=true;
        selectedPiece = clickedItem;
        selectionPos.first = r;
        selectionPos.second = k;
        updateMarking();

        return;
    }

    pair<int,int> activePiecePosition = selectedPiece->getPos();
    if (selectionPos == clickedPos) {
        pieceSelected = false;
        vector<pair<int,int>> validMoves = selectedPiece->validMoves(g);
        removeAllMarking();
        updateMarking();
        update();

        return;
    }
    vector<pair<int, int>> validMoves = selectedPiece->validMoves(g);

    auto it = std::find(validMoves.begin(), validMoves.end(), clickedPos);
    if (it != validMoves.end()) {
        g.move(selectedPiece, clickedPos.first, clickedPos.second);
        removeAllMarking();
        updateGameInfo(clickedPos, activePiecePosition, validMoves);

    }
        update();

}


void SchaakGUI::updateGameInfo(const pair<int, int> clickedPos, const pair<int, int> myPos, const vector<pair<int, int>> &validMoves) {
    g.moveCount++;

    g.undoStack.push(selectedPiece, clickedItem, selectionPos);


    if (selectedPiece->getNaam()==pion &&
        (clickedPos.first==0 || clickedPos.first==7))
        g.promote(clickedPos.first, clickedPos.second);

    g.updateEnPassantTarget(clickedPos, myPos, selectedPiece, selectionPos);
    if (selectedPiece->getNaam()==koning && selectedPiece->getKleur()==zwart) g.blackKingMoved=true;
    else if (selectedPiece->getNaam()==koning && selectedPiece->getKleur()==wit) g.whiteKingMoved=true;

    pieceSelected=false;
    updateMarking();



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
    if (!g.playAgainstAI) {  // not playing AI = go 1 move back (so black can also move again)
        message("UNDO");


        if (g.undoStack.previous_position.empty()) return;   // Undo-stacks empty -> = starting pos -> no undo possible

        pair<int, int> previous_position = g.undoStack.previous_position.back();    // target pos of this undo
        SchaakStuk *piece = g.undoStack.last_piece.back();
        pair<int, int> my_position(piece->getPos());

        auto captured = g.undoStack.captured_piece.back();

        g.setPiece(previous_position.first, previous_position.second, piece);   // move last moving piece back
        piece->setPos(previous_position);
        if (!g.undoStack.captured_piece.empty() &&g.undoStack.captured_piece.back() != nullptr)
            g.setPiece(my_position.first, my_position.second, captured);   // restore captured piece
        else g.setPiece(my_position.first, my_position.second, nullptr);  // clear square again if there was no capture
        update();


        // Check for castle
/*        if ((g.castling_rook_stack.back().first != nullptr) && piece->getNaam() == koning) {
            auto rook = g.castling_rook_stack.back().first;
            auto rook_pos_orig = g.castling_rook_stack.back().second;
            auto rookPos = rook->getPos();
            g.setPiece(rook_pos_orig.first, rook_pos_orig.second, rook);
            g.setPiece(rookPos.first, rookPos.second, nullptr);
            rook->setPos(rook_pos_orig);
            if (piece->getKleur()==wit) g.whiteKingMoved =false;
            else g.blackKingMoved = false;
            pair<SchaakStuk*, pair<int, int>> castling_rook_pair(rook, rookPos);
            g.rd_castling_rook_stack.push_back(castling_rook_pair);
            update();

        }*/




        // pop the Undo-Stacks
        g.undoStack.pop();
        g.redoStack.push(piece, captured, my_position);
//        g.castling_rook_stack.pop_back();

        g.moveCount--;  // decrement movecount
    }
}

void SchaakGUI::redo() {
    message("REDO");

    if (g.redoStack.last_piece.empty()) return; // Redo-stacks are empty


    // Note: No pieces to restore in an undo. (Just like regular moves)
    pair<int,int> previous_position = g.redoStack.previous_position.back();    // target pos of this redo
    SchaakStuk* piece = g.redoStack.last_piece.back();
    pair<int,int> my_position(piece->getPos());

    auto captured = g.getPiece(previous_position.first, previous_position.second);
    g.setPiece(previous_position.first, previous_position.second, piece);
    piece->setPos(previous_position);
    g.setPiece(my_position.first, my_position.second, nullptr);  // clear original square
    update();
/*    if (piece->getNaam() == koning && abs(previous_position.second-my_position.second)>1) {     // Castle
        auto rook = g.rd_castling_rook_stack.back().first;
        auto rookPos = rook->getPos();
        auto rook_prev_pos = g. rd_castling_rook_stack.back().second;
        g.setPiece(rook_prev_pos.first, rook_prev_pos.second, rook);
        g.setPiece(rookPos.first, rookPos.second, nullptr);
        rook->setPos(rook_prev_pos);
        pair<SchaakStuk*, pair<int, int>> castling_rook_pair(rook, rookPos);
        g.castling_rook_stack.push_back(castling_rook_pair);


        update();

    }*/



    g.redoStack.pop();
    g.undoStack.push(piece, captured, my_position);


    g.moveCount++; // increment movecount
}


void SchaakGUI::visualizationChange() {
    QString visstring = QString(displayMoves()?"T":"F")+(displayKills()?"T":"F")+(displayThreats()?"T":"F");
    message(QString("Nieuwe settings : ")+visstring);
}









