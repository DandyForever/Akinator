#include <iostream>
#include "Tree_t.h"
#include <assert.h>
#include "stack_t.h"

enum commands
{
    PLAY = 1,
    LOAD = 2,
    SAVE = 3,
    VIS  = 4,
    TELL = 5,
    COMP = 6,
    EXIT = 0
};

const int MaxString = 300, AnsSize = 3;

void Read                        (char* string);
void Readf                       (char* string, FILE* file, const char separator);
bool FindChar                    (const char* name, Stack_t* info, Node* current);
void TellAbout                   (Tree_t* data);
void CompareCharacters           (Tree_t* data);
void LoadNode                    (FILE* loading, Node* parent, const int depth, Tree_t* data, const char separator);
void LoadFromFile                (const char* file_name, Tree_t* data);
void SaveToFile                  (FILE* output_file, Node* current, const int depth);
void NewCharacter                (Tree_t* data, Node* current);
void PlayAkinator                (Tree_t* data);
void NodeVisual                  (FILE* output_file, Node* current, const int parent, int* number, const char* side);
void Visualise                   (Tree_t* data);
void Menu                        (Tree_t* data);

int main()
{
    Tree_t data;
    data.PushFirst ((char*) "Rasteniye");
    data.PushLeft (data.First (), (char*) "Kot");
    data.PushRight (data.First (), (char*) "Derevo");

    Menu (&data);
    data.Dump ();
    return 0;
}

void PlayAkinator (Tree_t* data)
{
    printf ("Otvechayte na voprosi Akinatora\n'YES' - Yes, 'NO' - No\n");

    Node* current = data->First ();
    assert (current);

    char* answer = new char [AnsSize];

    while (1)
    {
        printf ("Vash personazh %s?\n", current->value);

        Read (answer);

        if (!strcmp (answer, "NO") && !current->left)
        {
            NewCharacter (data, current);
            return;
        }

        else if (!strcmp (answer, "NO") && current->left)
            current = current->left;

        else if (!strcmp (answer, "YES") && !current->right)
        {
            printf ("Eto bilo netrudno\n");
            return;
        }

        else if (!strcmp (answer, "YES") && current->right)
            current = current->right;
    }
}

void NewCharacter (Tree_t* data, Node* current)
{
    printf ("Slozhno... Kak zovut tvoego personazha?\n");

    char* character = new char [MaxString];
    assert (character);

    Read (character);

    printf ("OGO! A chem personazh '%s' otlichaetsya ot personazha '%s'?\n", character, current->value);

    char* difference = new char [MaxString];
    assert (difference);

    Read (difference);

    data->PushLeft (current, current->value);
    data->PushRight (current, character);

    current->value = difference;

    printf ("Spasibo, segodnya ya uznal mnogo novogo!\n");
}

void Visualise (Tree_t* data)
{
    FILE* output_file = fopen ("Visual.dot", "w");
    assert (output_file);

    fprintf (output_file, "digraph Tree\n{\nrankdir=TB\n");
    fprintf (output_file, "\t\tnode [ shape = \"box\", color = \"black\" ]\n");
    fprintf (output_file, "\t\tedge [ color = \"black\" ]\n\n");

    int number = 1;

    if (data->Size () > 0)
        NodeVisual (output_file, data->First (), 0, &number, "none");

    fprintf (output_file, "}");
    fclose (output_file);


    system ("dot -Tpng Visual.dot -o Visual.png");
    system ("start Visual.png");
}

void NodeVisual (FILE* output_file, Node* current, const int parent, int* number, const char* side)
{
    const int number_copy = *number;
    if (current->left || current->right)
        fprintf (output_file, "\telem%d [ shape = \"record\", label = \"{<pointer> %s | {<right>DA | <left>NET } }\","
                 " color = \"blue\" ]\n", *number, current->value);

    else
        fprintf (output_file, "\telem%d [ shape = \"record\", label = \" <pointer> %s\", color = \"blue\" ]\n",
                  *number, current->value);

    if (!strcmp (side, "left"))
    {
        fprintf (output_file, "\t\telem%d:<left> -> elem%d:<pointer>\n", parent, *number);
    }
    else if (!strcmp (side, "right"))
    {
        fprintf (output_file, "\t\telem%d:<right> -> elem%d:<pointer>\n", parent, *number);
    }

    if (current->left)
    {
        (*number)++;
        NodeVisual (output_file, current->left, number_copy, number, "left");
    }

    if (current->right)
    {
        (*number)++;
        NodeVisual (output_file, current->right, number_copy, number, "right");
    }
}

void Read (char* string)
{
    int num = 0;

    char symbol = '0';

    while (symbol != '\n')
    {
        symbol = getchar ();
        string[num] = symbol;
        num++;
    }

    string[num - 1] = '\0';
}

void SaveToFile (FILE* output_file, Node* current, const int depth)
{
    for (int i = 0; i < depth; i++)
    {
        fprintf (output_file, "\t");
    }

    fprintf (output_file, "('%s'\n", current->value);

    if (current->left)
        SaveToFile (output_file, current->left, depth + 1);

    if (current->right)
        SaveToFile (output_file, current->right, depth + 1);

    for (int i = 0; i < depth; i++)
    {
        fprintf (output_file, "\t");
    }

    fprintf (output_file, ")\n");
}

void LoadFromFile (const char* file_name, Tree_t* data)
{
    FILE* loading = fopen (file_name, "rb");
    assert (loading);

    char* first = new char [MaxString];
    assert (first);

    const char separator = getc (loading);

    getc (loading);
    getc (loading);

    Readf (first, loading, separator);

    data->PushFirst (first);

    LoadNode (loading, data->First (), 0, data, separator);

    fclose (loading);
}

void LoadNode (FILE* loading, Node* parent, const int depth, Tree_t* data, const char separator)
{
    for (int i = 0; i < depth; i++)
        getc (loading);

    if (getc (loading) == '\t')
    {
        char* current_left = new char [MaxString];
        assert (current_left);

        Readf (current_left, loading, separator);

        data->PushLeft (parent, current_left);

        LoadNode (loading, parent->left, depth + 1, data, separator);

        char* current_right = new char [MaxString];

        Readf (current_right, loading, separator);

        data->PushRight (parent, current_right);

        LoadNode (loading, parent->right, depth + 1, data, separator);
    }

    for (int i = 0; i < depth; i++)
        getc (loading);

    getc (loading);
    getc (loading);

}

void Readf (char* string, FILE* file, const char separator)
{
    int length = 0;

    getc (file);
    getc (file);

    char sym = '0';
    while (sym != separator)
    {
        sym = getc (file);
        string[length] = sym;
        length++;
    }
    string[length - 1] = '\0';

    getc (file);
    getc (file);
}


void TellAbout (Tree_t* data)
{
    printf ("Pro kogo hochesh' usnat'?\n");

    char* character = new char [MaxString];
    assert (character);

    Read (character);

    Stack_t info_char;

    bool isintree = FindChar (character, &info_char, data->First ());

    if (isintree)
    {
        Stack_t information;

        while (info_char.Size ())
            information.Push (info_char.Pop ());

        printf ("%s: ", character);

        while (information.Size () != 1)
        {
            int cur = information.Pop ();

            if (cur > 0)
                printf ("%s, ", (char*) cur);

            else
                printf ("ne %s, ", (char*) -cur);
        }

        int cur = information.Pop ();

        if (cur > 0)
            printf ("%s\n", (char*) cur);

        else
            printf ("ne %s\n", (char*) -cur);
    }

    else
        printf ("%s... Ne znayu takogo personazha\n", character);
}

bool FindChar (const char* name, Stack_t* info, Node* current)
{
    if (current->left)
    {
        info->Push (-((int) current->value));
        bool path = FindChar (name, info, current->left);
        if (path) return path;
        info->Pop ();
    }

    if (current->right)
    {
        info->Push ((int) current->value);
        bool path = FindChar (name, info, current->right);
        if (path) return path;
        info->Pop ();
    }

    if (!current->left && !current->right)
    {
        if (!strcmp (name, current->value))
            return 1;

        else
            return 0;
    }

    return 0;
}

void CompareCharacters (Tree_t* data)
{
    printf ("Kakih personazhey ti hochesh' sravnit'?\n");
    printf ("Vvedi imya pervogo:\n");

    char* charater_1 = new char [MaxString];
    assert (charater_1);

    Read (charater_1);

    Stack_t info_first;

    bool is1stintree = FindChar (charater_1, &info_first, data->First ());

    if (!is1stintree)
    {
        printf ("%s... Ne znayu takogo personazha\n", charater_1);
        return;
    }

    printf ("Vvedi imya vtorogo:\n");

    char* charater_2 = new char [MaxString];
    assert (charater_2);

    Read (charater_2);

    Stack_t info_second;

    bool is2ndintree = FindChar (charater_2, &info_second, data->First ());

    if (!is2ndintree)
    {
        printf ("%s... Ne znayu takogo personazha\n", charater_2);
        return;
    }

    if (!strcmp (charater_1, charater_2))
    {
        printf ("Shutnik! Eto odin i tot zhe personazh\n");
        return;
    }

    Stack_t first;
    Stack_t second;

    while (info_first.Size ())
        first.Push (info_first.Pop ());

    while (info_second.Size ())
        second.Push (info_second.Pop ());

    int cur1 = first.Pop (), cur2 = second.Pop ();

    if (cur1 == cur2)
    {
        printf ("Pro kazhdogo iz personazhey mozhno skazat', chto on ");

        if (cur1 > 0)
            printf ("%s", (char*) cur1);

        else
            printf ("ne %s", (char*) -cur1);

        cur1 = first.Pop ();
        cur2 = second.Pop ();

        while (cur1 == cur2)
        {
            if (cur1 > 0)
                printf (", %s", (char*) cur1);

            else
                printf (", ne %s", (char*) -cur1);

            cur1 = first.Pop ();
            cur2 = second.Pop ();
        }
        printf ("\nNo ");
    }

    else
    {
        printf ("U etih personazhey net nichego obschego\nVed' ");
    }
    printf ("%s ", charater_1);

    if (cur1 > 0)
        printf ("%s", (char*) cur1);

    else
        printf ("ne %s", (char*) -cur1);

    while (first.Size ())
    {
        cur1 = first.Pop ();

        if (cur1 > 0)
            printf (", %s", (char*) cur1);

        else
            printf (", ne %s", (char*) -cur1);
    }
    printf ("\nA %s ", charater_2);

    if (cur2 > 0)
        printf ("%s", (char*) cur2);

    else
        printf ("ne %s", (char*) -cur2);

    while (second.Size ())
    {
        cur2 = second.Pop ();

        if (cur2 > 0)
            printf (", %s", (char*) cur2);

        else
            printf (", ne %s", (char*) -cur2);
    }
    printf ("\n");
}

void Menu (Tree_t* data)
{
    while (1)
    {
        printf ("Nazhmi sootvetstvuyuschuyu cifru:\n"
                "%d - Igrat'\n%d - Zagruzit' fayl\n%d - Sohranit' fayl\n"
                "%d - Visualisirovat' bazu dannikh\n%d - Rasskazat' o personazhe\n"
                "%d - Sravnit' dvuh personazhey\n%d - Viyti\n", PLAY, LOAD, SAVE, VIS, TELL, COMP, EXIT);

        char* mode = new char [AnsSize];
        assert (mode);

        Read (mode);

        char* name = new char [MaxString];
        assert (name);

        switch (mode[0] - '0')
        {
        case PLAY:
            PlayAkinator (data);
            break;

        case LOAD:
            printf ("Napishi imya fayla, otkuda chitat'\n");
            Read (name);
            LoadFromFile (name, data);
            printf ("Dannie zagruzheni\n");
            break;

        case SAVE:
        {
            printf ("Napishi imya fayla, v kotoriy sohranit'\n");
            Read (name);
            FILE* save = fopen (name, "w");
            fprintf (save, "'\n");
            SaveToFile (save, data->First (), 0);
            fclose (save);
            printf ("Tvoy fayl gotov\n");
            break;
        }

        case VIS:
            Visualise (data);
            break;

        case TELL:
            TellAbout (data);
            break;

        case COMP:
            CompareCharacters (data);
            break;

        case EXIT:
            return;
            break;

        default:
            printf ("Takoy komandi.net\n");
        }

        printf ("\n");
    }
}
