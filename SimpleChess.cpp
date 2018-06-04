#include "SimpleChess.h"

Game::Game() {
	state = gameState::PLAYING;
	turn = 0;
	/* -- Initialize board to chess starting position -- */
	unsigned piecerows = 0; unsigned pawnrows = 1; pieceColor color = pieceColor::WHITE;
	for (unsigned c = 0; c < 2; ++c) {
		for (unsigned i = 0; i < 8; ++i) {
			position.board[pawnrows][i] = new Piece(pieceName::PAWN, color);
			switch (i) {
					case 7: case 0: position.board[piecerows][i] = new Piece(pieceName::ROOK, color); break;
					case 5: case 1: position.board[piecerows][i] = new Piece(pieceName::BISHOP, color); break;
					case 6: case 2: position.board[piecerows][i] = new Piece(pieceName::KNIGHT, color); break;
					case 4: position.board[piecerows][i] = new Piece(pieceName::QUEEN, color); break;
					case 3: position.board[piecerows][i] = new Piece(pieceName::KING, color); break;
			}
		}
		piecerows = 7; pawnrows = 6; color = pieceColor::BLACK;
	}
	for (int i=2; i<6; ++i) {
		for (int j=0; j<8; ++j) {
			position.board[i][j] = new Piece();
		}
	}
	position.inCheck = false;
	calculateAllPossibleMoves(pieceColor::WHITE);
	calculateAllPossibleMoves(pieceColor::BLACK);
}

Piece* Game::getPiece(const unsigned &x, const unsigned &y) const {
	if (x < 0 || x > 7 || y < 0 || y > 7) return NULL;
	return position.board[x][y];
}

pieceColor Game::currentPlayer() const {
	pieceColor c;
	(turn % 2 == 0) ? c = pieceColor::WHITE : c = pieceColor::BLACK;
	return c;
}

pieceColor Game::nextPlayer() const {
	pieceColor c;
	(turn % 2 == 0) ? c = pieceColor::BLACK : c = pieceColor::WHITE;
	return c;
}

void Game::move(Move m) {
	pieceColor player = currentPlayer();
	assert(state == gameState::PLAYING);
	assert(getPiece(m.from_x, m.from_y)->color = player);

	movePieceTo(m);

	if (m.castles) {
		if (m.from_y < m.to_y) {	// Queenside Castling
			if (player == pieceColor::WHITE) movePieceTo(Move(7, 7, 7, 4));
			else movePieceTo(Move(0, 7, 0, 4));
		}
		else {						// Kingside Castling
			if (player == pieceColor::WHITE) movePieceTo(Move(7, 0, 7, 2));
			else movePieceTo(Move(0, 0, 0, 2));
		}
	}
	else if (m.enpassant) {
		unsigned direction;
		(player == pieceColor::WHITE) ? direction = 1 : direction = -1;
		position.board[m.to_x + direction][m.to_y] = new Piece();
	}

	++turn;
	calculateAllPossibleMoves(nextPlayer());
	position.inCheck = isInCheck();
	calculateAllPossibleMoves(player);
	checkIfEndPosition();
}

void Game::movePieceTo(Move m) {
	Piece* from = getPiece(m.from_x, m.from_y);
	Piece* to = getPiece(m.to_x, m.to_y);

	*to = *from;
	to->moved = true;
	position.board[m.from_x][m.from_y] = new Piece();
}

bool Game::isInCheck() const {
	pieceColor c = nextPlayer();
	for (unsigned i=0; i < 8; ++i) {
		for (unsigned j=0; j < 8; ++j) {
			Piece* attacker = getPiece(i, j);
			if (attacker->color == c) {
				for (const auto &m : attacker->legalMoves) {
					Piece* attackedSquare = getPiece(m.to_x, m.to_y);
					if (attackedSquare->name == pieceName::KING && attackedSquare->color != c)
						return true;
				}
			}
		}
	}
	return false;
}

void Game::checkIfEndPosition() {
	bool movesLeft = false;
	for (int i=0; i < 8; ++i) {
		for (int j=0; j < 8; ++j) {
			Piece* p = getPiece(i, j);
			if (p->color == currentPlayer() && !p->legalMoves.empty()) {
				movesLeft = true; break;
			}
		}
	}
	if (!movesLeft) {
		if (position.inCheck) {
			switch(currentPlayer()) {
				case pieceColor::WHITE: state = gameState::WON_WHITE; break;
				case pieceColor::BLACK: state = gameState::WON_BLACK; break;
				default: break;
			}
		}
		else state = gameState::DRAW;
	}
}

void Game::calculateAllPossibleMoves(const pieceColor &player) {
	for (unsigned i=0; i < 8; ++i) {
		for (unsigned j=0; j < 8; ++j) {
			Piece* p = getPiece(i, j);
			if (p->color == player) p->legalMoves = getLegalMoves(i, j);
		}
	}
}

std::vector<Move> Game::getLegalMoves(const unsigned &x, const unsigned &y) const {
	std::vector<Move> legalMoves;
	Piece* p = getPiece(x, y);

	/* -- adds move, makes sure the move is legal in general -- */
	auto addMove = [&] (Move m) -> bool {
		Piece* from = getPiece(m.from_x, m.from_y);
		Piece* to = getPiece(m.to_x, m.to_y);
		if (from != NULL && to != NULL
				&& from->color != to->color
				&& m.from_x >= 0 && m.from_x < 8
				&& m.from_y >= 0 && m.from_y < 8
				&& m.to_x >= 0 && m.to_x < 8
				&& m.to_y >= 0 && m.to_y < 8) {
			legalMoves.push_back(m);
			return true;
		}
		return false;
	};

	auto isOpponent = [&] (unsigned dx, unsigned dy) -> bool {
		Piece* opponent = getPiece(dx, dy);
		if (opponent == NULL) return false;
		return (opponent->name != pieceName::EMPTY
				&& opponent->color != p->color);
	};

	switch(p->name) {
		case pieceName::PAWN: {
			unsigned direction;
			(p->color == pieceColor::WHITE) ? direction = 1 : direction = -1;

			/* -- one step -- */
			if (getPiece(x + direction, y)->name == pieceName::EMPTY)
				addMove(Move(x, y, x + direction, y));

			/* -- two steps -- */
			if (!p->moved
					&& getPiece(x + direction, y)->name == pieceName::EMPTY
					&& getPiece(x + (2 * direction), y)->name == pieceName::EMPTY)
				addMove(Move(x, y, x + (2 * direction), y));

			/* -- taking -- */
			for (auto leftOrRight : { -1, 1 }) {
				if (isOpponent(x + direction, y + leftOrRight))
					addMove(Move(x, y, x + direction, y + leftOrRight));
			}

			/* -- en passant -- */
			bool isEnPassantSquare;
			(direction > 0) ? isEnPassantSquare = (x == 3) : isEnPassantSquare = (x == 4);
			if (isEnPassantSquare) {
				for (auto leftOrRight : { -1, 1 })
					if (isOpponent(x, y + leftOrRight) && getPiece(x + direction, y + leftOrRight)->name == pieceName::EMPTY)
						addMove(Move(x, y, x + direction, y + leftOrRight));
			}
		} break;
		case pieceName::QUEEN:
		case pieceName::ROOK: {
			for (int n=1; n < 9 && addMove(Move(x, y, x + 0, y + n)) && !isOpponent(x + 0, y + n); ++n);	// right
			for (int n=1; n < 9 && addMove(Move(x, y, x + 0, y - n)) && !isOpponent(x + 0, y - n); ++n);	// left
			for (int n=1; n < 9 && addMove(Move(x, y, x + n, y + 0)) && !isOpponent(x + n, y + 0); ++n);	// up
			for (int n=1; n < 9 && addMove(Move(x, y, x - n, y + 0)) && !isOpponent(x - n, y + 0); ++n);	// down
			if (p->name != pieceName::QUEEN) break;
		}
		case pieceName::BISHOP: {
			for (int n=1; n < 9 && addMove(Move(x, y, x + n, y + n)) && !isOpponent(x + n, y + n); ++n);	// up-right
			for (int n=1; n < 9 && addMove(Move(x, y, x + n, y - n)) && !isOpponent(x + n, y - n); ++n);	// up-left
			for (int n=1; n < 9 && addMove(Move(x, y, x - n, y + n)) && !isOpponent(x - n, y + n); ++n);	// down-right
			for (int n=1; n < 9 && addMove(Move(x, y, x - n, y - n)) && !isOpponent(x - n, y - n); ++n);	// down-left
		} break;
		case pieceName::KNIGHT: {
			for (auto dx : { -1, 1 }) {
				for (auto dy : { -1, 1 }) {
					addMove(Move(x, y, x + (dx * 2), y + (dy * 1)));
					addMove(Move(x, y, x + (dx * 1), y + (dy * 2)));
				}
			}
		} break;
		case pieceName::KING: {
			for (auto dx : { -1, 0, 1 }) {
				for (auto dy : { -1, 0, 1}) {
					addMove(Move(x, y, x+dx, y+dy));
				}
			}
			if (!p->moved) {
				/* -- queenside castling -- */
				Piece* rook;
				(p->color == pieceColor::WHITE) ? rook = getPiece(7, 7) : rook = getPiece(0, 7);
				if (!rook->moved
						&& getPiece(x, 4)->name == pieceName::EMPTY
						&& getPiece(x, 5)->name == pieceName::EMPTY
						&& getPiece(x, 6)->name == pieceName::EMPTY)
					addMove(Move(x, y, 5, y, true));
				/* -- kingside castling -- */
				(p->color == pieceColor::WHITE) ? rook = getPiece(7, 0) : rook = getPiece(0, 0);
				if (!rook->moved
						&& getPiece(x, 2)->name == pieceName::EMPTY
						&& getPiece(x, 1)->name == pieceName::EMPTY)
					addMove(Move(x, y, 1, y, true));
			}
		} break;
		default: break;
	}
	return legalMoves;
}

/* --------------------------------------
 * -- PART BELOW IS USED AS AN EXAMPLE --
 * --------------------------------------
*/


void print(const Game &g) {
	std::cout << "------------------------------------------------ MOVE " << g.turn << " -------------"<< std::endl;
	for (unsigned i=0; i < 8; ++i) {
		for (unsigned j=0; j < 8; ++j) {
			g.getPiece(i, j)->print();
			std::cout << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

int random(int a, int b) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(a, b);
	return dis(gen);
}

void waitForInput() {
	std::cout << "Press enter to continue...";
	std::cin.ignore();
	for (int i=0; i < 11; ++i) std::cout << "\e[A";
}

int main() {
	Game g;
	while (g.state == gameState::PLAYING) {
		print(g);
		waitForInput();

		/* -- do random move -- */
		Piece* p;
		do { p = g.getPiece(random(0, 7), random(0, 7));
		} while (p == NULL || p->color != g.currentPlayer() || p->legalMoves.empty());
		g.move(p->legalMoves.front());
	}
	/* -- game ended -- */
	print(g);
	switch(g.state) {
		case gameState::DRAW: std::cout << "Game is a draw!" << std::endl; break;
		case gameState::WON_WHITE: std::cout << "White won!" << std::endl; break;
		case gameState::WON_BLACK: std::cout << "Black won!" << std::endl; break;
		default: break;
	}
	return 0;
}