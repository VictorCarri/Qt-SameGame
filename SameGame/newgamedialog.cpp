#include "newgamedialog.hpp" // Class def
#include "ui_newgamedialog.h" // UI objects def

/* Qt objs */
#include <QMessageBox> // Message box

NewGameDialog::NewGameDialog(QWidget *parent) :
    QDialog(parent), // Parent constructor
    ui(new Ui::NewGameDialog), // UI creation
    wasAccepted(false) // Initialize boolean describing whether or not dialog was cancelled to false
{
    ui->setupUi(this); // Set up  UI
}

NewGameDialog::~NewGameDialog()
{
    delete ui; // Delete UI objs
}

/*** Controller methods ***/

/**
 * @brief NewGameDialog::on_buttonBox_accepted Handles a click on the "OK" button.
 */
void NewGameDialog::on_buttonBox_accepted()
{
    wasAccepted = true; // User chose to start a new game
}

/**
 * @brief NewGameDialog::on_buttonBox_rejected Handles a click on the "Cancel" button.
 */
void NewGameDialog::on_buttonBox_rejected()
{
    wasAccepted = false; // User chose not to start a new game
}

/*** Getters ***/

/**
 * @brief getNumCols Fetches the # of columns chosen by the user.
 * @return The # of columns chosen by the user.
 */
int NewGameDialog::getNumCols()
{
    return ui->colInp->value(); // Return the value of the column input
}

/**
 * @brief NewGameDialog::getNumRows Fetches the # of rows chosen by the user.
 * @return The # of rows chosen by the user.
 */
int NewGameDialog::getNumRows()
{
    return ui->rowInp->value(); // Return the value of the row input
}

/**
 * @brief NewGameDialog::getNumColours Fetches the # of colours chosen by the user.
 * @return The # of colours chosen by the user.
 */
int NewGameDialog::getNumColours()
{
    return ui->colourInp->value(); // Return the value of the number input
}

/**
 * @brief NewGameDialog::getWasAccepted Lets the other controllers know if the user chose to start a new game or not.
 * @return True if user clicked "OK", false if they clicked "Cancel".
 */
bool NewGameDialog::getWasAccepted()
{
    return wasAccepted; // Return the boolean set by the slots
}
