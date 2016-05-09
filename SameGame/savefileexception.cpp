#include "savefileexception.hpp"

/**
 * @brief SaveFileException::SaveFileException Constructor. Stores the given string to be returned by what().
 * @param msg The message to return in what()
 */
SaveFileException::SaveFileException(const char* msg) : std::runtime_error(msg) // Call parent constructor with given error message to store it
{
}
