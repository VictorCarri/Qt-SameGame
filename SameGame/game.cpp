#include "game.hpp"

/* STL Headers */
#include <utility> // std::pair<>
#include <ctime> // time()
#include <iostream> // cerr

/* Qt headers */
#include <QDebug> // qDebug()
#include <qglobal.h> // qsrand(), qrand()
#include <QDateTime> // QDateTime class (get # of seconds since epoch for randomization)

using namespace std; // To save some typing

/* Defines */
#define BLACK 0 // Colour black - index in colour vector

/*** Types ***/
enum Direction
{
    LEFT = 1, // Start at 1
    RIGHT,
    TOP,
    BOTTOM
};

/*** Constructors/destructors ***/

/**
 * @brief Game::Game Constructor. Creates a new Game object representing a single game.
 * @param rows The number of rows in this game.
 * @param cols The number of columns in this game
 * @param nColours The number of colours to use for this game. (excluding black, which is always used).
 */
Game::Game(int rows, int cols, int nColours)
{
    int i; // Loop counter
    int r, g, b; // Hold randomised red, green, and blue values

    /* Initialise variables */
    m_maxCol = cols; // Use the given # of columns
    m_maxRow = rows; // Use the given # of rows
    c_points = 0; // Initialize points to 0
    m_nColours = nColours; // Save # of colours
    //c_cBlocks(); // Create the queue of changed blocks
    c_board = new vector<vector<int>>(m_maxRow, vector<int>(m_maxCol, 0)); // Create the board vector and initialize it to m_maxRow rows, containing m_maxCol columns of black each
    c_colours = new vector<QColor>(); // Create the vector of colours

    c_colours->push_back(QColor(0, 0, 0)); // Add black to vector first

    /* Initialise vector of colours to contain nColours random colours */
    for (i = 0; i < nColours; i++) // Create nColors random colours
    {
        r = randIntInRange(1, 255); // Random red component, but not black
        g = randIntInRange(1, 255); // Random green component, but not black
        b = randIntInRange(1, 255); // Random blue component excluding black
        //qDebug() << "Game::Game(int, int, int): Creating colour (" << r << ", " << g << ", " << b << ").";
        c_colours->push_back(QColor(r, g, b)); // Create a new colour with random R, G, and B components, and add it to the list of colorus.
    }

    initBoard(); // Set up the board
}

/**
 * @brief Game::Game Constructor. Loads a game from a file.
 * @param fname The name of the file to parse.
 */
Game::Game(string fname)
{

}

/**
 * @brief Game::~Game Destructor. Cleans up memory and other stuff.
 */
Game::~Game()
{
    try
    {
        delete c_board; // Finally, Delete the board vector
        delete c_colours; // Delete the colour vector
    }

    catch (exception& e)
    {
        cerr << "Caught exception " << e.what() << endl;
    }
}

/**
 * @brief Game::getBlockColour Fetches the colour of a block at a specified x and y position.
 * @param m_x The x position of the blcok.
 * @param m_y The y position of the block.
 * @return -1 if either x or y are out of bounds or otherwise invalid, an index to the array of QColors representing that block's colour otherwise.
 */
int Game::getBlockColour(int m_x, int m_y)
{
    int colInd; // Index of colour in colours array. Fetched from board array.
    vector<int> tempVec; // Temporary vector to hold column vector

    /* Check x and y to ensure they are legal */
    if ((0 <= m_x && m_x < m_maxCol) && (0 <= m_y && m_y < m_maxRow)) // x and y must be on the board
    {
        // Everything's OK
        tempVec = c_board->at(m_y); // Fetch the columns at this row
        colInd = tempVec.at(m_x); // Fetch the colour at this column

        //qDebug() << "Game::getBlockColour: index @ (" << m_x << ", " << m_y << ") = " << colInd << endl;
        return colInd; // Return the colour at this index
    }

    else // x and y aren't legal
    {
        return -1; // Let caller know
    }
}

/**
 * @brief Game::getColourFromIndex Fetches the colour associated with a given index.
 * @param ind The index to the array of colours.
 * @return White if the index is invalid, a colour otherwise.
 */
QColor Game::getColourFromIndex(int ind)
{
    if (0 <= ind && ind < m_nColours) // Index is in range
    {
        return c_colours->at(ind); // Return the colour at this index
    }

    else
    {
        return QColor(255, 255, 255); // Return white to let caller know
    }
}

/**
 * @brief Game::isGameOver Determines if the game is over by checking the board to see if it is empty, or if no moves are
 * left.
 * @return True if the game is over, false otherwise.
 */
bool Game::isGameOver()
{
    return isBoardEmpty() || noMovesLeft();
}

/**
 * @brief Game::removeBlock Removes a block at the specified (x, y) position, if one exists. Returns the # of blocks deleted.
 * @param x The x position of the block to delete (in board units).
 * @param y The y position of the block to delete (in board units).
 * @return The # of blocks deleted, in the range [0, rows x cols].
 */
int Game::removeBlock(int m_x, int m_y)
{
    int m_nBlocksRemoved = 0; // # of blocks deleted by this invocation of the method

    if (errorCheck(m_x, m_y) == 0) // X and y aren't invalid, we're not trying to delete a background block, and the block has adjacent squares of the same colour
    {
        qDebug() << "Game::removeBlock: passed error check";

        if (hasAdjBlockOfSameColour(m_x, m_y)) // Can only remove a block if it has at least 1 neighbour of the same colour
        {
            qDebug() << "Game::removeBlock: passed adj of same colour check";
            m_nBlocksRemoved = removeBlocks(m_x, m_y, c_board->at(m_y).at(m_x)); // Remove all adjacent blocks of this colour
            qDebug() << "Game::removeBlock: passed removeBlocks";

            if (m_nBlocksRemoved > 0) // Blocks were removed
            {
                compactBoard(); // Push together blocks, top to bottom, right to left, to get rid of gaps caused by the deletion
            }
        }
    }

    return m_nBlocksRemoved; // Return # of blocks removed so that caller can do something with it
}

/**
 * @brief Game::isBoardEmpty Determines if the board is completely empty.
 * @return True if the board is empty, false otherwise.
 */
bool Game::isBoardEmpty()
{
    int r; // Row counter
    int c; // Column counter

    for (r = 0; r < m_maxRow; r++) // Loop through the rows
    {
        for (c = 0; c < m_maxCol; c++) // Loop through the columns
        {
            if (c_board->at(r).at(c) != BLACK) // This cell isn't black
            {
                return false; // Found a cell which isn't black, therefore the board isn't empty
            }
        }
    }

    return true;  // If we got here, the board is empty.
}

/**
 * @brief Game::getMaxRow Fetches the # of rows in this game.
 * @return  The # of rows in this game.
 */
int Game::getMaxRow()
{
    return m_maxRow;
}

/**
 * @brief Game::getMaxCol Fetches the # of columns in this game.
 * @return  The # of columns in this game.
 */
int Game::getMaxCol()
{
    return m_maxCol;
}

/**
  * @brief Game::getChangedBlocks Fetches the queue of recently-changed blocks.
  * @return The queue of recently-changed blocks.
  */
 QQueue<pair<int, int>> Game::getChangedBlocks()
 {
     return c_cBlocks; // Return the queue
 }

 /**
  * @brief Game::clearChangedBlocks Clears the queue of recently-changed blocks.
  */
 void Game::clearChangedBlocks()
 {
     c_cBlocks.clear(); // Clear the queue
 }

/*** Private methods ***/

/**
 * @brief Game::hasAdjBlockOfSameColour Determines if the block at the given location has at least 1 neighbour of the same colour.
 * @param row The row of the block to check.
 * @param col The column of the block to check.
 * @return 1 if a neighbour with the same colour exists, 0 otherwise. Returns -1 if there are errors.
 */
int Game::hasAdjBlockOfSameColour(int m_row, int m_col)
{
    int myCol = c_board->at(m_row).at(m_col); // Store the colour of this block for later comparison. Saves some calls.
    vector<pair<int, int>> adj; // Vector containing adjacent blocks
    pair<int, int> coord; // A single coordinate in the vector

    if (errorCheck(m_row, m_col)) // Check for errors in the given coords - x and y are valid, and this isn't a black square
    {
        adj = adjBlocks(m_row, m_col); // Get a list of this cell's neighbours

        for (vector<pair<int, int>>::iterator it = adj.begin(); it != adj.end(); it++) // Loop through list of adjacent blocks
        {
            coord = *it; // Store the coordinate of the adjacent block

            if (c_board->at(get<0>(coord)).at(get<1>(coord)) == myCol) // If the block at this position is of the same colour
            {
                return 1; // Found an adjacent block of the same colour
            }
        }

        return 0; // Didn't find any neighbours with the same colour
    }

    else
    {
        return -1; // Error 1: invalid coords
    }
}

/**
 * @brief Game::adjBlocks Finds all blocks on the board adjacent to a given block.
 * @param m_row The row of the block to check.
 * @param m_col The column of the block to check.
 * @return A vector containing coordinates of adjacent blocks.
 */
vector<pair<int, int>> Game::adjBlocks(int m_row, int m_col)
{
    vector<pair<int, int>> adjBs; // Vector of adjacent blocks

    /* Check for neighbours in all directions */

    // Check for neighbour to left
    if ((0 <= (m_col-1) && (m_col-1) < m_maxCol) // Check if m_col-1 is in range [0, m_maxCol)
        &&
        (0 <= m_row && m_row < m_maxRow)) // Check if m_row is in range [0, m_maxRow)
    {
        // There is a block to the left
        adjBs.push_back(pair<int, int>(m_row, m_col-1)); // Add the coord (m_row, m_col-1) to the vector
    }

    // Check for neighbour to right
    else if ((0 <= (m_col+1) && (m_col+1) < m_maxCol) // m_col+1 is in range [0, m_maxCol)
             &&
             (0 <= m_row && m_row < m_maxRow) // m_row is in range [0, m_maxRow)
             )
    {
        // There is a block to the right
        adjBs.push_back(pair<int, int>(m_row, m_col+1)); // Add coord (m_row, m_col+1) to list of coords
    }

    // Check for neighbour above
    else if ((0 <= m_col && m_col < m_maxCol) // m_col is in range [0, m_maxCol)
             &&
             (0 <= (m_row-1) && (m_row-1) < m_maxRow) // m_row-1 is in range [0, m_maxRow)
             )
    {
        // There is a square above
        adjBs.push_back(pair<int, int>(m_row-1, m_col));
    }

    // Check for neighbour below
    else if ((0 <= m_col && m_col < m_maxCol) // m_col is in range [0, m_maxCol)
             &&
             (0 <= (m_row+1) && (m_row+1) < m_maxRow) // m_row+1 is in range [0, m_maxRow)
             )
    {
        // There is a block below
        adjBs.push_back(pair<int, int>(m_row+1, m_col)); // Add the coord (m_row+1, m_col) to the vector
    }

    return adjBs; // Return the vector of pairs (coordinates)
}

/*
 * @brief Game::initBoard Sets up the board for a new game by creating the array and initialising it to the given size
 * (rows x cols), then setting each cell to a random colour.
 */
void Game::initBoard()
{
    /* Data structures */
    vector<int> usedDirs; // Vector which holds directions that have already been chosen (to prevent our selecting the same direction a second time)

    /* Loop-control booleans */
    bool dirChosen = false; // Controls loop which chooses a direction to go in

    /* Algo vars */
    int randColInd; // Index of random colour to use
    int randDir; // Random direction to move in
    int nFSqs; // Number of squares free in the chosen direction
    int c_curX; // Current x pos
    int c_curY; // Current y pos
    int nToFill; // # of squares to fill in a given direction. Must be <= nFSqs.

    /* Other vars */
    int r = 0; // Row loop counter
    int c = 0; // Column loop counter

    for (r = 0; r < m_maxRow; r++) // Loop through rows
    {
        for (c = 0; c < m_maxCol; c++) // Loop through columns
        {
            /*col = randIntInRange(1, c_colours->size()-1); // Store index for debugging. Exclude black, so that board is filled.
            //qDebug() << "Game::initBoard: colour index at (" << r << ", " << c << ") = " << col << endl;
            c_board->at(r)[c] = col; // Set the value at this row and column to the generated colour index
            c_cBlocks.enqueue(pair<int, int>(c, r)); // Add the coords of the initialised block to the queue of changed blocks so that it can be processed by the controller later on*/

            //qDebug() << "Checking cell (" << c << ", " << r << ")";

            if (c_board->at(r).at(c) == BLACK) // We don't want to change coloured squares
            {
                //qDebug() << "Cell (" << c << ", " << r << ") is black.";
                randColInd = randIntInRange(1, c_colours->size()-1); // Choose a random colour index
                //qDebug() << "Chosen colour index = " << randColInd;
                c_board->at(r)[c] = randColInd; // Set this cell's colour to the randomly-chosen one
                c_cBlocks.enqueue(pair<int, int>(c, r)); // Add changed block to queue
                //qDebug() << "After assignment, colour index at (" << c << ", " << r << ") = " << c_board->at(r).at(c);

                /* Choose a random direction with at least 1 black square */
                while (!dirChosen) // Loop until a direction with at least 1 black square has been chosen
                {
                    //qDebug() << "No dir chosen yet.";
                    randDir = randIntInRange(LEFT, BOTTOM); // Choose a random direction
                    //qDebug() << "Random dir = " << randDir;

                    /* Ensure that we haven't already checked that direction */
                    if (find(usedDirs.begin(), usedDirs.end(), randDir) == std::end(usedDirs)) // The list doesn't contain that direction, so we haven't used it yet
                    {
                        //qDebug() << "Checking (currently) unchecked dir " << randDir;

                        /* Check if there is at least 1 black square in that direction */
                        switch (randDir) // Handle checks in different directions separately
                        {
                            case LEFT: // Checking the left
                            {
                                //qDebug() << "Checking left" << endl << "errorCheck(" << c-1 << ", " << r << ") = " << errorCheck(c-1, r);

                                /* Use errorCheck to determine if square exists and is black. errorCheck returns -2 in this case */
                                if (errorCheck(c-1, r) == -2) // Square to left exists and is black
                                {
                                    dirChosen = true; // We have found the direction in which we want to go, end the while loop after the next bit of code which finds the # of squares free in this direction

                                    /* Set up for counting black squares */
                                    c_curX = c-2; // Start 2 cols to the left (we already checked the column just to our left)
                                    c_curY = r; // Start at this y pos
                                    nFSqs = 1; // Start w/ a count of 1 (the square to the left is known to be free)

                                    while (errorCheck(c_curX, c_curY) == -2) // Loop while the square to the left exists and is black
                                    {
                                        nFSqs++; // Increment # of free cells
                                        c_curX--; // Check the cell to the left on the next loop
                                    }

                                    nToFill = randIntInRange(1, nFSqs/2); // Choose between 1 and the # of free squares in this direction to fill.
                                    c_curX = c-1; // Move 1 step to the left

                                    while (nToFill > 0) // Keep filling squares until we have filled all of the ones which we wanted to fill
                                    {
                                        c_board->at(c_curY)[c_curX] = randColInd; // Set the square to the randomly-chosen colour
                                        c_cBlocks.enqueue(pair<int, int>(c_curX, c_curY)); // Add changed block to queue
                                     //   qDebug() << "Set (" << c_curX << ", " << c_curY << ") to " << c_board->at(c_curY).at(c_curX);
                                        c_curX--; // Move left for next loop
                                        nToFill--; // Count this square to stop loop eventually
                                    }
                                }

                                break;
                            }

                            case RIGHT: // Checking the right
                            {
                                //qDebug() << "Checking right" << endl << "errorCheck(" << c+1 << ", " << r << ") = " << errorCheck(c+1, r);

                                /* Use errorCheck to determine if square exists and is black. errorCheck returns -2 in this case */
                                if (errorCheck(c+1, r) == -2) // Square to right exists and is black
                                {
                                    dirChosen = true; // We have found the direction in which we want to go, end the while loop after the next bit of code which finds the # of squares free in this direction

                                    /* Set up for counting black squares */
                                    c_curX = c+2; // Start 2 cols to the right
                                    c_curY = r; // Start at this y pos
                                    nFSqs = 1; // Start w/ a count of 1 (we've already checked the square just to our right)

                                    while (errorCheck(c_curX, c_curY) == -2) // Loop while the square to the right exists and is black
                                    {
                                        nFSqs++; // Increment # of free cells
                                        c_curX++; // Check the cell to the right on the next loop
                                    }

                                    nToFill = randIntInRange(1, nFSqs); // Choose between 1 and the # of free squares in this direction to fill.
                                    c_curX = c+1; // Move 1 step to the right

                                    while (nToFill > 0) // Keep filling squares until we have filled all of the ones which we wanted to fill
                                    {
                                        c_board->at(c_curY)[c_curX] = randColInd; // Set the square to the randomly-chosen colour
                                        c_cBlocks.enqueue(pair<int, int>(c_curX, c_curY)); // Add changed block to queue
                                  //      qDebug() << "Set (" << c_curX << ", " << c_curY << ") to " << c_board->at(c_curY).at(c_curX);
                                        c_curX++; // Move right for next loop
                                        nToFill--; // Count this square to stop loop eventually
                                    }
                                }

                                break;
                            }

                            case TOP: // Checking the top
                            {
                                //qDebug() << "Checking top" << endl << "errorCheck(" << c << ", " << r-1 << ") = " << errorCheck(c, r-1);

                                /* Use errorCheck to determine if square exists and is black. errorCheck returns -2 in this case */
                                if (errorCheck(c, r-1) == -2) // Square above exists and is black
                                {
                                    dirChosen = true; // We have found the direction in which we want to go, end the while loop after the next bit of code which finds the # of squares free in this direction

                                    /* Set up for counting black squares */
                                    c_curX = c; // Stay in this column
                                    c_curY = r-2; // Start 2 rows above (we've already checked the row just above us)
                                    nFSqs = 1; // Start w/ a count of 1 (we've already checked the row just above us)

                                    while (errorCheck(c_curX, c_curY) == -2) // Loop while the square above exists and is black
                                    {
                                        nFSqs++; // Increment # of free cells
                                        c_curY--; // Check the cell above on the next loop
                                    }

                                    nToFill = randIntInRange(1, nFSqs); // Choose between 1 and the # of free squares in this direction to fill.
                                    c_curY = r-1; // Move 1 step up

                                    while (nToFill > 0) // Keep filling squares until we have filled all of the ones which we wanted to fill
                                    {
                                        c_board->at(c_curY)[c_curX] = randColInd; // Set the square to the randomly-chosen colour
                                        c_cBlocks.enqueue(pair<int, int>(c_curX, c_curY)); // Add changed block to queue
                                  //      qDebug() << "Set (" << c_curX << ", " << c_curY << ") to " << c_board->at(c_curY).at(c_curX);
                                        c_curY--; // Move up for next loop
                                        nToFill--; // Count this square to stop loop eventually
                                    }
                                }

                                break;
                            }

                            case BOTTOM: // Checking the bottom
                            {
                                //qDebug() << "Checking bottom" << endl << "errorCheck(" << c << ", " << r+1 << ") = " << errorCheck(c, r+1);

                                /* Use errorCheck to determine if square exists and is black. errorCheck returns -2 in this case */
                                if (errorCheck(c, r+1) == -2) // Square below exists and is black
                                {
                                    dirChosen = true; // We have found the direction in which we want to go, end the while loop after the next bit of code which finds the # of squares free in this direction

                                    /* Set up for counting black squares */
                                    c_curX = c; // Stay in this column
                                    c_curY = r+2; // Start 2 rows below (we've already checked the row just below us)
                                    nFSqs = 1; // Start w/ a count of 1 (we've already checked the row just below us)

                                    while (errorCheck(c_curX, c_curY) == -2) // Loop while the square above exists and is black
                                    {
                                        nFSqs++; // Increment # of free cells
                                        c_curY++; // Check the cell below on the next loop
                                    }

                                    nToFill = randIntInRange(1, nFSqs); // Choose between 1 and the # of free squares in this direction to fill.
                                    c_curY = r+1; // Move 1 step down

                                    while (nToFill > 0) // Keep filling squares until we have filled all of the ones which we wanted to fill
                                    {
                                        c_board->at(c_curY)[c_curX] = randColInd; // Set the square to the randomly-chosen colour
                                        c_cBlocks.enqueue(pair<int, int>(c_curX, c_curY)); // Add changed block to queue
                                 //       qDebug() << "Set (" << c_curX << ", " << c_curY << ") to " << c_board->at(c_curY).at(c_curX);
                                        c_curY++; // Move down for next loop
                                        nToFill--; // Count this square to stop loop eventually
                                    }
                                }

                                break;
                            }
                        }

                        usedDirs.push_back(randDir); // Remember that we used this direction
                    }
                }

                /* At this point, we have chosen a direction to move in */
            }
        }
    }
}

/**
 * @brief randIntInRange Generates a random integer in the range [lBound, uBound].
 * @param lBound The lower bound of the range to generate a random integer from.
 * @param uBound The upper bound of the range to generate a random integer from.
 * @return A random integer in the range [lBound, uBound].
 */
int Game::randIntInRange(int lBound, int uBound)
{
    //qsrand(QDateTime::currentMSecsSinceEpoch()); // Seed RNG with current milliseconds since epoch initial value
    qsrand(qrand());
    //srand(time(NULL)); // Seed RNG
    return qrand() % ((uBound + 1) - lBound) + lBound;
    //return qrand() % (uBound - lBound) + lBound + 1;
    //srand(rand());
    //return rand() % (uBound - lBound) + lBound + 1;
    //return rand() % ((uBound+1) - lBound) + 1;
}

/**
 * @brief Game::noMovesLeft Determines if there are no legal moves left.
 * @return True if there are no legal moves left, false otherwise.
 */
bool Game::noMovesLeft()
{
    // We only need to check if there are no squares w/ at least 1 adjacent square of the same colour
    int r; // Row counter
    int c; // Column counter

    for (r = 0; r < m_maxRow; r++) // Loop through the rows
    {
        for (c = 0; c < m_maxCol; c++) // Loop through the columns
        {
            if (c_board->at(r).at(c) != BLACK) // Found coloured square - we don't care about black ones
            {
                if (hasAdjBlockOfSameColour(r, c)) // Found an adjacent block of the same colour
                {
                    return false; // This block can be deleted - at least 1 block adjacent to it has the same colour
                }
            }
        }
    }

    return true; // If we got here, there are no more moves left
}

/**
 * @brief errorCheck Checks the given location for errors.
 * @param m_x The x coord of the block to check.
 * @param m_y The y coord of the block to check.
 * @return 0 for no errors, -1 if x/y is out of bounds, -2 if this is a black block.
 */
int Game::errorCheck(int m_x, int m_y)
{
    if ((0 <= m_x && m_x < m_maxCol)
        &&
        (0 <= m_y && m_y < m_maxRow)) // x and y are within bounds
    {
        if (c_board->at(m_y).at(m_x) != BLACK) // Not trying to delete a background block
        {
            return 0; // All error checks were successful.
        }

        else // Tried to delete a background block.
        {
            return -2; // Let caller know
        }
    }

    else // x | y are out of bounds
    {
        return -1;
    }
}

/**
 * @brief Game::removeBlocks Recursively removes this block and all adjacent ones of the same colour.
 * @param x The x coord of the block to remove on this invocation.
 * @param y The y coord of the block to remove on this invocation.
 * @param col The colour of the blocks to remove. An index to the array of colours.
 * @return The number of blocks removed, >= 0.
 */
int Game::removeBlocks(int m_x, int m_y, int m_col)
{
    int nDeleted = 0; // # of blocks deleted by this call

    if (errorCheck(m_x, m_y) == 0) // We can delete a block at this location
    {
        qDebug() << "Game::removeBlocks: passed error check";

        if (hasAdjBlockOfSameColour(m_x, m_y)) // Can only delete a block if it has adjacent blocks of the same colour
        {
            qDebug() << "Game::removeBlocks: passed adjacency check";

            c_board->at(m_y)[m_x] = BLACK; // Delete the piece at this location (set square to black)
            nDeleted = 1; // Deleted 1 block
            c_points++; // Increment user's score
            c_cBlocks.enqueue(pair<int, int>(m_x, m_y)); // Add the coords of the deleted block to the queue of changed blocks

            /** TODO: Add error checks around colour checks, to ensure that the square exists **/

            /* If there are any adjacent blocks of the same colour, remove them and check their neighbours as well */
            if (c_board->at(m_y).at(m_x-1) == m_col) // Block of same colour to left
            {
              nDeleted += removeBlocks(m_x-1, m_y, m_col); // Recursively remove that block and its neighbours, and count the # of deletions
            }

            if (c_board->at(m_y).at(m_x+1) == m_col) // Same colour block to right
            {
                nDeleted += removeBlocks(m_x+1, m_y, m_col); // Delete it and any neighbours of the same colour, and count the # of blocks deleted by that call
            }

            if (c_board->at(m_y-1).at(m_x) == m_col) // Same colour block above
            {
                nDeleted += removeBlocks(m_x, m_y-1, m_col); // Delete it and its neighbours, and include # of deletions in return value
            }

            if (c_board->at(m_y+1).at(m_x) == m_col) // Same colour block below
            {
                nDeleted += removeBlocks(m_x, m_y+1, m_col); // Delete block and its neighbours, and include count in total
            }
        }
    }

    return nDeleted; // Return the # of blocks deleted on this call
}

/**
 * @brief Game::compactBoard Compacts the board after a deletion by shifting blocks left and down as far as possible.
 */
void Game::compactBoard()
{
    int c_newX; // X value to move block to when moving left
    int c_newY; // Y value to move block to when moving down
    int r; // Row counter
    int c; // Column counter

    /* Phase 1: move coloured blocks left */
    for (c = 0; c < m_maxCol; c++) // Go from left to right, pushing things left
    {
        for (r = 0; r < m_maxRow; r++) // Loop through the rows from left to right
        {
            qDebug() << "Game::compactBoard: phase 1: checking (" << c << ", " << r << ")" << endl;

            if (c_board->at(r).at(c) != BLACK) // This cell isn't empty
            {
                c_newX = c; // Start at this column
                //qDebug() << "Game::compactBoard: phase 1: c_newX = " << c_newX << " before while" << endl;

                while (c_newX > 0 && c_board->at(r).at(c_newX-1) == BLACK) // While we see empty cells to the left
                {
                    c_newX--; // Move further left
                  //  qDebug() << "Game::comapctboard: phase 1: while: c_newX = " << c_newX << " after decrement" << endl;
                }

                if (c_newX != c) // c_newX changed, therefore block was moved
                {
                    c_board->at(r)[c_newX] = c_board->at(r).at(c); // Copy colour from block's old location to its new location
                    c_cBlocks.enqueue(pair<int, int>(c_newX, r)); // Add the coords of the changed block to the queue
                    c_board->at(r)[c] = BLACK; // Delete block from old location
                    c_cBlocks.enqueue(pair<int, int>(c, r)); // Add coords of changed block to queue
                }
            }
        }
    }

    /* Phase 2: moving down */
    for (c = 0; c < m_maxCol; c++) // For each column, from left to right
    {
        for (r = m_maxRow-1; r >= 0; r--) // For each row from bottom to top
        {
            if (c_board->at(r).at(c) != BLACK) // This cell isn't empty
            {
                c_newY = r; // While loop looks down, start 1 above it
                qDebug() << "Game::compactBoard: phase 2: c_newY = " << c_newY << " before while" << endl;

                while (c_newY < (m_maxRow-1) && c_board->at(c_newY+1).at(c) == BLACK) // While we see empty cells below us and we're not yet at the last row
                {
                    c_newY++; // Move further down
                    qDebug() << "Game::comapctboard: phase 2: while: c_newY = " << c_newY << " after inrement" << endl;
                }

                if (c_newY != r) // c_newY changed, therefore the block was moved
                {
                    c_board->at(c_newY)[c] = c_board->at(r).at(c); // Copy the old block's colour to the new location
                    c_cBlocks.enqueue(pair<int, int>(c, c_newY)); // Add coords of changed block to queue
                    c_board->at(r)[c] = BLACK; // Remove tile from old location
                    c_cBlocks.enqueue(pair<int, int>(c, r)); // Add coords of changed block to queue
                }
            }
        }
    }
}

/**
 * @brief Game::getNumCols Fetches the # of colours in the game.
 * @return The number of colours in this game.
 */
int Game::getNumCols()
{
    return m_nColours; // Return the # of colours
}

/**
 * @brief isCellEmpty Determines if a given cell is empty.
 * @param m_x The x coord of the cell to check.
 * @param m_y The y coord of the cell to check.
 * @return False if the cell DNE or isn't empty, true otherwise.
 */
bool Game::isCellEmpty(int m_x, int m_y)
{
    return errorCheck(m_x, m_y) == -2; // Errorcheck returns -2 if square exists but is black
}
