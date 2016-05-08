#ifndef GAME_HPP
#define GAME_HPP

/* QT headers */
#include <QColor> // Qt colour class for the board
#include <QQueue> // Qt queue class
#include <QString> // QT strings

/* C++ Headers */
#include <array> // STL Arrays
#include <vector> // STL vectors

using namespace std;

/**
 * @brief The Game class. This class contains all of the data for a single game. It is created when the game starts, and is
 * destroyed when it ends.
 */
class Game
{
    public:
        /* Constructors/destructors */
        Game(int rows, int cols, int nColours);
        Game(QString fname); // Constructor. Creates a new game object containing data loaded from a file with the name fname.
        ~Game(); // Destructor. Deletes the new-ed variables and performs other cleanup as necessary.
        int getBlockColour(int m_x, int m_y); // Fetches the colour index of the block at the given index
        QColor getColourFromIndex(int ind); // Fetches the colour associated with a given index
        bool isGameOver(); // Determines if the game is over and returns true if it is, false otherwise
        int removeBlock(int x, int y); // Removes the block at a given (x, y) position on the board, and returns the # of blocks deleted
        bool isBoardEmpty(); // Determines if the board is empty
        int getMaxRow(); // Fetches the # of rows in this game
        int getMaxCol(); // Fetches the # of columns in this game
        QQueue<pair<int, int>> getChangedBlocks(); // Returns the queue of recently-changed blocks
        void clearChangedBlocks(); // Empties the queue of changed blocks
        int getNumCols(); // Returns the number of colours
        bool isCellEmpty(int m_x, int m_y); // Returns true if the cell at the given (x, y) pos exists and is empty, false otherwise
        int getPoints(); // Fetches the user's score
        int save(QString fname); // Saves the game's data to the given file name

    private:
        /** Game methods **/
        void initBoard(); // Sets up the board for a new game
        int hasAdjBlockOfSameColour(int m_row, int m_col); // Determines if a given cell has any neighbour of the same colour.
        vector<pair<int, int>> adjBlocks(int m_row, int m_col); // Returns a vector containing the coordinates of all squares adjacent to the given one
        bool noMovesLeft(); // Returns true if no legal moves can be made, false otherwise
        int errorCheck(int m_x, int m_y); // Checks the given location for errors
        int removeBlocks(int m_x, int m_y, int m_col); // Removes all adjacent blocks of a given colour. Starts at the given (x, y) pos, and recurses on neighbouring blocks.
        void compactBoard(); // Compacts board after a deletion by shifting blocks left and down

        /* Helper functions */
        int randIntInRange(int lBound, int uBound); // Returns a random integer in the range [lBound, uBound]

        /* Game Data */
        vector<vector<int>> *c_board; // Vector of vectors which represents the board. Each int is an index to the array of colours. Initialized to contain 10 rows
        vector<QColor> *c_colours; // Vector of colours to pick cell colours from. A vector is used for extensibility - we can add more colours as we please.
        int m_maxCol; // Number of columns
        int m_maxRow; // Number of rows
        int c_points; // Number of points
        int m_nColours; // Number of colours
        QQueue<pair<int, int>> c_cBlocks; // Queue which holds coords of changed blocks for controller to query
};

#endif // GAME_HPP
