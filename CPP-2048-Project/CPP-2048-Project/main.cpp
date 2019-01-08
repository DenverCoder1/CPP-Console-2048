// C++ 2048 game
// Join the numbers and get to the 2048 tile!
// Use your arrow keys (Windows) or WASD (Unix) to move the tiles.
// When two tiles with the same number touch, they merge into one!
// Yonah Lawrence
// October 2018
// 

#include <iostream>
#include <string>
#include <vector> // to store grid values
#include <ctime> // for better rng
#include <fstream> // for saving highscore
using namespace std;

#if defined(_WIN32) || defined(WIN32)
#include <conio.h> // for arrow keys (_getch)
void clearScreen() {
	system("CLS");
}
#else
void clearScreen() {
	cout << string(50, '\n');
}
#endif

typedef vector <int> boardRow; // one row as vector
typedef vector <boardRow> boardGrid; // vector of vectors

class theBoard {
private:
	bool alreadyWon = false; // if already got 2048, don't ask for continue again
	int score = 0; // sum of all tiles merged (ex +8 if merged a 4 and 4)
	int bestScore = 0; // highscore
	int moves = 0; // number of times grid was shifted in a single game
	int gridSize; // number of rows/columns in current grid
	boardGrid board; // the grid
	boardGrid boardCopy; // backup of grid to allow user to undo a move
public:
	void printBoard(); // output grid to console
	int checkIfGameOver(); // check if no moves or got first 2048 tile
	void shiftGrid(char dir); // change grid by moving U, D, L, or R
	int beginMove(); // wait for input
	void insertNewNumber(); // add a 2 or 4 to a random spot on board
	void setup(); // ask for grid size, set up grid
	void readBestScore(); // set bestScore variable to score from bestscore.txt file
	void saveBestScore(); // output bestScore to bestscore.txt file
};

int main() {
	srand((unsigned int)time(nullptr)); // initialize random seed
	theBoard newBoard; // create object "newBoard" with class "theBoard"
	newBoard.setup(); // call function to set up game
	int GameOver = 0;
	newBoard.readBestScore();
	while (true) {
		// print board:
		newBoard.printBoard(); // output grid
		GameOver = newBoard.checkIfGameOver(); // check if won or lost (returns 0:continue, 1:new game, or -1:exit)
		if (GameOver == 0) { // continue playing
			GameOver = newBoard.beginMove(); // wait for input (returns 0:continue or 1:new game)
		}
		if (GameOver == 1) { // new game
			clearScreen(); // clear screen
			newBoard.setup();
		}
		else if (GameOver == -1) { // exit
			break;
		}
	}
	return 0;
}

void theBoard::printBoard() {
	clearScreen(); // clear screen
	cout << " Join the numbers and get to the 2048 tile!" << endl;
	cout << " Press \"n\" to start a new game or \"u\" to undo." << endl;

	// SCORE BOX:
	string scoreBox = " SCORE: " + to_string(score);
	scoreBox += " - BEST: " + to_string(bestScore);
	scoreBox += " - MOVES: " + to_string(moves);
	int scoreBoxLength = scoreBox.size();
	// top of score box:
	cout << " +";
	for (int j = 0; j < gridSize - 1; j++) {
		cout << "---------";
	}
	cout << "--------+" << endl;
	// score line:
	if (scoreBoxLength > 9 * gridSize - 2) { // if text is longer than grid
		// collapse to 3 lines:
		for (int j = 0; j < 3; j++) {
			switch (j) {
			case 0: scoreBox = " | SCORE: " + to_string(score); break;
			case 1: scoreBox = " | BEST: " + to_string(bestScore); break;
			case 2: scoreBox = " | MOVES: " + to_string(moves); break;
			}
			scoreBoxLength = scoreBox.size();
			cout << scoreBox;
			for (int j = 1; j < ((9 * gridSize) - scoreBoxLength); j++) {
				cout << " ";
			}
			cout << "  | " << endl;
		}
	}
	else { // if it fits in one line
		cout << " |";
		cout << scoreBox;
		if (scoreBoxLength < (9 * gridSize)) {
			for (int j = 1; j < ((9 * gridSize) - scoreBoxLength); j++) {
				cout << " ";
			}
			cout << "| " << endl;
		}
	}

	// GRID:
	string currentRow; // text string of a row of numbers in the grid
	string currentCell; // value of cell at grid coordinate as a string
	// top of grid:
	cout << " +";
	for (int j = 0; j < gridSize; j++) {
		cout << "--------+";
	}
	cout << endl;
	// remainder of grid:
	for (int j = 0; j < gridSize; j++) {
		currentRow = " |";
		for (int i = 0; i < gridSize; i++) {
			currentCell = to_string(board[j][i]);
			if (currentCell == "0") { // show nothing if zero
				currentRow += "        ";
			}
			// different spacing depending on number of digits: (10 chars with number in center (or slightly to the right)
			else {
				int digitsInCell = currentCell.length();
				for (int spaces = 0; spaces < ((8 - digitsInCell) / 2.0); spaces++) { // left spaces (if 2 digit, 4 left spaces; if 1 digit 5 left spaces)
					currentRow += " ";
				}
				currentRow += currentCell;
				for (int spaces = 0; spaces < ((8 - digitsInCell) / 2); spaces++) { // right spaces (if 2 digit, 4 right spaces; if 1 digit 4 right spaces)
					currentRow += " ";
				}
			}

			currentRow += "|";
		}
		cout << " |";
		for (int j = 0; j < gridSize; j++) {
			cout << "        |";
		}
		cout << endl << currentRow << endl;
		cout << " |";
		for (int j = 0; j < gridSize; j++) {
			cout << "        |";
		}
		cout << endl << " +";
		for (int j = 0; j < gridSize; j++) {
			cout << "--------+";
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
		string keepPlaying;
		while (keepPlaying != "Y" && keepPlaying != "N" && keepPlaying != "YES" && keepPlaying != "NO") {
			cout << " Continue? (Y/N) ";
			cin >> keepPlaying;
			int responseLength = keepPlaying.length();
			for (int i = 0; i < responseLength; i++) {
				keepPlaying.at(i) = toupper(keepPlaying.at(i));
			}
			cout << endl;
		}
		if (keepPlaying == "Y" || keepPlaying == "YES") {
			printBoard();
			return 0; // continue playing
		}
		else if (keepPlaying == "N" || keepPlaying == "NO") {
			return 1; // play again
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
		if (!hasAMove) {
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
				cout << endl;
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
	boardGrid boardCopyCopy = boardCopy; // create backup of backup in case no movement is made
	boardCopy = board; // create backup of board
	bool nothingMoved = true; // if no moves, won't add a number
	// SHIFT UP:
	if (dir == 'W') {
		//shift up to 3 times (assuming 4x4) to empty if possible
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
	else if (dir == 'D') {
		//shift up to 3 times (assuming 4x4) to empty if possible
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
	else if (dir == 'S') {
		//shift up to 3 times (assuming 4x4) to empty if possible
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
	else if (dir == 'A') {
		//shift up to 3 times (assuming 4x4) to empty if possible
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
		insertNewNumber(); // insert 2 or 4 at random
		moves++; // add 1 to moves
	}
	else { // if nothing moved
		boardCopy = boardCopyCopy; // go back to previous backup to allow undo
	}

	bestScore = score > bestScore ? score : bestScore; // set bestScore to score if score is larger
}

int theBoard::beginMove() {
#if defined(_WIN32) || defined(WIN32)
	/* Non-standard input method using <conio.h> and _getch(): */
	string playAgain;
	bool keyPressed = false;
	// See key scan codes: https://docs.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-6.0/aa299374(v=vs.60)
	while (!keyPressed) {
		switch (_getch()) { // wait for key to be pressed
		case 72: //KEY UP
		case 113: // KEY W
			shiftGrid('W');
			keyPressed = true;
			break;
		case 75: //KEY LEFT
		case 97: // KEY A
			shiftGrid('A');
			keyPressed = true;
			break;
		case 80: //KEY DOWN
		case 115: // KEY S
			shiftGrid('S');
			keyPressed = true;
			break;
		case 77: //KEY RIGHT
		case 100: // KEY D
			shiftGrid('D');
			keyPressed = true;
			break;
		case 110: //KEY 'n' (new game)
			playAgain = "";
			cout << endl << " Are you sure you want to start a new game?" << endl;
			while (playAgain != "Y" && playAgain != "N" && playAgain != "YES" && playAgain != "NO") {
				cout << " New Game? (Y/N) ";
				cin >> playAgain;
				int responseLength = playAgain.length();
				for (int i = 0; i < responseLength; i++) {
					playAgain.at(i) = toupper(playAgain.at(i));
				}
				cout << endl;
			}
			if (playAgain == "Y" || playAgain == "YES") {
				saveBestScore();
				cout << endl;
				return 1; // play again
				keyPressed = true;
			}
			else { // clear questions by reprinting board
				printBoard();
			}
			break;
		case 117: //KEY 'u' (undo)
			playAgain = "";
			cout << endl << " Are you sure you want to undo?" << endl;
			while (playAgain != "Y" && playAgain != "N" && playAgain != "YES" && playAgain != "NO") {
				cout << " Undo? (Y/N) ";
				cin >> playAgain;
				int responseLength = playAgain.length();
				for (int i = 0; i < responseLength; i++) {
					playAgain.at(i) = toupper(playAgain.at(i));
				}
				cout << endl;
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
			else { // clear questions by reprinting board
				printBoard();
			}
			break;
		default:
			break;
		}
	}
#else
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
				cout << endl;
			}
			if (playAgain == "Y" || playAgain == "YES") {
				saveBestScore();
				cout << endl;
				return 1; // play again
				keyPressed = true;
			}
			else { // clear questions by reprinting board
				printBoard();
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
				cout << endl;
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
			else { // clear questions by reprinting board
				printBoard();
			}
		}
		else if (dir == 'W' || dir == 'A' || dir == 'S' || dir == 'D') {
			shiftGrid(dir);
			keyPressed = true;
		}
	}

#endif

	return 0; // continue playing
}

void theBoard::insertNewNumber() {
	// collect a list of all available cells
	vector <int> availableSquares;
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			if (board[i][j] < 2) { // if empty square
				availableSquares.push_back(i * gridSize + j);
			}
		}
	}

	int cell = availableSquares[rand() % availableSquares.size()]; // choose random available cell

	board[cell / gridSize][cell % gridSize] = (rand() % 10 < 9) ? 2 : 4; // 10% chance of 4, 90% chance of 2
}

void theBoard::setup() {
	score = 0;
	moves = 0;
	gridSize = 0;
	alreadyWon = false;
	cout << " Join the numbers and get to the 2048 tile!" << endl;
#if defined(_WIN32) || defined(WIN32)
	cout << " HOW TO PLAY: Use your arrow keys or WASD to move the tiles." << endl;
#else
	cout << " HOW TO PLAY: Use WASD input to move the tiles." << endl;
#endif
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
			cin.ignore();
		}
	}

	board = boardGrid(gridSize, boardRow(gridSize)); // set size of a vector of vectors (2d matrix)
	boardCopy = board;

	// create first 2 numbers:
	for (int k = 0; k < 2; k++) {
		insertNewNumber(); // insert 2 or 4 at random
	}
}

void theBoard::readBestScore() {
	ifstream fileIn("bestscore.txt");
	if (fileIn) {
		fileIn >> bestScore; // read bestScore from file
		fileIn.close();
	}
}

void theBoard::saveBestScore() {
	ofstream fileOut("bestscore.txt");
	if (fileOut) {
		fileOut << bestScore << endl; // write bestScore to file
		fileOut.close();
	}
}