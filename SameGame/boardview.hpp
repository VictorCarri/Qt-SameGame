#ifndef BOARDVIEW_H
#define BOARDVIEW_H

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

    protected:
        /* Event handlers */
        void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE; // Handles painting by redrawing the board

    signals:

    public slots:

    private: // Private stuff
        bool canDraw; // Used to tell paintEvent whether or not it is allowed to draw anything but black
        vector<vector<QColor>> c_grid; // 2D vector holding colours of each square to draw
};

#endif // BOARDVIEW_H
