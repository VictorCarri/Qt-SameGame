#include "mainview.hpp"

/**
 * @brief MainView::MainView Constructor. Initializes main view widget.
 * @param parent The parent of this widget. In this application, it's the main window.
 */
MainView::MainView(QWidget *parent) : QWidget(parent)
{
    scoreLabel = new QLabel("Score: ", this); // Create the score label
}

/**
 * @brief MainView::~MainView Destructor. Cleans up our resources.
 */
MainView::~MainView()
{
    delete scoreLabel; // Delete the label
}
