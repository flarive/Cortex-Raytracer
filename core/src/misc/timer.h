#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>

class timer
{
public:
    void start();
    void stop();
    double elapsedMilliseconds();
    double elapsedSeconds();
    std::chrono::time_point<std::chrono::system_clock> getStartTime();
    std::chrono::time_point<std::chrono::system_clock> getEndTime();
    void reset();
    std::string format_duration(double dms);
    void displayTime();

private:
    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
    std::chrono::time_point<std::chrono::system_clock> m_EndTime;
    bool m_bRunning = false;
};