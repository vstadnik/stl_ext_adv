/////////////////////////////////////////////////////////////////
//
//          Copyright Vadim Stadnik 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
/////////////////////////////////////////////////////////////////
//
//  See folder "../doc" for documentation
//
/////////////////////////////////////////////////////////////////

#ifndef _TEST_CONTAINERS_HPP
#define _TEST_CONTAINERS_HPP

#include "test_sequence.hpp"
#include "test_associative.hpp"

#include "bp_tree.hpp"
#include "bp_tree_idx.hpp"
#include "bp_tree_idx_acc.hpp"
#include "bpt_map.hpp"
#include "bpt_sequence.hpp"
#include "bpt_set.hpp"


namespace test_bpt
{
    //  abbreviate names of types to simplify support
    //  for 80 characters requirement
    typedef size_t                                  _T   ;
    typedef std::less<_T>                           _Ls  ;
    typedef std::allocator<size_t>                  _AT  ;
    typedef std::pair <const _T, _T>                _T2  ;
    typedef std::allocator<_T2>                     _AT2 ;

    #define _CBT container_bptree


    //  test STL variants of containers using class bp_tree ,
    //  this class does NOT support <container_bptree::sequence> ;
    void adapters ( const size_t  sz_test , const size_t n_dupl )
    {
        _CBT::set     <_T, _Ls, _AT, _CBT::bp_tree>         t_set   ;
        _CBT::multiset<_T, _Ls, _AT, _CBT::bp_tree>         t_mset  ;
        _CBT::map     <_T, _T , _Ls, _AT2, _CBT::bp_tree>   t_map   ;
        _CBT::multimap<_T, _T , _Ls, _AT2, _CBT::bp_tree>   t_mmap  ;

        test_set      ( t_set  , sz_test , n_dupl ) ;
        test_multi_set( t_mset , sz_test , n_dupl ) ;
        test_map      ( t_map  , sz_test , n_dupl ) ;
        test_multi_map( t_mmap , sz_test , n_dupl ) ;
    }


    //  test STL variants of containers using class bp_tree_idx ;
    void adapters_idx ( const size_t  sz_test , const size_t n_dupl )
    {
        //  generate classes of STL containers using 
        //  explicit specification of template arguments 
    //  _CBT::sequence<_T, _AT,_CBT::bp_tree_idx>               t_seqce ;
    //  _CBT::set     <_T, _Ls, _AT, _CBT::bp_tree_idx>         t_set   ;
    //  _CBT::multiset<_T, _Ls, _AT, _CBT::bp_tree_idx>         t_mset  ;
    //  _CBT::map     <_T, _T , _Ls, _AT2, _CBT::bp_tree_idx>   t_map   ;
    //  _CBT::multimap<_T, _T , _Ls, _AT2, _CBT::bp_tree_idx>   t_mmap  ;

        //  generate classes of STL containers using 
        //  default arguments for template parameters 
        _CBT::sequence<_T>      t_seqce ;
        _CBT::set     <_T>      t_set   ;
        _CBT::multiset<_T>      t_mset  ;
        _CBT::map     <_T, _T>  t_map   ;
        _CBT::multimap<_T, _T>  t_mmap  ;

        test_sequence ( t_seqce, sz_test , n_dupl ) ;
        test_set      ( t_set  , sz_test , n_dupl ) ;
        test_multi_set( t_mset , sz_test , n_dupl ) ;
        test_map      ( t_map  , sz_test , n_dupl ) ;
        test_multi_map( t_mmap , sz_test , n_dupl ) ;
    }


    //  test STL variants of containers using class bp_tree_idx_acc ;
    void adapters_idx_acc ( const size_t  sz_test , const size_t n_dupl )
    {
        _CBT::sequence<_T, _AT,_CBT::bp_tree_idx_acc>               t_seqce ;
        _CBT::set     <_T, _Ls, _AT, _CBT::bp_tree_idx_acc>         t_set   ;
        _CBT::multiset<_T, _Ls, _AT, _CBT::bp_tree_idx_acc>         t_mset  ;
        _CBT::map     <_T, _T , _Ls, _AT2, _CBT::bp_tree_idx_acc>   t_map   ;
        _CBT::multimap<_T, _T , _Ls, _AT2, _CBT::bp_tree_idx_acc>   t_mmap  ;

        test_sequence ( t_seqce, sz_test , n_dupl ) ;
        test_set      ( t_set  , sz_test , n_dupl ) ;
        test_multi_set( t_mset , sz_test , n_dupl ) ;
        test_map      ( t_map  , sz_test , n_dupl ) ;
        test_multi_map( t_mmap , sz_test , n_dupl ) ;
    }

}


#endif  //  _TEST_CONTAINERS_HPP


