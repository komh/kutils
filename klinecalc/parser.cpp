/*
 * Numerical expression parser
 *
 * Copyright (C) 2017 KO Myung-Hun <komh@chollian.net>
 *
 * This file is part of K Line Calculator.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */

/** \file parser.cpp */

#include "parser.h"

#include <cctype>

#include <string>

/**
 * Convert string to lower case
 *
 * \param[in] str String to convert
 * \return String converted to lower case
 */
static inline std::string strlwr( const std::string& str )
{
    std::string s( str );

    for( auto& c: s )
        c = std::tolower( c );

    return s;
}

/**
 * Parse the given numerical expression
 *
 * \param[in] expr numerical expression
 * \return true on success, false on error with setting _error to true and
 *         storing error messages to _errorStream
 */
bool Parser::parse( const std::string& expr )
{
    bool numExpected = true;
    std::size_t pos = 0;
    int parentheses = 0;

    while( pos < expr.length())
    {
        // skip spaces
        while( pos < expr.length() && isspace( expr[ pos ]))
            ++pos;

        if( pos == expr.length())
            break;

        if( numExpected )
        {
            // opening parenthesis may come in number place
            std::string paren(expr.substr( pos, 1 ));
            if( paren[ 0 ] == '(')
            {
                ++parentheses;

                push_back( TokenType::ParenOpen, paren );

                ++pos;

                continue;
            }

            // function or constant may come in number place
            std::string name;
            while( isalpha( expr[ pos ] ))
                name += expr[ pos++ ];
            if( !name.empty())
            {
                name = strlwr( name );
                if( _funcs.find( name ) != _funcs.end())
                    push_back( TokenType::Func, name );
                else
                {
                    if( _consts.find( name ) != _consts.end())
                    {
                        push_back( TokenType::Const, name );

                        numExpected = false;
                    }
                    else
                    {
                        _errorStream << "Unexpected name at " << pos
                                     << " (" << name << ")";
                        _error = true;

                        return false;
                    }
                }

                continue;
            }

            // check sign part
            std::string sign;
            while( pos < expr.length()
                   && ( expr[ pos ] == '+' || expr[ pos ] == '-'
                        || isspace( expr[ pos ])))
            {
                if( isspace( expr[ pos ]))
                    ++pos;
                else if( sign.empty())
                    sign = expr[ pos++ ];
                else if( sign[ 0 ] == expr[ pos++ ])
                    sign = "+";
                else
                    sign = "-";
            }

            if( !sign.empty())
            {
                push_back( TokenType::Sign, sign );

                continue;
            }

            // number format: I[.D[e+-E]]

            // check integral part
            std::string integral;
            while( pos < expr.length() && isdigit( expr[ pos ]))
                integral += expr[ pos++ ];

            // check decimal part
            std::string decimal;
            if( pos < expr.length() && expr[ pos ] == '.')
            {
                ++pos;
                decimal += ".";
                while( pos < expr.length() && isdigit( expr[ pos ]))
                    decimal += expr[ pos++ ];
            }

            // check if number missed
            if( integral.empty() && decimal.length() < 2 )
            {
                _errorStream << "Number missed at " << pos;
                _error = true;

                return false;
            }

            // check exponential part
            std::string exp;
            if( pos < expr.length()
                && std::tolower( expr[ pos ]) == 'e')
            {
                ++pos;
                exp += "e";

                // check sign part
                std::string expSign;
                while( pos < expr.length()
                       && ( expr[ pos ] == '+' || expr[ pos ] == '-'))
                {
                    if( expSign.empty() || expSign[ 0 ] == expr[ pos++ ])
                        expSign = "+";
                    else
                        expSign = "-";
                }

                // check integral part
                std::string expInt;
                while( pos < expr.length() && isdigit( expr[ pos ]))
                    expInt += expr[ pos++ ];

                exp += expSign + expInt;
            }

            // check if exponential number missed
            if( exp.length() == 1 )
            {
                _errorStream << "Exponential number missed at " << pos;
                _error = true;

                return false;
            }

            // check if redundant . given
            if( pos < expr.length() && expr[ pos ] == '.')
            {
                _errorStream << "Redundant '.' at " << pos;
                _error = true;

                return false;
            }

            std::string number;

            if( !integral.empty())
                number += integral;
            else
                number += "0";  // .x to 0.x

            if( decimal.length() > 1 )
                number += decimal;

            if( exp.length() > 1 )
                number += exp;

            push_back( TokenType::Number, number );

            numExpected = false;
        }
        else // if( !numExpected )
        {
            // Tokenize operators
            if( pos < expr.length())
            {
                switch( expr[ pos ])
                {
                    case '+':
                        push_back( TokenType::Add, "+");
                        break;

                    case '-':
                        push_back( TokenType::Sub, "-");
                        break;

                    case '*':
                        push_back( TokenType::Mul, "*");
                        break;

                    case '/':
                        push_back( TokenType::Div, "/");
                        break;

                    case '^':
                        push_back( TokenType::Pow, "^");
                        break;

                    case ')':
                        push_back( TokenType::ParenClose, ")");

                        if( --parentheses < 0 )
                        {
                            _errorStream << "Too many ')' at " << pos;
                            _error = true;

                            return false;
                        }

                        ++pos;

                        numExpected = false;

                        continue;

                    default:
                        _errorStream << "Operator expected at " << pos
                                     << " (" << expr[ pos ] << ")";
                        _error = true;

                        return false;
                }
            }

            ++pos;

            numExpected = true;
        }
    }

    // check if number missed
    if( numExpected )
    {
        _errorStream << "Number expected at " << pos;
        _error = true;

        return false;
    }

    // check if ')' missed
    if( parentheses > 0 )
    {
        _errorStream << "')' expected at " << pos;
        _error = true;

        return false;
    }

    return true;
}
