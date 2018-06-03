#pragma once

#include <vector>
#include <cassert>
#include <iostream>
#include <random>

enum class gameState { PLAYING, WON_WHITE, WON_BLACK, DRAW };
enum class pieceName { EMPTY, PAWN, ROOK, BISHOP, KNIGHT, KING, QUEEN };
enum class pieceColor { EMPTY, WHITE, BLACK };

inline std::ostream& operator<<(std::ostream& os, const pieceName& p) {
	switch(p) {
		case pieceName::EMPTY:		os << " ";	return os;
		case pieceName::PAWN:		os << "P";	return os;
		case pieceName::ROOK:		os << "R";	return os;
		case pieceName::BISHOP:		os << "B";	return os;
		case pieceName::KNIGHT:		os << "N";	return os;
		case pieceName::KING:		os << "K";	return os;
		case pieceName::QUEEN:		os << "Q";	return os;
	}
	return os;
}
inline std::ostream& operator<<(std::ostream& os, const pieceColor& c) {
	switch(c) {
		case pieceColor::WHITE:	os << "w";	return os;
		case pieceColor::BLACK:	os << "b";	return os;
		case pieceColor::EMPTY: os << " ";	return os;
	}
	return os;
}

struct Move {
	Move(unsigned f_x=0, unsigned f_y=0, unsigned t_x=0, unsigned t_y=0, bool c=false, bool p=false)
		: from_x(f_x), from_y(f_y), to_x(t_x), to_y(t_y), castles(c), enpassant(p) {}
	unsigned from_x, from_y, to_x, to_y;
	bool castles, enpassant;
};

struct Piece {
	Piece(pieceName n = pieceName::EMPTY, pieceColor c = pieceColor::EMPTY) : name(n), color(c), moved(false) {}
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
		void checkIfEndPosition();
		void calculateAllPossibleMoves(const pieceColor &player);
		std::vector<Move> getLegalMoves(const unsigned &x, const unsigned &y) const;
};