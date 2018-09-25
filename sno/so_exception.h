#ifndef SO_EXCEPTION_H
#define SO_EXCEPTION_H

#include <exception>
#include <stdexcept>

namespace so
{
/**
 * @brief The runtime_error class represents errors that are not detectable
 * until runtime.
 */
class runtime_error : public std::runtime_error
{
public:
  template<class T>
  runtime_error(const T what)
    : std::runtime_error(what)
  {

  }

};

/**
 * @brief The logic_error class represents errors that are fundamental flaws
 * in the application's logic, errors that could be detected at compile time
 * or through static analysis
 */
class logic_error : public std::logic_error
{
public:
  template<class T>
  logic_error(const T what)
    :std::logic_error(what)
  {

  }
};

class Invalid_argument : public so::runtime_error
{
public:
  template<class T>
  Invalid_argument(const T what)
    : so::runtime_error(what)
  {

  }
};

class Read_error : public so::runtime_error
{
public:
  template<class T>
  Read_error(const T what)
    : so::runtime_error(what)
  {

  }
};

class Write_error : public so::runtime_error
{
public:
  template<class T>
  Write_error(const T what)
    : so::runtime_error(what)
  {

  }
};

} // namespace so

#endif
