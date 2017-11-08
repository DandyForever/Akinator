#include "Tree_t.h"

FILE* LOG_txt = nullptr;

#define assert(condition)                                                   \
        if (!condition)                                                     \
        {                                                                   \
        printf("ASSERTION %s in %s: function %s (%d)\n", #condition, __FILE__, __func__, __LINE__);\
        fclose (LOG_txt);                                                   \
        abort();                                                            \
        }

#ifdef DEBUG_
    #define DEBUG
#else
    #define DEBUG if (0)
#endif // DEBUG

#ifdef DEBUG_
    #define ASSERT_TREE(); \
            DEBUG fprintf (LOG_txt, "asserting...\n");\
            if (!TreeOK (first_node))\
            {                 \
            Dump();    \
            assert(0);        \
            }
#else
    #define ASSERT_TREE();
#endif

Tree_t::Tree_t ():
    first_node (nullptr),
    size (0)

    {
        LOG_txt = fopen (LOG, "w");

        ASSERT_TREE()

        DEBUG fprintf (LOG_txt, "constructed successfully\n\n");
    }

Tree_t::~Tree_t ()
{
    DEBUG fprintf (LOG_txt, "destructing...\n");

    ASSERT_TREE()

    if (size > 0)
        DeleteNode (first_node);

    DEBUG fprintf (LOG_txt, "destructed successfully\n\n");

    fclose (LOG_txt);
}

void Tree_t::DeleteNode (Node* current)
{
    DEBUG fprintf (LOG_txt, "%s started\n", __func__);

    ASSERT_TREE()

    if (!NodeOK (current))
    {
        fprintf (LOG_txt, "BAD POINTER [%p]\n", current);
        assert (0);
    }

    if (current->left)
    {
        Node* cur_cop = current->left;

        current->left = nullptr;

        DeleteNode (cur_cop);
    }

    if (current->right)
    {
        Node* cur_cop = current->right;

        current->right = nullptr;

        DeleteNode (cur_cop);
    }

    delete current;
    size--;

    ASSERT_TREE()

    DEBUG fprintf (LOG_txt, "%s ended successfully\n\n", __func__);
}

void Tree_t::PushLeft (Node* node, const TreeElem_t value)
{
    DEBUG fprintf (LOG_txt, "%s started\n", __func__);

    ASSERT_TREE()

    if (!NodeOK (node))
    {
        fprintf (LOG_txt, "BAD POINTER [%p]\n", node);
        assert (0);
    }

    if (size >= MaxTree)
    {
        fprintf (LOG_txt, "NOT ENOUGH MEMORY TO PUSH LEFT\n");
        assert (0);
    }

    if (node->left)
        DeleteNode (node->left);

    Node* node_left = new Node;

    node_left->value = value;
    node_left->first = first_node;

    node_left->left = nullptr;
    node_left->right = nullptr;

    node->left = node_left;
    size++;

    ASSERT_TREE()

    DEBUG fprintf (LOG_txt, "%s ended successfully\n\n", __func__);
}

void Tree_t::PushRight (Node* node, const TreeElem_t value)
{
    DEBUG fprintf (LOG_txt, "%s started\n", __func__);

    ASSERT_TREE()

    if (!NodeOK (node))
    {
        fprintf (LOG_txt, "BAD POINTER [%p]\n", node);
        assert (0);
    }

    if (size >= MaxTree)
    {
        fprintf (LOG_txt, "NOT ENOUGH MEMORY TO PUSH RIGHT\n");
        assert (0);
    }

    if (node->right)
        DeleteNode (node->right);

    Node* node_right = new Node;

    node_right->value = value;
    node_right->first = first_node;

    node_right->left = nullptr;
    node_right->right = nullptr;

    node->right = node_right;
    size++;

    ASSERT_TREE()

    DEBUG fprintf (LOG_txt, "%s ended successfully\n\n", __func__);
}

bool Tree_t::NodeOK (const Node* node)
{
    DEBUG fprintf (LOG_txt, "CHECKING NODE\n");

    if (size == 0)
        return 1;

    return node && node->first == first_node && node->canaryleft == Crashcan1 && node->canaryright == Crashcan2;
}

bool Tree_t::TreeOK (const Node* current)
{
    if (size == 0)
        return 1;

    if (current == first_node)
        return this && first_node && size >= 0 && canaryleft == Crashcan1 && canaryright == Crashcan2 &&
                NodeOK (current) && TreeOK (current->left) && TreeOK (current->right);

    else
        return NodeOK (current) && TreeOK (current->left) && TreeOK (current->right) || !current;
}

void Tree_t::Dump ()
{
    FILE* output_file = fopen ("Dump.dot", "w");
    assert (output_file);

    fprintf (output_file, "digraph Tree\n{\nrankdir=TB\n");
    fprintf (output_file, "\t\tnode [ shape = \"box\", color = \"black\" ]\n");
    fprintf (output_file, "\t\tedge [ color = \"black\" ]\n\n");

    if (canaryleft != Crashcan1)
        fprintf (output_file, "\tCanaryLEFT [ label = \"CanaryLEFT = %d\\nBUT EXPECTED %d\","
                 "color = \"red\" ]\n", canaryleft, Crashcan1);
    else
        fprintf (output_file, "\tCanaryLEFT [ label = \"CanaryLEFT = %d\" ]\n", canaryleft);

    if (canaryright != Crashcan2)
        fprintf (output_file, "\tCanaryRIGHT [ label = \"CanaryRIGHT = %d\\nBUT EXPECTED %d\","
                 "color = \"red\" ]\n", canaryright, Crashcan2);
    else
        fprintf (output_file, "\tCanaryRIGHT [ label = \"CanaryRIGHT = %d\" ]\n", canaryright);

    if (size >= 0)
        fprintf (output_file, "\tsize [ label = \"size = %d\" ]\n\n", size);
    else
        fprintf (output_file, "\tsize [ label = \"size = %d\\n<0\", color = \"red\"]\n\n", size);

    int number = 1;

    if (size > 0)
        NodeDumps (output_file, first_node, 0, &number, "none");

    if (number != size)
        fprintf (output_file, "\tsize [color = \"red\" ]");

    fprintf (output_file, "}");
    fclose (output_file);


    system ("dot -Tpng Dump.dot -o Dump.png");
}

void Tree_t::NodeDumps (FILE* output_file, const Node* cur, const int parent, int* number, const char* side)
{
    const int number_copy = *number;
    fprintf (output_file, "subgraph clust%d{\nrankdir = HR;\n", *number);

    fprintf (output_file, "\telem%d [ shape = \"record\", label = \"{ <pointer> %d\\n%p|"
             "{ canaryLEFT\\n%d | canaryRIGHT\\n%d } |  { value\\n "
             , *number, *number, cur, cur->canaryleft, cur->canaryright);

    bool isnormal = dump (cur->value, output_file) && NodeOK (cur);

    fprintf (output_file, " | <first> first\\n%p } | { <left> left\\n%p | <right> right\\n%p } }\", color = "
             , cur->first, cur->left, cur->right);

    if (isnormal)
        fprintf (output_file, "\"blue\" ]\n");
    else
        fprintf (output_file, "\"red\" ]\n");

    fprintf (output_file, "}\n\n");

    if (!strcmp (side, "left"))
    {
        fprintf (output_file, "\t\telem%d:<left> -> elem%d\n", parent, *number);
    }
    else if (!strcmp (side, "right"))
    {
        fprintf (output_file, "\t\telem%d:<right> -> elem%d\n", parent, *number);
    }

    //fprintf (output_file, "\t\telem%d:<first> -> elem1:<pointer>\n", *number);

    if (cur->left)
    {
        (*number)++;
        NodeDumps (output_file, cur->left, number_copy, number, "left");
    }

    if (cur->right)
    {
        (*number)++;
        NodeDumps (output_file, cur->right, number_copy, number, "right");
    }
}

void Tree_t::PushFirst (const TreeElem_t value)
{
    DEBUG fprintf (LOG_txt, "%s started\n", __func__);

    ASSERT_TREE()

    if (size)
    {
        DeleteNode (first_node);
    }

    Node* first = new Node;

    first->first = first;

    first->left = nullptr;
    first->right = nullptr;

    first->value = value;

    first_node = first;

    size++;

    ASSERT_TREE()

    DEBUG fprintf (LOG_txt, "%s ended successfully\n\n", __func__);
}

Node* Tree_t::First ()
{
    DEBUG fprintf (LOG_txt, "%s started\n", __func__);

    ASSERT_TREE()

    if (!NodeOK (first_node))
    {
        fprintf (LOG_txt, "EMPTY TREE\n");
        assert (0);
    }

    DEBUG fprintf (LOG_txt, "%s ended successfully\n\n", __func__);

    return first_node;
}


int Tree_t::Size ()
{
    DEBUG fprintf (LOG_txt, "%s started\n", __func__);
    ASSERT_TREE()
    DEBUG fprintf (LOG_txt, "%s ended successfully\n\n", __func__);

    return size;
}

bool Tree_t::dump (const int value, FILE* output_file)
{
    fprintf (output_file, "%d ", value);

    return 1;
}

bool Tree_t::dump (const double value, FILE* output_file)
{
    if (isnan (value))
    {
        fprintf (output_file, "%f\\nnot a number ", value);

        return 0;
    }

    else
    {
        fprintf (output_file, " %f ", value);

        return 1;
    }
}

bool Tree_t::dump (const void* value, FILE* output_file)
{
    TreeElem_t* val = (TreeElem_t*) value;

    if (val == nullptr)
    {
        fprintf (output_file, " %p\\nzero pointer ", val);

        return 0;
    }

    else
    {
        fprintf (output_file, " %p ", val);

        return 1;
    }
}
