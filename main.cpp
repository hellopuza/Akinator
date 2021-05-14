/*------------------------------------------------------------------------------
    * File:        main.cpp                                                    *
    * Description: Program for guessing characters.                            *
    * Created:     18 apr 2021                                                 *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#include "Akinator.h"

#ifdef _WIN32
#include "windows.h"
#endif // _WIN32

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif // _WIN32

    Akinator akn;

    if (argc == 1)
    {
        Akinator akn;
        akn.Run();
    }
    else
    {
        Akinator akn(argv[1]);
        akn.Run();
    }

    return 0;
}