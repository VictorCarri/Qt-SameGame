#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>

namespace Ui {
class NewGameDialog;
}

class NewGameDialog : public QDialog
{
    Q_OBJECT

public:
    /* Constructor/destructor */
    explicit NewGameDialog(QWidget *parent = 0);
    ~NewGameDialog();

    /* Getters */
    int getNumCols(); // Fetches the # of columns chosen by the user
    int getNumRows(); // Fetches the # of rows chosen by the user
    int getNumColours(); // Fetches the # of colours chosen by the user
    bool getWasAccepted(); // Lets the other controllers know if the user pressed "OK" or "Cancel"

private slots:
    /* Controller */
    void on_buttonBox_accepted(); // "OK" clicked
    void on_buttonBox_rejected(); // "Cancel" clicked

private:
    /* View */
    Ui::NewGameDialog *ui; // UI object

    /* Model */
    bool wasAccepted; // True if user pressed "OK", false if they cancelled the operation.
    QEvent* event;
};

#endif // NEWGAMEDIALOG_H
