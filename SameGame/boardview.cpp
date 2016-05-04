#include "boardview.hpp"

/* Qt includes */
#include <QPainter> // Painter for painting

/* STL includes */
#include <vector> // vector
#include <exception> // exception

/* C includes */
#include <cmath> // floor

/* Qt includes */
#include <QDebug> // qDebug()

/*** Public methods ***/

/**
 * @brief BoardView::BoardView Constructor. Creates a board display widget.
 * @param parent The widget's parent.
 */
BoardView::BoardView(QWidget *parent) : QWidget(parent), // Initialise parent
    gameInProg(false), // Don't draw score text initially
    c_grid(5, vector<QColor>(5, QColor(0, 0, 0))), // Initialize grid to 5x5 array of black
    dispScore(0) // Display 0 initially
{
    /* Widget setup */
    update(); // Initial paint
}

/**
 * @brief BoardView::~BoardView Destructor.
 */
BoardView::~BoardView()
{
}

/**
 * @brief BoardView::toModelCoords Converts the given click coordinates to model coordinates.
 * @param m_clickX The x co-ord of the click.
 * @param m_clickY The click's y co-ord.
 * @return A pair of integers representing an (x, y) coord in model coords.
 */
pair<int, int> BoardView::toModelCoords(int m_clickX, int m_clickY)
{
    /* Part 1: Calculate block size on screen */
    int screenWidth = width(); // Get screen width
    int screenHeight = height(); // Get screen height
    int cellWidth; // Width of a cell
    int cellHeight; // Cell height

    /* Part 2 vars */
    int c_modelX; // Model x coord
    int c_modelY; // Model y coord

    cellWidth = screenWidth / c_grid[0].size(); // Width of a cell = total width / # of columns
    cellHeight = screenHeight / c_grid.size(); // Cell height = total height / # of rows

    /* Part 2: convert click point to model point */
    c_modelX = floor(m_clickX / cellWidth); // Dividing the distance from 0 to x (x) by the width of a cell gives you the # of “cell width units” away from the left - the x co-ord of the cell, in model (cell) units
    c_modelY = floor(m_clickY / cellHeight); // Same principle as above, but for y and height

    return pair<int, int>(c_modelX, c_modelY); // Return a pair of integers containing the model coords
}

/**
 * @brief BoardView::reset Resets widget to display only black.
 */
void BoardView::reset()
{
    unsigned r; // Row counter
    unsigned c; // Column counter

    /* Set all squares to black */
    for (r = 0; r < c_grid.size(); r++) // Loop through rows
    {
        for (c = 0; c < c_grid[r].size(); c++) // Loop through columns
        {
            c_grid[r][c] = QColor(0, 0, 0); // Set the square to black
        }
    }

    update(); // Repaint
}

/**
 * @brief BoardView::setSquareColour Sets the colour of the square at (x, y) to col and redraws the board.
 * @param m_x The x coord of the square whose colour we will change.
 * @param m_y The y coord of the square whose colour we will change.
 * @param m_col The new colour of the square.
 * @return -1 if the coords are out of bounds, 0 if the change was successfully completed.
 */
int BoardView::setSquareColour(unsigned m_x, unsigned m_y, QColor m_col)
{
    /* Bounds check */
    if (m_y < c_grid.size() // y is within bounds
            && // Also need to check x
        m_x < c_grid[0].size()) // x is within bounds
    {
        c_grid[m_y][m_x] = m_col; // Store the new colour
        update(); // Redraw the board
        return 0; // Indicate successful completion
    }

    else // Incorrect coords
    {
        return -1; // Indicate error to user
    }
}

/**
 * @brief BoardView::setBoardSize Sets the size of the new grid to
 * @param m_newWidth The new width of the board.
 * @param m_newHeight The board's new height.
 */
void BoardView::setBoardSize(unsigned m_newWidth, unsigned m_newHeight)
{
    unsigned r; // Row counter

    if (m_newWidth > 0 && m_newHeight > 0) // Bounds check
    {
        c_grid.resize(m_newHeight, vector<QColor>(m_newWidth, QColor(0, 0, 0))); // Resize row vector to m_newHeight rows, filling in empty spaces with a vector of m_newWidth cols of black

        /* Ensure each column vector is of the right size */
        for (r = 0; r < c_grid.size(); r++) // Loop through the rows
        {
            c_grid[r].resize(m_newWidth, QColor(0, 0, 0)); // Resize this column vector to the new width, adding black elements if necessary
        }

        update(); // Redraw the board with the new size
    }
}

/*** Protected methods ***/

/**
 * @brief BoardView::paintEvent Paints the board on the widget when necessary.
 * @param event The paint event.
 */
void BoardView::paintEvent(QPaintEvent * m_event)
{
    QPainter painter(this); // Painter for drawing board
    QColor black(0, 0, 0); // Black colour
    int r; // Row counter
    int c; // Column counter
    int nRows = c_grid.size(); // # of rows to draw
    int nCols = c_grid[0].size(); // # of cols to draw
    int cellWidth = width() / nRows; // Width of a cell is total width / # of cells
    int  cellHeight = height() / nCols; // Cell height = total height / # of rows
    QFont scoreFont("Times New Roman", 20, 5); // Bold, non-italic, size 20 Times New Roman
    QString scoreStr = QString("Score: %1").arg(dispScore); // String to use for score. Contains token to be replaced by current score before display.

    try
    {
        //qDebug() << "Cell width = " << cellWidth << endl << "Cell Height = " << cellHeight << endl;

        painter.setPen(QPen(QBrush(black), 5)); // Draw the outline in black

        /* Draw squares of board */
        for (r = 0; r < nRows; r++) // Loop through rows of board
        {
            for (c = 0; c < nCols; c++) // Loop through columns of board
            {
                painter.drawRect(c*cellWidth, r*cellHeight, cellWidth, cellHeight); // Draw the outline at this position in black
                painter.fillRect(c*cellWidth, r*cellHeight, cellWidth, cellHeight, c_grid[r][c]); // Draw the coloured square at this position using its colour
            }
        }

        /* Determine whether or not to draw score */
        if (gameInProg) // Game is in progress, draw the score
        {
            painter.setFont(scoreFont); // Use the score font
            painter.setBrush(QBrush(QColor(255, 255, 255))); // Draw in white
            painter.setPen(QPen(QColor(255, 255, 255))); // Draw text in white
            painter.drawText((nCols-2)*cellWidth, cellHeight, scoreStr); // Draw the score string
        }
    }

    catch (exception& e) // Catch all exceptions
    {
        qDebug() << "BoardView caught standard exception, with message: " << e.what() << endl; // Print a message
    }
}

/**
 * @brief BoardView::setDispScore Sets the score to be displayed.
 * @param m_newScore The new score to display.
 */
void BoardView::setDispScore(unsigned m_newScore) // Sets the score to display
{
    dispScore = m_newScore; // Store the new score
    update(); // Redraw the screen to display the score
}

/**
 * @brief setGameInProg Sets the boolean which controls whether or not the score is drawn.
 * @param m_newGameInProg The new value of the boolean to use.
 */
void BoardView::setGameInProg(bool m_newGameInProg)
{
    gameInProg = m_newGameInProg; // Store the new value
    update(); // Redraw the screen
}
