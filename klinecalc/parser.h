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

/** \file parser.h */

#ifndef KLINECALC_PARSER_H
#define KLINECALC_PARSER_H

#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <vector>

/**
 * Numerical expression parser
 */
class Parser
{
public:
    /**
     * Token types
     */
    enum class TokenType {
        None,       ///< None
        Sign,       ///< Sign
        Number,     ///< Number
        Add,        ///< Addition
        Sub,        ///< Subtraction
        Mul,        ///< Multiplication
        Div,        ///< Division
        Pow,        ///< Power
        Func,       ///< Function
        ParenOpen,  ///< Opening parenthesis
        ParenClose, ///< Closing parenthesis
        Const       ///< Constant
    };

    /**
     * Consructor
     *
     * \param[in] funcs Function to support
     * \param[in] consts Constants to support
     */
    Parser( const std::map< std::string,
                            std::function< float( float )> >& funcs,
            const std::map< std::string, float >& consts )
        : _current( 0 )
        , _error( false )
        , _funcs( funcs )
        , _consts( consts ) {}

    /**
     * Parse numerical expression
     *
     * \param[in] expr Numerical expression
     * \return true on success, false on error
     */
    bool parse( const std::string& expr );

    /**
     * Peek a token type at current index
     *
     * \return Token type at current index
     * \return TokenType::None if no available tokens
     */
    inline TokenType peekType() const
    {
        return _current == _tokens.size() ?
                           TokenType::None : _tokens[ _current ].first;
    }

    /**
     * Peek a token string at current index
     *
     * \return Token string at current index
     * \return Empty string if no available tokens
     */
    inline std::string peekToken() const
    {
        return _current == _tokens.size() ?
                           std::string() : _tokens[ _current ].second;
    }

    /**
     * Get a token type at current index and move to a next token
     *
     * \return Token type at current index
     * \return TokenType::None if no available tokens
     * \remark Move to a next token if tokens are available
     */
    inline TokenType getType()
    {
        return _current == _tokens.size() ?
                           TokenType::None : _tokens[ _current++ ].first;
    }

    /**
     * Get a token string at current index and move to a next token
     *
     * \return Token string at current index
     * \return Empty string if no available tokens
     */
    inline std::string getToken()
    {
        return _current == _tokens.size() ?
                           std::string() : _tokens[ _current++ ].second;
    }

    /**
     * Get a sign at current index and move to a next token if a sign
     *
     * \return -1 if negative(-) sign is at current index, otherwise 1
     */
    inline float getSign()
    {
        return ( peekType() == TokenType::Sign
                 && getToken().compare("-") == 0 ) ? -1 : 1;
    }

    /**
     * Get a number at current index and move to a next token
     *
     * \return Number if number is at current index, otherwise 0
     */
    inline float getNumber()
    {
        return peekType() == TokenType::Number ? std::stof( getToken()) : 0;
    }

    /**
     * Move to a next token
     */
    inline void next()
    {
        if( _current < _tokens.size())
            ++_current;
    }

    /**
     * Get error status
     *
     * \return true on error, otherwise false
     */
    bool error() const { return _error; }

    /**
     * Get error string
     *
     * \return Error string
    */
    std::string errorStr() const { return _errorStream.str(); }

private:
    /// Current index of tokens
    std::size_t _current;
    /// Tokens
    std::vector< std::pair< TokenType, std::string > > _tokens;

    /// Error indicator
    bool _error;
    /// Error message
    std::stringstream _errorStream;

    /// Function list
    std::map< std::string, std::function< float( float )> > _funcs;
    /// Constant list
    std::map< std::string, float > _consts;

    /**
     * Append a token
     *
     * \param[in] type Token type
     * \param[in] token Token string
     */
    inline void push_back( TokenType type, const std::string& token )
    {
        _tokens.push_back( std::make_pair( type, token ));
    }
};

#endif
