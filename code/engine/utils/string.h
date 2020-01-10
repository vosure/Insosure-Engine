#pragma once

struct string
{
    char *Text;
    unsigned int Length;

    string operator= (char *String)
    {
        Text = String;
        Length = ArrayCount(String);

        return *this;
    }

    string operator= (const char *String)
    {
        Text = (char*)String;
        Length = ArrayCount(String);

        return *this;
    }

    bool operator== (string String)
    {

    }

    string operator= (string String)
    {

    }

    string operator+ (string String)
    {

    }

    string operator+ (char *String)
    {

    }

    void operator+= (char *String)
    {

    }

    void operator+= (string String)
    {
        
    }
};