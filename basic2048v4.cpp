// 2048 main file - WASD key controls and no clearing of console
//

#include "iostream"
#include "string"
#include "vector" // to store grid values
#include "ctime" // for better rng
#include "climits" // for INT_MAX in int validation
#include "fstream" // for saving highscore
using namespace std;

typedef vector <int> boardRow; // one row as vector
typedef vector <boardRow> boardGrid; // vector of vectors

class theBoard {
private:
	bool alreadyWon = false;
	int score = 0;
	int bestScore = 0;
	int moves = 0;
	int gridSize;
	boardGrid board;
	boardGrid boardCopy;
public:
	void printBoard();
	int checkIfGameOver();
	int beginMove();
	void setup();
	void shiftGrid(char dir);
	void setBestScore(string bestScoreString);
	void readBestScore();
	void saveBestScore();
};

void theBoard::printBoard() {
	string currentRow;
	string currentCell;
	cout << endl;
	// top of table:
	cout << " +";
	for (int j = 0; j < gridSize - 1; j++) {
		cout << "-----------";
	}
	cout << "----------+" << endl;
	// title line:
	cout << " |";
	string title = " SCORE: " + to_string(score);
	title += " - BEST: " + to_string(bestScore);
	title += " - MOVES: " + to_string(moves);
	cout << title;
	int titleLength = title.size();
	if (titleLength < (11 * gridSize)) {
		for (int j = 1; j < ((11 * gridSize) - titleLength); j++) {
			cout << " ";
		}
		cout << "| ";
	}
	// top of grid:
	cout << endl << " +";
	for (int j = 0; j < gridSize; j++) {
		cout << "----------+";
	}
	cout << endl;
	// remainder of grid:
	for (int j = 0; j < gridSize; j++) {
		currentRow = " |";
		for (int i = 0; i < gridSize; i++) {
			currentCell = to_string(board[j][i]);
			if (currentCell == "0") { // show nothing if zero
				currentRow += "          ";
			}
			// different spacing depending on num of digits:
			else if (currentCell.length() == 1) {
				currentRow += "     " + currentCell + "    ";
			}
			else if (currentCell.length() == 2) {
				currentRow += "    " + currentCell + "    ";
			}
			else if (currentCell.length() == 3) {
				currentRow += "    " + currentCell + "   ";
			}
			else if (currentCell.length() == 4) {
				currentRow += "   " + currentCell + "   ";
			}
			else if (currentCell.length() == 5) {
				currentRow += "   " + currentCell + "  ";
			}
			else {
				currentRow += currentCell;
			}
			currentRow += "|";
		}
		cout << " |";
		for (int j = 0; j < gridSize; j++) {
			cout << "          |";
		}
		cout << endl << currentRow << endl;
		cout << " |";
		for (int j = 0; j < gridSize; j++) {
			cout << "          |";
		}
		cout << endl << " +";
		for (int j = 0; j < gridSize; j++) {
			cout << "----------+";
		}
		cout << endl;
	}
}

int theBoard::checkIfGameOver() {
	bool hasAMove = false;
	int numOfValues = 0;
	int maxValue = 0;
	for (int i = 0; i < gridSize; i++) { // ex. row 1 -> 4
		for (int j = 0; j < gridSize; j++) { // ex. col 1 -> 4
			if (board[i][j] >= 2) { // not empty
				numOfValues++; // count value to determine if full
				if (board[i][j] > maxValue) {
					maxValue = board[i][j]; // to find highest value
				}
			}
		}
	}
	if (maxValue == 2048 && alreadyWon == false) {
		alreadyWon = true;
		cout << endl << " You won!" << endl;
		string playAgain;
		while (playAgain != "Y" && playAgain != "N" && playAgain != "YES" && playAgain != "NO") {
			cout << " Continue? (Y/N) ";
			cin >> playAgain;
			int responseLength = playAgain.length();
			for (int i = 0; i < responseLength; i++) {
				playAgain.at(i) = toupper(playAgain.at(i));
			}
		}
		if (playAgain == "Y" || playAgain == "YES") {
			return 1; // play again
		}
		else if (playAgain == "N" || playAgain == "NO") {
			return -1; // exit
		}
	}
	if (numOfValues == gridSize * gridSize) { // grid is full
		for (int i = 1; i < gridSize; i++) { // ex. row 2 -> 4
			for (int j = 0; j < gridSize; j++) { // ex. col 1 -> 4
				if (board[i][j] == board[i - 1][j]) { // value is not same as value above
					hasAMove = true;
				}
			}
		}
		for (int j = 1; j < gridSize; j++) { // ex. col 2 -> 4
			for (int i = 0; i < gridSize; i++) { // ex. row 1 -> 4
				if (board[i][j] == board[i][j - 1]) { // value is not same as value to left
					hasAMove = true;
				}
			}
		}
		if (!hasAMove) { // to do: print board before end
			cout << endl << " Game Over!" << endl;
			saveBestScore();
			string playAgain;
			while (playAgain != "Y" && playAgain != "N" && playAgain != "YES" && playAgain != "NO") {
				cout << " Play again? (Y/N) ";
				cin >> playAgain;
				int responseLength = playAgain.length();
				for (int i = 0; i < responseLength; i++) {
					playAgain.at(i) = toupper(playAgain.at(i));
				}
			}
			if (playAgain == "Y" || playAgain == "YES") {
				return 1; // play again
			}
			else if (playAgain == "N" || playAgain == "NO") {
				return -1; // exit
			}
		}
	}
	return 0; // continue playing
}

void theBoard::shiftGrid(char dir) {
	boardCopy = board;
	bool nothingMoved = true; // if no moves, won't add a number
	// SHIFT UP:
	if (dir == 'U') {
		//shift up to 3 times to empty if possible
		for (int k = 0; k < gridSize - 1; k++) { // loop (gridSize - 1) times to go from bottom to top
			for (int j = 0; j < gridSize - 1; j++) { // ex. row 1 -> 3
				for (int i = 0; i < gridSize; i++) { // ex. col 1 -> 4
					if (board[j][i] < 2 && board[j + 1][i] >= 2) {
						board[j][i] = board[j + 1][i];
						board[j + 1][i] = 0;
						nothingMoved = false;
					}
				}
			}
		}
		// merge if possible
		for (int j = 0; j < gridSize - 1; j++) { // ex. row 1 -> 3
			for (int i = 0; i < gridSize; i++) { // ex. col 1 -> 4
				if (board[j][i] >= 2 && board[j + 1][i] == board[j][i]) {
					score += board[j][i] * 2;
					board[j][i] = (board[j][i] * 2);
					board[j + 1][i] = 0;
					nothingMoved = false;
				}
			}
		}
		//shift 1 more time to empty if possible
		for (int j = 0; j < gridSize - 1; j++) { // ex. row 1 -> 3
			for (int i = 0; i < gridSize; i++) { // ex. col 1 -> 4
				if (board[j][i] < 2 && board[j + 1][i] >= 2) {
					board[j][i] = board[j + 1][i];
					board[j + 1][i] = 0;
					nothingMoved = false;
				}
			}
		}
	}
	// SHIFT RIGHT:
	else if (dir == 'R') {
		//shift up to 3 times to empty if possible
		for (int k = 0; k < gridSize - 1; k++) { // loop (gridSize - 1) times to go from top to bottom
			for (int i = gridSize - 1; i > 0; i--) { // ex. col 4 -> 2
				for (int j = 0; j < gridSize; j++) { // ex. row 1 -> 4
					if (board[j][i] < 2 && board[j][i - 1] >= 2) {
						board[j][i] = board[j][i - 1];
						board[j][i - 1] = 0;
						nothingMoved = false;
					}
				}
			}
		}
		// merge if possible
		for (int i = gridSize - 1; i > 0; i--) { // ex. col 4 -> 2
			for (int j = 0; j < gridSize; j++) { // ex. row 1 -> 4
				if (board[j][i] >= 2 && board[j][i - 1] == board[j][i]) {
					score += board[j][i] * 2;
					board[j][i] = (board[j][i] * 2);
					board[j][i - 1] = 0;
					nothingMoved = false;
				}
			}
		}
		//shift 1 more time to empty if possible
		for (int i = gridSize - 1; i > 0; i--) { // ex. col 4 -> 2
			for (int j = 0; j < gridSize; j++) { // ex. row 1 -> 4
				if (board[j][i] < 2 && board[j][i - 1] >= 2) {
					board[j][i] = board[j][i - 1];
					board[j][i - 1] = 0;
					nothingMoved = false;
				}
			}
		}
	}
	// SHIFT DOWN:
	else if (dir == 'D') {
		//shift up to 3 times to empty if possible
		for (int k = 0; k < gridSize - 1; k++) { // loop (gridSize - 1) times to go from top to bottom
			for (int j = gridSize - 1; j > 0; j--) { // ex. row 4 -> 2
				for (int i = 0; i < gridSize; i++) { // ex. col 1 -> 4
					if (board[j][i] < 2 && board[j - 1][i] >= 2) {
						board[j][i] = board[j - 1][i];
						board[j - 1][i] = 0;
						nothingMoved = false;
					}
				}
			}
		}
		// merge if possible
		for (int j = gridSize - 1; j > 0; j--) { // ex. row 4 -> 2
			for (int i = 0; i < gridSize; i++) { // ex. col 1 -> 4
				if (board[j][i] >= 2 && board[j - 1][i] == board[j][i]) {
					score += board[j][i] * 2;
					board[j][i] = (board[j][i] * 2);
					board[j - 1][i] = 0;
					nothingMoved = false;
				}
			}
		}
		//shift 1 more time to empty if possible
		for (int j = gridSize - 1; j > 0; j--) { // ex. row 4 -> 2
			for (int i = 0; i < gridSize; i++) { // ex. col 1 -> 4
				if (board[j][i] < 2 && board[j - 1][i] >= 2) {
					board[j][i] = board[j - 1][i];
					board[j - 1][i] = 0;
					nothingMoved = false;
				}
			}
		}
	}
	// SHIFT LEFT:
	else if (dir == 'L') {
		//shift up to 3 times to empty if possible
		for (int k = 0; k < gridSize - 1; k++) { // loop (gridSize - 1) times to go from top to bottom
			for (int i = 0; i < gridSize - 1; i++) { // ex. col 1 -> 3
				for (int j = 0; j < gridSize; j++) { // ex. row 1 -> 4
					if (board[j][i] < 2 && board[j][i + 1] >= 2) {
						board[j][i] = board[j][i + 1];
						board[j][i + 1] = 0;
						nothingMoved = false;
					}
				}
			}
		}
		// merge if possible
		for (int i = 0; i < gridSize - 1; i++) { // ex. col 1 -> 3
			for (int j = 0; j < gridSize; j++) { // ex. row 1 -> 4
				if (board[j][i] >= 2 && board[j][i + 1] == board[j][i]) {
					score += board[j][i] * 2;
					board[j][i] = (board[j][i] * 2);
					board[j][i + 1] = 0;
					nothingMoved = false;
				}
			}
		}
		//shift 1 more time to empty if possible
		for (int i = 0; i < gridSize - 1; i++) { // ex. col 1 -> 3
			for (int j = 0; j < gridSize; j++) { // ex. row 1 -> 4
				if (board[j][i] < 2 && board[j][i + 1] >= 2) {
					board[j][i] = board[j][i + 1];
					board[j][i + 1] = 0;
					nothingMoved = false;
				}
			}
		}
	}

	// generate new number and add to board:
	if (!nothingMoved) { // if something moved
		int tempRow = rand() % gridSize;
		int tempCol = rand() % gridSize;
		do {
			tempRow = rand() % gridSize;
			tempCol = rand() % gridSize;
		} while (board[tempRow][tempCol] >= 2);
		int startInt = (rand() % 10 < 9) ? 2 : 4; // 10% chance of 4
		board[tempRow][tempCol] = startInt;
		moves++; // add 1 to moves
	}

	bestScore = score > bestScore ? score : bestScore;
}

int theBoard::beginMove() {
	/* Alternate method using cin instead of _getch (WASD): */
	string playAgain;
	bool keyPressed = false;
	while (!keyPressed) {
		cout << "Choose a direction (WASD): ";
		char dir;
		cin >> dir;
		dir = toupper(dir);
		if (dir == 'N') {
			playAgain = "";
			cout << endl << " Are you sure you want to start a new game?" << endl;
			while (playAgain != "Y" && playAgain != "N" && playAgain != "YES" && playAgain != "NO") {
				cout << " New Game? (Y/N) ";
				cin >> playAgain;
				int responseLength = playAgain.length();
				for (int i = 0; i < responseLength; i++) {
					playAgain.at(i) = toupper(playAgain.at(i));
				}
			}
			if (playAgain == "Y" || playAgain == "YES") {
				saveBestScore();
				cout << endl;
				return 1; // new game
				keyPressed = true;
			}
		}
		else if (dir == 'U') {
			playAgain = "";
			cout << endl << " Are you sure you want to undo?" << endl;
			while (playAgain != "Y" && playAgain != "N" && playAgain != "YES" && playAgain != "NO") {
				cout << " Undo? (Y/N) ";
				cin >> playAgain;
				int responseLength = playAgain.length();
				for (int i = 0; i < responseLength; i++) {
					playAgain.at(i) = toupper(playAgain.at(i));
				}
			}
			if (playAgain == "Y" || playAgain == "YES") {
				if (board != boardCopy) { // haven't already used backup
					board = boardCopy;
					printBoard();
					keyPressed = true;
				}
				else {
					cout << "Sorry. You may not undo any further." << endl;
				}
			}
		}
		else if (dir == 'W' || dir == 'A' || dir == 'S' || dir == 'D') {
			dir = dir == 'W' ? 'U' : dir == 'A' ? 'L' : dir == 'S' ? 'D' : dir == 'D' ? 'R' : dir; // convert WASD to ULDR:
			shiftGrid(dir);
			keyPressed = true;
		}
	}

	return 0; // continue playing
}

void theBoard::setup() {
	score = 0;
	moves = 0;
	gridSize = 0;
	alreadyWon = false;
	cout << " Join the numbers and get to the 2048 tile!" << endl;
	cout << " HOW TO PLAY: Use your arrow keys to move the tiles." << endl;
	cout << " When two tiles with the same number touch, they merge into one!" << endl;
	cout << " Press \"n\" to start a new game or \"u\" to undo." << endl << endl;
	while (true) {
		cout << " How many rows and columns? ";
		cin >> gridSize;
		if (cin.good() && gridSize >= 2 && gridSize <= 16) {
			break;
		}
		else {
			cout << " Please enter a valid positive integer between 2 and 16." << endl;
			cin.clear();
			cin.ignore(INT_MAX, '\n');
		}
	}

	board = boardGrid(gridSize, boardRow(gridSize)); // set size of a vector of vectors (2d matrix)
	boardCopy = board;

	// create first 2 numbers:
	for (int k = 0; k < 2; k++) {
		int tempRow = rand() % gridSize;
		int tempCol = rand() % gridSize;
		int i = 0;
		do {
			tempRow = rand() % gridSize;
			tempCol = rand() % gridSize;
			i++;
		} while (board[tempRow][tempCol] >= 2 && i < gridSize * gridSize);
		int startInt = (rand() % 10 < 9) ? 2 : 4; // 10% chance of 4
		board[tempRow][tempCol] = startInt;
	}
}

void theBoard::setBestScore(string bestScoreString) {
	bestScore = stoi(bestScoreString);
};

void theBoard::readBestScore() {
	string bestScoreString = "";
	char letter;
	ifstream fileIn("bestscore.txt");
	if (fileIn) {
		for (int i = 0; !fileIn.eof(); i++) {
			fileIn.get(letter);
			bestScoreString += letter;
		}
		if (bestScoreString.size() > 1) {
			setBestScore(bestScoreString);
		}
		fileIn.close();
	}
}

void theBoard::saveBestScore() {
	ofstream fileOut("bestscore.txt");
	if (fileOut) {
		fileOut << bestScore << endl;
		fileOut.close();
	}
}

int main() {
	srand((unsigned int)time(NULL)); // reset timer for better rng
	theBoard newBoard;
	newBoard.setup(); // call function to set up game
	newBoard.readBestScore();
	int GameOver = false;
	while (true) {
		// print board:
		newBoard.printBoard();
		GameOver = newBoard.checkIfGameOver();
		if (GameOver == 0) { // continue playing
			GameOver = newBoard.beginMove();
		}
		if (GameOver == 1) { // play again
			newBoard.setup();
		}
		else if (GameOver == -1) { // exit
			break;
		}
	}
	return 0;
}