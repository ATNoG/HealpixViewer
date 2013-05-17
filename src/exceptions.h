#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <QString>

class HealpixException : public std::exception {
public:
    HealpixException(std::string const& msg)
      : _msg(msg)
    { }

    virtual ~HealpixException() throw()
    { }

    virtual const char* what() const throw()
    {
        return _msg.c_str();
    }

private:
    std::string _msg;
};

class HealpixMapException : public HealpixException {
public:
    HealpixMapException(std::string const& msg)
      : HealpixException(msg)
    { }
};

class FieldMapException : public HealpixException {
public:
    FieldMapException(std::string const& msg)
      : HealpixException(msg)
    { }
};


class ColormapsNotFoundException : public HealpixException {
public:
    ColormapsNotFoundException(std::string const& msg)
      : HealpixException(msg)
    { }
};

#endif // EXCEPTIONS_H
