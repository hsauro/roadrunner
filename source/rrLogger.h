#ifndef rrLoggerH
#define rrLoggerH
#include <sstream>
#include "rrExporter.h"


namespace Poco {
class Logger;
}

namespace rr
{

/**
 * Poco LogStream dumps to the log when a newline i.e. std::endl is encountered,
 * howeve the old proprietary logging system dumps basically when the stream object
 * goes out of scope.
 *
 * This object allows us to to use the new Poco logging system and maintain
 * compatability with all existing code.
 *
 * This object is returne from the rr::Logger, exposes a ostream interface, and
 * and dumps to the log when it goes out of scope.
 */
class RR_DECLSPEC LoggingBuffer
{
public:
    LoggingBuffer(int level, const char* file, int line);

    /**
     * dump the contents of the stringstream to the log.
     */
    ~LoggingBuffer();

    /**
     * get the stream this buffer holds.
     */
    std::ostream &stream();

private:
    std::stringstream buffer;
    int level;
    const char* file;
    int line;
};

/**
 * The roadrunner logger.
 *
 * A set of static method for setting the logging level.
 */
class RR_DECLSPEC Logger
{
public:
    /**
     * same as Poco level, repeat here to avoid including any Poco files
     * as Poco is usually linked statically so third parties would not need
     * to have Poco installed.
     *
     * TODO: is this reasoning correct???
     */
    enum Level
    {
        PRIO_FATAL = 1,   /// A fatal error. The application will most likely terminate. This is the highest priority.
        PRIO_CRITICAL,    /// A critical error. The application might not be able to continue running successfully.
        PRIO_ERROR,       /// An error. An operation did not complete successfully, but the application as a whole is not affected.
        PRIO_WARNING,     /// A warning. An operation completed with an unexpected result.
        PRIO_NOTICE,      /// A notice, which is an information with just a higher priority.
        PRIO_INFORMATION, /// An informational message, usually denoting the successful completion of an operation.
        PRIO_DEBUG,       /// A debugging message.
        PRIO_TRACE        /// A tracing message. This is the lowest priority.
    };


    Logger();
    ~Logger();

    /**
     * old home grown logging functions, here for compatibility
     */

    static void setLevel(int level);

    static int getLevel();

    static void stopLogging();

    static void enableLoggingToConsole();

    static void disableLoggingToConsole();

    static void init(const std::string&, int level);

    static void init(const std::string&, int level, const std::string& fileName);

    static std::string getLevelAsString();

    static std::string getFileName();
};

/**
 * old logging levels, here for compatibility
 */
enum LogLevel
{
    lShowAlways = Logger::PRIO_FATAL,
    lError      = Logger::PRIO_ERROR,
    lWarning    = Logger::PRIO_WARNING,
    lInfo       = Logger::PRIO_INFORMATION,
    lDebug      = Logger::PRIO_DEBUG,
    lDebug1     = Logger::PRIO_TRACE,
    lDebug2     = Logger::PRIO_TRACE,
    lDebug3     = Logger::PRIO_TRACE,
    lDebug4     = Logger::PRIO_TRACE,
    lDebug5     = Logger::PRIO_TRACE,
    lAny        = Logger::PRIO_TRACE,
    lUser       = Logger::PRIO_TRACE
};

/**
 * the real logger is actually a Poco::Logger named "RoadRunner", i.e.
 * Poco::Logger::get("RoadRunner").
 *
 * This returns that logger.
 */
RR_DECLSPEC Poco::Logger &getLogger();

/**
 * legacy macro to get the old proprietary logger interface.
 */
#define gLog Logger()

#ifndef NO_LOGGER
#define Log(level) \
    if (level > rr::Logger::getLevel()) { ; } \
    else rr::LoggingBuffer(level, __FILE__, __LINE__).stream()
#else
#define Log(level) \
    if (true) {  }\
    else \
    LoggingBuffer(level, __FILE__, __LINE__)
#endif


} /* namespace rr */

#include "rrLogLevel.h"

#endif /* rrLoggerH */
