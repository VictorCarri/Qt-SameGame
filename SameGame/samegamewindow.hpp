#ifndef SAMEGAMEWINDOW_H
#define SAMEGAMEWINDOW_H

/* Qt classes */
#include <QMainWindow>

/* My classes */
#include "boardview.hpp" // Board widget
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

    /* Controller methods */
private slots:
    void on_actionNew_Game_triggered(); // Handles a click on the "New Game" menu item.

private:
    Ui::SameGameWindow *c_view;
    Game *c_model; // Pointer to object which holds the current game
    State e_curStat; // Current state of game
    int m_uMaxRow; // Contains # of rows set by user. Used to create a Game object.
    int m_uMaxCol; // Contains # of columns set by user. Used to create a new Game object.
    int m_nColours; // # of colours to use for the game.
};

#endif // SAMEGAMEWINDOW_H
