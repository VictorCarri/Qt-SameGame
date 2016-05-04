#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget> // Widget stuff
#include <QLabel> // Labels

class MainView : public QWidget
{
    Q_OBJECT

    public:
        explicit MainView(QWidget *parent = 0); // Constructor
        ~MainView(); // Destructor

    signals:

    public slots:

    private: // Private vars
        QLabel* scoreLabel; // Displays the score
};

#endif // MAINVIEW_H
