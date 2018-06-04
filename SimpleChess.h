#pragma once

#include <vector>
#include <cassert>
#include <iostream>
#include <random>
#include <array>
#include <limits>

enum class gameState { PLAYING, WON_WHITE, WON_BLACK, DRAW };
enum class pieceName { EMPTY, PAWN, ROOK, BISHOP, KNIGHT, KING, QUEEN };
enum class pieceColor { EMPTY, WHITE, BLACK };

struct Move {
	Move(unsigned f_x=0, unsigned f_y=0, unsigned t_x=0, unsigned t_y=0, bool c=false, bool p=false)
		: from_x(f_x), from_y(f_y), to_x(t_x), to_y(t_y), castles(c), enpassant(p) {}
	unsigned from_x, from_y, to_x, to_y;
	bool castles, enpassant;
};

struct Piece {
	Piece(pieceName n = pieceName::EMPTY, pieceColor c = pieceColor::EMPTY) : name(n), color(c), moved(false) {}
	void print() {
		switch(name) {
			case pieceName::EMPTY:	std::cout << "."; break;
			case pieceName::PAWN:	(color != pieceColor::WHITE) ? std::cout << "\u2659" : std::cout << "\u265F";	break;
			case pieceName::ROOK:	(color != pieceColor::WHITE) ? std::cout << "\u2656" : std::cout << "\u265C";	break;
			case pieceName::BISHOP:	(color != pieceColor::WHITE) ? std::cout << "\u2657" : std::cout << "\u265D";	break;
			case pieceName::KNIGHT:	(color != pieceColor::WHITE) ? std::cout << "\u2658" : std::cout << "\u265E";	break;
			case pieceName::KING:	(color != pieceColor::WHITE) ? std::cout << "\u2654" : std::cout << "\u265A";	break;
			case pieceName::QUEEN:	(color != pieceColor::WHITE) ? std::cout << "\u2655" : std::cout << "\u265B";	break;
		}
	}
	pieceName name;
	pieceColor color;

	bool moved;
	std::vector<Move> legalMoves;
};

struct Board {
	Piece* board[8][8];
	bool inCheck;
};

class Game {
	public:
		Game();	// initialize chess board and such
		void move(Move m);
		Piece* getPiece(const unsigned &x, const unsigned &y) const;
		
		pieceColor currentPlayer() const;
		pieceColor nextPlayer() const;
		unsigned turn;
		gameState state;
	private:
		Board position;

		bool isInCheck() const;
		void movePieceTo(Move m);
		void checkIfEndPosition();
		void calculateAllPossibleMoves(const pieceColor &player);
		std::vector<Move> getLegalMoves(const unsigned &x, const unsigned &y) const;
};