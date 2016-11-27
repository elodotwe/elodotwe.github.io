#include <cstdlib>
#include <iostream>

using namespace std;

class ss_square
{
    public:
        char SingleNumber(void); //Returns 0 if not a single number or the
                                   //contained number if it is.
        char NumberIsPossible(char num); //Returns nonzero if the specified
                                         //number is possible, zero if not
        void RemoveNumber(char num); //Makes number impossible
        
             ss_square(void); //Constructor, initializes number array
    private:
        char ss_available_numbers[9];
};

class ss_board_handler
{
    public:
        void SetKnownSquare(char x, char y, char value); //Sets the known value
                                                     //to the square specified.
        char SquareContents(char x, char y); //Returns 0 if the square contains
        //more than one possible number, or the value if it only contains one
                                                            //possible number.
        char IsPossible(char x, char y, char num);
        int RemoveNumber(char x, char y, char value); //Removes possible
                                               //number from specified square
        int RemoveNumberFromQuadrant(char QuadX, char QuadY, char value);
        char QuadrantX(char val);
        char QuadrantY(char val);
        int RemoveNumberFromRow(char RowY, char value);
        int RemoveNumberFromColumn(char ColX, char value);
        char IsSudokuValid(void);
        ss_board_handler(void); //The contructor. Doesn't do anything right now.
    private:
        ss_square ss_board[9][9];
};
