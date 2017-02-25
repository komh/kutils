/*
 * Numerical expression calculator
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

/** \file numcalc.cpp */

#include "numcalc.h"

#include <cassert>

#include <string>
#include <functional>

/**
 * Constructor
 *
 * \param[in] expr Expression to calculate
 */
NumCalc::NumCalc( const std::string& expr )
    : _parser( _funcs, _consts ), _result( 0 )
{
    if( _parser.parse( expr ))
        calc();
}

/**
 * Calculate with tokens
 */
void NumCalc::calc()
{
    _result = level1();
}

/**
 * Common function for binary operators
 *
 * \param[in] opLevel Level of operators
 * \param[in] upLevel Function for higher level operator
 * \return Result of operations
 */
float NumCalc::biop( int opLevel,
                     const std::function< float()>& upLevel )
{
    float res;

    while( getOpLevel( _parser.peekType()) >= opLevel )
    {
        auto type = _parser.peekType();

        if( getOpLevel( type ) == opLevel )  // same level op ?
        {
            _parser.next();

            // calculate
            res = _biOpFuncs[ type ]( res, upLevel());
        }
        else    // higher-level op
            res = upLevel();
    }

    return res;
}

/**
 * Get a level of operators
 *
 * \param[in] type Token type
 * \return Level of given operator
 */
int NumCalc::getOpLevel( Parser::TokenType type )
{
    switch( type )
    {
        case Parser::TokenType::ParenClose:
            return 0;

        case Parser::TokenType::Add:
        case Parser::TokenType::Sub:
            return 1;

        case Parser::TokenType::Mul:
        case Parser::TokenType::Div:
            return 2;

        case Parser::TokenType::Pow:
            return 3;

        case Parser::TokenType::Func:
            return 4;

        case Parser::TokenType::ParenOpen:
            return 5;

        case Parser::TokenType::Sign:
        case Parser::TokenType::Number:
        case Parser::TokenType::Const:
            return 6;

        case Parser::TokenType::None:
            break;
    }

    return -1;
}

/**
 * Level 1 operators: Addition(+), Subtraction(-)
 *
 * \return Result of operations
 */
float NumCalc::level1()
{
    return biop( getOpLevel( Parser::TokenType::Add ),
                 [ this ](){ return this->level2(); } );
}

/**
 * Level 2 operators: Multiplication(*), Division(/)
 *
 * \return Result of operations
 */
float NumCalc::level2()
{
    return biop( getOpLevel( Parser::TokenType::Mul ),
                 [ this ](){ return this->level3(); } );
}

/**
 * Level 3 operators: Power(^)
 *
 * \return Result of operations
 */
float NumCalc::level3()
{
    return biop( getOpLevel( Parser::TokenType::Pow),
                 [ this ](){ return this->level4(); } );
}

/**
 * Level 4 operators: Functions
 *
 * \return Result of operations
 */
float NumCalc::level4()
{
    float res;

    if( _parser.peekType() == Parser::TokenType::Func )
    {
        std::string f = _parser.getToken();

        res = _funcs[ f ]( level4());
    }
    else
        res = level5();

    return res;
}

/**
 * Level 5 operators: Parentheses (, )
 *
 * \return Result of operations
 */
float NumCalc::level5()
{
    float res;

    switch( _parser.peekType())
    {
        case Parser::TokenType::ParenOpen:
            _parser.next();

            res = level1();

            assert(_parser.peekToken().compare(")") == 0 );

            // skip ')'
            _parser.next();

            break;

        case Parser::TokenType::ParenClose:
            assert(!"')' has been encountered at level5()");
            break;

        default:
            res = level6();
            break;
    }

    return res;
}

/**
 * Level 6 operators: Number and constants
 *
 * \return Number of a token
 */
float NumCalc::level6()
{
    float res;

    switch( _parser.peekType())
    {
        case Parser::TokenType::Sign:
            res = _parser.getSign() * level2();
            break;

        case Parser::TokenType::Number:
            res = _parser.getNumber();
            break;

        case Parser::TokenType::Const:
            res =  _consts[ _parser.getToken()];
            break;

        default:
            assert(!"Unsupported token type has been encountered at level6()");
            break;
    }

    return res;
}
