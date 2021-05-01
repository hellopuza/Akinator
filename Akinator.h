/*------------------------------------------------------------------------------
    * File:        Akinator.h                                                  *
    * Description: Declaration of functions and data types used for Akinator   *
    * Created:     18 apr 2021                                                 *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#ifndef AKINATOR_H_INCLUDED
#define AKINATOR_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG

//#define TX_USE_SPEAK
#include "C:\Users\admin\Documents\.Programming\C\TX\TXLib.h"

#if defined (__GNUC__) || defined (__clang__) || defined (__clang_major__)
    #define __FUNC_NAME__   __PRETTY_FUNCTION__

#elif defined (_MSC_VER)
    #define __FUNC_NAME__   __FUNCSIG__

#else
    #define __FUNC_NAME__   __FUNCTION__

#endif



#include "StringLib/StringLib.h"
#include "StackLib/Stack.h"
#include "TreeLib/Tree.h"

#include <stdio.h>
#include <locale.h>
#include <assert.h>


//==============================================================================
/*------------------------------------------------------------------------------
                   Akinator errors                                             *
*///----------------------------------------------------------------------------
//==============================================================================


enum AkinatorErrors
{
    AKN_NOT_OK = -1                                                    ,
    AKN_OK = 0                                                         ,
    AKN_NO_MEMORY                                                      ,

    AKN_DESTRUCTED                                                     ,
    AKN_INCORRECT_INPUT_SYNTAX_BASE                                    ,
    AKN_NULL_INPUT_AKINATOR_PTR                                        ,
    AKN_NULL_INPUT_FILENAME                                            ,
};

static const char* akn_errstr[] =
{
    "ERROR"                                                            ,
    "OK"                                                               ,
    "Failed to allocate memory"                                        ,

    "Akinator has already destructed"                                  ,
    "Incorrect input syntax base"                                      ,
    "The input value of the Akinator pointer turned out to be zero"    ,
    "The input value of the Akinator filename turned out to be zero"   ,
};

static const char* AKINATOR_LOGNAME = "akinator.log";

#define BASE_CHECK if (tree_.Check ())                              \
                   {                                                \
                     tree_.Dump();                                  \
                     TREE_ASSERTOK(tree_.errCode_, tree_.errCode_); \
                   }                                                \
                   if (checkBase (tree_.root_))                     \
                   {                                                \
                     AKN_ASSERTOK(state_, state_);                  \
                   }

#define AKN_ASSERTOK(cond, err) if (cond)                                                                  \
                                {                                                                          \
                                  AknPrintError(AKINATOR_LOGNAME, __FILE__, __LINE__, __FUNC_NAME__, err); \
                                  tree_.Dump();                                                            \
                                  exit(err); /**/                                                          \
                                }


//==============================================================================
/*------------------------------------------------------------------------------
                   Akinator constants and types                                *
*///----------------------------------------------------------------------------
//==============================================================================


static const char* GRAPH_FILENAME   = "Base.dot";
static const char* DEFAULT_BASENAME = "Base.dat";
const size_t MAX_STR_LEN = 128;

class Akinator
{
private:

    int state_;
    char* filename_ = (char*)DEFAULT_BASENAME;

    Tree<char*> tree_;

public:

//------------------------------------------------------------------------------
/*! @brief   Akinator default constructor.
*/

    Akinator ();

//------------------------------------------------------------------------------
/*! @brief   Akinator constructor.
 *
 *  @param   filename    Name of a base data file
 */

    Akinator (char* filename);

//------------------------------------------------------------------------------
/*! @brief   Assembler copy constructor (deleted).
 *
 *  @param   obj         Source akinator
 */

    Akinator (const Akinator& obj);

    Akinator& operator = (const Akinator& obj); // deleted

//------------------------------------------------------------------------------
/*! @brief   Akinator destructor.
 */

   ~Akinator ();

//------------------------------------------------------------------------------
/*! @brief   Execution process.
 *
 *  @note    """
 *
 *  @return  error code
 */

    int Run ();

/*------------------------------------------------------------------------------
                   Private functions                                           *
*///----------------------------------------------------------------------------

private:

//------------------------------------------------------------------------------
/*! @brief   Character guessing process.
 *
 *  @note    """
 *
 *  @return  error code
 */

    int Guessing();

//------------------------------------------------------------------------------
/*! @brief   Character finding process.
 *
 *  @note    """
 *
 *  @return  error code
 */

    int CharFind();

//------------------------------------------------------------------------------
/*! @brief   Character comparison process.
 *
 *  @note    """
 *
 *  @return  error code
 */

    int CharCmp();

//------------------------------------------------------------------------------
/*! @brief   Get a number from stdin.
 * 
 *  @param   start       Start of the range
 *  @param   end         End of the range
 *
 *  @return  int number
 */

    int scanNum (int start, int end);

//------------------------------------------------------------------------------
/*! @brief   Get an answer from stdin (yes or no).
 *
 *  @return  true or false
 */

    bool scanAns ();

//------------------------------------------------------------------------------
/*! @brief   Get character name from stdin.
 *
 *  @return  character name
 */

    char* scanChar (char c);

//------------------------------------------------------------------------------
/*! @brief   Print feature to console.
 *
 *  @param   path        Path to the element
 *  @param   item        Path item number
 *
 *  @return  1 if found, 0 if not
 */

    inline void printFeature (const Stack<size_t>& path, size_t item);

//------------------------------------------------------------------------------
/*! @brief   Add new answer to the tree.
 *
 *  @param   node_cur    Current node
 *
 *  @return  error code
 */

    int addAns (Node<char*>* node_cur);

//------------------------------------------------------------------------------
/*! @brief   Check base for errors.
 * 
 *  @param   node_cur    Current node
 *
 *  @return  error code
 */

    int checkBase (Node<char*>* node_cur);

//------------------------------------------------------------------------------
/*! @brief   Print the contents of the tree like a graphviz dot file.
 *
 *  @param   graphname   Name of the graph file
 */

    void printGraphBase (const char* graphname = GRAPH_FILENAME);

//------------------------------------------------------------------------------
/*! @brief   Recursive print the contents of the tree like a graphviz dot file.
 *
 *  @param   graph       Dump graphviz dot file
 */
    void printGraphNode (FILE* graph, Node<char*>* node_cur);

//------------------------------------------------------------------------------
};

//------------------------------------------------------------------------------
/*! @brief   Prints an error wih description to the console and to the log file.
 *
 *  @param   logname     Name of the log file
 *  @param   file        Name of the program file
 *  @param   line        Number of line with an error
 *  @param   function    Name of the function with an error
 *  @param   err         Error code
 */

void AknPrintError (const char* logname, const char* file, int line, const char* function, int err);

//------------------------------------------------------------------------------

#endif // AKINATOR_H_INCLUDED
