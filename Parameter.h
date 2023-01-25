#ifndef Parameter_H
#define Parameter_H

#define NUMBER_OF_OUTFLOWS 4


#include <iostream>
#include <cstdlib>
#include "Element.h"

using namespace std;

rgb outputCol(0, 0, 0);

class Parameter : public Element
{

private:
    int state;
    int move;

public:
    Parameter(){}



    Parameter(int state, int move)
    {
        this->state = state;
        this->move = move;
    }

    Parameter(char state)
    {
        this->state = (int)state;
    }

    void setState(int s)
    {
        state = s;
    }

    int getState()
    {
        return state;
    }

    void setMove(int s)
    {
        move = s;
    }

    int getMove()
    {
        return move;
    }

    void composeElement(char *str)
    {
        char *pComma = (char *)memchr(str, ',', strlen(str));
        char *pSquare = str - 1;
        *pComma = 0;
        *pSquare = 0;
        this->state = atoi(str + 1);
        this->move = atoi(pComma + 1);
    }

    char *stringEncoding()
    {
        char *zstr = new char[23];
        sprintf(zstr, "[%d,%d]", state, move);
        return zstr;
    }

    rgb* outputValue()
    {
        if (state == 0)
            outputCol = rgb(1, 1, 1);
        if (state == 1)
        {
            outputCol = rgb(0, 0, 0);
        }

        return &outputCol;
    }

    void startStep(int step)
    {
        //state = 0;
    }

    friend ostream &operator<<(ostream &out, const Parameter &P)
    {
        out << P.state << "\n";
        return out;
    }
};

#endif
