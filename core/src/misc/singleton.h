#pragma once

#include <string>

#include "renderParameters.h"
//#include "../randomizers/randomizer.h"

/**
 * The Singleton class defines the `GetInstance` method that serves as an
 * alternative to constructor and lets clients access the same instance of this
 * class over and over.
 */ 
class Singleton
{

    /**
     * The Singleton's constructor should always be private to prevent direct
     * construction calls with the `new` operator.
     */

public:
    Singleton(const renderParameters value) : value_(value)
    {
        
    }

    static Singleton* singleton_;

    //std::string value_;

    renderParameters value_;
    //randomizer rnd_;

public:

    /**
     * Singletons should not be clonable.
     */
    Singleton(Singleton& other) = delete;
    /**
     * Singletons should not be assignable.
     */
    void operator=(const Singleton&) = delete;
    /**
     * This is the static method that controls the access to the singleton
     * instance. On the first run, it creates a singleton object and places it
     * into the static field. On subsequent runs, it returns the client existing
     * object stored in the static field.
     */

    //static Singleton* GetInstance(const std::string& value);
    static Singleton* getInstance();
    

    renderParameters value() const {
        return value_;
    }

    //randomizer& rnd() {
    //    return rnd_;
    //}
};