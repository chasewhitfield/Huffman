#pragma once
#include <string>
using namespace std;

class huffman_tree {
private:
    int frequency;
    char character;
    huffman_tree* left;
    huffman_tree* right;
    string binary;
    huffman_tree() : frequency(0), character('\0'), binary(""), left(nullptr), right(nullptr) {}
    huffman_tree(int x, char y) : frequency(x), character(y), binary(""), left(nullptr), right(nullptr) {}
    huffman_tree* root;

public:

    struct comp //used to compare nodes based on their frequencies
    {
        bool operator()(const huffman_tree* x, const huffman_tree* y)
        {
            return x->frequency > y->frequency; //logic via https://stackoverflow.com/questions/15646780/priority-queue-of-nodes but applied for min priority
        }
    };

    /*
    Preconditions: input is a string of characters with ascii values 0-127
    Postconditions: Reads the characters of input and constructs a
    huffman tree based on the character frequencies of the file contents
    */


    huffman_tree(const string& input);

    string traverse(huffman_tree* h, string val, char character) const;


    /*
    Preconditions: input is a character with ascii value between 0-127
    Postconditions: Returns the Huffman code for character if character is in the tree
    and an empty string otherwise.
    */

    string get_character_code(char character) const;


    /*
    Preconditions: input is a string of characters with ascii values 0-127
    Postconditions: Returns the Huffman encoding for the contents of input
    If input contains letters not present in the huffman_tree,
    return an empty string
    */

    string encode(const string& input) const;


    /*
    Preconditions: string_to_decode is a string containing Huffman-encoded text
    Postconditions: Returns the plaintext represented by the string if the string
    is a valid Huffman encoding and an empty string otherwise
    */

    string decode(const string& string_to_decode) const;
};