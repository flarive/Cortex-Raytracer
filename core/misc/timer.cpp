#include "timer.h"


void timer::start()
{
    m_StartTime = std::chrono::system_clock::now();
    m_bRunning = true;
}

void timer::stop()
{
    m_EndTime = std::chrono::system_clock::now();
    m_bRunning = false;
}

double timer::elapsedMilliseconds() const
{
    std::chrono::time_point<std::chrono::system_clock> endTime;

    if (m_bRunning)
    {
        endTime = std::chrono::system_clock::now();
    }
    else
    {
        endTime = m_EndTime;
    }

    return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count());
}

double timer::elapsedSeconds() const
{
    return elapsedMilliseconds() / 1000.0;
}

std::chrono::time_point<std::chrono::system_clock> timer::getStartTime()
{
    return m_StartTime;
}

std::chrono::time_point<std::chrono::system_clock> timer::getEndTime()
{
    return m_EndTime;
}

void timer::reset()
{
    m_StartTime = m_EndTime = {};
}

std::string timer::format_duration(double dms)
{
    std::chrono::duration<double, std::milli> ms{ dms };

    auto secs = duration_cast<std::chrono::seconds>(ms);
    auto mins = duration_cast<std::chrono::minutes>(secs);
    secs -= duration_cast<std::chrono::seconds>(mins);
    auto hour = duration_cast<std::chrono::hours>(mins);
    mins -= duration_cast<std::chrono::minutes>(hour);

    std::stringstream ss;
    ss << hour.count() << "h " << mins.count() << "mn " << secs.count() << "s";
    return ss.str();
}

std::string timer::format_duration_fine(double dms)
{
    std::chrono::duration<double, std::milli> ms{ dms };

    auto secs = duration_cast<std::chrono::seconds>(ms);
    auto mins = duration_cast<std::chrono::minutes>(secs);
    secs -= duration_cast<std::chrono::seconds>(mins);
    auto hour = duration_cast<std::chrono::hours>(mins);
    mins -= duration_cast<std::chrono::minutes>(hour);

    // Calculate remaining milliseconds after removing seconds
    ms -= std::chrono::duration_cast<std::chrono::milliseconds>(secs);


    std::stringstream ss;
    ss << hour.count() << "h " << mins.count() << "mn " << secs.count() << "s " << ms.count() << "ms";
    return ss.str();
}

void timer::displayTime()
{
    std::string www = format_duration_fine(elapsedMilliseconds());
    std::cout << "[INFO] Execution Time: " << www.c_str() << std::endl;
}