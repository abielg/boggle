/*
 * File: Boggle.cpp
 * ----------------
 * Name: Abiel Gutierrez
 * Section: Kenji Hata
 * This file is the main starter file for Assignment #4, Boggle.
 * This program implements the game of Boggle. The user can choose either a 4x4 or 5x5
 * configuration, as well as to whether have a random set of characters on the board or
 * a personalized set. Once the board is ready, the user must enter all words he/she can
 * form; the program will give feedback on them being correct or incorrect. Then it will
 * be the computer's turn, which will find all remaining words existant in the english
 * word lexicon. The scoreboard will be updated with each new word, and the user will
 * have the option to play again.
 */
 
#include <iostream>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
#include "math.h"
#include <cctype>
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;
const int LENGTH_ERROR = 0;
const int INEXISTANT_ERROR = 1;
const int REPEATED_ERROR = 2;
const int IMPOSSIBLE_ERROR = 3;
const int MILLISECS_PAUSE = 500;


const string STANDARD_CUBES[16]  = {
   "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
   "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
   "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
   "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};
 
const string BIG_BOGGLE_CUBES[25]  = {
   "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
   "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
   "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
   "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
   "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

/* Function prototypes */

void welcome();
void giveInstructions();
bool wantsBigBoard();
string checkYesOrNoResponse(string response);
bool wantsForcedConfiguration();
string setUpForcedConfiguration(bool bigBoard);
string setUpRandomConfiguration(bool bigBoard);
string getCubeLettersFromVector(Vector<string> & vec);
void writeLettersOnCubes(string cubeLetters);
void humanTurn(Lexicon & englishWords, Grid<char> & letterGrid, Lexicon & usedWords);
void printError(int error);
Grid<char> makeGrid(string lettersOnCube);
bool gridContainsWord(Grid<char> letterGrid, string response, int startRow, int startCol);
void highlightWord(Grid<char> letterGrid, bool flag);
void computerTurn(Lexicon & englishWords, Grid<char> letterGrid, string currentWord, Lexicon & usedWords, int startRow, int startCol);
bool wantsToPlayAgain();


/* Main program */

int main() {
   GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
   initGBoggle(gw);
   welcome();
   giveInstructions();
   while (true) {
        bool bigBoard = wantsBigBoard();
        if (bigBoard)
            drawBoard(5,5);
        else
            drawBoard(4,4);
        string lettersOnCube;
        if (wantsForcedConfiguration())
            lettersOnCube = setUpForcedConfiguration(bigBoard);
        else
            lettersOnCube = setUpRandomConfiguration(bigBoard);
        Grid<char> letterGrid = makeGrid(lettersOnCube);
        Lexicon englishWords("EnglishWords.dat");
        Lexicon usedWords;
         humanTurn(englishWords, letterGrid, usedWords);
        computerTurn(englishWords, letterGrid, "", usedWords, -1, -1);
        cout << endl << "That's the final score.";
        if (!wantsToPlayAgain())
            break;
   }

   return 0;
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */

void welcome() {
   cout << "Welcome!  You're about to play an intense game ";
   cout << "of mind-numbing Boggle.  The good news is that ";
   cout << "you might improve your vocabulary a bit.  The ";
   cout << "bad news is that you're probably going to lose ";
   cout << "miserably to this little dictionary-toting hunk ";
   cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions()
 * --------------------------
 * Print out the instructions for the user.
 */
void giveInstructions() {
   cout << endl;
   cout << "The boggle board is a grid onto which ";
   cout << "I will randomly distribute cubes. These ";
   cout << "6-sided cubes have letters rather than ";
   cout << "numbers on the faces, creating a grid of ";
   cout << "letters on which you try to form words. ";
   cout << "You go first, entering all the words you can ";
   cout << "find that are formed by tracing adjoining ";
   cout << "letters. Two letters adjoin if they are next ";
   cout << "to each other horizontally, vertically, or ";
   cout << "diagonally. A letter can only be used once ";
   cout << "in each word. Words must be at least four ";
   cout << "letters long and can be counted only once. ";
   cout << "You score points based on word length: a ";
   cout << "4-letter word is worth 1 point, 5-letters ";
   cout << "earn 2 points, and so on. After your puny ";
   cout << "brain is exhausted, I, the supercomputer, ";
   cout << "will find all the remaining words and double ";
   cout << "or triple your paltry score." << endl << endl;
   cout << "Hit return when you're ready...";
   getLine();
}

/*
 * Function: wantsBigBoard
 * Usage: if (wantsBigBoard())
 * --------------------------
 * Asks the user whether he wants to play with a 4x4 or a 5x5 board; returns true if a 5x5
 * board is wanted; false otherwise.
 */
bool wantsBigBoard(){
    cout << endl << "You can choose standard Boggle (4x4 grid) or Big Boggle (5x5)." << endl;
    while (true) {
        string userResponse = getLine("Would you like Big Boggle?");
        if (checkYesOrNoResponse(userResponse) == "true")
            return true;
        else if (checkYesOrNoResponse(userResponse) == "false")
            return false;
        else
            cout << "Please answer yes or no" << endl;
    }
}

/*
 * Function: checkYesOrNoResponse
 * Usage: if (checkYesOrNoResponse(userResponse))
 * --------------------------
 * Asks the user whether he wants to play with a 4x4 or a 5x5 board; returns a "true" if a 5x5
 * board is wanted; "false" if its not wanted, and "error" if the user's response is extraneous.
 */
string checkYesOrNoResponse(string response){
    if (response == "yes" || response == "y" || response == "YES" || response == "Y")
        return "true";
    else if (response == "no" || response == "n" || response == "NO" || response == "N")
        return "false";
    else
        return "error";
}

/*
 * Function: wantsForcedConfiguration()
 * Usage: if (wantsForcedConfiguration())
 * --------------------------
 * Asks the user whether he wants to input his own board configuration. Returns true if he/she
 * does; false otherwise.
 */
bool wantsForcedConfiguration(){
    cout << endl << "I'll give you a chance to set up the board to your specification, which makes";
    cout << "it easier to confirm your boggle program is working." << endl;
    while (true) {
        string userResponse = getLine("Do you want to force the board configuration?");
        if (checkYesOrNoResponse(userResponse) == "true")
            return true;
        else if (checkYesOrNoResponse(userResponse) == "false")
            return false;
        else
            cout << "Please answer yes or no" << endl;
    }
}

/*
 * Function: setUpForcedConfiguration(bool bigBoard)
 * Usage: setUpForcedConfiguration(bigBoard)
 * --------------------------
 * Asks user to input a string with 16 or 25 characters (depending on the size of the board) and
 * then calls the methodwriteLettersOnCube to put each letter from the string into a cube. This
 * method works for either board configuration.
 */
string setUpForcedConfiguration(bool bigBoard) {
    int totalNumberOfCubes;
    int cubesPerRow;

    if (bigBoard) {
        totalNumberOfCubes = 25;
        cubesPerRow = 5;
    } else {
        totalNumberOfCubes = 16;
        cubesPerRow = 4;
    }
    cout << endl << "Enter a " << totalNumberOfCubes << "-character string to identify which letters you ";
    cout << "want on the cubes. The first " << cubesPerRow << " letters are the cubes on the top row from ";
    cout << "left to right, the next " << cubesPerRow << " letters are the second row, and so on." << endl;
    string cubeLetters = "";
    while (true) {
        string str = getLine("Enter the string: ");
        if (str.length() >= totalNumberOfCubes) {
            cubeLetters = str;
            break;
        }
        else
            cout << endl << "Please enter at least " << totalNumberOfCubes << " characters." << endl;
    }
    if (bigBoard)
        writeLettersOnCubes(cubeLetters.substr(0,25));
    else
        writeLettersOnCubes(cubeLetters.substr(0,16));
    return cubeLetters;
}

/*
 * Function: setUpRandomConfiguration(bool bigBoard)
 * Usage: setUpRandomConfiguration(bigBoard)
 * --------------------------
 * Shuffles the predetermined cubes into a random order, and then sends them to getCubeLettersFromVector.
 * This method will return a string with the letters that will be placed in each cube, and this method
 * will pass this string to writeLettersOnCubes to make the actual writing.
 */
string setUpRandomConfiguration(bool bigBoard) {
    Vector<string> vec;

    if (bigBoard) {
        for (string str : BIG_BOGGLE_CUBES) {
            vec.add(str);
        }
    } else {
        for (string str : STANDARD_CUBES) {
            vec.add(str);
        }
    }

    for (int i = 0; i < vec.size(); i++) {
        int randomIndex = randomInteger(i,vec.size() - 1);
        string temp = vec[i];
        vec[i] = vec[randomIndex];
        vec[randomIndex] = temp;
    }
    string cubeLetters = getCubeLettersFromVector(vec);
    writeLettersOnCubes(cubeLetters);
    return cubeLetters;
}

/*
 * Function: getCubeLettersFromVector(Vector<string> & vec)
 * Usage: string cubeLetters = getCubeLettersFromVector(vec)
 * --------------------------
 * Takes in a vector of cubes formatted as strings, chooses a random value for each one, and
 * returns a string with only one letter from each cube.
 */
string getCubeLettersFromVector(Vector<string> & vec) {
    string result = "";
    for (string str : vec) {
        int randomIndex = randomInteger(0,5);
        result += str[randomIndex];
    }
    return result;
}

/*
 * Function: writeLettersOnCubes(string cubeLetters)
 * Usage: writeLettersOnCubes(cubeLetters)
 * --------------------------
 * Takes in a string of letters, and prints them in the cubes in order from left to right, top to down.
 */
void writeLettersOnCubes(string cubeLetters) {
    string letters = cubeLetters;
    int length = sqrt(double(cubeLetters.size()));
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            labelCube(i,j,letters[0]);
            letters = letters.substr(1);
        }
    }
}

/*
 * Function: humanTurn(Lexicon & englishWords, Grid<char> & letterGrid)
 * Usage: humanTurn(englishWords, letterGrid)
 * --------------------------
 * Asks user to enter his responses according to the letters on the board. If his response is illegal,
 * and error message is printed with the reason why its wrong. If the response is correct, the word
 * is added to the lexicon of usedWords and the method recordWordForPlayer is called with the word as
 * an argument.
 */
void humanTurn(Lexicon & englishWords, Grid<char> & letterGrid, Lexicon & usedWords) {
    cout << endl << "Ok, take all the time you want and find all the words you can! Signal that you're";
    cout << " finished by entering an empty line." << endl;
    while(true) {
        string response = getLine("Enter a word: ");
        if (response == "")
            break;
        response = toUpperCase(response);
        if (response.length() < 4)
            printError(LENGTH_ERROR);
        else if (!englishWords.contains(response))
            printError(INEXISTANT_ERROR);
        else if (usedWords.contains(response))
            printError(REPEATED_ERROR);
        else if (!gridContainsWord(letterGrid, response, -1, -1))
            printError(IMPOSSIBLE_ERROR);
        else {
            usedWords.add(response);
            recordWordForPlayer(response, HUMAN);
        }
    }
}

/*
 * Function: printError(int error)
 * Usage: printError(LENGTH_EROR)
 * --------------------------
 * Gets called whenever a user's response is incorrect. The 'error' argument determines which
 * error message is going to be printed, according to which mistake was made by the user.
 */
void printError(int error) {
    switch (error) {
        case LENGTH_ERROR: cout << endl << "I'm sorry, but we have our standards." << endl;
                       cout << "That word does not meet our minimum word length" << endl; break;
        case INEXISTANT_ERROR: cout << endl << "That's not a word!" << endl; break;
        case REPEATED_ERROR: cout << endl << "You've already guessed that!" << endl; break;
        case IMPOSSIBLE_ERROR: cout << endl << "You can't make that word!" << endl; break;
    }
}

/*
 * Function: makeGrid(string lettersOnCube)
 * Usage: Grid<char> letterGrid =  makeGrid(lettersOnCube)
 * --------------------------
 * Takes in a string of letters, and places them all in a grid in order from left to right, top to
 * down. Then returns the grid. This grid is representative of the boggle game board.
 */
Grid<char> makeGrid(string lettersOnCube) {
    int length = sqrt(double(lettersOnCube.size()));
    string letters = lettersOnCube;
    Grid<char> result(length,length);
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            result.set(i,j,letters[0]);
            letters = letters.substr(1);
        }
    }
    return result;
}

/*
 * Function: gridContainsWord(Grid<char> letterGrid, string response, int startRow, int startCol)
 * Usage: if(!gridContainsWord(letterGrid, response, -1, -1))
 * --------------------------
 * Checks if the 'response' inputed by the user can be formed in 'letterGrid'. Uses recursive calls
 * at two different points to accomplish this: once after finding the first letter, and then everytime
 * after finding a subsequent letter. If the response is correct, the words are highlighted on the grid
 * and 'true' is returned.
 */
bool gridContainsWord(Grid<char> letterGrid, string response, int startRow, int startCol) {
    //base case: if response is an empty string
    if (response.length() == 0) {
        highlightWord(letterGrid, true);
        return true;
    }

    //Finds first letter on the board
    if (startRow == -1 || startCol == -1) {
        for (int i = 0; i < letterGrid.numRows(); i++) {
            for (int j = 0; j < letterGrid.numCols(); j++) {
                if (letterGrid[i][j] == response[0]) {
                    Grid<char> newGrid = letterGrid;
                    newGrid[i][j] = EOF;
                    //recursive call -- only called after getting the first letters of a word
                    if (gridContainsWord(newGrid, response.substr(1), i, j))
                        return true;
                }
            }
        }
        //This gets called when the first letter of the user's response was not on the board.
        return false;
    }

    for (int i = startRow - 1; i <= startRow + 1; i++) {
        for (int j = startCol - 1; j <= startCol + 1; j++) {
            if (letterGrid.inBounds(i,j) && letterGrid[i][j] == response[0]) {
                Grid<char> newGrid = letterGrid;
                newGrid[i][j] = EOF;
                //recursive call
                if (gridContainsWord(newGrid, response.substr(1), i, j))
                    return true;
            }
        }
    }
    return false;
}

/*
 * Function: highlightWord(Grid<char> letterGrid, bool flag)
 * Usage: highlightWord(letterGrid, true)
 * --------------------------
 * Highlights the squares that make up the user's correct response by identifying which ones are
 * marked as EOF and calling the method highlightCube for each of them. It has one recursive call
 * in which the argument 'flag' gets changed to false, to revert the colors. The grid of characters
 * is passed in as an argument from the gridContainsWord method.
 */
void highlightWord(Grid<char> letterGrid, bool flag) {
    for (int i = 0; i < letterGrid.numRows(); i++) {
        for (int j = 0; j < letterGrid.numCols(); j++) {
            if (letterGrid[i][j] == EOF)
                highlightCube(i,j,flag);
        }
    }
    if (!flag)
        return;
    pause(MILLISECS_PAUSE);
    //recursive call to change the colors back to the original after 500 milliseconds
    highlightWord(letterGrid, !flag);
}

/*
 * Function: computerTurn(Lexicon & englishWords, Grid<char> letterGrid, string currentWord, Lexicon & wordsUsedByComputer, int startRow, int startCol)
 * Usage: computerTurn(englishWords, letterGrid, "", wordsUsedByComputer, -1, -1)
 * --------------------------
 * Highlights the squares that make up the user's correct response by identifying which ones are
 * marked as EOF and calling the method highlightCube for each of them. It has one recursive call
 * in which the argument 'flag' gets changed to false, to revert the colors. The grid of characters
 * is passed in as an argument from the gridContainsWord method.
 */
void computerTurn(Lexicon & englishWords, Grid<char> letterGrid, string currentWord, Lexicon & usedWords, int startRow, int startCol) {
    //This is called only when we're looking for the first letter
    if (currentWord == "") {
        cout << endl << "It's the computer's turn!";
        for (int i = 0; i < letterGrid.numRows(); i++) {
            for (int j = 0; j < letterGrid.numCols(); j++) {
                Grid<char> newGrid = letterGrid;
                newGrid[i][j] = EOF;
                string newWord;
                newWord += letterGrid[i][j];
                //recursive call
                computerTurn(englishWords, newGrid, newWord, usedWords, i, j);
            }
        }
        return;
    }
    //prunes dead end searches
    if (!englishWords.containsPrefix(currentWord)) {
        return;
    }
    //if currentWord is a word, this records it on the scoreboard
    if (englishWords.contains(currentWord) && currentWord.length() > 3 && !usedWords.contains(currentWord)) {
        usedWords.add(currentWord);
        recordWordForPlayer(currentWord, COMPUTER);
    }
    //Looks for new words
    for (int i = startRow - 1; i <= startRow + 1; i++) {
        for (int j = startCol - 1; j <= startCol + 1; j++) {
            if (letterGrid.inBounds(i,j) && letterGrid[i][j] != EOF) {
                Grid<char> newGrid = letterGrid;
                newGrid[i][j] = EOF;
                string newWord = currentWord + letterGrid[i][j];
                //recursive call
                computerTurn(englishWords, newGrid, newWord, usedWords, i, j);
            }
        }
    }

}

/*
 * Function: wantsToPlayAgain()
 * Usage: if (!wantsToPlayAgain()) break;
 * --------------------------
 * Asks the user if he wants to play again. If the answer is no, 'false' is returned and the
 * while loop in main() method is broken to end the program.
 */
bool wantsToPlayAgain() {
    cout << endl << "Would you like to play again?" << endl;
    while (true) {
        string userResponse = getLine("Enter response: ");
        if (checkYesOrNoResponse(userResponse) == "true")
            return true;
        else if (checkYesOrNoResponse(userResponse) == "false")
            return false;
        else
            cout << "Please answer yes or no" << endl;
    }
}
