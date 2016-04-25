#include "samegamewindow.hpp" // This window's header
#include "ui_samegamewindow.h" // This window's UI header

/* Our includes */
#include "boardview.hpp" // Board widget

/* Qt includes */
#include <QMessageBox> // Message box
#include <QEvent> // For events
#include <QMouseEvent> // For mouse events

/* STL includes */
#include <utility> // pair

/* Debugging */
#include <QDebug> // qDebug()

/**
 * @brief SameGameWindow::SameGameWindow Constructor. Sets up BoardView and game.
 * @param parent The parent of this widget (none, because it's a main window)
 */
SameGameWindow::SameGameWindow(QWidget *parent) :
    QMainWindow(parent), // Construct parent
    c_view(new Ui::SameGameWindow), // Construct main view
    c_ngdiag(new NewGameDialog), // Create the "new game "New Game" dialog
    c_model(0), // Init model to NULL to prevent accidental deletion
    e_curStat(WSTART), // Start in start state
    m_uMaxRow(5), // Start with x rows
    m_uMaxCol(5), // Start with x columns
    m_nColours(4) // Start with 4 colours
{
    c_view->setupUi(this); // Setup UI
    c_view->centralWidget->installEventFilter(this); // We will catch and handle the board's events
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
    delete c_ngdiag; // Delete "New Game" dialog
    delete c_view; // Delete main view
}

/*** Controller methods ***/

/**
 * @brief SameGameWindow::on_actionNew_Game_triggered Handles a click on the "New Game" menu item.
 */
void SameGameWindow::on_actionNew_Game_triggered()
{
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

    /** Create a game **/
    c_ngdiag->exec(); // Show the dialog modally (block input to main window)

    /* Only start a new game if the user clicked "OK" */
    if (c_ngdiag->getWasAccepted()) // User clicked "OK"
    {
        /* Set up game variables based on user input */
        m_uMaxRow = c_ngdiag->getNumRows(); // Store the # of rows chosen by the user
        m_uMaxCol = c_ngdiag->getNumCols(); // Store the # of columns chosen by the user
        m_nColours = c_ngdiag->getNumColours(); // Store the # of colours chosen by the user

        c_view->centralWidget->setBoardSize(m_uMaxRow, m_uMaxCol); // Tell view to resize itself to m_uMaxRow x m_uMaxCol
        c_model = new Game(m_uMaxRow, m_uMaxCol, m_nColours); // Create a new game with the current size, and the current # of colours
        updateView(); // Update the view with the new changes in the model
        e_curStat = IGAM; // Change to "in game" state
    }
}

/*** About menu actions ***/

/**
 * @brief SameGameWindow::on_actionGame_triggered Handles a click on About->Game, which displays a message on the game.
 */
void SameGameWindow::on_actionGame_triggered()
{
    QMessageBox msgBox; // Create the message box
    msgBox.setText("Click on a block to delete all of the adjacent blocks of the same colour.\nThe more you delete at once, the more points you earn per block.\nTry to beat the high scores!"); // Set help text
    msgBox.exec(); // Show the dialog while blocking the rest of the application
}

/*** Board actions ***/

/**
 * @brief eventFilter An event filter which handles clicks on the board.
 * @param object The object which the event was originally for (in our case, the board).
 * @param event The event which occurred.
 * @return True to catch the event and stop the boardView from processing it, false to let th event pass on.
 */
bool SameGameWindow::eventFilter(QObject *object, QEvent *event)
{
    QMouseEvent* mouseEv; // Holds cast event
    QMessageBox mb; // DEBUGGING
    QString msgUnformatted = "Detected click on board at view coords (%1, %2).\nEquivalent model coords: (%3, %4)."; // Message to show user. Initialized to constant values plus parameters.
    QString msgFormatted; // Holds the formatted message
    pair<int, int> modelCoords; // Pair which holds model coords (converted by view)

    if (object == c_view->centralWidget && event->type() == QEvent::MouseButtonPress) // We will handle "clicks" on the board (a mouse button press)
    {
        if (e_curStat == IGAM) // We only care about clicks during a game
        {
            mouseEv = static_cast<QMouseEvent*>(event); // Convert the event to a mouse event

            /* DEBUGGING: Show message w/ click coords */
            //qDebug() << "Click coords: (" << mouseEv->x() << ", " << mouseEv->y() << ")"; // Log click point to see if we're fetching that

            modelCoords = c_view->centralWidget->toModelCoords(mouseEv->x(), mouseEv->y()); // Get view to convert click coords to model coords, and store them

           /* msgFormatted = msgUnformatted.arg(mouseEv->x()).arg(mouseEv->y()).arg(get<0>(modelCoords)).arg(get<1>(modelCoords)); // Replace params w/ x and y coords, and store result
            mb.setText(msgFormatted); // Set message box text to coords text
            mb.exec(); // Show the message*/

            /* Check if it's a black square. We don't care about clicks on them. */
            if (!c_model->isCellEmpty(get<0>(modelCoords), get<1>(modelCoords))) // This cell isn't empty, so we can delete blocks
            {
                c_model->removeBlock(get<0>(modelCoords), get<1>(modelCoords)); // Tell model to remove a block at this position
                updateView(); // Update the view to display the changed board

                /* Check if the game is over */
                if (c_model->isGameOver()) // The game has ended, for some reason
                {
                    e_curStat = GEND; // Go to "end" state

                    /* Check if the user lost or won */
                    if (c_model->isBoardEmpty()) // Board is empty, so user won
                    {
                        mb.setText("Congratulations, you cleared the board and won!\nHip-hip-hurray!"); // Display a congratulatory message
                        mb.exec(); // Show the message while blocking
                    }

                    else // Board isn't empty and game is over, so user must have lost (no moves left)
                    {
                        mb.setText("Unfortunately, you don't have any moves left.\nBetter luck next time!");
                        mb.exec();
                    }

                    delete c_model; // Delete the model, now that the game has finished
                    c_model = 0;
                }
            }

            return true; // We don't want the boardView to handle this event
        }

        else // We don't care about this click
        {
            return false; // Pass it on to the view
        }
    }

    else // Some other event, let boardView handle it
    {
        return false; // Pass this event on
    }
}

/** Private methods **/

/**
 * @brief updateView Updates the view by sending it updates for each changed block in the model's queue, then clears the model's queue.
 */
void SameGameWindow::updateView()
{
     /* Variables */
     QQueue<pair<int, int>> m_changedBlocks; // Holds blocks which were changed in model
     pair<int, int> c_curBlock; // Holds the coords of the current block to change

     m_changedBlocks = c_model->getChangedBlocks(); // Get the queue of changed blocks from the model

     while (!m_changedBlocks.isEmpty()) // Loop through all changes in queue
     {
         c_curBlock = m_changedBlocks.dequeue(); // Fetch the coords of the next block to change
         c_view->centralWidget->setSquareColour(get<0>(c_curBlock), get<1>(c_curBlock), c_model->getColourFromIndex(c_model->getBlockColour(get<0>(c_curBlock), get<1>(c_curBlock)))); // Update the view's colour at this location with the new colour in the model
     }

     c_model->clearChangedBlocks(); // Tell the model to clear its queue
}
