#include "samegamewindow.hpp"
#include "ui_samegamewindow.h"
#include "boardview.hpp"

/* Qt includes */
#include <QMessageBox> // Message box
#include <QQueue> // For queue of changed blocks

/**
 * @brief SameGameWindow::SameGameWindow Constructor. Sets up BoardView and game.
 * @param parent The parent of this widget (none, because it's a main window)
 */
SameGameWindow::SameGameWindow(QWidget *parent) :
    QMainWindow(parent), // Construct parent
    c_view(new Ui::SameGameWindow), // Construct view
    e_curStat(WSTART), // Start in start state
    m_uMaxRow(4), // Start with 10 rows
    m_uMaxCol(4), // Start with 10 columns
    m_nColours(4) // Start with 4 colours
{
    c_view->setupUi(this); // Setup UI
}

/**
 * @brief SameGameWindow::~SameGameWindow Destructor. Frees memory.
 */
SameGameWindow::~SameGameWindow()
{
    /* Delete our objects */
    if (c_model != 0) // Check if model is allocated (MAKE SURE TO SET IT TO NULL AFTER DELETION)
    {
        delete c_model; // If it is, delete it
        c_model = 0; // Set model to NULL to prevent double deletions
    }

    /* Delete Qt objects */
    delete c_view;
}

/** Controller methods **/

/**
 * @brief SameGameWindow::on_actionNew_Game_triggered Handles a click on the "New Game" menu item.
 */
void SameGameWindow::on_actionNew_Game_triggered()
{
    QQueue<pair<int, int>> m_changedBlocks; // Holds blocks which were changed in model
    pair<int, int> c_curBlock; // Holds the coords of the current block to change

    /* Determine what to do based on current state */

    if (e_curStat == IGAM || e_curStat == GEND) // Click during or at the end of a game requires same handling
    {
        c_view->centralWidget->reset(); // Reset the board view to display only black

        /* Delete model if necessary */
        if (c_model != 0) // Model exists
        {
            delete c_model; // Delete it
            c_model = 0; // Set it to null to indicate that it doesn't need to be deleted again
        }
    }

    /* Create a game */
    c_view->centralWidget->setBoardSize(m_uMaxRow, m_uMaxCol); // Tell view to resize itself to m_uMaxRow x m_uMaxCol
    c_model = new Game(m_uMaxRow, m_uMaxCol, m_nColours); // Create a new game with the current size, and the current # of colours
    m_changedBlocks = c_model->getChangedBlocks(); // Get the queue of changed blocks from the model

    while (!m_changedBlocks.isEmpty()) // Loop through all changes in queue
    {
        c_curBlock = m_changedBlocks.dequeue(); // Fetch the coords of the next block to change
        c_view->centralWidget->setSquareColour(get<0>(c_curBlock), get<1>(c_curBlock), c_model->getColourFromIndex(c_model->getBlockColour(get<0>(c_curBlock), get<1>(c_curBlock)))); // Update the view's colour at this location with the new colour in the model
    }

    c_model->clearChangedBlocks(); // Tell the model to clear its queue
    e_curStat = IGAM; // Change to "in game" state
}
