#include "board_handler.h"

using namespace std;

char ss_square::SingleNumber(void)
{
    char i = 0;
    char possibilities = 0;
    char last_value = 0;
    for (i = 0; i < 9; i++)
        if (!(ss_available_numbers[i]==0)) {possibilities++; last_value = (i+1);}
    if (possibilities==1) return last_value;
    else if (possibilities==0) return 100;
    else return 0;
}

char ss_square::NumberIsPossible(char num)
{
    if (!(ss_available_numbers[num-1]==0)) return 1;
    else return 0;
}

void ss_square::RemoveNumber(char num)
{
    ss_available_numbers[num-1]=0;
}

ss_square::ss_square(void)
{
    char i = 0;
    for (i=0;i<9;i++)
        ss_available_numbers[i] = 1;
}

void ss_board_handler::SetKnownSquare(char x, char y, char value)
{
    char i = 0;
    for (i = 0; i<9; i++)
        if (!((i+1)==value)) ss_board[x-1][y-1].RemoveNumber(i+1);
}

char ss_board_handler::SquareContents(char x, char y)
{
    return ss_board[x-1][y-1].SingleNumber();
}

char ss_board_handler::IsPossible(char x, char y, char num)
{
    return ss_board[x-1][y-1].NumberIsPossible(num);
}

int ss_board_handler::RemoveNumber(char x, char y, char value)
{
    if (ss_board[x-1][y-1].NumberIsPossible(value) > 0)
    {
        ss_board[x-1][y-1].RemoveNumber(value);
        return 1;
    }
    else
        return 0;
}

int ss_board_handler::RemoveNumberFromQuadrant(char QuadX, char QuadY, char value)
{
    char OrigX, OrigY;
    char x, y;
    char changes;
    changes = 0;
    
    OrigX = (QuadX - 1) * 3 + 1;
    OrigY = (QuadY - 1) * 3 + 1;

    for (x = OrigX; x < (OrigX + 3); x++)
    {
        for (y = OrigY; y < (OrigY + 3); y++)
        {
            if (!(SquareContents(x,y)==value))
            {
                if (RemoveNumber(x,y,value) > 0)
                    changes++;
            }
        }    
    }
    
    if (changes > 0)
        return 1;
    else
        return 0;
}

char ss_board_handler::QuadrantX(char val)
{
    if ((val >= 1) && (val <= 3))
        return 1;
    if ((val >= 4) && (val <= 6))
        return 2;
    if ((val >= 7) && (val <= 9))
        return 3;
    else
        return 0;
}

char ss_board_handler::QuadrantY(char val)
{
    if ((val >= 1) && (val <= 3))
        return 1;
    if ((val >= 4) && (val <= 6))
        return 2;
    if ((val >= 7) && (val <= 9))
        return 3;
    else
        return 0;
}

int ss_board_handler::RemoveNumberFromRow(char RowY, char value)
{
    char x;
    char changes;
    changes = 0;
    for (x=1;x<=9;x++)
    {
        if (!(SquareContents(x,RowY)==value))
        {
            if (RemoveNumber(x,RowY,value) > 0)
                changes++;
        }
    }
    
    if (changes > 0)
        return 1;
    else
        return 0;
}

int ss_board_handler::RemoveNumberFromColumn(char ColX, char value)
{
    char y;
    char changes;
    changes = 0;
    for (y=1;y<=9;y++)
    {
        if (!(SquareContents(ColX,y)==value))
        {
            if (RemoveNumber(ColX,y,value) > 0)
                changes++;
        }
    }
    if (changes > 0)
        return 1;
    else
        return 0;    
}

char ss_board_handler::IsSudokuValid(void)
{
    //I plan on writing a validation function here, but not now.
    return 1;
}

ss_board_handler::ss_board_handler(void)
{ }
