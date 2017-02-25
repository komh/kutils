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

/** \file numcalc.h */

#ifndef KLINECALC_NUMCALC_H
#define KLINECALC_NUMCALC_H

#include "parser.h"

#include <cmath>

#include <functional>
#include <map>
#include <string>

/**
 * Numerical Expression Calculator
 */
class NumCalc
{
public:
    NumCalc( const std::string& expr = std::string());

    /**
     * Get the result of calculation
     *
     * \return Result of calculation
     */
    float result() const { return _result; }

    /**
     * Get error status
     *
     * \return true on error, otherwise false
     */
    bool error() const { return _parser.error(); }

    /**
     * Get error string
     *
     * \return Error string
     */
    std::string errorStr() const { return _parser.errorStr(); }

private:
    /// Function list
    std::map< std::string, std::function< float( float )> > _funcs {
        {"sin", []( float x ){ return sin( x ); }},
        {"cos", []( float x ){ return cos( x ); }},
        {"tan", []( float x ){ return tan( x ); }},
        {"ln",  []( float x ){ return log( x ); }},
        {"log", []( float x ){ return log10( x );}},
    };

    /// Constant list
    std::map< std::string, float > _consts {
        { "e", M_E },
        { "pi", M_PI },
    };

    /// Binary operation list
    std::map< Parser::TokenType,
              std::function< float( float, float )> > _biOpFuncs {
        { Parser::TokenType::Add,
          []( float lv, float rv ) { return lv + rv; }},
        { Parser::TokenType::Sub,
          []( float lv, float rv ) { return lv - rv; }},
        { Parser::TokenType::Mul,
          []( float lv, float rv ) { return lv * rv; }},
        { Parser::TokenType::Div,
          []( float lv, float rv ) { return lv / rv; }},
        { Parser::TokenType::Pow,
          []( float lv, float rv ) { return pow( lv, rv ); }},
    };

    /// Numerical expression parser
    Parser _parser;
    /// Result of calculation
    float _result;

    void calc();
    float biop( int opLevel, const std::function< float()>& upLevel );
    int getOpLevel( Parser::TokenType type );
    float level1();
    float level2();
    float level3();
    float level4();
    float level5();
    float level6();
};

#endif
