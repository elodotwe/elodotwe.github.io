#include "board_handler.h"
#include <fstream>

void PrintBoard(void);
int PopulateSudokuFromUser(void);
int PopulateSudokuFromFile(void);

int SolveMethodA(void);
int SolveMethodB(void);

ss_board_handler s;

int main(int argc, char *argv[])
{
    int ret;
    cout << "Sudoku Solver  Copyright (C) 2009  Jacob Rau\n";
    cout << "This program comes with ABSOLUTELY NO WARRANTY.\n";
    cout << "This is free software, and you are welcome to redistribute it\n";
    cout << "under certain conditions; see the bundled file LICENSE for details.\n\n";
    cout << "Input Sudoku from 1)user or 2)file? ";
    cin >> ret;
    if (ret==1) ret=PopulateSudokuFromUser();
    else ret=PopulateSudokuFromFile();
    if (ret)
    {
        cout << "Populate failed. Ending program.\n";
        return 0;
    }
    
    PrintBoard();
    ret = SolveMethodA();
    while (1)
    {
        PrintBoard();
        ret = SolveMethodA();
        if (ret) continue;
        ret = SolveMethodB();
        if (ret) continue;
        break;
    }
    cout << "Final board:\n";
    PrintBoard();
    return EXIT_SUCCESS;
}

void PrintBoard(void)
{
    char x;
    char y;
    char invalid;
    invalid = 0;
    cout << "\n";
    for (y=1; y<=9; y++)
    {
        for (x=1; x<=9; x++)
        {
            if (s.SquareContents(x,y)==100)
            {
                cout << "X ";
                invalid = 1;
            }
            else if (!(s.SquareContents(x,y)==0))
                cout << (int)s.SquareContents(x,y) << " ";
            else
                cout << "- ";
        }
        cout << "\n";
    }
    if (invalid) cout << "This sudoku has at least one invalid square--a square with no possibilities.\n";
}

int PopulateSudokuFromUser(void)
{
    char x;
    char y;
    int val = 0;
    for (y = 1; y<=9;y++)
    {
        for (x = 1;x<=9;x++)
        {
            cout << "Value for (" << (int)x << ", " << (int)y << ")? (0 for unknown, 99 to quit) ";
            cin >> val;
            //val = 1;
            if (val==99) return 1;
            if (!(val==0))
            {
                if ((val < 1) || (val > 9))
                    cout << "Invalid value! Setting to unknown!\n";
                else
                    s.SetKnownSquare(x,y,val);
            }
        }
    }
    return 0;
}

int PopulateSudokuFromFile(void)
{
    char filename[32];
    cout << "Which file? ";
    cin >> filename;
    ifstream i;
    i.open(filename);
    char x,y, val;
    for (y=1;y<=9;y++)
    {
        for (x=1;x<=9;x++)
        {
            i.get(val);
            if (i.fail())
            {
                cout << "Error getting sudoku!!!\n";
                return 1;
            }
            if (!(val==0))
            {
                s.SetKnownSquare(x,y,val);
                //cout << "Populated (" << (int)x << "," << (int)y << ") with " << (int)val << ".\n";
            }
        }
    }
    cout << "Got entire sudoku successfully!\n";
    return 0;
}
    

int SolveMethodA(void) //Solve method A looks at knowns and removes them from the rest of their row block and col
{
    char x, y;
    int changes;
    changes = 0;
    char firstpart = 0;
    cout << "Starting Solve Method A:\n";
    //Loop through all the rows and columns.
    for (y=1;y<=9;y++)
    {
        for (x=1;x<=9;x++)
        {
            if (!(s.SquareContents(x,y)==0))
            {
                firstpart = 0;
                if (s.RemoveNumberFromRow(y,s.SquareContents(x,y)))
                {
                    changes++;
                    if (!firstpart) cout << "Since " << (int)s.SquareContents(x,y) << " occurs in (" << (int)x << "," << (int)y << "), ";
                    firstpart = 1;
                    cout << "I removed it from its row. ";
                }
                if (s.RemoveNumberFromColumn(x,s.SquareContents(x,y)))
                {
                    changes++;
                    if (!firstpart) cout << "Since " << (int)s.SquareContents(x,y) << " occurs in (" << (int)x << "," << (int)y << "), ";
                    firstpart = 1;
                    cout << "I removed it from its column. ";
                }
                if (s.RemoveNumberFromQuadrant(s.QuadrantX(x), s.QuadrantY(y), s.SquareContents(x,y)))
                {
                    changes++;
                    if (!firstpart) cout << "Since " << (int)s.SquareContents(x,y) << " occurs in (" << (int)x << "," << (int)y << "), ";
                    firstpart = 1;
                    cout << "I removed it from its block.";                    
                }
                if (firstpart) cout << "\n";
            }
        }
    }
    cout << "Solve method A done. ";
    if (changes > 0)
    {
        cout << "Changes were made.\n";
        return 1;
    }
    else
    {
        cout << "No changes were made.\n";
        return 0;
    }
}

int SolveMethodB(void) //SMB looks for numbers that only occur once in their row, col, and block
{
    char x, y, num, poss, i, lastx, lasty;
    char changes;
    changes = 0;
    cout << "Starting solve method B:\n";
    for (y=1;y<=9;y++)
    {
        for (num=1;num<=9;num++)
        {
            poss = 0;
            for (x=1;x<=9;x++)
                if (s.IsPossible(x,y,num)) {poss++; lastx = x;}
            if (poss==1)
            {
                for (i=1;i<=9;i++) //Crop the number
                {
                    if (!(i==num))
                        if (s.RemoveNumber(lastx,y,i)) {changes++; cout << (int)num << " only occurs once in its row, so I removed " << (int)i << " from (" << (int)lastx << ", " << (int)y << ").\n";}
                }    
            }
        }
    }
    for (x=1;x<=9;x++)
    {
        
        for (num=1;num<=9;num++)
        {
            poss = 0;
            for (y=1;y<=9;y++)
                if (s.IsPossible(x,y,num)) {poss++; lasty = y;}
            if (poss==1)
            {
                for (i=1;i<=9;i++) //Crop the number
                {
                    if (!(i==num))
                        if (s.RemoveNumber(x,lasty,i)) {changes++; cout << (int)num << " only occurs once in its column, so I removed " << (int)i << " from (" << (int)x << ", " << (int)lasty << ").\n";}
                }    
            }
        }
    }
    cout << "Ending solve method B. ";
    if (changes==0)
    {
        cout << "No changes were made.\n";
        return 0;
    }
    else
    {
        cout << "Changes were made.\n";
        return 1;
    }
}
