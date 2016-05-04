#ifndef SAMEGAMEWINDOW_HPP
#define SAMEGAMEWINDOW_HPP

/* Qt classes */
#include <QMainWindow> // Base class
#include <QQueue> // For queue of changed blocks

/* My classes */

/* View */
#include "boardview.hpp" // Board widget
#include "newgamedialog.hpp" // New game dialog

/* Model */
#include "game.hpp" // Model

namespace Ui {
class SameGameWindow;
}

/*
 * State enum. Used to describe current state of game.
 *
 * States
 * ======
 * - WSTART
 *  _ Waiting for game to start
 *
 * - IGAM
 *  _ Game is in progress (user is clicking blocks to delete them)
 *
 * - GEND
 *  _ Game has ended
 *  _ Display message, destroy board instance and reset view.
 *  _ Go back to WSTART
*/
enum State { WSTART, IGAM, GEND };

/**
 * @brief The SameGameWindow class. This class is both the main View class and the controller.
 */
class SameGameWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit SameGameWindow(QWidget *parent = 0);
        ~SameGameWindow();

    /** Controller methods **/
    private slots:
        /* File menu actions */
        void on_actionNew_Game_triggered(); // Handles a click on the File->"New Game" menu item.

        /* Help menu actions */
        void on_actionGame_triggered(); // Handles a click on the Help->"How to play" menu item.

        /* Other */
        bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE; // Filters events for the board view and handles clicks

    private:
            /* Helper methods */
            void updateView(); // Updates the view using the model's queue of changed blocks, and also clears the model's queue

            /* View vars */
            Ui::SameGameWindow *c_view; // Game window
            NewGameDialog *c_ngdiag; // New game dialog

            /* Model vars */
            Game *c_model; // Pointer to object which holds the current game

            /* Controller vars */
            State e_curStat; // Current state of game
            int m_uMaxRow; // Contains # of rows set by user. Used to create a Game object.
            int m_uMaxCol; // Contains # of columns set by user. Used to create a new Game object.
            int m_nColours; // # of colours to use for the game.
            QEvent* event;
};

#endif // SAMEGAMEWINDOW_HPP
