/*------------------------------------------------------------------------------
    * File:        Tree.ipp                                                    *
    * Description: Functions for binary trees.                                 *
    * Created:     18 apr 2021                                                 *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

template <typename TYPE>
Tree<TYPE>::Tree () : errCode_ (TREE_NOT_CONSTRUCTED) { }

//------------------------------------------------------------------------------

template <typename TYPE>
Tree<TYPE>::Tree (char* tree_name) :
    name_      (tree_name),
    id_        (tree_id++),
    errCode_   (TREE_OK)
{
    root_ = new Node;
}

//------------------------------------------------------------------------------

template <typename TYPE>
Tree<TYPE>::Tree (char* tree_name, Node<TYPE>* root) :
    name_      (tree_name),
    root_      (Node),
    id_        (tree_id++),
    errCode_   (TREE_OK)
{}

//------------------------------------------------------------------------------

template <typename TYPE>
Tree<TYPE>::Tree (char* tree_name, char* base_filename) :
    name_      (tree_name),
    id_        (tree_id++),
    errCode_   (TREE_OK)
{
    TREE_ASSERTOK((tree_name == nullptr), TREE_WRONG_INPUT_TREE_NAME);

    root_ = new Node<TYPE>;

    Text base(base_filename);

    TREE_ASSERTOK((base.num_ < 2), TREE_WRONG_SYNTAX_INPUT_BASE);
    TREE_ASSERTOK((strcmp(base.lines_[0].str,             OPEN_BRACKET ) != 0), TREE_WRONG_SYNTAX_INPUT_BASE);
    TREE_ASSERTOK((strcmp(base.lines_[base.num_ - 1].str, CLOSE_BRACKET) != 0), TREE_WRONG_SYNTAX_INPUT_BASE);

    size_t line_cur = 1;
    if (strcmp(base.lines_[line_cur].str, CLOSE_BRACKET) != 0)
    {
        int err = root_->AddFromBase(base, line_cur);
        TREE_ASSERTOK(err, err);
    }


    // DUMP_PRINT{ Dump(__FUNC_NAME__); }
}

//------------------------------------------------------------------------------

template <typename TYPE>
Tree<TYPE>::~Tree ()
{
    if (errCode_ == TREE_NOT_CONSTRUCTED) {}

    else if (errCode_ != TREE_DESTRUCTED)
    {
        if (root_ != nullptr)
            root_->destruct();

        root_ = nullptr;

        errCode_ = TREE_DESTRUCTED;
    }
}

//------------------------------------------------------------------------------

template <typename TYPE>
int Node<TYPE>::AddFromBase (const Text& base, size_t& line_cur)
{
    assert(line_cur < base.num_);

    if (strcmp(base.lines_[line_cur].str, CLOSE_BRACKET) == 0) return 0;

    if constexpr (std::is_same<TYPE, char*>::value)
    {
        data_ = new char [base.lines_[line_cur].len + 2];
        strcpy(data_, base.lines_[line_cur].str);
        ++line_cur;

        is_dynamic_ = true;
    }
    else
        sscanf(base.lines_[line_cur++].str, PRINT_FORMAT<TYPE>, &data_);

    if (strcmp(base.lines_[line_cur].str, OPEN_BRACKET) == 0)
    {
        right_ = new Node<TYPE>;
        right_->prev_ = this;
        right_->depth_ = depth_ + 1;

        int err = right_->AddFromBase(base, ++line_cur);
        if (err) return err;
        ++line_cur;
    }
    else if (strcmp(base.lines_[line_cur].str, CLOSE_BRACKET) == 0) return 0;
    else return TREE_WRONG_SYNTAX_INPUT_BASE;

    if (strcmp(base.lines_[line_cur].str, OPEN_BRACKET) == 0)
    {
        left_ = new Node<TYPE>;
        left_->prev_ = this;
        left_->depth_ = depth_ + 1;

        int err = left_->AddFromBase(base, ++line_cur);
        if (err) return err;
        ++line_cur;
    }

    return 0;
}

//------------------------------------------------------------------------------

template <typename TYPE>
void Node<TYPE>::destruct ()
{
    assert(this != nullptr);
    
    if (left_  != nullptr) left_->destruct();
    if (right_ != nullptr) right_->destruct();

    left_  = nullptr;
    right_ = nullptr;
    prev_  = nullptr;

    if (is_dynamic_) delete [] data_;

    data_ = POISON<TYPE>;
    
    delete this;
}

//------------------------------------------------------------------------------

template <typename TYPE>
void Tree<TYPE>::dCopy (const Tree& obj)
{

}

//------------------------------------------------------------------------------

template <typename TYPE>
int Tree<TYPE>::Dump (const char* funcname, const char* logfile)
{
    printf("tree dump");

    return TREE_OK;
}

//------------------------------------------------------------------------------

template <typename TYPE>
int Tree<TYPE>::write (const char* basename)
{
    FILE* base = fopen(basename, "w");
    assert(base != nullptr);

    fprintf(base, "[\n");
    if (root_ != nullptr) root_->write(base);
    fprintf(base, "]");

    fclose(base);

    return TREE_OK;
}

//------------------------------------------------------------------------------

template <typename TYPE>
void Node<TYPE>::write (FILE* base)
{
    assert(base != nullptr);

    for (int i = 0; i <= depth_; ++i) fprintf(base, "    ");
    fprintf(base, PRINT_FORMAT<TYPE>, data_);
    fprintf(base, "\n");

    if (right_ != nullptr)
    {
        for (int i = 0; i <= depth_; ++i) fprintf(base, "    ");
        fprintf(base, "[\n");

        right_->write(base);
        
        for (int i = 0; i <= depth_; ++i) fprintf(base, "    ");
        fprintf(base, "]\n");
    }

    if (right_ != nullptr)
    {
        for (int i = 0; i <= depth_; ++i) fprintf(base, "    ");
        fprintf(base, "[\n");

        if (left_ != nullptr) left_->write(base);

        for (int i = 0; i <= depth_; ++i) fprintf(base, "    ");
        fprintf(base, "]\n");
    }
}

//------------------------------------------------------------------------------

template <typename TYPE>
void Node<TYPE>::recountDepth ()
{
    if (prev_ == nullptr)
        depth_ = 0;
    else
        depth_ = prev_->depth_ + 1;

    if (right_ != nullptr) right_->recountDepth();
    if (left_  != nullptr) left_->recountDepth();
}

//------------------------------------------------------------------------------

template <typename TYPE>
int Tree<TYPE>::findPath (Stack<size_t>& path, TYPE elem)
{
    TREE_ASSERTOK((isPOISON(elem)), TREE_INPUT_DATA_POISON);

    bool found = root_->findPath(path, elem);

    return found;
}

//------------------------------------------------------------------------------

template <typename TYPE>
int Node<TYPE>::findPath (Stack<size_t>& path, TYPE elem)
{
    TREE_ASSERTOK((isPOISON(elem)), TREE_INPUT_DATA_POISON);

    path.Push((size_t)this);
    
    bool found = false;
    if (right_ != nullptr)
    {
        found = right_->findPath(path, elem);
        if (found) return found;
    }
    if (left_ != nullptr)
    {
        found = left_->findPath(path, elem);
        if (found) return found;
    }

    if ((left_ == nullptr) && (right_ == nullptr))
    {
        if constexpr (std::is_same<TYPE, char*>::value)
        {
            if (strcmp(elem, data_) == 0) found = true;
        }
        else found = (elem == data_);
    }

    if (not found) path.Pop();

    return found;
}

//------------------------------------------------------------------------------

static void TreePrintError (const char* logname, const char* file, int line, const char* function, int err)
{
    assert(function != nullptr);
    assert(logname  != nullptr);
    assert(file     != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    fprintf(log, "********************************************************************************\n");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(log, "TIME: %d-%02d-%02d %02d:%02d:%02d\n\n",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec);

    fprintf(log, "ERROR: file %s  line %d  function %s\n\n", file, line, function);
    fprintf(log, "%s\n", tree_errstr[err + 1]);

    printf("ERROR: file %s  line %d  function %s\n", file, line, function);
    printf("%s\n\n", tree_errstr[err + 1]);

    fclose(log);
}

//------------------------------------------------------------------------------
