#ifndef SAVEFILEFORMATEXCEPTION_H
#define SAVEFILEFORMATEXCEPTION_H

/* STL includes */
#include <stdexcept> // runtime_error

/* Qt includes */
#include <QString> // Qt strings

/**
 * @brief The SaveFileFormatException class implements an exception which is thrown when the Game constructor detects
 *          invalid formatting in the save file. The Game class will pass a string describing the error to the exception
 *          constructor. The user can access this string using the exception's what() method.
 */
class SaveFileException : public std::runtime_error
{
    public:
        SaveFileException(const char* msg); // Constructs the exception and stores the given message
};

#endif // SAVEFILEFORMATEXCEPTION_H
