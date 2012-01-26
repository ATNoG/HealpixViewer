#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <QString>

class HealpixMapException: public exception
{
  public:
    HealpixMapException(const std::string msg) : _msg(msg) {}

    ~HealpixMapException() throw() {}

    virtual const char* what() const throw()
    {
        return _msg.c_str();
    }

  private:
    std::string _msg;
};


class FieldMapException: public exception
{
  public:
    FieldMapException(const std::string msg) : _msg(msg) {}

    ~FieldMapException() throw() {}

    virtual const char* what() const throw()
    {
        return _msg.c_str();
    }

  private:
    std::string _msg;
};

#endif // EXCEPTIONS_H
