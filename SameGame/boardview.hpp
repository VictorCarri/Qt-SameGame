#ifndef BOARDVIEW_HPP
#define BOARDVIEW_HPP

/* Qt includes */
#include <QWidget>

/* STL includes */
#include <vector> // vector
#include <utility> // pair

/* My includes */
#include "game.hpp"

/* Namespaces */
using namespace std; // Use the STL namespace to save typing

class BoardView : public QWidget
{
    Q_OBJECT

    public:
        explicit BoardView(QWidget *parent = 0);
        ~BoardView(); // Destructor
        void reset(); // Resets the view to display only black rectangles
        pair<int, int> toModelCoords(int m_clickX, int m_clickY); // Converts a click position to a model position
        int setSquareColour(unsigned m_x, unsigned m_y, QColor m_col); // Sets the colour of a square at the given (x, y) pos to the given colour and redraws the board
        void setBoardSize(unsigned m_newWidth, unsigned m_newHeight); // Resizes grid to new width and height
        void setDispScore(unsigned m_newScore); // Sets the score to display
        void setGameInProg(bool m_newGameInProg); // Sets the "game in progress" variable which determines whether or not to draw the score

    protected:
        /* Event handlers */
        void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE; // Handles painting by redrawing the board

        /* Event signals */
    signals:

        /* Event handlers */
    public slots:

    private: // Private stuff
        vector<vector<QColor>> c_grid; // 2D vector holding colours of each square to draw
        bool gameInProg; // Used to determine whether to draw score or not
        int dispScore; // Score to display
};

#endif // BOARDVIEW_HPP
