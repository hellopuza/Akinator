/*------------------------------------------------------------------------------
    * File:        Tree.h                                                      *
    * Description: Declaration of functions and data types used for binary     *
                   trees.                                                      *
    * Created:     18 apr 2021                                                 *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG

#include "../StringLib/StringLib.h"
#include "../StackLib/Stack.h"
#include "TreeConfig.h"
#include <type_traits>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <new>


#define TREE_CHECK if (Check ())                        \
                   {                                    \
                     Dump(DUMP_NAME);                   \
                     TREE_ASSERTOK(errCode_, errCode_); \
                   }


#define TREE_ASSERTOK(cond, err) if (cond)                                                                 \
                                 {                                                                         \
                                   TreePrintError (TREE_LOGNAME , __FILE__, __LINE__, __FUNC_NAME__, err); \
                                   exit(err); /**/                                                         \
                                 }

static int tree_id = 0;

#define newTree(NAME, STK_TYPE) \
        Tree<STK_TYPE> NAME ((char*)#NAME);

#define newTree_root(NAME, root, STK_TYPE) \
        Tree<STK_TYPE> NAME ((char*)#NAME, root);

#define newTree_base(NAME, base, STK_TYPE) \
        Tree<STK_TYPE> NAME ((char*)#NAME, base);


template <typename TYPE>
struct Node
{
    TYPE data_ = POISON<TYPE>;

    Node* left_  = nullptr;
    Node* right_ = nullptr;
    Node* prev_  = nullptr;

    size_t depth_ = 0;

    bool is_dynamic_ = false;

//------------------------------------------------------------------------------
/*! @brief   Tree default constructor.
*/

    Node ();

//------------------------------------------------------------------------------
/*! @brief   Create a tree from the base text.
 *
 *  @param   base        Base text
 *  @param   line_cur    Current line in the base text
 * 
 *  @return  error code
 */

    int AddFromBase (const Text& base, size_t& line_cur);

//------------------------------------------------------------------------------
/*! @brief   Recursive tree destruction.
 */

    void destruct ();

//------------------------------------------------------------------------------
/*! @brief   Node copy constructor (deleted).
 *
 *  @param   obj         Source node
 */

    Node (const Node& obj);

    Node& operator = (const Node& obj); // deleted

//------------------------------------------------------------------------------
/*! @brief   Recursive tree writing to file.
 *
 *  @param   base        Base file
 */

    void Write (FILE* base);

//------------------------------------------------------------------------------
/*! @brief   Recursive depth recount.
 */

    void recountDepth ();

//------------------------------------------------------------------------------
/*! @brief   Recursively find path to the element.
 *
 *  @param   path        Path to the element
 *  @param   elem        Data of node
 *
 *  @return  1 if found, 0 if not
 */

    int findPath (Stack<size_t>& path, TYPE elem);

//------------------------------------------------------------------------------
/*! @brief   Node deep copy constructor.
 *
 *  @param   obj         Source node
 */

    void dCopy (const Node& obj);

//------------------------------------------------------------------------------
/*! @brief   Recursive node checker.
 *
 *  @return  error code
 */

    int Check ();

//------------------------------------------------------------------------------
/*! @brief   Recursive print the contents of the tree like a graphviz dot file.
 *
 *  @param   dump        Dump graphviz dot file
 */

    void Dump (FILE* dump);

//------------------------------------------------------------------------------
};


template <typename TYPE>
struct Tree
{
    const char* name_ = nullptr;

    Node<TYPE>* root_ = nullptr;

    int id_ = 0;
    int errCode_;

//------------------------------------------------------------------------------
/*! @brief   Tree default constructor.
*/

    Tree ();

//------------------------------------------------------------------------------
/*! @brief   Tree constructor with one node.
 *
 *  @param   tree_name   Tree variable name
 */

    Tree (char* tree_name);

//------------------------------------------------------------------------------
/*! @brief   Tree constructor with root.
 *
 *  @param   tree_name   Tree variable name
 *  @param   root        Tree root
 */

    Tree (char* tree_name, Node<TYPE>* root);

//------------------------------------------------------------------------------
/*! @brief   Tree constructor with base.
 *
 *  @param   tree_name   Tree variable name
 *  @param   base_name   Base filename
 */

    Tree (char* tree_name, char* base_name);

//------------------------------------------------------------------------------
/*! @brief   Stack destructor.
 */

   ~Tree ();

//------------------------------------------------------------------------------
/*! @brief   Tree copy constructor (deleted).
 *
 *  @param   obj         Source tree
 */

    Tree (const Tree& obj);

    Tree& operator = (const Tree& obj); // deleted

//------------------------------------------------------------------------------
/*! @brief   Tree deep copy constructor.
 *
 *  @param   obj         Source tree
 */

    void dCopy (const Tree& obj);

//------------------------------------------------------------------------------
/*! @brief   Print the contents of the tree like a graphviz dot file.
 *
 *  @param   dumpname    Name of the dump file
 */

    void Dump (const char* dumpname = DUMP_NAME);

//------------------------------------------------------------------------------
/*! @brief   Write the tree data to the base file.
 *
 *  @param   basename    Base file name
 */

    void Write (const char* basename = DEFAULT_BASE_NAME);

//------------------------------------------------------------------------------
/*! @brief   Find path in the tree to the element.
 *
 *  @param   path        Path to the element
 *  @param   elem        Data of node
 *
 *  @return  1 if found, 0 if not
 */

    int findPath (Stack<size_t>& path, TYPE elem);

//------------------------------------------------------------------------------
/*! @brief   Check tree for problems.
 *
 *  @return  error code
 */

    int Check ();

//------------------------------------------------------------------------------
};


//------------------------------------------------------------------------------
/*! @brief   Print error explanations to log file and to console.
 *
 *  @param   logname     Name of the log file
 *  @param   file        Name of the file from which this function was called
 *  @param   line        Line of the code from which this function was called
 *  @param   function    Name of the function from which this function was called
 *
 *  @return  error code
 */

static void TreePrintError (const char* logname, const char* file, int line, const char* function, int err);

//------------------------------------------------------------------------------

#include "Tree.ipp"

#endif // TREE_H_INCLUDED