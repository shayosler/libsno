/**
 * \class Err_log Error logging class
 * Err_log provides a basic error logging class. The class provides error
 * logging through the stream insertion operator (<<).
 *
 */
//todo: documentations

#ifndef ERRLOG_H
#define ERRLOG_H

#include <string>
#include <stdio.h>
#include <fstream>

#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/current_function.hpp>

//////////////////////////////////////////////////////////////////////////////

/**
 * @brief The null_deleter struct is used as a no-op deleter for
 * boost::shared_ptrs to statically allocated objects
 */
struct null_deleter
{
  void operator()(void const *) const
  {
  }
};

//////////////////////////////////////////////////////////////////////////////

/**
 * Struct is used to define the default stream type and the functions used for
 * functors that manipulate streams *
 */
template<typename C,typename T = std::char_traits<C> >
struct stream_info
{
  typedef std::basic_ostream<C,T>& (*StrFunc)(std::basic_ostream<C,T>&);
  static std::basic_ostream<C,T>& Get_default();
};

//////////////////////////////////////////////////////////////////////////////

template<typename C,typename T = std::char_traits<C> >
class Err_log
{
public:
  //Variables
  /**
   * @brief Enumerated error magnitudes for error messages
   */
  enum EMag
  {
    EMAG_Unknown  = -1,
    EMAG_Debug    = 10,
    EMAG_Info     = 20,
    EMAG_Warning  = 30,
    EMAG_Severe   = 40,
    EMAG_Fatal    = 50,
  };

  //Functions
  /**
   * @brief Errlog Log errors
   * @param scope Scope of the current error log message
   * @param mag Enumerated magnitude of the message
   */
  Err_log(std::string scope, EMag mag)
    :
      m_alt_stream()
    ,m_curr_mag(mag)
    ,m_lock(m_mutex)
  {
    *m_out_stream << get_msg_prefix(scope);
  }

  //////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Errlog Log errors to a particular file, not to the "normal" log
   * location written to by m_out_stream
   * @param scope Scope of the current error log message
   * @param mag Enumerated logging magnitude
   * @param file Filename to write to
   */
  Err_log(std::string scope, EMag mag, std::string file)
    :
      m_alt_stream(new std::ofstream(file.c_str(), std::ios_base::app))
    ,m_curr_mag(mag)
    ,m_lock(m_mutex)
  {
    *m_alt_stream << get_msg_prefix(scope);
  }

  //////////////////////////////////////////////////////////////////////////////

  /**
   * Destructor
   */
  ~Err_log()
  {
    //todo: write std::endl to streams??
  }

  //////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Set the logging level to a particular magnitude
   * @param mag The enumerated magnitude to set the logging level to
   */
  static void Set_logging_level(const EMag mag)
  {
    boost::mutex::scoped_lock lock(m_mutex);
    m_mag = mag;
  }

  //////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Set_log_file Set the log file to write to
   * @param filename Filename to write to
   */
  static void Set_log_file(std::string filename)
  {
    boost::mutex::scoped_lock(m_mutex);
    m_out_stream.reset(new std::ofstream(filename.c_str(),std::ios_base::app));
  }

  //////////////////////////////////////////////////////////////////////////////

  /**
   * Stream insertion operator.
   */
  template<class R>
  Err_log& operator<<(R obj)
  {
    if(m_curr_mag >= m_mag)
    {
      if(m_alt_stream)
      {
        *m_alt_stream << obj;
      }else if(m_out_stream)
      {
        *m_out_stream << obj;
      }
    }
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////////

  /**
   * Stream insertion operator.
   */
  Err_log& operator<<(typename stream_info<C,T>::StrFunc func)
  {
    if(m_curr_mag >= m_mag)
    {
      if(m_alt_stream)
      {
        func(*m_alt_stream);
      }else if(m_out_stream)
      {
        func(*m_out_stream);
      }
    }
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////////

protected:
  //Variables
  /**
   * @brief m_mutex
   */
  static boost::mutex m_mutex;

  /**
   * Error logging level
   */
  static EMag m_mag;

  /**
   * @brief m_out_stream Main stream for error logging. Defaults to std::cout
   * or std::wcout, but can be set to a file
   */
  static boost::shared_ptr<std::basic_ostream<C,T> > m_out_stream;

  /**
   * @brief m_local_stream Alternate stream for writing to a particular file
   */
  boost::shared_ptr<std::basic_ostream<C,T> > m_alt_stream;

  /**
   * @brief m_curr_mag Logging level of the current message
   */
  EMag m_curr_mag;

  /**
   * @brief m_lock Scoped lock, locked when Errlog is instantiated
   */
  boost::unique_lock<boost::mutex> m_lock;

  //Functions
  /**
   * @brief set_ostream
   * @param stream
   */
  static void set_ostream(boost::shared_ptr<std::basic_ostream<C,T> > stream)
  {
    m_out_stream = stream;
  }

  //////////////////////////////////////////////////////////////////////////////

  /**
   * @brief get_msg_prefix Get the start of the log message. The message prefix
   *  takes the form 'LEVEL--[class::function] '
   * @param curr_func Current function string
   * @return String representing the prefix for this log message
   */
  std::string get_msg_prefix(std::string curr_func)
  {
    std::string err_prefix;
    //Write the logging level
    switch(m_curr_mag)
    {
    case(Err_log::EMAG_Debug): err_prefix = "DEBUG--";break;
    case(Err_log::EMAG_Info): err_prefix = "INFO--";break;
    case(Err_log::EMAG_Warning): err_prefix = "WARNING--";break;
    case(Err_log::EMAG_Severe): err_prefix = "SEVERE--";break;
    case(Err_log::EMAG_Fatal): err_prefix = "FATAL--";break;
    default: err_prefix = " --";
    }

    //Get the scope
    size_t args_start = curr_func.find_last_of('(');
    std::string func_no_args = curr_func.substr(0,args_start);
    size_t scope_start = func_no_args.find_last_of(' ') + 1;
    std::string func = func_no_args.substr(scope_start);
    err_prefix = err_prefix + "[" + func + "] ";

    return err_prefix;
  }

private:
};

//////////////////////////////////////////////////////////////////////////////
// Initialize static members
/**
 * @brief Mutex
 */
template<typename C, typename T>
boost::mutex Err_log<C, T>::m_mutex;

/**
 * @brief The current error logging magnitude
 */
template<typename C, typename T>
typename Err_log<C, T>::EMag Err_log<C ,T>::m_mag(Err_log<C,T>::EMAG_Debug);
//boost::atomic<Errlog<C,T>::EMag> Errlog<C,T>::m_mag(Errlog::EMAG_Debug);

/**
 * @brief m_out_stream The stream for writing errors to
 */
//TODO: why is this a shared pointer?
template<typename C,typename T>
boost::shared_ptr<std::basic_ostream<C,T> > Err_log<C,T>::m_out_stream(&stream_info<C,T>::Get_default(), null_deleter());

//////////////////////////////////////////////////////////////////////////////
// Typedefs for common types of error loggers
typedef Err_log<char> Errlog;
typedef Err_log<wchar_t> WErrlog;

//Macros to automatically insert scope
#define Log_error(...) Errlog(BOOST_CURRENT_FUNCTION,__VA_ARGS__)
#define Log_werror(...) WErrlog(BOOST_CURRENT_FUNCTION,__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////////


#endif
