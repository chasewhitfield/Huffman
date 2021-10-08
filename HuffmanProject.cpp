
#include <iostream> //include statements needed for compilation
#include <string>
#include <map>
#include <vector>
#include <queue>

using namespace std; //standard namespace used as not others are

class huffman_tree {
private: //declaration of data structures here
    struct treeNode { //treeNode, essentially a container for each character, its frequency, and left and right children
        int frequency; //frequency obviously stored as an int 
        int ASCIIInt; //we use the int value of each character read in so that all nodes generated as sums of character frequencies in tree construction can have invalid value 128 and be easily filtered 

        treeNode* left;
        treeNode* right;
        treeNode() : frequency(0), ASCIIInt(128), left(nullptr), right(nullptr) {} //default values used for generated sums in tree creation
        treeNode(int x, int y) : frequency(x), ASCIIInt(y), left(nullptr), right(nullptr) {} //used to create a new node for each character
    };

    struct comp //used to compare nodes based on their frequencies
    {
        bool operator()(const treeNode* x, const treeNode* y) //requires 2 treeNode pointers to be used as means of comparison
        {
            return x->frequency > y->frequency; //logic via https://stackoverflow.com/questions/15646780/priority-queue-of-nodes but applied for min priority
        }
    };

    treeNode* root; //define root variable to originate from when traversing
    map<int, int> freq; //map 1 maps each ASCII value to its frequency of appearence
    map<char, string> code; //map 2 is more explicit, mapping each ASCII in character mode (as read in) to its binary code
    priority_queue<treeNode*, vector<treeNode*>, comp> nodes; //min priority queue used in construction of Huffman tree

public:

    /*
    Preconditions: input is a string of characters with ascii values 0-127
    Postconditions: Reads the characters of input and constructs a
    huffman tree based on the character frequencies of the file contents
    */

    huffman_tree(const string& input) //constructor requiring string to be read in
    {
        for (unsigned int n = 0; n < input.length(); n++) //read in characters to frequency char via ordered map
        {
            freq[(int)input.at(n)]++;
        }

        if (input.length() != 0) //if we don't have an empty input (avoid runtime errors)
        {
            for (auto i : freq) //iterate through freq map
            {
                treeNode* temp = new treeNode(i.second, i.first); //create a new treeNode with the frequency from our freq map and its ASCII int value
                nodes.push(temp); //add it to the min priority queue upon creation
            }
            while (nodes.size() != 1) //while we have more than the eventual root node in the queue
            {
                treeNode* hold1 = nodes.top(); //hold1 = ASCII value with lowest frequency
                nodes.pop();
                treeNode* hold2 = nodes.top(); //hold2 = ASCII value with 2nd lowest frequency (note: could be same as hold1 but behind hold1 lexographically likely)
                nodes.pop();
                treeNode* newguy = new treeNode(hold1->frequency + hold2->frequency, 128); //create a new node that is the sum of those 2 frequencies
                newguy->left = hold1; //assign hold1 as its left child
                newguy->right = hold2; //assign hold 2 as its right child
                nodes.push(newguy); //add new node into priority queue
            }

            root = nodes.top(); //our root value is whatever remains at the top of the heap (a sum)

            treeNode* temp = root; //assign temporary node to root as it will begin traversal process soon
            if (root->left == nullptr && root->right == nullptr) //if we only have the root (one character exists)
            {
                code[(char)root->ASCIIInt] = "0"; //it gets value 0 for binary
            }
            else
            {
                traverse("", temp); //otherwise, we can traverse as normal
            }
        }
    }
        

        

    /*
    Preconditions: input is a character with ascii value between 0-127
    Postconditions: Returns the Huffman code for character if character is in the tree
    and an empty string otherwise.
    */

    void traverse(string s, treeNode* t)
    {
        if (t->left == nullptr && t->right == nullptr) //if we have reached a leaf
        {
            if (t->ASCIIInt < 128) //and if the ASCII int value is in the valid range (meaning it wasn't assigned otherwise due to it being a generated sum)
            {
                char c = (char)t->ASCIIInt; //char variable is equal to the ASCII value casted to a char
                code[c] = s; //the second map comes into play, map the char to the string binary value generated 
            }
        }
        else //not a leaf?
        {
            if (t->left != nullptr)
            {
                traverse(s + "0", t->left); //move to left child if it exists and tack on a 0 to its binary string definition
            }
            if (t->right != nullptr)
            {
                traverse(s + "1", t->right); //move to right child if it exists and tack on a 1 to its binary string defintion
            }
        }
    }

    string get_character_code(char character) const //used eventually in encoding process
    {
        bool found = false; //the character hasn't been found yet in the second map of chars to their binary values
        for (auto it : code) //iterate through map of chars to binarys
        {
            if (it.first == character) //if we've found the desired key
            {
                found = true; //found is now true
                return it.second; //return the binary value it corresponds to
            }
        }
        if (!found) //if we couldn't find it, return the empty string
        {
            return "";
        }
    }


    /*
    Preconditions: input is a string of characters with ascii values 0-127
    Postconditions: Returns the Huffman encoding for the contents of input
    If input contains letters not present in the huffman_tree,
    return an empty string
    */

    string encode(const string& input) const
    {
        string encoded = ""; //start with empty string 
        if (input.length() == 0)
        {
            return ""; //no input to encode = no need for output beyond empty string
        }
        else if (input.length() == 1) //length of 1 obviously only warrants the 0 value to be returned as an encoder
        {
            return "0";
        }
        else //now if we have an input beyond all this
        {
            for (unsigned int n = 0; n < input.length(); n++) //go through each part of the encoded string
            {
                string bin = get_character_code(input.at(n)); //get the character code of that char as described in above methods
                if (bin == "")
                {
                    return ""; //empty string means that character has no code so we can't encode it and thus return the empty string
                }
                else
                {
                    encoded += bin; //otherwise, tack on that binary value to our encoded string
                }
            }
        }
        return encoded; //once completed, return the encoded string in its entirety
    }


    /*
    Preconditions: string_to_decode is a string containing Huffman-encoded text
    Postconditions: Returns the plaintext represented by the string if the string
    is a valid Huffman encoding and an empty string otherwise
    */

    string decode(const string& string_to_decode) const
    {
        string decoded = ""; //start with empty decode string that will be consistently added to
        if (string_to_decode.length() == 0)
        {
            return ""; //clearly nothing to show decoded if nothing is encoded
        }
        treeNode* temp = root;
        if (temp->left == nullptr && temp->right == nullptr) //if we have the root only, there is only one character in existence
        {
            for (unsigned int n = 0; n < string_to_decode.length(); n++)
            {
                decoded += (char)temp->ASCIIInt; //add that one character as many times as there are numbers in the encoded string (one character = code 0 so length directly relates to number of repeated entries of that one character)
            }
            return decoded; //return that string
        }
        else //more than just a root
        {
            for (unsigned int n = 0; n < string_to_decode.length(); n++) //while we have more to decode
            {
                if (string_to_decode.at(n) == '0') //if it is a 0
                {
                    temp = temp->left; //go to left child
                }
                else
                {
                    temp = temp->right; //1 = go to right child
                }

                if (temp->left == nullptr && temp->right == nullptr) //if we've reached a leaf
                {
                    decoded += (char)temp->ASCIIInt; //tack on that nodes, character for its ASCII value to the decoded string
                    temp = root; //return to root since we have to reiterate
                }
            }
        }
        return decoded; //return our final decoded message
    }
};

