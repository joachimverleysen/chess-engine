//
// Created by toonc on 12/17/2021.
//

#include "SchaakGUI.h"
#include "guicode/message.h"
#include "guicode/fileIO.h"
#include "game.h"
#include <iostream>
#include <algorithm>
using std::cout;


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

void SchaakGUI::handleSelection(int r, int k) {
    clickedItem = g.getPiece(r, k);
    if (clickedItem == nullptr) return;
    if (clickedItem->getKleur() != g.colorToMove()) return;
    pieceSelected=true;
    selectedPiece = clickedItem;
    selectionPos = {r, k};
    updateMarking();
}

void SchaakGUI::clicked(int r, int k) {
    if (g.schaakmat(wit) || g.schaakmat(zwart) || g.pat(wit) || g.pat(zwart)) return;
    pair<int, int> clickedPos(r, k);
    clickedItem = g.getPiece(r, k);

    if (!pieceSelected) {
        handleSelection(r, k);
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
        updateGameInfo(clickedPos, activePiecePosition);

    }
    update();
}

void SchaakGUI::updateGameInfo(const pair<int, int> clickedPos, const pair<int, int> current_pos) {
    g.moveCount++;

    g.undoStack.push(selectedPiece, clickedItem, selectionPos);
    g.redoStack.push(nullptr, clickedItem, selectionPos);

    CastlingRook castlingRook(nullptr, pair<int, int>());
    if (g.isCastleMove_) {
        castlingRook.piece = g.castlingRook.piece;
        castlingRook.position = g.castlingRook.position;
    }
    g.undoStack.pushCastlingRook(castlingRook);

    if (selectedPiece->getNaam()==pion &&
        (clickedPos.first==0 || clickedPos.first==7)) {
        g.promote(clickedPos.first, clickedPos.second);
    }

    if (selectedPiece->getNaam()==koning && selectedPiece->getKleur()==wit &&
    g.firstWhiteKingMove==-1) {
        g.firstWhiteKingMove = g.moveCount;
    }
    if (selectedPiece->getNaam()==koning && selectedPiece->getKleur()==zwart &&
        g.firstBlackKingMove==-1) {
        g.firstBlackKingMove = g.moveCount;
    }

    g.updateEnPassantTarget(clickedPos, current_pos, selectedPiece);
    if (selectedPiece->getNaam()==koning && selectedPiece->getKleur()==zwart);
    else if (selectedPiece->getNaam()==koning && selectedPiece->getKleur()==wit);

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
    message("UNDO");

    if (g.undoStack.previous_position.empty()) return;
    pair<int, int> previous_position = g.undoStack.previous_position.back();
    SchaakStuk *piece = g.undoStack.moving_piece.back();
    pair<int, int> current_position(piece->getPos());

    auto captured = g.undoStack.captured_piece.back();

    g.setPiece(previous_position.first, previous_position.second, piece);
    piece->setPos(previous_position);
    if (!g.undoStack.captured_piece.empty() && g.undoStack.captured_piece.back() != nullptr)
        g.setPiece(current_position.first, current_position.second, captured);
    else g.setPiece(current_position.first, current_position.second, nullptr);
    update();

    if ((g.undoStack.castling_rook.back().piece != nullptr) && piece->getNaam() == koning) {
        CastlingRook castling_rook(
                g.undoStack.castling_rook.back().piece,
                g.undoStack.castling_rook.back().piece->getPos()
        );
        auto rook_pos_orig = g.undoStack.castling_rook.back().position;
        auto rookPos = castling_rook.piece->getPos();
        g.setPiece(rook_pos_orig.first, rook_pos_orig.second, castling_rook.piece);
        g.setPiece(rookPos.first, rookPos.second, nullptr);
        castling_rook.piece->setPos(rook_pos_orig);
        if (piece->getKleur()==wit);
        g.redoStack.pushCastlingRook(castling_rook);
        update();
    }

    if (piece->getNaam()==koning) {
        if (piece->getKleur()==wit && g.firstWhiteKingMove==g.moveCount) {
            g.firstWhiteKingMove = -1;
        }
    }
    if (piece->getNaam()==koning) {
        if (piece->getKleur()==zwart && g.firstBlackKingMove==g.moveCount) {
            g.firstBlackKingMove = -1;
        }
    }

    g.undoStack.pop();
    g.undoStack.popCastlingRook();
    g.redoStack.push(piece, captured, current_position);
    g.moveCount--;  // decrement movecount
}

void SchaakGUI::redo() {
    message("REDO");

    if (g.redoStack.moving_piece.empty()) return;
    if (g.redoStack.moving_piece.back() == nullptr) return;

    pair<int,int> previous_position = g.redoStack.previous_position.back();
    SchaakStuk* piece = g.redoStack.moving_piece.back();
    pair<int,int> current_position(piece->getPos());

    auto captured = g.getPiece(previous_position.first, previous_position.second);
    g.setPiece(previous_position.first, previous_position.second, piece);
    piece->setPos(previous_position);
    g.setPiece(current_position.first, current_position.second, nullptr);
    update();

    CastlingRook castlingRook(nullptr, pair<int,int>());

    pair<SchaakStuk*, pair<int, int>> castling_rook_pair(nullptr, pair<int,int>());

    if (piece->getNaam() == koning && abs(previous_position.second - current_position.second) > 1) {
        auto rook = g.redoStack.castling_rook.back().piece;
        auto rookPos = rook->getPos();
        auto rook_prev_pos = g. redoStack.castling_rook.back().position;
        g.setPiece(rook_prev_pos.first, rook_prev_pos.second, rook);
        g.setPiece(rookPos.first, rookPos.second, nullptr);
        rook->setPos(rook_prev_pos);
        castlingRook.piece=rook;
        castlingRook.position=rookPos;
        g.redoStack.popCastlingRook();
        update();

    }

    g.redoStack.pop();
    g.undoStack.push(piece, captured, current_position, castlingRook);

    g.moveCount++;
}


void SchaakGUI::visualizationChange() {
    QString visstring = QString(displayMoves()?"T":"F")+(displayKills()?"T":"F")+(displayThreats()?"T":"F");
    message(QString("Nieuwe settings : ")+visstring);
}