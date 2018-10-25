// 2048 main file - Arrow keys (Windows) and WASD (Unix)
// 

#include "iostream"
#include "string"
#include "vector" // to store grid values
#include "ctime" // for better rng
#include "climits" // for INT_MAX in int validation
#include "fstream" // for saving highscore
using namespace std;

#ifdef __unix__
int _getch() { return 0; } // overload _getch() to return false
void clearScreen() {
	cout << string(50, '\n');
	//system("clear");
}
#elif defined(_WIN32) || defined(WIN32)
#include "conio.h" // for arrow keys (_getch)
void clearScreen() {
	system("CLS");
}
#else
void clearScreen() {
	system("clear||CLS");
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
	void setup(); // ask for grid size, set up grid
	void readBestScore(); // set bestScore variable to score from bestscore.txt file
	void saveBestScore(); // output bestScore to bestscore.txt file
};

void theBoard::printBoard() {
	clearScreen(); // clear screen
	string currentRow; // text string of a row of numbers in the grid
	string currentCell; // value of cell at grid coordinate as a string
	cout << " Join the numbers and get to the 2048 tile!" << endl;
	cout << " Press \"n\" to start a new game or \"u\" to undo." << endl;
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
			printBoard();
			return 0; // continue playing
		}
		else if (playAgain == "N" || playAgain == "NO") {
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
	boardGrid boardCopyCopy = boardCopy; // create backup of backup in case no move is made
	boardCopy = board; // create backup of board
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
		} while (board[tempRow][tempCol] >= 2); // make sure random location is empty
		int startInt = (rand() % 10 < 9) ? 2 : 4; // 10% chance of 4
		board[tempRow][tempCol] = startInt;
		moves++; // add 1 to moves
	}
	else { // if nothing moved
		boardCopy = boardCopyCopy; // go back to previous backup to allow undo
	}

	bestScore = score > bestScore ? score : bestScore; // set bestScore to score if score is larger
}

int theBoard::beginMove() {
	if (_getch()) { // if able to run _getch
		/* Non-standard input method using <conio.h> and _getch(): */
		string playAgain;
		bool keyPressed = false;
		// See key scan codes: https://docs.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-6.0/aa299374(v=vs.60)
		while (!keyPressed) {
			switch (_getch()) { // wait for key to be pressed
			case 72: //KEY UP
				shiftGrid('U');
				keyPressed = true;
				break;
			case 77: //KEY RIGHT
				shiftGrid('R');
				keyPressed = true;
				break;
			case 80: //KEY DOWN
				shiftGrid('D');
				keyPressed = true;
				break;
			case 75: //KEY LEFT
				shiftGrid('L');
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
				}
				if (playAgain == "Y" || playAgain == "YES") {
					saveBestScore();
					cout << endl;
					return 1; // play again
					keyPressed = true;
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
				break;
			default:
				break;
			}
		}
	}
	else {
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
					return 1; // play again
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
		} while (board[tempRow][tempCol] >= 2); // // set bestScore to score if score is larger
		int startInt = (rand() % 10 < 9) ? 2 : 4; // 10% chance of 4
		board[tempRow][tempCol] = startInt;
	}
}

void theBoard::readBestScore() {
	string bestScoreString = "";
	char letter;
	ifstream fileIn("bestscore.txt");
	if (fileIn) {
		for (int i = 0; !fileIn.eof(); i++) {
			fileIn.get(letter);
			bestScoreString += letter;
		}
		if (bestScoreString.size() > 1) { // not blank file
			bestScore = stoi(bestScoreString);
		}
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

int main() {
	srand((unsigned int)time(NULL)); // reset timer for better rng
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
	system("pause"); // pause before closing console
	return 0;
}