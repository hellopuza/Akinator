/*------------------------------------------------------------------------------
    * File:        Akinator.cpp                                                *
    * Description: Functions for Akinator.                                     *
    * Created:     18 apr 2021                                                 *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright � 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#include "Akinator.h"

//------------------------------------------------------------------------------

Akinator::Akinator () :
    tree_  ((char*)"Akinator tree", (char*)DEFAULT_BASE_NAME),
    state_ (AKN_OK)
{ }

//------------------------------------------------------------------------------

Akinator::Akinator (char* filename) :
    tree_     ((char*)"Akinator tree", filename),
    filename_ (filename),
    state_    (AKN_OK)
{ }

//------------------------------------------------------------------------------

Akinator::~Akinator ()
{
    AKN_ASSERTOK((this == nullptr),           AKN_NULL_INPUT_AKINATOR_PTR);
    AKN_ASSERTOK((state_ == AKN_DESTRUCTED),  AKN_DESTRUCTED             );

    filename_ = nullptr;

    tree_.~Tree();

    state_ = AKN_DESTRUCTED;
}

//------------------------------------------------------------------------------

int Akinator::Run ()
{
    //checkBase();
    AKN_ASSERTOK((this == nullptr), AKN_NULL_INPUT_AKINATOR_PTR);

    printf("\n$$$ Akinator game (c) Artem Puzankov, 2021 $$$\n");

    bool running = true;
    while (running)
    {
        printf("\nChoose a gamemode please:\n");
        printf("\t[1]: Guessing a character\n");
        printf("\t[2]: Find a character\n");
        printf("\t[3]: Character comparison\n");
        printf("\t[4]: Exit\n");
        printf("Enter a number: ");

        int mode = scanNum(1, 4);

        switch (mode)
        {
        case 1:
            Guessing();
            break;
        case 2:
            CharFind();
            break;
        case 3:
            CharCmp();
            break;
        case 4:
            running = false;
            break;
        default:
            assert(0);
        }
    }

    return AKN_OK;
}

//------------------------------------------------------------------------------

int Akinator::Guessing ()
{
    Node<char*>* node_cur = tree_.root_;

    while (node_cur != nullptr)
    {
        bool isAns = false;

        char question[MAX_STR_LEN] = "";
        if (node_cur->data_[0] == '?')
            strcpy(question, node_cur->data_ + 1);

        else if (node_cur->data_[0] == '\"')
        {
            strcpy(question, node_cur->data_ + 1);
            question[strlen(question) - 1] = '?';
            isAns = true;
        }
        else AKN_ASSERTOK(AKN_INCORRECT_INPUT_SYNTAX_BASE, AKN_INCORRECT_INPUT_SYNTAX_BASE);

        printf("\n��� �������� - %s\n", question);
        printf("Answer [Y/n]? ");

        if (scanAns())
        {
            if (isAns)
            {
                printf("� ������!\n");
                return AKN_OK;
            }
            
            node_cur = node_cur->right_;
        }
        else
        {
            if (isAns)
            {
                printf("� �� ������\n");
                printf("���������� ��������� ��� ���� ���������� �������\n");

                addAns(node_cur);
                return AKN_OK;
            }

            node_cur = node_cur->left_;
        }
    }

    return AKN_OK;
}

//------------------------------------------------------------------------------

int Akinator::CharFind ()
{
    printf("������� ���������, � ������� ������ ������: ");
    char* charname = scanChar();

    newStack(path, size_t);
    bool found = tree_.findPath(path, charname);
    if (not found)
    {
        printf("����� �������� �� ������\n");
        return AKN_OK;
    }

    printf("%s\b - ", charname + 1);
    delete [] charname;

    for (int i = 0; i < path.getSize() - 1; ++i)
        printFeature(path, i);

    printf("\n");

    return AKN_OK;
}

//------------------------------------------------------------------------------

int Akinator::CharCmp ()
{
    printf("������� ������� ���������, �������� ������ ��������: ");
    char* char1 = scanChar();

    newStack(path1, size_t);
    bool found = tree_.findPath(path1, char1);
    if (not found)
    {
        printf("����� �������� �� ������\n");
        return AKN_OK;
    }

    printf("������� ������� ���������, �������� ������ ��������: ");
    char* char2 = scanChar();

    newStack(path2, size_t);
    found = tree_.findPath(path2, char2);
    if (not found)
    {
        printf("����� �������� �� ������\n");
        return AKN_OK;
    }

    size_t i1 = 0;
    size_t i2 = 0;

    if ((Node<char*>*)path1[i1 + 1] != (Node<char*>*)path2[i2 + 1])
        printf("\n%s\b � %s\b ����� �� �����", char1 + 1, char2 + 1);
    else
    {
        printf("\n%s\b � %s\b ����� ���, ��� ", char1 + 1, char2 + 1);
        while (((Node<char*>*)path1[i1 + 1] == (Node<char*>*)path2[i2 + 1]) && (i1 < path1.getSize() - 1) && (i2 < path2.getSize() - 1))
        {
            printFeature(path1, i1);

            ++i1;
            ++i2;
        }
    }
    printf("\n");

    printf("�� %s\b ���������� ���, ��� ", char1 + 1);
    for (; i1 < path1.getSize() - 1; ++i1)
        printFeature(path1, i1);

    printf("\n");

    printf("� %s\b ���������� ���, ��� ", char2 + 1);
    for (; i2 < path2.getSize() - 1; ++i2)
        printFeature(path2, i2);

    printf("\n");

    printf("\n��� ��� �������� � �������� �������� %s\b � %s\b \n", char1 + 1, char2 + 1);

    delete [] char1;
    delete [] char2;

    return AKN_OK;
}

//------------------------------------------------------------------------------

int Akinator::scanNum (int start, int end)
{
    assert(start < end);

    int num = 0;
    char str[MAX_STR_LEN] = "";
    char* endstr = (char*)"";

    char* err = fgets(str, MAX_STR_LEN - 2, stdin);
    num = strtol(str, &endstr, 20);

    while ((endstr[0] != '\n') || (num < start) || (num > end) || !err)
    {
        printf("Try again: ");
        err = fgets(str, 18, stdin);
        num = strtol(str, &endstr, 20);
    }

    return num;
}

//------------------------------------------------------------------------------

bool Akinator::scanAns ()
{
    char ans[MAX_STR_LEN] = "";

    char* err = fgets(ans, MAX_STR_LEN - 2, stdin);
    ans[0] = toupper(ans[0]);

    while ((ans[0] != 'Y') && (ans[0] != 'N') || (ans[1] != '\n') || !err)
    {
        printf("Try again [Y/n]? ");
        err = fgets(ans, MAX_STR_LEN - 2, stdin);
        ans[0] = toupper(ans[0]);
    }

    return ((ans[0] == 'Y') ? 1 : 0);
}

//------------------------------------------------------------------------------

char* Akinator::scanChar ()
{
    char* charname = new char [MAX_STR_LEN];
    for (char* err = fgets(charname + 1, MAX_STR_LEN - 2, stdin); !err; printf("Try again [Y/n]? "));
    size_t len = strlen(charname);
    charname[0] = '\"';
    charname[len - 1] = '\"';

    return charname;
}

//------------------------------------------------------------------------------

inline void Akinator::printFeature (const Stack<size_t>& path, size_t item)
{
    if (((Node<char*>*)path[item])->left_ == (Node<char*>*)path[item + 1])
        printf("�� ");
    else if (((Node<char*>*)path[item])->right_ != (Node<char*>*)path[item + 1])
        assert(0);

    printf("%s\b.", ((Node<char*>*)path[item])->data_ + 1);
    if (item != path.getSize() - 2) printf("\b, ");
}

//------------------------------------------------------------------------------

int Akinator::addAns (Node<char*>* node_cur)
{
    assert(node_cur != nullptr);

    printf("������� ������ ���������: ");
    char* newchar = scanChar();

    char oldchar[MAX_STR_LEN] = "";
    strcpy(oldchar, node_cur->data_ + 1);
    oldchar[strlen(oldchar) - 1] = '\0';

    printf("������� ������� ���������� %s �� %s: ", newchar, oldchar);
    char* feature = new char [MAX_STR_LEN];
    char* err = scanChar();

    Node<char*>* prevNode = node_cur->prev_;

    Node<char*>* featureNode = new Node<char*>;
    featureNode->prev_ = prevNode;
    featureNode->data_ = feature;
    featureNode->is_dynamic_ = true;

    if (prevNode != nullptr)
        if (node_cur == prevNode->right_)
            prevNode->right_ = featureNode;
        else
            prevNode->left_ = featureNode;
    else
        tree_.root_ = featureNode;

    Node<char*>* newcharNode = new Node<char*>;
    newcharNode->prev_ = featureNode;
    node_cur->prev_    = featureNode;

    newcharNode->data_ = newchar;
    newcharNode->is_dynamic_ = true;

    featureNode->right_ = newcharNode;
    featureNode->left_  = node_cur;

    featureNode->recountDepth();

    printf("\n��������� � ����?\n");
    printf("Answer [Y/n]? ");
    if (scanAns())
    {
        tree_.write(filename_);
    }

    delete [] newchar;
    delete [] feature;

    return AKN_OK;
}

//------------------------------------------------------------------------------

void AknPrintError (const char* logname, const char* file, int line, const char* function, int err)
{
    assert(function != nullptr);
    assert(logname  != nullptr);
    assert(file     != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(log, "###############################################################################\n");
    fprintf(log, "TIME: %d-%02d-%02d %02d:%02d:%02d\n\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(log, "ERROR: file %s  line %d  function %s\n\n", file, line, function);
    fprintf(log, "%s\n", akn_errstr[err + 1]);

    printf (     "ERROR: file %s  line %d  function %s\n",   file, line, function);
    printf (     "%s\n\n", akn_errstr[err + 1]);

    fclose(log);
}

//------------------------------------------------------------------------------

