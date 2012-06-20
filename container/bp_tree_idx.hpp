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

#ifndef _BP_TREE_IDX_HPP
#define _BP_TREE_IDX_HPP

#include <iterator>
#include <utility>
#include <memory>
#include <algorithm>
#include "bpt_helpers.hpp"


_BPT_OPEN

template
<
    class _Ty_Key ,
    class _Ty_Map ,
    class _Ty_Val ,
    class _KeyOfV ,
    class _MapOfV ,
    class _Pred   ,
    class _Alloc
 >
class bp_tree_idx
{
public:
    typedef bp_tree_idx < _Ty_Key , _Ty_Map , _Ty_Val ,
                          _KeyOfV , _MapOfV , _Pred   , _Alloc  >
                                                _Ty_This        ;
    typedef _Ty_Key                             key_type        ;
    typedef _Ty_Map                             mapped_type     ;
    typedef _Ty_Val                             value_type      ;
    typedef typename _Alloc::size_type          size_type       ;
    typedef typename _Alloc::difference_type    difference_type ;
    typedef typename _Alloc::pointer            pointer         ;
    typedef typename _Alloc::const_pointer      const_pointer   ;
    typedef typename _Alloc::reference          reference       ;
    typedef typename _Alloc::const_reference    const_reference ;

protected:

    struct  _NodeLight ;
    typedef _NodeLight *    _NodeLightPtr ;
    struct  _NodeHeavy ;
    typedef _NodeHeavy *    _NodeHeavyPtr ;

    struct  _NodeLight
    {
        _NodeLight ( _Ty_Val const &  ty_x = _Ty_Val ( ) ) :
            elem      ( ty_x ) ,
            p_light_next ( 0 ) ,
            p_light_prev ( 0 ) ,
            p_heavy_predr( 0 )
        {  }

        reference           _elem( )        { return elem ; }
        const_reference     _elem( ) const  { return elem ; }

        _Ty_Val         elem ;
        _NodeLightPtr   p_light_next  ;
        _NodeLightPtr   p_light_prev  ;
        _NodeHeavyPtr   p_heavy_predr ;

    private:
        _NodeLight &
        operator = ( const _NodeLight & ) ;
        _NodeLight ( const _NodeLight & ) ;
    } ;


    struct  _NodeHeavy
    {
        void             _set_node_light ( _NodeLightPtr  p_light )
                                         { p_node_light = p_light ; }
        _NodeLightPtr    _get_node_light ( ) const
                                         { return p_node_light ; }
        const_reference  _elem() const   { return p_node_light->elem ; }

        _NodeHeavyPtr   p_next  ;
        _NodeHeavyPtr   p_prev  ;
        _NodeHeavyPtr   p_succr ;
        _NodeHeavyPtr   p_predr ;
        size_type       m_subsz ;

    private:
        _NodeLightPtr   p_node_light ;
    } ;

public:

    class _iter_base
    {
    friend class bp_tree_idx < _Ty_Key , _Ty_Map , _Ty_Val ,
                               _KeyOfV , _MapOfV , _Pred   , _Alloc  > ;

    protected:
        _iter_base ( ) : m_ptr(0), m_index(0) , m_p_cont(0) { }
        _iter_base ( _NodeLightPtr     pln ,
                     difference_type   ind ,
                     _Ty_This const *  ctr ) :
                     m_ptr(pln), m_index(ind), m_p_cont( (_Ty_This*)ctr ) { }
        _iter_base ( _NodeLightPtr     pln ,
                     difference_type   ind ,
                     _Ty_This *        ctr ) :
                     m_ptr(pln) , m_index(ind) , m_p_cont( ctr ) { }

    public:
        bool operator == ( const _iter_base & it_x ) const
                         { return m_index == it_x.m_index ; }
        bool operator != ( const _iter_base & it_x ) const
                         { return m_index != it_x.m_index ; }
        bool operator <  ( const _iter_base & it_x ) const
                         { return m_index <  it_x.m_index ; }
        bool operator <= ( const _iter_base & it_x ) const
                         { return m_index <= it_x.m_index ; }
        bool operator >  ( const _iter_base & it_x ) const
                         { return m_index >  it_x.m_index ; }
        bool operator >= ( const _iter_base & it_x ) const
                         { return m_index >= it_x.m_index ; }

        bool valid_data ( ) const
        {
            return m_p_cont!=0 && m_ptr!=0 ;
        }

        bool valid_mapping ( ) const
        {
            if ( ! valid_data( ) )
                return false ;
            _NodeLightPtr   p_find =
                            m_p_cont->_find_node_light ( size_type(m_index) ) ;
            return ( p_find == m_ptr ) ;
        }

        void update_pointer ( )
        {
            if ( valid_data( ) )
            {
                m_ptr = m_p_cont->_find_node_light ( size_type(m_index) ) ;
            }
        }

        void update_index ( )
        {
            if ( valid_data( ) )
            {
                size_type   idx = m_p_cont->_find_node_index ( m_ptr ) ;
                m_index = difference_type ( idx ) ;
            }
        }

    protected:
        difference_type     _size ( )      const
            { return valid_data() ? m_p_cont->_size_dt() : 0 ; }
        _NodeLightPtr       _end  ( )      const
            { return valid_data() ? m_p_cont->_external_end() : 0 ; }
        _NodeLightPtr       _pointer ( )   const    { return m_ptr    ; }
        difference_type     _index   ( )   const    { return m_index  ; }
        const _Ty_This *    _container ( ) const    { return m_p_cont ; }

        bool _index_valid ( difference_type  ind ) const
        {
            if ( !valid_data( ) )
                return false ;
            if ( ind<0 || ind>=_size( ) )
                return false ;
            return true ;
        }

        void _set_index ( const difference_type  ind )
        {
            m_index = ind ;
        }

        void _move_idx_impl ( const difference_type  dist_idx )
        {
            if ( dist_idx == 0 )
                return ;

            m_index += dist_idx ;
            if ( !_index_valid(m_index) )
            {
                m_ptr = _end ( ) ;
                return ;
            }

            difference_type     idx_node   = ( m_ptr==_end() ) ? _size() :
                                             ( m_index-dist_idx ) ;
            difference_type     dist_nodes = m_index - idx_node ;

            if ( dist_nodes >= 0 )
            {
                if ( size_type(dist_nodes) <= m_p_cont->_min_degree() )
                {
                    while ( dist_nodes )
                    {
                        m_ptr = m_ptr->p_light_next ;
                        --dist_nodes ;
                    }
                }
                else if ( size_type(dist_nodes) <= m_p_cont->_local_limit() )
                {
                    while ( dist_nodes )
                    {
                        m_ptr = m_ptr->p_light_next ;
                        --dist_nodes ;
                    }
                }
                else
                {
                    m_ptr = m_p_cont->_find_node_light( size_type(m_index) ) ;
                }
            }
            else
            {
                dist_nodes = -dist_nodes ;
                size_type   dist_temp = size_type ( dist_nodes ) ;
                if ( dist_temp <= m_p_cont->_min_degree() )
                {
                    while ( dist_temp )
                    {
                        m_ptr = m_ptr->p_light_prev ;
                        --dist_temp ;
                    }
                }
                else if ( dist_temp <= m_p_cont->_local_limit() )
                {
                    while ( dist_temp )
                    {
                        m_ptr = m_ptr->p_light_prev ;
                        --dist_temp ;
                    }
                }
                else
                {
                    m_ptr = m_p_cont->_find_node_light( size_type(m_index) ) ;
                }
            }
        }

        void _increment ( )
        {
            ++m_index ;
            if ( _index_valid(m_index) )
                m_ptr = m_ptr->p_light_next ;
            else
                m_ptr = _end ( ) ;
        }

        void _decrement ( )
        {
            --m_index ;
            if ( _index_valid(m_index) )
                m_ptr = m_ptr->p_light_prev ;
            else
                m_ptr = _end ( ) ;
        }

        _NodeLightPtr       m_ptr    ;
        difference_type     m_index  ;
        _Ty_This *          m_p_cont ;
    } ;


    template < class _Cat, class _Val, class _Diff, class _Ptr, class _Ref >
    class _iter_base_stl : public _iter_base
    {
    public:
        typedef _Cat        iterator_category ;
        typedef _Val        value_type        ;
        typedef _Diff       difference_type   ;
        typedef _Diff       distance_type     ;
        typedef _Ptr        pointer           ;
        typedef _Ref        reference         ;

    protected:
        _iter_base_stl ( ) : _iter_base ( ) {  }
        _iter_base_stl ( _NodeLightPtr     pln ,
                         difference_type   ind ,
                         _Ty_This const *  ctr ) :
                         _iter_base ( pln, ind , ctr ) { }
        _iter_base_stl ( _NodeLightPtr     pln ,
                         difference_type   ind ,
                         _Ty_This *        ctr ) :
                         _iter_base ( pln, ind , ctr ) { }
    } ;


    class iterator ;
    friend
    class iterator ;
    typedef _iter_base_stl< std::random_access_iterator_tag,
                            value_type, difference_type, pointer, reference >
                            _iter_base_stl_alias ;


    class iterator : public _iter_base_stl_alias
    {
    friend class bp_tree_idx < _Ty_Key , _Ty_Map , _Ty_Val ,
                               _KeyOfV , _MapOfV , _Pred   , _Alloc  > ;

        iterator ( _NodeLightPtr pln, difference_type ind, _Ty_This * ctr ) :
            _iter_base_stl_alias ( pln , ind , ctr ) { }

    public:
        iterator ( ) : _iter_base_stl_alias ( ) { }

        reference   operator*  ( ) const
                    { return _iter_base_stl_alias::m_ptr->_elem( ) ; }
        pointer     operator-> ( ) const    { return (&**this) ; }
        reference   operator[ ]( difference_type  ind ) const
                                            { return ( *(*this + ind) ) ; }

        iterator &  operator ++ ( )
                    { _iter_base_stl_alias::_increment() ; return (*this) ; }
        iterator &  operator -- ( )
                    { _iter_base_stl_alias::_decrement() ; return (*this) ; }
        iterator    operator ++ ( int )
                    { iterator tmp = *this ; ++(*this) ; return tmp ; }
        iterator    operator -- ( int )
                    { iterator tmp = *this ; --(*this) ; return tmp ; }
        iterator &  operator += ( difference_type _m )
                    { _iter_base_stl_alias::_move_idx_impl(+_m); return *this;}
        iterator &  operator -= ( difference_type _m )
                    { _iter_base_stl_alias::_move_idx_impl(-_m); return *this;}
        iterator    operator +  ( difference_type _m ) const
                    { iterator tmp = *this ; return ( tmp += _m ) ; }
        iterator    operator -  ( difference_type _m ) const
                    { iterator tmp = *this ; return ( tmp -= _m ) ; }

        difference_type operator -  ( const iterator & it_x ) const
                    { return ( _iter_base_stl_alias::m_index -
                               it_x._iter_base_stl_alias::m_index ) ; }
    } ;


    class const_iterator ;
    friend
    class const_iterator ;

    class const_iterator : public _iter_base_stl_alias
    {
    friend class bp_tree_idx < _Ty_Key , _Ty_Map , _Ty_Val ,
                               _KeyOfV , _MapOfV , _Pred   , _Alloc  > ;

        const_iterator ( _NodeLightPtr     pln ,
                         difference_type   ind ,
                         _Ty_This const *  ctr ) :
                         _iter_base_stl_alias ( pln , ind , ctr ) { }

    public:
        const_iterator ( ) : _iter_base_stl_alias ( ) { }
        const_iterator ( const iterator &  iter ) :
            _iter_base_stl_alias ( iter._iter_base_stl_alias::m_ptr    ,
                                   iter._iter_base_stl_alias::m_index  ,
                                   iter._iter_base_stl_alias::m_p_cont ) { }

        const_reference     operator*  ( ) const
                            { return _iter_base_stl_alias::m_ptr->_elem() ; }
        const_pointer       operator-> ( ) const    { return ( &**this ) ; }
        const_reference     operator[ ]( difference_type  ind ) const
                                                    { return (*(*this+ind)) ; }

        const_iterator &    operator ++ ( )
                    { _iter_base_stl_alias::_increment() ; return (*this) ; }
        const_iterator &    operator -- ( )
                    { _iter_base_stl_alias::_decrement() ; return (*this) ; }
        const_iterator      operator ++ ( int )
                    { const_iterator tmp = *this ; ++(*this) ; return tmp ; }
        const_iterator      operator -- ( int )
                    { const_iterator tmp = *this ; --(*this) ; return tmp ; }
        const_iterator &    operator += ( difference_type _m )
                    { _iter_base_stl_alias::_move_idx_impl(+_m); return *this ;}
        const_iterator &    operator -= ( difference_type _m )
                    { _iter_base_stl_alias::_move_idx_impl(-_m); return *this ;}
        const_iterator      operator +  ( difference_type _m ) const
                    { const_iterator tmp = *this ; return ( tmp += _m ) ; }
        const_iterator      operator -  ( difference_type _m ) const
                    { const_iterator tmp = *this ; return ( tmp -= _m ) ; }

        difference_type     operator -  ( const const_iterator & cit_x ) const
                    { return ( _iter_base_stl_alias::m_index -
                               cit_x._iter_base_stl_alias::m_index ) ; }
    } ;


    bp_tree_idx  ( const _Pred &     pred ,
                   bool              mul  ,
                   bool              ord  ,
                   const _Alloc &    alr  ) ;
    bp_tree_idx  ( const _Ty_This &  that ) ;
    _Ty_This &
    operator =   ( const _Ty_This &  that ) ;
    ~bp_tree_idx ( ) ;

    iterator        begin ( )
                    { return iterator      (_external_begin(), 0, this ) ; }
    const_iterator  begin ( ) const
                    { return const_iterator(_external_begin(), 0, this ) ; }
    iterator        end   ( )
                    { return iterator      (_external_end(), _size_dt(), this);}
    const_iterator  end   ( ) const
                    { return const_iterator(_external_end(), _size_dt(), this);}

    size_type   size ( )          const { return m_size_light ; }
    size_type   max_size ( )      const { return m_allr_ty_val.max_size() ; }
    bool        empty ( )         const { return ( size ( ) == 0 ) ; }
    _Alloc      get_allocator ( ) const { return m_allr_ty_val ; }
    _Pred       key_comp ( )      const { return m_k_comp  ; }

    std::pair<iterator, bool>
                insert ( const value_type &  elem_x ) ;
    iterator    insert ( iterator  pos , const value_type &  elem_x ) ;
    template <class _InpIter>
    void        insert_set ( _InpIter  pos_a , _InpIter  pos_b )
                { _insert_iter_ordered ( pos_a , pos_b ) ; }
    template <class _InpIter>
    void        insert_map ( _InpIter  pos_a , _InpIter  pos_b )
                { _insert_iter_ordered ( pos_a , pos_b ) ; }

    iterator    erase ( iterator         pos   ) ;
    iterator    erase ( iterator         pos_a ,
                        iterator         pos_b ) ;
    size_type   erase ( const _Ty_Key &  key_x ) ;
    void        clear ( )  { _clear ( ) ; }
    void        swap  ( _Ty_This &  ctr_x ) ;

    iterator        find        ( const _Ty_Key &  key_x )       ;
    const_iterator  find        ( const _Ty_Key &  key_x ) const ;
    size_type       count       ( const _Ty_Key &  key_x ) const ;
    iterator        lower_bound ( const _Ty_Key &  key_x )       ;
    const_iterator  lower_bound ( const _Ty_Key &  key_x ) const ;
    iterator        upper_bound ( const _Ty_Key &  key_x )       ;
    const_iterator  upper_bound ( const _Ty_Key &  key_x ) const ;
    std::pair<iterator, iterator>
                    equal_range ( const _Ty_Key &  key_x )       ;
    std::pair<const_iterator, const_iterator>
                    equal_range ( const _Ty_Key &  key_x ) const ;

    size_type           capacity ( ) const ;
    void                reserve  ( size_type   sz ) ;

    const_reference     at ( size_type  ind ) const ;
    reference           at ( size_type  ind )       ;
    const_reference     operator[] ( size_type  ind ) const ;
    reference           operator[] ( size_type  ind )       ;

    reference           front ( )       ;
    const_reference     front ( ) const ;
    reference           back  ( )       ;
    const_reference     back  ( ) const ;

    void                push_back     ( const_reference  val ) ;
    void                push_front    ( const_reference  val ) ;
    iterator            _insert_seqce ( iterator         pos ,
                                        const_reference  val ) ;
    void                _insert_seqce_count
                                      ( iterator         pos ,
                                        size_type        cnt ,
                                        const_reference  val ) ;
    template<class _InpIter>
    void                _insert_seqce_iter ( iterator  pos   ,
                                             _InpIter  pos_a ,
                                             _InpIter  pos_b )
    {
        _insert_seqce_impl ( pos , pos_a , pos_b ) ;
    }

    void    pop_front ( ) ;
    void    pop_back  ( ) ;
    void    remove ( const_reference  val ) ;
    template < class _PredRem >
    void    remove_if ( _PredRem  pred ) { _remove_if ( pred ) ; }
    void    unique ( ) ;
    template < class _PredRem >
    void    unique ( _PredRem  pred ) { _unique_pred ( pred ) ; }

    void    resize  ( size_type   sz_new ) ;
    void    resize  ( size_type   sz_new ,
                      const_reference
                                  val    ) ;
    iterator splice ( iterator    pos    ,
                      _Ty_This &  that   ) ;
    iterator splice ( iterator    pos    ,
                      _Ty_This &  that   ,
                      iterator    pos_a  ) ;
    iterator splice ( iterator    pos    ,
                      _Ty_This &  that   ,
                      iterator    pos_a  ,
                      iterator    pos_b  ) ;

    template < class _PredMerge >
    void    merge  ( _Ty_This &  that , _PredMerge  pred )
            { _merge_pred ( that , pred ) ; }

    void    reverse ( ) ;
    void    sort ( ) ;
    template < class _PredSort >
    void    sort ( _PredSort  pred ) { _sort_pred ( pred ) ; }

    mapped_type     accumulate  ( const_iterator       it_start ,
                                  const_iterator       it_end   ,
                                  mapped_type          val_in   ) const ;
    void            write_shallow
                                ( iterator             pos      ,
                                  const mapped_type &  val_new  ) ;
    iterator        write_deep  ( iterator             pos      ,
                                  const _Ty_Val &      val_new  ) ;

protected:
    _NodeLightPtr   _create_node_light ( const value_type &  val_x  ) ;
    _NodeLightPtr   _insert_node_light ( _NodeLightPtr       p_posn ,
                                         const value_type &  elem_x ) ;
    void            _link_node_light   ( _NodeLightPtr       p_posn ,
                                         _NodeLightPtr       p_new  ) ;
    void            _create_head_light ( ) ;
    void            _delete_node_light ( _NodeLightPtr &     p_del  ) ;
    void            _delete_head_light ( ) ;

    _NodeHeavyPtr   _create_node_heavy ( _NodeLightPtr   p_light ) ;
    void            _delete_node_heavy ( _NodeHeavyPtr & p_node  ) ;
    void            _create_head_heavy ( ) ;

    void     _init ( ) ;
    void     _copy ( const _Ty_This &  that ) ;
    void     _clear   ( ) ;
    void     _destroy ( ) ;

    std::pair<iterator, bool>
             _insert_ordered     ( const value_type &  elem_x   ) ;
    iterator _insert_seqce_posn  ( difference_type     index    ,
                                   _NodeLightPtr       p_lt_pos ,
                                   const value_type &  elem_x   ) ;
    void     _insert_b_tree      ( _NodeLightPtr       p_lt_new ) ;
    void     _insert_heavy_node  ( _NodeHeavyPtr       p_posn   ,
                                   _NodeLightPtr       p_light  ) ;
    void     _split_node         ( _NodeHeavyPtr       p_parent ,
                                   _NodeHeavyPtr       p_pos    ) ;
    void     _split_node_external
                                 ( _NodeHeavyPtr       p_parent ,
                                   _NodeLightPtr       p_lt_pos ) ;
    void     _insert_top_level ( ) ;
    void     _increase_tree_height ( ) ;

    iterator _erase_bp_tree    ( iterator              pos       ) ;
    void     _erase_heavy_node ( _NodeHeavyPtr &       p_posn    ) ;
    void     _erase_top_level  ( ) ;
    void     _merge_node_with  ( _NodeHeavyPtr         p_left    ,
                                 size_type &           subsize   ) ;
    void     _balance          ( const difference_type n_ch_left ,
                                 const difference_type n_ch_right,
                                 const _NodeHeavyPtr   p_mid     ) ;
    void     _balance_external ( const _NodeHeavyPtr   p_parent  ) ;
    void     _update_data      ( const _NodeHeavyPtr   p_parent  ,
                                 const size_type       sub_sz    ) ;
    void     _link_to_parent_ext
                               ( _NodeHeavyPtr         p_parent  ) ;
    void     _link_to_parent   ( _NodeHeavyPtr         p_parent  ) ;
    bool     _find_two_groups  ( const _NodeHeavyPtr   ptr_level ,
                                 const _NodeHeavyPtr   ptr_input ,
                                 _NodeHeavyPtr &       ptr_left  ,
                                 _NodeHeavyPtr &       ptr_mid   ,
                                 _NodeHeavyPtr &       ptr_right ) const ;

    difference_type
                _size_dt ( )  const { return difference_type(size()) ; }
    size_type   _count_level  ( _NodeHeavyPtr          p_l_end ) const ;
    size_type   _count_level  ( const _NodeHeavyPtr    p_from  ,
                                const _NodeHeavyPtr    p_to    ) const ;
    void        _move_level   ( _NodeHeavyPtr &        ptr     ,
                                const difference_type  n_move  ) ;
    void        _move_level_ext
                              ( _NodeLightPtr &        ptr     ,
                                const difference_type  n_move  ) ;
    size_type   _move_subsize ( _NodeHeavyPtr &        ptr     ,
                                const difference_type  n_move  ) ;

    size_type       _local_limit ( ) const ;
    _NodeHeavyPtr   _bottom_begin( ) const
                    { return m_p_head_heavy->p_predr->p_next ; }
    _NodeHeavyPtr   _bottom_end  ( ) const
                    { return m_p_head_heavy->p_predr ; }
    _NodeHeavyPtr   _top_begin   ( ) const
                    { return m_p_head_heavy->p_succr->p_next ; }
    _NodeHeavyPtr   _top_end     ( ) const
                    { return m_p_head_heavy->p_succr ; }

    _NodeLightPtr   _external_begin ( ) const
                    { return m_p_head_light->p_light_next ; }
    _NodeLightPtr   _external_end   ( ) const
                    { return m_p_head_light ; }
    _NodeLightPtr   _external_last  ( ) const
                    { return m_p_head_light->p_light_prev ; }

    void            _find_lower_bound(const _Ty_Key &   key_x   ,
                                      difference_type & index   ,
                                      _NodeLightPtr &   p_lt_pos) const ;
    void            _find_upper_bound(const _Ty_Key &   key_x   ,
                                      difference_type & index   ,
                                      _NodeLightPtr &   p_lt_pos) const ;
    _NodeLightPtr   _find_node_light (const size_type   idx_pos ) const ;
    size_type       _find_node_index (_NodeLightPtr     p_node  ) const ;

    size_type       _tree_height ( ) const ;
    bool            _size_second_top_is_less
                                     ( const size_type  sz_lim  ) const ;
    _NodeHeavyPtr   _move_to_parent  ( _NodeHeavyPtr    p_1st_ch) const ;

    iterator        _splice_impl ( iterator         pos     ,
                                   _Ty_This &       that    ,
                                   iterator         pos_a   ,
                                   iterator         pos_b   ) ;

    template <class _InpIter>
    void _insert_iter_ordered ( _InpIter  pos_a , _InpIter  pos_b )
    {
        if ( this->empty() || !m_multi )
        {
            while ( pos_a != pos_b )
            {
                insert (*pos_a) ;
                ++pos_a ;
            }
        }
        else
        {
            _Ty_This    ctr_copy ( m_k_comp , m_multi , m_ordered , m_allr_ty_val ) ;
            while ( pos_a != pos_b )
            {
                ctr_copy . insert (*pos_a) ;
                ++pos_a ;
            }

            const_iterator  cur = ctr_copy . begin() ;
            const_iterator  end = ctr_copy . end  () ;
            while ( cur != end )
            {
                this->insert ( *cur ) ;
                ++cur ;
            }
        }
    }

    template<class _InpIter>
    void _insert_seqce_impl ( iterator  pos, _InpIter  pos_a, _InpIter  pos_b )
    {
        while ( pos_a != pos_b )
        {
            pos = _insert_seqce ( pos , *pos_a ) ;
            ++pos ;
            ++pos_a ;
        }
    }

    template < class _PredRem >
    void _remove_if ( _PredRem  pred )
    {
        iterator    it_curr = begin ( ) ;
        iterator    it_next ( it_curr ) ;
        size_type   cnt = size ( ) ;

        while ( cnt-- )
        {
            ++it_next ;
            if ( pred ( *it_curr ) )
                it_next = erase ( it_curr ) ;
            it_curr = it_next ;
        }
    }

    template < class _PredRem >
    void _unique_pred ( _PredRem  pred )
    {
        size_type   cnt = size ( ) ;
        if ( cnt < 2 )
            return ;

        iterator    it_curr = begin ( ) ;
        iterator    it_next ( it_curr ) ;

        --cnt ;
        ++it_next ;
        while ( cnt-- )
        {
            if ( pred ( *it_curr , *it_next ) )
            {
                it_next = erase ( it_next ) ;
            }
            else
            {
                it_curr = it_next ;
                ++it_next ;
            }
        }
    }

    template < class _PredMerge >
    void _merge_pred ( _Ty_This &  that , _PredMerge  pred )
    {
        if ( this==&that )
            return ;

        iterator    cur_this = begin() ;
        iterator    cur_that = that.begin() ;

        while ( cur_this!=this->end() && cur_that!=that.end() )
        {
            if ( pred ( *cur_that ,*cur_this ) )
            {
                cur_this = this->_insert_seqce ( cur_this , *cur_that ) ;
                ++cur_this ;
                cur_that = that . erase ( cur_that ) ;
            }
            else
            {
                ++cur_this ;
            }
        }

        if ( cur_that!=that.end() )
        {
            iterator    it_end = this->end() ;
            _splice_impl ( it_end , that , cur_that , that.end() ) ;
        }
    }

    template < class _PredSort >
    void _sort_pred ( _PredSort  pred )
    {
        bp_tree_idx <_Ty_Key, mapped_type, value_type,
                     _KeyOfV, _MapOfV, _PredSort, _Alloc>
            contr_ord( pred, this->m_multi, true, this->get_allocator() ) ;

        while ( ! this->empty() )
        {
            contr_ord.insert ( *(this->begin()) ) ;
            this->erase      (   this->begin()  ) ;
        }

        while ( !contr_ord.empty() )
        {
            this->push_back   (*(contr_ord.begin()) ) ;
            contr_ord . erase (  contr_ord.begin()  ) ;
        }
    }


    typedef typename _Alloc::template rebind<_NodeLight>::other
                                _NodeLightAllocr  ;

    _NodeLightAllocr            m_allr_node_light ;
    _NodeLightPtr               m_p_head_light    ;
    size_type                   m_size_light      ;

    typedef typename _Alloc::template rebind<_NodeHeavy>::other
                                _NodeHeavyAllocr  ;

    _NodeHeavyAllocr            m_allr_node_heavy ;
    _Alloc                      m_allr_ty_val     ;
    _NodeHeavyPtr               m_p_head_heavy    ;
    _Pred                       m_k_comp          ;
    bool                        m_multi           ;
    bool                        m_ordered         ;


    static size_type _min_degree() { return 8 ; }
    static size_type _max_degree() { return 2*_min_degree() ; }

} ;


#define TEMPL_DECL  template< class _Ty_Key , class _Ty_Map , class _Ty_Val , \
                              class _KeyOfV , class _MapOfV , \
                              class _Pred   , class _Alloc  > inline
#define BP_TREE_TY  bp_tree_idx < _Ty_Key , _Ty_Map , _Ty_Val , \
                                  _KeyOfV , _MapOfV , _Pred   , _Alloc >


TEMPL_DECL
typename BP_TREE_TY::_NodeLightPtr
BP_TREE_TY::_create_node_light ( const value_type &  val )
{
    _NodeLightPtr   p_n = m_allr_node_light . allocate ( 1 , 0 ) ;

    try
    {
        p_n->p_light_next  = 0 ;
        p_n->p_light_prev  = 0 ;
        p_n->p_heavy_predr = 0 ;
        pointer     p_elem = &(p_n->_elem()) ;
        m_allr_ty_val . construct ( p_elem , val ) ;
    }
    catch (...)
    {
        m_allr_node_light . deallocate ( p_n , 1 ) ;
        throw ;
    }

    return p_n ;
}


TEMPL_DECL
typename BP_TREE_TY::_NodeLightPtr
BP_TREE_TY::_insert_node_light ( _NodeLightPtr       p_lt_posn ,
                                 const value_type &  elem_x    )
{
    _NodeLightPtr   p_lt_new = _create_node_light ( elem_x ) ;
    _link_node_light ( p_lt_posn , p_lt_new ) ;
    return p_lt_new ;
}


TEMPL_DECL
void BP_TREE_TY::_link_node_light ( _NodeLightPtr  p_posn ,
                                    _NodeLightPtr  p_new  )
{
    _NodeLightPtr   p_prev= p_posn->p_light_prev ;
    p_new ->p_light_next  = p_posn ;
    p_new ->p_light_prev  = p_prev ;
    p_prev->p_light_next  = p_new  ;
    p_posn->p_light_prev  = p_new  ;
    ++m_size_light ;
}


TEMPL_DECL
void BP_TREE_TY::_create_head_light ( )
{
    m_p_head_light = _create_node_light ( _Ty_Val() ) ;
    m_p_head_light->p_light_next = m_p_head_light ;
    m_p_head_light->p_light_prev = m_p_head_light ;
}


TEMPL_DECL
void BP_TREE_TY::_delete_node_light ( _NodeLightPtr &  p_del )
{
    if ( p_del )
    {
        _NodeLightPtr       p_bef = p_del->p_light_prev ;
        _NodeLightPtr       p_aft = p_del->p_light_next ;
        p_bef->p_light_next = p_aft ;
        p_aft->p_light_prev = p_bef ;
        pointer             p_elem = &(p_del->_elem()) ;
        m_allr_ty_val . destroy ( p_elem ) ;
        m_allr_node_light . deallocate ( p_del , 1 ) ;
        --m_size_light ;
        p_del = 0 ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_delete_head_light ( )
{
    if ( m_p_head_light )
    {
        _delete_node_light ( m_p_head_light ) ;
        m_size_light = 0 ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::_NodeHeavyPtr
BP_TREE_TY::_create_node_heavy ( _NodeLightPtr  p_light )
{
    _NodeHeavyPtr   p_n = m_allr_node_heavy . allocate ( 1 , 0 ) ;

    try
    {
        p_n->p_next  = 0 ;
        p_n->p_prev  = 0 ;
        p_n->p_succr = 0 ;
        p_n->p_predr = 0 ;
        p_n->m_subsz = 0 ;
        p_n->_set_node_light ( p_light ) ;
    }
    catch (...)
    {
        m_allr_node_heavy . deallocate ( p_n , 1 ) ;
        throw ;
    }

    return p_n ;
}


TEMPL_DECL
void BP_TREE_TY::_delete_node_heavy ( _NodeHeavyPtr &  p_node )
{
    if ( p_node )
    {
        m_allr_node_heavy . deallocate ( p_node , 1 ) ;
        p_node = 0 ;
    }
}


TEMPL_DECL
BP_TREE_TY::bp_tree_idx ( const _Pred &   pred ,
                          bool            mul  ,
                          bool            ord  ,
                          const _Alloc &  alr  ) :
    m_allr_node_light ( alr ) ,
    m_p_head_light    (   0 ) ,
    m_size_light      (   0 ) ,
    m_allr_node_heavy ( alr ) ,
    m_allr_ty_val     ( alr ) ,
    m_p_head_heavy    (   0 ) ,
    m_k_comp          ( pred) ,
    m_multi           ( mul ) ,
    m_ordered         ( ord )
{
    _init ( ) ;
}


TEMPL_DECL
BP_TREE_TY::bp_tree_idx ( const BP_TREE_TY &  that ) :
    m_allr_node_light ( that . m_allr_node_light ) ,
    m_p_head_light    (  0 ) ,
    m_size_light      (  0 ) ,
    m_allr_node_heavy ( that . m_allr_node_heavy ) ,
    m_allr_ty_val     ( that . m_allr_ty_val     ) ,
    m_p_head_heavy    (  0 ) ,
    m_k_comp          ( that . m_k_comp  ) ,
    m_multi           ( that . m_multi   ) ,
    m_ordered         ( that . m_ordered )
{
    _init ( ) ;
    _copy ( that ) ;
}


TEMPL_DECL
BP_TREE_TY &
BP_TREE_TY::operator = ( const BP_TREE_TY &  that )
{
    if ( this != &that )
    {
        _clear ( ) ;
        m_k_comp  = that . m_k_comp  ;
        m_multi   = that . m_multi   ;
        m_ordered = that . m_ordered ;
        _copy ( that ) ;
    }
    return ( *this ) ;
}


TEMPL_DECL
BP_TREE_TY::~bp_tree_idx ( )
{
    _destroy ( ) ;
}


TEMPL_DECL
void BP_TREE_TY::_init ( )
{
    _create_head_light ( ) ;
    _create_head_heavy ( ) ;
    _insert_top_level  ( ) ;
    m_p_head_light->p_heavy_predr = _bottom_end() ;
}


TEMPL_DECL
void BP_TREE_TY::_copy ( const BP_TREE_TY & that )
{
    const_iterator  cur = that . begin ( ) ;
    const_iterator  end = that . end   ( ) ;
    if ( m_ordered )
    {
        for ( ; cur != end ; ++cur )
            insert (*cur) ;
    }
    else
    {
        for ( ; cur != end ; ++cur )
            push_back (*cur) ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_clear ( )
{
    while ( ! empty ( ) )
        erase ( begin ( ) ) ;
}


TEMPL_DECL
void BP_TREE_TY::_destroy ( )
{
    _clear ( ) ;

    if ( m_p_head_heavy )
    {
        _delete_node_heavy ( m_p_head_heavy->p_predr ) ;
        _delete_node_heavy ( m_p_head_heavy ) ;
    }

    _delete_head_light ( ) ;
}


TEMPL_DECL
void BP_TREE_TY::_create_head_heavy ( )
{
    m_p_head_heavy = _create_node_heavy ( m_p_head_light ) ;
    m_p_head_heavy->p_next  = 0 ;
    m_p_head_heavy->p_prev  = 0 ;
    m_p_head_heavy->p_succr = m_p_head_heavy ;
    m_p_head_heavy->p_predr = m_p_head_heavy ;
}


TEMPL_DECL
void BP_TREE_TY::_insert_heavy_node ( _NodeHeavyPtr  p_posn  ,
                                      _NodeLightPtr  p_light )
{
    _NodeHeavyPtr   p_new = _create_node_heavy ( p_light ) ;
    _NodeHeavyPtr   p_bef = p_posn->p_prev ;
    p_new ->p_prev = p_bef  ;
    p_new ->p_next = p_posn ;
    p_posn->p_prev = p_new  ;
    p_bef ->p_next = p_new  ;
}


TEMPL_DECL
void BP_TREE_TY::_split_node
    (
        _NodeHeavyPtr   p_parent ,
        _NodeHeavyPtr   p_pos
    )
{
    _insert_heavy_node ( p_parent->p_next , p_pos->_get_node_light() ) ;

    _NodeHeavyPtr   p_par_new = p_parent->p_next ;
    p_par_new->p_predr = p_parent->p_predr ;
    p_par_new->p_succr = p_pos ;

    size_type       sub_sz = 0 ;
    _NodeHeavyPtr   p_tmp  = p_pos ;
    while ( p_tmp->p_predr == p_parent )
    {
        sub_sz        += p_tmp->m_subsz ;
        p_tmp->p_predr = p_par_new      ;
        p_tmp          = p_tmp->p_next  ;
    }

    p_par_new->m_subsz  = sub_sz ;
    p_parent ->m_subsz -= sub_sz ;
}


TEMPL_DECL
void BP_TREE_TY::_split_node_external
    (
        _NodeHeavyPtr   p_parent ,
        _NodeLightPtr   p_lt_pos
    )
{
    _insert_heavy_node ( p_parent->p_next , p_lt_pos ) ;

    _NodeHeavyPtr   p_par_new = p_parent->p_next ;
    p_par_new->p_predr = p_parent->p_predr ;
    p_par_new->p_succr = 0 ;

    size_type       sub_sz   = 0 ;
    _NodeLightPtr   p_lt_tmp = p_lt_pos ;
    while ( p_lt_tmp->p_heavy_predr == p_parent )
    {
        ++sub_sz ;
        p_lt_tmp->p_heavy_predr = p_par_new ;
        p_lt_tmp = p_lt_tmp->p_light_next  ;
    }

    p_par_new->m_subsz  = sub_sz ;
    p_parent ->m_subsz -= sub_sz ;
}


TEMPL_DECL
void BP_TREE_TY::_insert_top_level ( )
{
    _NodeHeavyPtr   p_top  = m_p_head_heavy->p_succr ;
    _NodeLightPtr   p_light= m_p_head_heavy->_get_node_light() ;
    _NodeHeavyPtr   p_new  = _create_node_heavy ( p_light ) ;
    _NodeHeavyPtr   p_bef  = p_top->p_predr ;
    p_new->p_predr = p_bef ;
    p_new->p_succr = p_top ;
    p_top->p_predr = p_new ;
    p_bef->p_succr = p_new ;
    p_new->p_next  = p_new ;
    p_new->p_prev  = p_new ;
}


TEMPL_DECL
void BP_TREE_TY::_increase_tree_height ( )
{
    _NodeHeavyPtr   p_top_beg = _top_begin() ;
    _NodeHeavyPtr   p_top_end = m_p_head_heavy->p_succr ;
    _NodeLightPtr   p_lt_beg  = p_top_beg->_get_node_light() ;

    _insert_top_level  ( ) ;
    _insert_heavy_node ( m_p_head_heavy->p_succr, p_lt_beg ) ;

    _NodeHeavyPtr   p_new_top_beg = _top_begin() ;
    p_new_top_beg->p_succr = p_top_beg ;
    p_new_top_beg->p_predr = 0 ;
    p_new_top_beg->m_subsz = m_size_light ;

    while ( p_top_beg != p_top_end )
    {
        p_top_beg->p_predr = p_new_top_beg ;
        p_top_beg = p_top_beg->p_next ;
    }
}


TEMPL_DECL
std::pair<typename BP_TREE_TY::iterator, bool>
BP_TREE_TY::_insert_ordered ( const value_type &  elem_x )
{
    _NodeLightPtr       p_lt_new  = 0 ;
    difference_type     index     = 0 ;
    _NodeLightPtr       p_lt_posn = 0 ;

    if ( m_multi )
    {
        _find_upper_bound ( _KeyOfV() ( elem_x ) , index , p_lt_posn ) ;
    }
    else
    {
        _find_lower_bound ( _KeyOfV() ( elem_x ) , index , p_lt_posn ) ;
        bool  contains = ( ( p_lt_posn!=m_p_head_light ) &&
              !m_k_comp( _KeyOfV()(elem_x), _KeyOfV()( p_lt_posn->_elem())) ) ;
        if ( contains )
            return std::pair<iterator, bool>
                   ( iterator( p_lt_posn, index, this) , false ) ;
    }

    p_lt_new = _insert_node_light ( p_lt_posn , elem_x ) ;
    _insert_b_tree ( p_lt_new ) ;

    iterator    it_ins ( p_lt_new , index , this ) ;
    return std::pair<iterator, bool> ( it_ins , true ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::_insert_seqce_posn ( difference_type     index     ,
                                 _NodeLightPtr       p_lt_posn ,
                                 const value_type &  elem_x    )
{
    _NodeLightPtr   p_lt_new = _insert_node_light ( p_lt_posn, elem_x ) ;
    _insert_b_tree ( p_lt_new ) ;

    iterator        it_ins ( p_lt_new , index , this ) ;
    return it_ins ;
}


TEMPL_DECL
void BP_TREE_TY::_insert_b_tree ( _NodeLightPtr  p_lt_new )
{
    if ( m_size_light == 1 )
    {
        _NodeHeavyPtr   p_n_heavy  = _bottom_end() ;
        _insert_heavy_node ( p_n_heavy , p_lt_new ) ;
        p_n_heavy->p_prev->m_subsz = 1 ;
        p_lt_new->p_heavy_predr    = p_n_heavy->p_prev ;
        return ;
    }

    _NodeHeavyPtr       p_parent = 0 ;
    if ( p_lt_new->p_light_prev == m_p_head_light )
    {
        p_parent = _bottom_begin() ;
        p_lt_new->p_heavy_predr = p_parent ;

        _NodeHeavyPtr   p_beg = p_parent ;
        while ( p_beg )
        {
            p_beg->_set_node_light ( p_lt_new ) ;
            ++( p_beg->m_subsz ) ;
            p_beg = p_beg->p_predr ;
        }
    }
    else
    {
        _NodeLightPtr   p_light_tmp = p_lt_new->p_light_prev ;
        p_parent = p_light_tmp->p_heavy_predr ;
        p_lt_new->p_heavy_predr = p_parent ;

        _NodeHeavyPtr   p_tmp = p_parent ;
        while ( p_tmp != 0 )
        {
            ++( p_tmp->m_subsz ) ;
            p_tmp = p_tmp->p_predr ;
        }
    }

    if ( p_parent->m_subsz <= _max_degree() )
        return ;

    _NodeLightPtr    p_lt_mid = p_parent->_get_node_light( ) ;
    difference_type  n_move   = difference_type ( _min_degree() ) ;
    _move_level_ext ( p_lt_mid , n_move ) ;
    _split_node_external ( p_parent , p_lt_mid ) ;

    _NodeHeavyPtr   p_forw = 0 ;
    _NodeHeavyPtr   p_back = 0 ;
    size_type       n_back = 0 ;
    size_type       n_forw = 0 ;
    _NodeHeavyPtr   p_parent_up = p_parent->p_predr ;

    while ( p_parent_up != 0 )
    {
        p_forw = p_parent->p_next ;
        n_forw = 0 ;
        while ( p_forw->p_predr == p_parent_up )
        {   p_forw = p_forw->p_next ; ++n_forw ; }

        p_back = p_parent ;
        n_back = 0 ;
        while ( p_back->p_predr == p_parent_up )
        {   p_back = p_back->p_prev ; ++n_back ; }

        _NodeHeavyPtr   p_mid = p_parent->p_next ;
        p_parent    = p_parent_up ;
        p_parent_up = p_parent->p_predr ;

        if ( ( n_forw + n_back ) <= _max_degree() )
            break ;

        difference_type n_move = difference_type ( _min_degree() ) -
                                 difference_type ( n_back ) ;
        _move_level ( p_mid    , n_move ) ;
        _split_node ( p_parent , p_mid  ) ;
    }

    if ( ( p_parent_up==0 ) &&
         ( _count_level(m_p_head_heavy->p_succr) > _min_degree() ) )
    {
        _increase_tree_height ( ) ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::_erase_bp_tree ( iterator  pos )
{
    iterator            it_res = pos ;
    ++it_res ;
    it_res . _set_index ( pos._index ( ) ) ;

    _NodeLightPtr       p_lt_erase  = pos . _pointer ( ) ;
    _NodeLightPtr       p_lt_after  = p_lt_erase->p_light_next  ;
    _NodeHeavyPtr       p_parent    = p_lt_erase->p_heavy_predr ;
    _NodeHeavyPtr       p_level_cur = 0 ;
    _NodeHeavyPtr       p_level_top = 0 ;

    if ( m_size_light < 2 )
    {
        _erase_heavy_node  ( p_parent ) ;
        _delete_node_light ( p_lt_erase ) ;
        return it_res ;
    }

    if ( p_lt_erase == m_p_head_light->p_light_next )
    {
        _NodeHeavyPtr   p_beg = p_parent ;
        do
        {
            p_beg->_set_node_light ( p_lt_after ) ;
            --( p_beg->m_subsz ) ;
            p_beg = p_beg->p_predr ;
        }
        while ( p_beg ) ;
    }
    else
    {
        _NodeHeavyPtr   p_tmp = p_parent ;
        do
        {
            --( p_tmp->m_subsz ) ;
            p_tmp = p_tmp->p_predr ;
        }
        while ( p_tmp ) ;

        p_tmp = p_lt_erase->p_heavy_predr ;
        if ( p_tmp->_get_node_light() != p_lt_erase )
            p_tmp = 0 ;

        while ( p_tmp )
        {
            p_tmp->_set_node_light  ( p_lt_after ) ;
            p_tmp = _move_to_parent ( p_tmp ) ;
        }
    }

    _NodeHeavyPtr       p_left  = 0 ;
    _NodeHeavyPtr       p_mid   = 0 ;
    _NodeHeavyPtr       p_right = 0 ;
    size_type           sub_size_tot = 0 ;

    p_level_cur = _bottom_end() ;
    _find_two_groups ( p_level_cur , p_parent , p_left , p_mid , p_right ) ;

    if ( p_mid && ( p_left->m_subsz + p_mid->m_subsz <= _max_degree() ) )
    {
        _link_to_parent_ext ( p_left ) ;
        _merge_node_with    ( p_left , sub_size_tot ) ;
    }
    else
    {
        _delete_node_light ( p_lt_erase ) ;
        _balance_external  ( p_left ) ;
        return it_res ;
    }

    size_type       n_ch_left   = 0 ;
    size_type       n_ch_right  = 0 ;
    _NodeHeavyPtr   p_par_break = 0 ;

    p_level_cur = _bottom_end() ;
    p_level_top = _top_end () ;
    while ( p_level_cur != p_level_top )
    {
        p_level_cur= p_level_cur->p_predr ;
        p_parent   = p_left     ->p_predr ;
        if ( !_find_two_groups ( p_level_cur , p_parent ,
                                 p_left , p_mid , p_right ) )
            break ;

        n_ch_left  = _count_level ( p_left->p_succr , p_mid  ->p_succr ) ;
        n_ch_right = _count_level ( p_mid ->p_succr , p_right->p_succr ) ;
        if ( ( n_ch_left + n_ch_right ) <= _max_degree() )
        {
            _link_to_parent  ( p_left ) ;
            _merge_node_with ( p_left , sub_size_tot ) ;
            continue ;
        }

        p_par_break = p_left ;
        break ;
    }

    _update_data ( p_par_break , sub_size_tot ) ;
    _balance ( difference_type(n_ch_left)  ,
               difference_type(n_ch_right) ,
               p_par_break                 ) ;

    if ( _size_second_top_is_less ( _min_degree() ) )
        _erase_top_level ( ) ;

    _delete_node_light ( p_lt_erase ) ;
    return it_res ;
}


TEMPL_DECL
void BP_TREE_TY::_erase_heavy_node ( _NodeHeavyPtr &  p_posn )
{
    p_posn->p_prev->p_next = p_posn->p_next ;
    p_posn->p_next->p_prev = p_posn->p_prev ;
    if ( p_posn->p_succr )  p_posn->p_succr->p_predr = 0 ;
    if ( p_posn->p_predr )  p_posn->p_predr->p_succr = 0 ;
    _delete_node_heavy ( p_posn ) ;
}


TEMPL_DECL
void BP_TREE_TY::_erase_top_level ( )
{
    _NodeHeavyPtr   p_tl_end = m_p_head_heavy->p_succr ;
    _NodeHeavyPtr   p_cur    = p_tl_end->p_next ;
    _NodeHeavyPtr   p_temp   = 0 ;

    while ( p_cur != p_tl_end )
    {
        p_temp = p_cur->p_next ;
        _erase_heavy_node ( p_cur ) ;
        p_cur = p_temp ;
    }

    p_tl_end->p_predr->p_succr = p_tl_end->p_succr ;
    p_tl_end->p_succr->p_predr = p_tl_end->p_predr ;
    _delete_node_heavy ( p_tl_end ) ;

    p_tl_end = m_p_head_heavy->p_succr ;
    p_cur    = p_tl_end->p_next ;
    while ( p_cur != p_tl_end )
    {
        p_cur->p_predr = 0 ;
        p_cur = p_cur->p_next ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_merge_node_with
    (
        _NodeHeavyPtr    p_left      ,
        size_type &      subsz_accum
    )
{
    _NodeHeavyPtr   p_mid   = p_left->p_next ;
    _NodeHeavyPtr   p_right = p_mid->p_next  ;
    size_type       sz_mid  = p_mid->m_subsz ;

    if ( p_mid->p_predr && p_mid->p_predr->p_succr==p_mid )
    {
        _NodeHeavyPtr   p_mid_par  = p_mid->p_predr ;
        _NodeHeavyPtr   p_left_par = p_mid_par->p_prev ;

        p_mid_par->p_succr = p_right ;
        subsz_accum += sz_mid ;

        p_mid_par->m_subsz  -= subsz_accum ;
        p_left_par->m_subsz += subsz_accum ;
    }
    else
    {
        subsz_accum = 0 ;
    }

    p_left->m_subsz += sz_mid ;
    p_mid ->p_succr  = 0 ;
    p_mid ->p_predr  = 0 ;
    _erase_heavy_node ( p_mid ) ;
}


TEMPL_DECL
void BP_TREE_TY::_balance ( const difference_type  n_ch_left  ,
                            const difference_type  n_ch_right ,
                            const _NodeHeavyPtr    p_parent   )
{
    if ( 0 == p_parent )
        return ;

    const _NodeHeavyPtr  p_mid  = p_parent->p_next ;
    difference_type      n_move = ( n_ch_right - n_ch_left ) / 2 ;
    if ( 0 == n_move )
        return ;

    _NodeHeavyPtr   p_mid_new  = p_mid->p_succr ;
    size_type       move_subsz = _move_subsize ( p_mid_new , n_move ) ;

    _NodeHeavyPtr   p_tmp = p_mid ;
    do
    {
        p_tmp->_set_node_light ( p_mid_new->_get_node_light ( ) ) ;

        if ( n_move >= 0 )
        {
            p_tmp->m_subsz          -= move_subsz ;
            p_tmp->p_prev->m_subsz  += move_subsz ;
        }
        else
        {
            p_tmp->m_subsz          += move_subsz ;
            p_tmp->p_prev->m_subsz  -= move_subsz ;
        }
        p_tmp = _move_to_parent ( p_tmp ) ;
    }
    while ( p_tmp != 0 ) ;

    difference_type n = n_move ;
    _NodeHeavyPtr   p_upd_ch     = (n>0) ? p_mid->p_succr : p_mid_new      ;
    _NodeHeavyPtr   p_upd_ch_end = (n>0) ? p_mid_new      : p_mid->p_succr ;
    _NodeHeavyPtr   p_upd_parent = (n>0) ? p_mid->p_prev  : p_mid          ;
    while ( p_upd_ch != p_upd_ch_end )
    {
        p_upd_ch->p_predr = p_upd_parent ;
        p_upd_ch = p_upd_ch->p_next ;
    }

    p_mid->p_succr = p_mid_new ;
}


TEMPL_DECL
void BP_TREE_TY::_balance_external( const _NodeHeavyPtr  p_parent )
{
    if ( p_parent==0 )
        return ;

    const _NodeHeavyPtr     p_left = p_parent ;
    const _NodeHeavyPtr     p_mid  = p_left->p_next ;

    const difference_type   n_ch_left = difference_type ( p_left->m_subsz ) ;
    const difference_type   n_ch_right= difference_type ( p_mid ->m_subsz ) ;
    const difference_type   n_move    = ( n_ch_right - n_ch_left ) / 2 ;
    if ( 0 == n_move )
        return ;

    _NodeLightPtr           p_lt_mid_new = p_mid->_get_node_light ( ) ;
    _move_level_ext ( p_lt_mid_new , n_move ) ;

    difference_type n = n_move ;
    _NodeLightPtr   p_lt_mid     = p_mid->_get_node_light( ) ;
    _NodeLightPtr   p_upd_ch     = (n>0) ? p_lt_mid     : p_lt_mid_new ;
    _NodeLightPtr   p_upd_ch_end = (n>0) ? p_lt_mid_new : p_lt_mid     ;
    _NodeHeavyPtr   p_upd_parent = (n>0) ? p_mid->p_prev: p_mid        ;
    while ( p_upd_ch != p_upd_ch_end )
    {
        p_upd_ch->p_heavy_predr = p_upd_parent ;
        p_upd_ch = p_upd_ch->p_light_next ;
    }

    _NodeHeavyPtr   p_tmp = p_mid ;
    do
    {
        p_tmp->_set_node_light ( p_lt_mid_new ) ;

        if ( n_move >= 0 )
        {
            p_tmp->m_subsz         -= size_type ( n_move ) ;
            p_tmp->p_prev->m_subsz += size_type ( n_move ) ;
        }
        else
        {
            p_tmp->m_subsz         += size_type ( -n_move ) ;
            p_tmp->p_prev->m_subsz -= size_type ( -n_move ) ;
        }

        p_tmp = _move_to_parent ( p_tmp ) ;
    }
    while ( p_tmp != 0 ) ;
}


TEMPL_DECL
void BP_TREE_TY::_update_data
    (
        const _NodeHeavyPtr   p_par_start  ,
        const size_type       sub_size_tot
    )
{
    if ( ! p_par_start )
        return ;

    _NodeHeavyPtr   p_upd      = p_par_start->p_next ;
    _NodeLightPtr   p_lt_succr = p_upd->p_succr->_get_node_light( ) ;

    p_upd->_set_node_light  ( p_lt_succr ) ;
    p_upd = _move_to_parent ( p_upd ) ;
    while ( p_upd )
    {
        p_upd->_set_node_light ( p_lt_succr )  ;
        p_upd->m_subsz         -= sub_size_tot ;
        p_upd->p_prev->m_subsz += sub_size_tot ;
        p_upd = _move_to_parent ( p_upd ) ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_link_to_parent ( _NodeHeavyPtr  p_parent )
{
    _NodeHeavyPtr   p_mid   = p_parent->p_next ;
    _NodeHeavyPtr   p_right = p_mid   ->p_next ;
    _NodeHeavyPtr   p_cur   = p_mid  ->p_succr ;
    _NodeHeavyPtr   p_end   = p_right->p_succr ;

    while ( p_cur != p_end )
    {
        p_cur->p_predr = p_parent ;
        p_cur = p_cur->p_next ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_link_to_parent_ext ( _NodeHeavyPtr  p_parent )
{
    _NodeHeavyPtr   p_mid    = p_parent->p_next ;
    _NodeHeavyPtr   p_right  = p_mid   ->p_next ;
    _NodeLightPtr   p_lt_cur = p_mid  ->_get_node_light() ;
    _NodeLightPtr   p_lt_end = p_right->_get_node_light() ;

    while ( p_lt_cur != p_lt_end )
    {
        p_lt_cur->p_heavy_predr = p_parent ;
        p_lt_cur = p_lt_cur->p_light_next ;
    }
}


TEMPL_DECL
bool BP_TREE_TY::_find_two_groups
    (
        const _NodeHeavyPtr  p_level_end ,
        const _NodeHeavyPtr  p_input     ,
        _NodeHeavyPtr &      ptr_left    ,
        _NodeHeavyPtr &      ptr_mid     ,
        _NodeHeavyPtr &      ptr_right
    ) const
{
    ptr_left  = 0 ;
    ptr_mid   = 0 ;
    ptr_right = 0 ;

    if ( p_input == 0 )
        return false ;

    if      ( p_input->p_next != p_level_end )
    {
        ptr_left = p_input ;
        ptr_mid  = ptr_left->p_next ;
        ptr_right= ptr_mid ->p_next ;
    }
    else if ( p_input->p_prev != p_level_end )
    {
        ptr_mid  = p_input ;
        ptr_left = ptr_mid->p_prev ;
        ptr_right= ptr_mid->p_next ;
    }

    return ( ptr_left != 0 ) ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::_count_level ( _NodeHeavyPtr  p_level_end ) const
{
    return _count_level ( p_level_end->p_next , p_level_end ) ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::_count_level ( const _NodeHeavyPtr  p_from ,
                           const _NodeHeavyPtr  p_to   ) const
{
    size_type       ne    = 0 ;
    _NodeHeavyPtr   p_cur = p_from ;
    for (  ; p_cur != p_to ; p_cur = p_cur->p_next , ++ne ) { }
    return ne ;
}


TEMPL_DECL
void BP_TREE_TY::_move_level ( _NodeHeavyPtr &        ptr    ,
                               const difference_type  n_move )
{
    if ( n_move < 0 )
    {
        for ( difference_type i = n_move ; i < 0 ; ++i )
            ptr = ptr->p_prev ;
    }
    else
    {
        for ( difference_type i = 0 ; i < n_move ; ++i )
            ptr = ptr->p_next ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_move_level_ext ( _NodeLightPtr &        ptr    ,
                                   const difference_type  n_move )
{
    if ( n_move > 0 )
    {
        for ( difference_type  i = 0 ; i < n_move ; ++i )
            ptr = ptr->p_light_next ;
    }
    else
    {
        for ( difference_type  i = n_move ; i < 0 ; ++i )
            ptr = ptr->p_light_prev ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::_move_subsize ( _NodeHeavyPtr &        ptr    ,
                            const difference_type  n_move )
{
    size_type   tot_subsz = 0 ;

    if ( n_move < 0 )
    {
        for ( difference_type i = n_move ; i < 0 ; ++i )
        {
            ptr        = ptr->p_prev ;
            tot_subsz += ptr->m_subsz ;
        }
    }
    else
    {
        for ( difference_type i = 0 ; i < n_move ; ++i )
        {
            tot_subsz += ptr->m_subsz ;
            ptr        = ptr->p_next ;
        }
    }

    return tot_subsz ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::_tree_height ( ) const
{
    size_type       n_res = 0 ;
    _NodeHeavyPtr   p_end = m_p_head_heavy ;
    _NodeHeavyPtr   p_cur = p_end->p_succr ;
    while ( p_cur != p_end )
    {
        ++n_res ;
        p_cur = p_cur->p_succr ;
    }

    return n_res ;
}


TEMPL_DECL
bool BP_TREE_TY::_size_second_top_is_less ( const size_type  sz_lim ) const
{
    _NodeHeavyPtr   p_level_end = m_p_head_heavy->p_succr->p_succr ;
    if ( p_level_end == m_p_head_heavy )
        return false ;

    size_type       count = 0 ;
    _NodeHeavyPtr   p_cur = p_level_end->p_next ;
    while ( p_cur != p_level_end )
    {
        ++count ;
        if ( count >= sz_lim )
            break ;
        p_cur = p_cur->p_next ;
    }

    return ( count < sz_lim ) ;
}


TEMPL_DECL
typename BP_TREE_TY::_NodeHeavyPtr
BP_TREE_TY::_move_to_parent ( _NodeHeavyPtr  p_1st_child ) const
{
    _NodeHeavyPtr   p_parent = p_1st_child->p_predr ;
    if ( p_parent && p_parent->p_succr==p_1st_child )
        return p_parent ;
    else
        return 0 ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::_local_limit ( ) const
{
    return ( _tree_height( ) + 1 ) * _min_degree() ;
}


TEMPL_DECL
void BP_TREE_TY::_find_lower_bound ( const _Ty_Key &     key_x ,
                                     difference_type &   index ,
                                     _NodeLightPtr &     p_lt_posn ) const
{
    if ( m_p_head_light->p_light_next == m_p_head_light )
    {
        index     = 0 ;
        p_lt_posn = _external_end( ) ;
        return ;
    }

    _NodeLightPtr   p_lt_begin = _external_begin() ;
    _NodeLightPtr   p_lt_last  = _external_last () ;

    if      ( ! m_k_comp ( _KeyOfV( ) ( p_lt_begin->_elem() ) , key_x ) )
    {
        index     = 0 ;
        p_lt_posn = _external_begin( ) ;
    }
    else if (   m_k_comp ( _KeyOfV( ) ( p_lt_last->_elem() )  , key_x ) )
    {
        index     = _size_dt() ;
        p_lt_posn = _external_end( ) ;
    }
    else
    {
        _NodeHeavyPtr   p_h_cur = _top_end( )->p_prev ;
        index  = _size_dt() ;
        index -= difference_type (p_h_cur->m_subsz) ;

        while ( p_h_cur )
        {
            while ( ! m_k_comp ( _KeyOfV( ) ( p_h_cur->_elem() ) , key_x ) )
            {
                p_h_cur = p_h_cur->p_prev  ;
                index  -= difference_type (p_h_cur->m_subsz) ;
            }

            if ( p_h_cur->p_succr == 0 )
                break ;

            index  += difference_type (p_h_cur->m_subsz) ;
            p_h_cur = p_h_cur->p_next->p_succr->p_prev ;
            index  -= difference_type (p_h_cur->m_subsz) ;
        }

        index    += difference_type (p_h_cur->m_subsz) ;
        p_h_cur   = p_h_cur->p_next ;
        p_lt_posn = p_h_cur->_get_node_light() ;
        p_lt_posn = p_lt_posn->p_light_prev ;
        --index ;
        while ( ! m_k_comp ( _KeyOfV( ) ( p_lt_posn->_elem() ) , key_x ) )
        {
            p_lt_posn = p_lt_posn->p_light_prev ;
            --index ;
        }
        p_lt_posn = p_lt_posn->p_light_next ;
        ++index ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_find_upper_bound ( const _Ty_Key &    key_x ,
                                     difference_type &  index ,
                                     _NodeLightPtr &    p_lt_posn ) const
{
    if ( m_p_head_light->p_light_next == m_p_head_light )
    {
        index     = 0 ;
        p_lt_posn = _external_end( ) ;
        return ;
    }

    _NodeLightPtr   p_lt_beg  = _external_begin() ;
    _NodeLightPtr   p_lt_last = _external_last () ;

    if (    ! m_k_comp ( key_x , _KeyOfV( ) ( p_lt_last->_elem() ) ) )
    {
        index     = _size_dt() ;
        p_lt_posn = _external_end( ) ;
    }
    else if ( m_k_comp ( key_x , _KeyOfV( ) ( p_lt_beg->_elem()  ) ) )
    {
        index     = 0 ;
        p_lt_posn = _external_begin( ) ;
    }
    else
    {
        _NodeHeavyPtr   p_h_cur = _top_end( )->p_prev ;
        index  = _size_dt() ;
        index -= difference_type (p_h_cur->m_subsz) ;

        while ( p_h_cur )
        {
            while ( m_k_comp ( key_x , _KeyOfV( ) ( p_h_cur->_elem() ) ) )
            {
                p_h_cur = p_h_cur->p_prev  ;
                index  -= difference_type (p_h_cur->m_subsz) ;
            }

            if ( p_h_cur->p_succr == 0 )
                break ;

            index  += difference_type (p_h_cur->m_subsz) ;
            p_h_cur = p_h_cur->p_next->p_succr->p_prev ;
            index  -= difference_type (p_h_cur->m_subsz) ;
        }

        index    += difference_type (p_h_cur->m_subsz) ;
        p_h_cur   = p_h_cur->p_next ;
        p_lt_posn = p_h_cur->_get_node_light() ;
        p_lt_posn = p_lt_posn->p_light_prev ;
        --index ;
        while ( m_k_comp ( key_x , _KeyOfV( ) ( p_lt_posn->_elem() ) ) )
        {
            --index ;
            p_lt_posn = p_lt_posn->p_light_prev ;
        }
        ++index ;
        p_lt_posn = p_lt_posn->p_light_next ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::_NodeLightPtr
BP_TREE_TY::_find_node_light ( const size_type  idx_pos ) const
{
    if ( idx_pos >= size ( ) )
        return _external_end ( ) ;
    if ( idx_pos == 0 )
        return _external_begin ( ) ;

    _NodeHeavyPtr   p_cur = _top_begin ( ) ;
    size_type       n_tot = 0 ;

    do
    {
        while ( n_tot < idx_pos )
        {
            n_tot += p_cur->m_subsz ;
            p_cur  = p_cur->p_next  ;
        }

        if ( n_tot > idx_pos )
        {
            p_cur  = p_cur->p_prev  ;
            n_tot -= p_cur->m_subsz ;
        }

        if ( p_cur->p_succr )
            p_cur = p_cur->p_succr ;
        else
            break ;
    }
    while ( p_cur ) ;

    _NodeLightPtr   p_lt_res = p_cur->_get_node_light( ) ;
    while ( n_tot < idx_pos )
    {
        p_lt_res = p_lt_res->p_light_next ;
        ++n_tot ;
    }

    return p_lt_res ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::_find_node_index ( _NodeLightPtr  p_node ) const
{
    if ( p_node == _external_end( ) )
        return size( ) ;
    if ( p_node == _external_begin( ) )
        return 0 ;

    size_type       subsz    = 0 ;
    _NodeLightPtr   p_lt_cur = p_node ;
    while ( p_lt_cur->p_heavy_predr->_get_node_light() != p_lt_cur )
    {
        p_lt_cur = p_lt_cur->p_light_prev ;
        ++subsz ;
    }

    _NodeHeavyPtr   p_heavy  = p_lt_cur->p_heavy_predr ;
    _NodeHeavyPtr   p_level  = _bottom_end( ) ;
    _NodeHeavyPtr   p_lev_end= _top_end( ) ;
    while ( p_level != p_lev_end )
    {
        while ( p_heavy->p_predr->p_succr != p_heavy )
        {
            p_heavy = p_heavy->p_prev  ;
            subsz  += p_heavy->m_subsz ;
        }

        p_heavy = p_heavy->p_predr ;
        p_level = p_level->p_predr ;
    }

    _NodeHeavyPtr   p_h_beg = _top_begin ( ) ;
    while ( p_heavy != p_h_beg )
    {
        p_heavy = p_heavy->p_prev  ;
        subsz  += p_heavy->m_subsz ;
    }

    return subsz ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::_insert_seqce ( iterator  pos , const_reference  val )
{
    difference_type idx       = pos._index ( ) ;
    _NodeLightPtr   p_lt_posn = pos._pointer() ;
    iterator        res = _insert_seqce_posn ( idx , p_lt_posn , val ) ;
    return res ;
}


TEMPL_DECL
void BP_TREE_TY::_insert_seqce_count ( iterator         pos ,
                                       size_type        cnt ,
                                       const_reference  val )
{
    while ( cnt-- )
    {
        pos = _insert_seqce ( pos , val ) ;
        ++pos ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::_splice_impl ( iterator    pos   ,
                           _Ty_This &  that  ,
                           iterator    pos_a ,
                           iterator    pos_b )
{
    iterator            pos_res ;
    difference_type     cnt = pos_b - pos_a ;
    if ( this == &that )
    {
        if ( pos < pos_a )
        {
            while ( cnt-- )
            {
                pos = this->_insert_seqce ( pos , *pos_a ) ;
                ++pos ;
                pos_a._set_index ( pos_a._index() + 1 ) ;
                pos_a = that . erase ( pos_a ) ;
            }
        }
        else if ( pos >= pos_b )
        {
            while ( cnt-- )
            {
                pos = this->_insert_seqce ( pos , *pos_a ) ;
                pos_a = that . erase ( pos_a ) ;
            }
        }

        pos_res = pos ;
    }
    else
    {
        while ( cnt-- )
        {
            pos = this->_insert_seqce ( pos , *pos_a ) ;
            ++pos ;
            pos_a = that . erase ( pos_a ) ;
        }

        pos_res = pos ;
    }

    return pos_res ;
}


TEMPL_DECL
std::pair<typename BP_TREE_TY::iterator, bool>
BP_TREE_TY::insert ( const value_type &  elem_x )
{
    std::pair<iterator, bool>
                    ib_res = _insert_ordered ( elem_x ) ;
    return ib_res ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::insert ( iterator  , const value_type &  elem_x )
{
    std::pair<iterator, bool>
                    ib_res = _insert_ordered ( elem_x ) ;
    return ib_res . first ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::erase ( iterator  pos )
{
    iterator    it_res = _erase_bp_tree ( pos ) ;
    return it_res ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::erase ( iterator  pos_a , iterator  pos_b )
{
    difference_type     cnt = pos_b - pos_a ;
    while ( cnt-- )
    {
        pos_a = erase ( pos_a ) ;
    }
    return pos_a ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::erase ( const _Ty_Key &  key_x )
{
    std::pair<iterator, iterator>
                eq_r = equal_range ( key_x ) ;
    size_type   cnt  = size_type ( eq_r.second - eq_r.first ) ;
    erase ( eq_r.first , eq_r.second ) ;
    return cnt ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::find ( const _Ty_Key &  key_x )
{
    iterator        pos = lower_bound ( key_x ) ;
    if ( pos == end( ) || m_k_comp ( key_x  , _KeyOfV( ) (*pos) ) )
        return end( ) ;
    return pos ;
}


TEMPL_DECL
typename BP_TREE_TY::const_iterator
BP_TREE_TY::find ( const _Ty_Key &  key_x ) const
{
    const_iterator  pos = lower_bound ( key_x ) ;
    if ( pos == end( ) || m_k_comp ( key_x  , _KeyOfV( ) (*pos) ) )
        return end( ) ;
    return pos ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::lower_bound ( const _Ty_Key &  key_x )
{
    difference_type     i_low     = 0 ;
    _NodeLightPtr       p_lt_posn = 0 ;
    _find_lower_bound  ( key_x , i_low , p_lt_posn ) ;
    return iterator ( p_lt_posn , i_low , this ) ;
}


TEMPL_DECL
typename BP_TREE_TY::const_iterator
BP_TREE_TY::lower_bound ( const _Ty_Key &  key_x ) const
{
    difference_type     i_low     = 0 ;
    _NodeLightPtr       p_lt_posn = 0 ;
    _find_lower_bound ( key_x , i_low , p_lt_posn ) ;
    return  const_iterator ( p_lt_posn , i_low , this ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::upper_bound ( const _Ty_Key &  key_x )
{
    difference_type     i_upp     = 0 ;
    _NodeLightPtr       p_lt_posn = 0 ;
    _find_upper_bound ( key_x , i_upp , p_lt_posn ) ;
    return  iterator ( p_lt_posn , i_upp , this ) ;
}


TEMPL_DECL
typename BP_TREE_TY::const_iterator
BP_TREE_TY::upper_bound ( const _Ty_Key &  key_x ) const
{
    difference_type     i_upp     = 0 ;
    _NodeLightPtr       p_lt_posn = 0 ;
    _find_upper_bound ( key_x , i_upp , p_lt_posn ) ;
    return  const_iterator ( p_lt_posn , i_upp , this ) ;
}


TEMPL_DECL
std::pair < typename BP_TREE_TY::iterator ,
            typename BP_TREE_TY::iterator >
BP_TREE_TY::equal_range ( const _Ty_Key &  key_x )
{
    return std::pair<iterator, iterator> ( lower_bound ( key_x ) ,
                                           upper_bound ( key_x ) ) ;
}


TEMPL_DECL
std::pair < typename BP_TREE_TY::const_iterator ,
            typename BP_TREE_TY::const_iterator >
BP_TREE_TY::equal_range ( const _Ty_Key &  key_x ) const
{
    return std::pair<const_iterator, const_iterator>( lower_bound ( key_x ) ,
                                                      upper_bound ( key_x ) ) ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::count ( const _Ty_Key &  key_x ) const
{
    std::pair<const_iterator, const_iterator>
                eq_r = equal_range ( key_x ) ;
    return size_type ( eq_r . second - eq_r . first ) ;
}


TEMPL_DECL
void BP_TREE_TY::swap ( BP_TREE_TY &  ctr_x )
{
    std::swap ( m_p_head_light , ctr_x . m_p_head_light ) ;
    std::swap ( m_size_light   , ctr_x . m_size_light   ) ;
    std::swap ( m_p_head_heavy , ctr_x . m_p_head_heavy ) ;
    std::swap ( m_k_comp       , ctr_x . m_k_comp       ) ;
    std::swap ( m_multi        , ctr_x . m_multi        ) ;
    std::swap ( m_ordered      , ctr_x . m_ordered      ) ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::capacity ( ) const
{
    return max_size ( ) ;
}


TEMPL_DECL
void BP_TREE_TY::reserve ( size_type  sz )
{
    sz = sz ;
}


TEMPL_DECL
typename BP_TREE_TY::const_reference
BP_TREE_TY::at ( size_type  ind ) const
{
    if ( ind >= size() )
        throw std::out_of_range("sequence: index out of range") ;

    _NodeLightPtr   p_elem = _find_node_light ( ind ) ;
    const_reference cref   = p_elem->_elem ( ) ;
    return cref ;
}


TEMPL_DECL
typename BP_TREE_TY::reference
BP_TREE_TY::at ( size_type  ind )
{
    if ( ind >= size() )
        throw std::out_of_range("sequence: index out of range") ;

    _NodeLightPtr   p_elem = _find_node_light ( ind ) ;
    reference       ref    = p_elem->_elem ( ) ;
    return ref ;
}


TEMPL_DECL
typename BP_TREE_TY::const_reference
BP_TREE_TY::operator[] ( size_type  ind ) const
{
    _NodeLightPtr   p_elem = _find_node_light ( ind ) ;
    const_reference cref   = p_elem->_elem ( ) ;
    return cref ;
}


TEMPL_DECL
typename BP_TREE_TY::reference
BP_TREE_TY::operator[] ( size_type  ind )
{
    _NodeLightPtr   p_elem = _find_node_light ( ind ) ;
    reference       ref    = p_elem->_elem ( ) ;
    return ref ;
}


TEMPL_DECL
typename BP_TREE_TY::reference
BP_TREE_TY::front ( )
{
    reference       ref = _external_begin()->_elem ( ) ;
    return ref ;
}


TEMPL_DECL
typename BP_TREE_TY::const_reference
BP_TREE_TY::front ( ) const
{
    const_reference cref = _external_begin()->_elem ( ) ;
    return cref ;
}


TEMPL_DECL
typename BP_TREE_TY::reference
BP_TREE_TY::back ( )
{
    reference       ref = _external_last()->_elem ( ) ;
    return ref ;
}


TEMPL_DECL
typename BP_TREE_TY::const_reference
BP_TREE_TY::back ( ) const
{
    const_reference cref = _external_last()->_elem ( ) ;
    return cref ;
}


TEMPL_DECL
void BP_TREE_TY::push_back ( const_reference  val )
{
    difference_type     idx       = _size_dt() ;
    _NodeLightPtr       p_lt_posn = _external_end ( ) ;
    _insert_seqce_posn ( idx , p_lt_posn , val ) ;
}


TEMPL_DECL
void BP_TREE_TY::push_front ( const_reference  val )
{
    difference_type     idx       = 0 ;
    _NodeLightPtr       p_lt_posn = _external_begin ( ) ;
    _insert_seqce_posn ( idx , p_lt_posn , val ) ;
}


TEMPL_DECL
void BP_TREE_TY::pop_front ( )
{
    if ( ! empty ( ) )
        erase ( begin ( ) ) ;
}


TEMPL_DECL
void BP_TREE_TY::pop_back ( )
{
    if ( ! empty ( ) )
        erase ( --end ( ) ) ;
}


TEMPL_DECL
void BP_TREE_TY::remove ( const_reference  val )
{
    iterator    it_curr = begin ( ) ;
    size_type   cnt = size ( ) ;

    while ( cnt-- )
    {
        if ( *it_curr == val )
            it_curr = erase ( it_curr ) ;
        else
            ++it_curr ;
    }
}


TEMPL_DECL
void BP_TREE_TY::unique ( )
{
    size_type   cnt = size ( ) ;
    if ( cnt < 2 )
        return ;

    iterator    it_curr = begin ( ) ;
    iterator    it_next ( it_curr ) ;

    --cnt ;
    ++it_next ;
    while ( cnt-- )
    {
        if ( *it_curr == *it_next )
        {
            it_next = erase ( it_next ) ;
        }
        else
        {
            it_curr = it_next ;
            ++it_next ;
        }
    }
}


TEMPL_DECL
void BP_TREE_TY::resize ( size_type  sz_new )
{
    resize ( sz_new , value_type() ) ;
}


TEMPL_DECL
void BP_TREE_TY::resize ( size_type  sz_new , const_reference  val )
{
    if ( size() < sz_new )
    {
        _insert_seqce_count ( end() , sz_new-size() , val ) ;
    }
    else if ( sz_new < size() )
    {
        erase ( begin() + difference_type(sz_new) , end() ) ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::splice ( iterator  pos , _Ty_This &  that )
{
    return _splice_impl ( pos , that , that.begin() , that.end() ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::splice ( iterator  pos , _Ty_This &  that , iterator  pos_a )
{
    iterator    pos_b ( pos_a ) ;
    ++pos_b ;
    return _splice_impl ( pos , that , pos_a , pos_b ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::splice ( iterator    pos   ,
                     _Ty_This &  that  ,
                     iterator    pos_a ,
                     iterator    pos_b )
{
    return _splice_impl ( pos , that , pos_a , pos_b ) ;
}


TEMPL_DECL
void BP_TREE_TY::reverse ( )
{
    const size_type     sz_min = 2 ;
    if ( size() < sz_min )
        return ;

    _NodeLightPtr   p_forw = _external_begin() ;
    _NodeLightPtr   p_back = _external_last () ;

    do
    {
        reference       val_f = p_forw->_elem( ) ;
        reference       val_b = p_back->_elem( ) ;
        std::swap ( val_f , val_b ) ;
        p_forw = p_forw->p_light_next ;
        if ( p_forw == p_back )
            break ;
        p_back = p_back->p_light_prev ;
    }
    while ( p_forw != p_back ) ;
}


TEMPL_DECL
void BP_TREE_TY::sort ( )
{
    _Ty_This    contr_ord(_Pred(), this->m_multi, true, this->get_allocator());

    while ( ! this->empty() )
    {
        contr_ord.insert ( *(this->begin()) ) ;
        this->erase      (   this->begin()  ) ;
    }

    std::swap ( m_p_head_light , contr_ord . m_p_head_light ) ;
    std::swap ( m_p_head_heavy , contr_ord . m_p_head_heavy ) ;
    std::swap ( m_size_light   , contr_ord . m_size_light   ) ;
}


TEMPL_DECL
typename BP_TREE_TY::mapped_type
BP_TREE_TY::accumulate ( const_iterator  it_start ,
                         const_iterator  it_end   ,
                         mapped_type     val_in   ) const
{
    if ( ( size_type( it_start._index() ) > size() ) ||
         ( size_type( it_end  ._index() ) > size() ) || ( it_end < it_start) )
        throw std::range_error("accumulate: range error") ;

    _Ty_Map         sum_res    = val_in ;
    size_type       dist_limit = size_type ( it_end - it_start ) ;
    size_type       dist_cur   = 0 ;
    _NodeLightPtr   p_lt_cur   = it_start._pointer() ;

    while ( dist_cur < dist_limit )
    {
        ++dist_cur ;
        sum_res += _MapOfV()( p_lt_cur->_elem() ) ;
        p_lt_cur = p_lt_cur->p_light_next ;
    }

    return sum_res ;
}


TEMPL_DECL
void BP_TREE_TY::write_shallow ( iterator             pos     ,
                                 const mapped_type &  val_new )
{
    _Ty_Map &       ref_map = (_Ty_Map&)(_MapOfV()(*pos)) ;
    ref_map = val_new ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::write_deep ( iterator         pos     ,
                         const _Ty_Val &  val_new )
{
    iterator    res ;
    res = erase  ( pos ) ;
    res = insert ( res , val_new ) ;
    return res ;
}


#undef TEMPL_DECL
#undef BP_TREE_TY

_BPT_CLOSE

#endif  //  _BP_TREE_IDX_HPP

