/////////////////////////////////////////////////////////////////
//
//          Copyright Vadim Stadnik 2011-2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
/////////////////////////////////////////////////////////////////
//
//  See folder "../doc" for documentation
//
/////////////////////////////////////////////////////////////////

#ifndef _BPT_HELPERS_HPP
#define _BPT_HELPERS_HPP


#define _BPT_OPEN   namespace container_bptree {
#define _BPT_CLOSE  }


_BPT_OPEN

template < class _Ty >
struct GetSelf
{
    const _Ty &  operator() ( const _Ty &  ty_x ) const
    {
        return ty_x ;
    }
} ;

template < class _Ty_Pair , class _Ty_1st >
struct Get1st
{
    const _Ty_1st &  operator ( ) ( const _Ty_Pair &  ty_x ) const
    {
        return ty_x . first ;
    }
} ;

template < class _Ty_Pair , class _Ty_2nd >
struct Get2nd
{
    const _Ty_2nd &  operator ( ) ( const _Ty_Pair &  ty_x ) const
    {
        return ty_x . second ;
    }
} ;

_BPT_CLOSE

#endif  //  _BPT_HELPERS_HPP

