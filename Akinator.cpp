/*------------------------------------------------------------------------------
    * File:        Akinator.cpp                                                *
    * Description: Functions for Akinator.                                     *
    * Created:     18 apr 2021                                                 *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#include "Akinator.h"

//------------------------------------------------------------------------------

Akinator::Akinator () :
    tree_      ((char*)"default", (char*)DEFAULT_BASE_NAME),
    path2node_ ((char*)"path to problem node"),
    state_     (AKN_OK)
{
    BASE_CHECK;
}

//------------------------------------------------------------------------------

Akinator::Akinator (char* filename) :
    tree_      (filename, filename),
    path2node_ ((char*)"path2node_"),
    filename_  (filename),
    state_     (AKN_OK)
{
    BASE_CHECK;
}

//------------------------------------------------------------------------------

Akinator::~Akinator ()
{
    AKN_ASSERTOK((this == nullptr),          AKN_NULL_INPUT_AKINATOR_PTR);
    AKN_ASSERTOK((state_ == AKN_DESTRUCTED), AKN_DESTRUCTED             );

    filename_ = nullptr;

    state_ = AKN_DESTRUCTED;
}

//------------------------------------------------------------------------------

int Akinator::Run ()
{
    AKN_ASSERTOK((this == nullptr), AKN_NULL_INPUT_AKINATOR_PTR);

    tree_.Dump();

    printf("\n$$$ Akinator game (c) Artem Puzankov, 2021 $$$\n");

    bool running = true;
    while (running)
    {
        BASE_CHECK;
    
        printf("\nChoose a gamemode please:\n");
        printf("\t[1]: Guessing a character\n");
        printf("\t[2]: Find a character\n");
        printf("\t[3]: Character comparison\n");
        printf("\t[4]: View the base\n");
        printf("\t[5]: Exit\n");
        printf("Enter a number: ");

        int mode = scanNum(1, 5);

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
            printGraphBase();
            break;
        case 5:
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
        if (node_cur->getData()[0] == FEAT_SIGN)
            strcpy(question, node_cur->getData() + 1);

        else if (node_cur->getData()[0] == CHAR_SIGN)
        {
            strcpy(question, node_cur->getData() + 1);
            question[strlen(question) - 1] = FEAT_SIGN;
            isAns = true;
        }
        else AKN_ASSERTOK(AKN_INCORRECT_INPUT_SYNTAX_BASE, AKN_INCORRECT_INPUT_SYNTAX_BASE);

        printf("\nВаш персонаж - %s\n", question);
        printf("Answer [Y/n]? ");

        if (scanAns())
        {
            if (isAns)
            {
                printf("\nЯ угадал!\n");
                return AKN_OK;
            }
            
            node_cur = node_cur->right_;
        }
        else
        {
            if (isAns)
            {
                printf("\nЯ не угадал\n");
                printf("Пожалуйста дополните мою базу правильным ответом\n");

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
    printf("Введите персонажа, о котором хотите узнать: ");
    char* charname = scanChar(CHAR_SIGN);

    newStack(path, size_t);
    bool found = tree_.findPath(path, charname);
    if (not found)
    {
        printf("Такой персонаж не найден\n");
        return AKN_OK;
    }

    printf("%s\b - ", charname + 1);
    delete [] charname;

    for (int i = 0; i < path.getSize() - 1; ++i)
        printFeature(path, i);

    printf(".\n");
    
    return AKN_OK;
}

//------------------------------------------------------------------------------

int Akinator::CharCmp ()
{
    printf("Введите первого персонажа, которого хотите сравнить: ");
    char* char1 = scanChar(CHAR_SIGN);

    newStack(path1, size_t);
    bool found = tree_.findPath(path1, char1);
    if (not found)
    {
        printf("Такой персонаж не найден\n");
        return AKN_OK;
    }

    printf("Введите второго персонажа, которого хотите сравнить: ");
    char* char2 = scanChar(CHAR_SIGN);

    newStack(path2, size_t);
    found = tree_.findPath(path2, char2);
    if (not found)
    {
        printf("Такой персонаж не найден\n");
        return AKN_OK;
    }

    size_t i1 = 0;
    size_t i2 = 0;

    if ((Node<char*>*)path1[i1 + 1] != (Node<char*>*)path2[i2 + 1])
        printf("\n%s\b и %s\b ничем не схожи", char1 + 1, char2 + 1);
    else
    {
        printf("\n%s\b и %s\b схожи тем, что ", char1 + 1, char2 + 1);
        while (((Node<char*>*)path1[i1 + 1] == (Node<char*>*)path2[i2 + 1]) && (i1 < path1.getSize() - 1) && (i2 < path2.getSize() - 1))
        {
            printFeature(path1, i1);

            ++i1;
            ++i2;
        }
    }
    printf("\n");

    printf("но %s\b отличается тем, что ", char1 + 1);
    for (; i1 < path1.getSize() - 1; ++i1)
        printFeature(path1, i1);

    printf(",\n");

    printf("a %s\b отличается тем, что ", char2 + 1);
    for (; i2 < path2.getSize() - 1; ++i2)
        printFeature(path2, i2);

    printf(".\n");

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

char* Akinator::scanChar (char c)
{
    char* charname = new char [MAX_STR_LEN] {};
    charname[0] = c;

    for (char* err = fgets(charname + 1, MAX_STR_LEN - 2, stdin); !err; printf("Try again [Y/n]? "));
    size_t len = strlen(charname);
    assert(len);
    charname[len - 1] = c;

    return charname;
}

//------------------------------------------------------------------------------

inline void Akinator::printFeature (const Stack<size_t>& path, size_t item)
{
    if (((Node<char*>*)path[item])->left_ == (Node<char*>*)path[item + 1])
        printf("не ");
    else if (((Node<char*>*)path[item])->right_ != (Node<char*>*)path[item + 1])
        assert(0);

    printf("%s\b", ((Node<char*>*)path[item])->getData() + 1);
    if (item != path.getSize() - 2) printf(", ");
}

//------------------------------------------------------------------------------

int Akinator::addAns (Node<char*>* node_cur)
{
    assert(node_cur != nullptr);

    printf("Введите вашего персонажа: ");
    char* newchar = scanChar(CHAR_SIGN);

    newStack(path, size_t);
    if (tree_.findPath(path, newchar))
    {
        printf("Такой персонаж уже есть: %s\b - ", newchar + 1);
        for (int i = 0; i < path.getSize() - 1; ++i)
            printFeature(path, i);
        printf(".\n");

        return AKN_OK;
    }

    char oldchar[MAX_STR_LEN] = "";
    strcpy(oldchar, node_cur->getData() + 1);
    oldchar[strlen(oldchar) - 1] = '\0';

    printf("Введите признак отличающий %s от %s: ", newchar, oldchar);
    char* feature = scanChar(FEAT_SIGN);

    Node<char*>* prevNode = node_cur->prev_;

    Node<char*>* featureNode = new Node<char*>;
    featureNode->prev_ = prevNode;

    featureNode->setData(feature);

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

    newcharNode->setData(newchar);

    featureNode->right_ = newcharNode;
    featureNode->left_  = node_cur;

    delete[] feature;
    delete[] newchar;

    featureNode->recountDepth();

    printf("\nСохранить в базу?\n");
    printf("Answer [Y/n]? ");
    if (scanAns())
        tree_.Write(filename_);

    return AKN_OK;
}

//------------------------------------------------------------------------------

int Akinator::checkBase (Node<char*>* node_cur)
{
    assert(node_cur != nullptr);

    size_t len = strlen(node_cur->getData());
    
    if ((node_cur->left_ == nullptr) && (node_cur->right_ == nullptr))
        if ((node_cur->getData()[0] != CHAR_SIGN) || (node_cur->getData()[len - 1] != CHAR_SIGN))
        {
            path2node_.Push(node_cur->getData());
            return AKN_WRONG_SYNTAX_TREE_LEAF;
        }
        else;
    else
    if ((node_cur->left_ != nullptr) && (node_cur->right_ != nullptr))
        if ((node_cur->getData()[0] != FEAT_SIGN) || (node_cur->getData()[len - 1] != FEAT_SIGN))
        {
            path2node_.Push(node_cur->getData());
            return AKN_WRONG_SYNTAX_TREE_NODE;
        }
        else;
    else
    {
        path2node_.Push(node_cur->getData());
        return AKN_WRONG_TREE_ONE_CHILD;
    }

    int err = AKN_OK;

    if (node_cur->right_ != nullptr)
        err = checkBase(node_cur->right_);
    state_ = err;

    if (err)
    {
        path2node_.Push(node_cur->getData());
        return err;
    }

    if (node_cur->left_ != nullptr)
        err = checkBase(node_cur->left_);
    state_ = err;

    if (err) path2node_.Push(node_cur->getData());

    return err;
}

//------------------------------------------------------------------------------

void Akinator::printGraphBase (const char* graphname)
{
    FILE* graph = fopen(graphname, "w");
    assert(graph != nullptr);

    fprintf(graph, "digraph G{\n" "rankdir = HR;\n node[shape=box];\n");

    printGraphNode(graph, tree_.root_);

    fprintf(graph, "\tlabelloc=\"t\";"
                  "\tlabel=\"Akinator base: %s\";"
                  "}\n", tree_.name_);

    fclose(graph);

    char command[128] = "";

    sprintf(command, "win_iconv -f 1251 -t UTF8 \"%s\" > \"new%s\"", graphname, graphname);
    system(command);

    char* truename = new char[128] {};
    strcpy(truename, graphname);

    sprintf(command, "dot -Tpng -o %s.png new%s", GetTrueFileName((char*)truename), graphname);
    system(command);

    sprintf(command, "del new%s", graphname);
    system(command);

    sprintf(command, "start %s.png", truename);
    system(command);

    delete[] truename;
}

//------------------------------------------------------------------------------

void Akinator::printGraphNode (FILE* graph, Node<char*>* node_cur)
{
    assert(graph != nullptr);

    if (node_cur->right_ == nullptr && node_cur->left_ == nullptr)
        fprintf(graph, "\t \"%s\" [shape = box, style = filled, color = black, fillcolor = orange]\n", node_cur->getData());
    else
        fprintf(graph, "\t \"%s\" [shape = box, style = filled, color = black, fillcolor = lightskyblue]\n", node_cur->getData());

    if (node_cur->left_ != nullptr)
        fprintf(graph, "\t \"%s\" -> \"%s\" [label=\"No\"]\n", node_cur->getData(), node_cur->left_->getData());

    if (node_cur->right_ != nullptr)
        fprintf(graph, "\t \"%s\" -> \"%s\" [label=\"Yes\"]\n", node_cur->getData(), node_cur->right_->getData());

    if (node_cur->left_  != nullptr) printGraphNode(graph, node_cur->left_);
    if (node_cur->right_ != nullptr) printGraphNode(graph, node_cur->right_);
}

//------------------------------------------------------------------------------

void Akinator::PrintError (const char* logname, const char* file, int line, const char* function, int err)
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

    if (path2node_.getSize() != 0)
    {
        fprintf(log, "%s", path2node_.getName());
        for (int i = path2node_.getSize() - 1; i > -1; --i)
            fprintf(log, " -> [%s]", path2node_[i]);

        fprintf(log, "\n");
    }
    fprintf(log, "You can look tree dump in %s\n", DUMP_PICT_NAME);
    fclose(log);

    ////

    printf (     "ERROR: file %s  line %d  function %s\n",   file, line, function);
    printf (     "%s\n\n", akn_errstr[err + 1]);

    if (path2node_.getSize() != 0)
    {
        printf("%s", path2node_.getName());
        for (int i = path2node_.getSize() - 1; i > -1; --i)
            printf(" -> [%s]", path2node_[i]);

        printf("\n");
    }
    printf (     "You can look tree dump in %s\n", DUMP_PICT_NAME);
}

//------------------------------------------------------------------------------
