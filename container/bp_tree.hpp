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

#ifndef _BP_TREE_HPP
#define _BP_TREE_HPP

#include <iterator>
#include <utility>
#include <memory>
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
class bp_tree
{
public:
    typedef bp_tree<_Ty_Key, _Ty_Map, _Ty_Val, 
                    _KeyOfV, _MapOfV, _Pred  , _Alloc>
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
    friend class bp_tree<_Ty_Key, _Ty_Map, _Ty_Val,
                         _KeyOfV, _MapOfV, _Pred  , _Alloc> ;

    protected:
        _iter_base ( ) : m_ptr(0), m_p_cont(0) { }
        _iter_base ( _NodeLightPtr  pln, _Ty_This const *  ctr ) :
                         m_ptr(pln), m_p_cont( (_Ty_This*)ctr ) {  }
        _iter_base ( _NodeLightPtr  pln, _Ty_This *  ctr ) :
                         m_ptr(pln), m_p_cont( ctr ) {  }

    public:
        bool operator == ( const _iter_base & it_x ) const
                         { return m_ptr == it_x.m_ptr ; }
        bool operator != ( const _iter_base & it_x ) const
                         { return !(*this==it_x) ; }

        bool valid_data ( ) const     { return m_p_cont!=0 && m_ptr!=0 ; }

    protected:
        _NodeLightPtr       _pointer ( )   const    { return m_ptr     ; }
        const _Ty_This *    _container ( ) const    { return m_p_cont  ; }

        void _increment ( ) { m_ptr = m_ptr->p_light_next ; }
        void _decrement ( ) { m_ptr = m_ptr->p_light_prev ; }

        _NodeLightPtr       m_ptr    ;
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
        _iter_base_stl ( _NodeLightPtr pln , _Ty_This const * ctr ) :
                            _iter_base ( pln , ctr ) { }
        _iter_base_stl ( _NodeLightPtr pln , _Ty_This * ctr ) :
                            _iter_base ( pln , ctr ) { }
    } ;


    class iterator ;
    friend
    class iterator ;
    typedef _iter_base_stl< std::bidirectional_iterator_tag, value_type,
                            difference_type, pointer, reference >
                            _iter_base_stl_alias ;


    class iterator : public _iter_base_stl_alias
    {
    friend class bp_tree<_Ty_Key, _Ty_Map, _Ty_Val,
                         _KeyOfV, _MapOfV, _Pred  , _Alloc> ;

        iterator ( _NodeLightPtr pln, _Ty_This * ctr ) :
            _iter_base_stl_alias ( pln , ctr ) { }

    public:
        iterator ( ) : _iter_base_stl_alias ( ) { }

        reference   operator*  ( ) const
                    { return _iter_base_stl_alias::m_ptr->_elem() ; }
        pointer     operator-> ( ) const
                    { return (&**this) ; }

        iterator &  operator++ ( )
                    { _iter_base_stl_alias::_increment() ; return (*this) ; }
        iterator &  operator-- ( )
                    { _iter_base_stl_alias::_decrement() ; return (*this) ; }
        iterator    operator++ ( int )
                    { iterator tmp = *this ; ++(*this) ; return tmp ; }
        iterator    operator-- ( int )
                    { iterator tmp = *this ; --(*this) ; return tmp ; }
    } ;


    class const_iterator ;
    friend
    class const_iterator ;


    class const_iterator : public _iter_base_stl_alias
    {
    friend class bp_tree<_Ty_Key, _Ty_Map, _Ty_Val,
                         _KeyOfV, _MapOfV, _Pred  , _Alloc> ;

        const_iterator ( _NodeLightPtr pln , _Ty_This const * ctr ) :
            _iter_base_stl_alias( pln, ctr ) { }

    public:
        const_iterator ( ) : _iter_base_stl_alias ( ) { }
        const_iterator ( const iterator &  iter ) :
            _iter_base_stl_alias( iter._iter_base_stl_alias::m_ptr    ,
                                  iter._iter_base_stl_alias::m_p_cont ) {  }

        const_reference     operator*  ( ) const
                            { return _iter_base_stl_alias::m_ptr->_elem() ; }
        const_pointer       operator-> ( ) const
                            { return ( &**this ) ; }

        const_iterator &    operator ++ ( )
                            { _iter_base_stl_alias::_increment() ;
                              return (*this) ; }
        const_iterator &    operator -- ( )
                            { _iter_base_stl_alias::_decrement() ;
                              return (*this) ; }
        const_iterator      operator ++ ( int )
                            { const_iterator tmp = *this ; ++(*this) ;
                              return tmp ; }
        const_iterator      operator -- ( int )
                            { const_iterator tmp = *this ; --(*this) ;
                              return tmp ; }
    } ;


    bp_tree    ( const _Pred &     pred ,
                 bool              mul  ,
                 bool              ord  ,
                 const _Alloc &    alr  ) ;
    bp_tree    ( const _Ty_This &  that ) ;
    _Ty_This &
    operator = ( const _Ty_This &  that ) ;
    ~bp_tree ( ) ;

    iterator        begin ( )
                    { return iterator      (_external_begin(), this ) ; }
    const_iterator  begin ( ) const
                    { return const_iterator(_external_begin(), this ) ; }
    iterator        end   ( )
                    { return iterator      (_external_end(), this); }
    const_iterator  end   ( ) const
                    { return const_iterator(_external_end(), this); }

    size_type   size ( )          const { return m_size_light ; }
    size_type   max_size ( )      const { return m_allr_ty_val.max_size() ; }
    bool        empty ( )         const { return ( size() == 0 ) ; }
    _Alloc      get_allocator ( ) const { return m_allr_ty_val ; }
    _Pred       key_comp ( )      const { return m_k_comp  ; }

    std::pair<iterator, bool>
                insert ( const value_type &  elem_x ) ;
    iterator    insert ( iterator  pos , const value_type &  elem_x ) ;
    template <class _InpIter>
    void        insert ( _InpIter  pos_a , _InpIter  pos_b )
    {
        while ( pos_a != pos_b )  { insert (*pos_a) ; ++pos_a ; }
    }

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
    void     _merge_node_with  ( _NodeHeavyPtr         p_left    ) ;
    void     _balance          ( const difference_type n_ch_left ,
                                 const difference_type n_ch_right,
                                 const _NodeHeavyPtr   p_mid     ) ;
    void     _balance_external ( const _NodeHeavyPtr   p_parent  ) ;
    void     _update_data      ( const _NodeHeavyPtr   p_parent  ) ;
    void     _link_to_parent_ext
                               ( _NodeHeavyPtr         p_parent  ) ;
    void     _link_to_parent   ( _NodeHeavyPtr         p_parent  ) ;
    bool     _find_two_groups  ( const _NodeHeavyPtr   ptr_level ,
                                 const _NodeHeavyPtr   ptr_input ,
                                _NodeHeavyPtr &        ptr_left  ,
                                _NodeHeavyPtr &        ptr_mid   ,
                                _NodeHeavyPtr &        ptr_right ) const ;

    size_type   _count_level  ( _NodeHeavyPtr          p_l_end ) const ;
    size_type   _count_level  ( const _NodeHeavyPtr    p_from  ,
                                const _NodeHeavyPtr    p_to    ) const ;
    void        _move_level   ( _NodeHeavyPtr &        ptr     ,
                                const difference_type  n_move  ) ;
    void        _move_level_ext
                              ( _NodeLightPtr &        ptr     ,
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
                                      _NodeLightPtr &   p_lt_pos) const ;
    void            _find_upper_bound(const _Ty_Key &   key_x   ,
                                      _NodeLightPtr &   p_lt_pos) const ;

    size_type       _tree_height ( ) const ;
    bool            _size_second_top_is_less
                                     ( const size_type  sz_lim  ) const ;
    _NodeHeavyPtr   _move_to_parent  ( _NodeHeavyPtr    p_1st_ch) const ;


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
#define BP_TREE_TY  bp_tree < _Ty_Key , _Ty_Map , _Ty_Val , \
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
BP_TREE_TY::bp_tree ( const _Pred &   pred ,
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
BP_TREE_TY::bp_tree ( const BP_TREE_TY &  that ) :
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
BP_TREE_TY::~bp_tree ( )
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
    for ( ; cur != end ; ++cur )
        insert (*cur) ;
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

    size_type       count = 0 ;
    _NodeHeavyPtr   p_tmp = p_pos ;
    while ( p_tmp->p_predr == p_parent )
    {
        ++count ;
        p_tmp->p_predr = p_par_new      ;
        p_tmp          = p_tmp->p_next  ;
    }

    p_par_new->m_subsz  = count ;
    p_parent ->m_subsz -= count ;

    if ( p_parent->p_predr )
    {
        ++( p_parent->p_predr->m_subsz ) ;
    }
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

    size_type       count    = 0 ;
    _NodeLightPtr   p_lt_tmp = p_lt_pos ;
    while ( p_lt_tmp->p_heavy_predr == p_parent )
    {
        ++count ;
        p_lt_tmp->p_heavy_predr = p_par_new ;
        p_lt_tmp = p_lt_tmp->p_light_next  ;
    }

    p_par_new->m_subsz  = count ;
    p_parent ->m_subsz -= count ;

    if ( p_parent->p_predr )
    {
        ++( p_parent->p_predr->m_subsz ) ;
    }
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

    size_type       count = 0 ;
    while ( p_top_beg != p_top_end )
    {
        p_top_beg->p_predr = p_new_top_beg ;
        p_top_beg = p_top_beg->p_next ;
        ++count ;
    }

    p_new_top_beg->m_subsz = count ;
}


TEMPL_DECL
std::pair<typename BP_TREE_TY::iterator, bool>
BP_TREE_TY::_insert_ordered ( const value_type &  elem_x )
{
    _NodeLightPtr       p_lt_new  = 0 ;
    _NodeLightPtr       p_lt_posn = 0 ;

    if ( m_multi )
    {
        _find_upper_bound ( _KeyOfV() ( elem_x ) , p_lt_posn ) ;
    }
    else
    {
        _find_lower_bound ( _KeyOfV() ( elem_x ) , p_lt_posn ) ;
        bool  contains = ( ( p_lt_posn!=m_p_head_light ) &&
              !m_k_comp( _KeyOfV()(elem_x), _KeyOfV()( p_lt_posn->_elem())) ) ;
        if ( contains )
            return std::pair<iterator, bool>
                   ( iterator( p_lt_posn, this) , false ) ;
    }

    p_lt_new = _insert_node_light ( p_lt_posn , elem_x ) ;
    _insert_b_tree ( p_lt_new ) ;

    iterator    it_ins ( p_lt_new , this ) ;
    return std::pair<iterator, bool> ( it_ins , true ) ;
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
        _NodeLightPtr   p_lt_next = p_lt_new->p_light_next ;
        p_parent = p_lt_next->p_heavy_predr ;
        p_lt_new->p_heavy_predr = p_parent ;
        ++( p_parent->m_subsz ) ;

        _NodeHeavyPtr   p_beg = p_parent ;
        while ( p_beg )
        {
            p_beg->_set_node_light ( p_lt_new ) ;
            p_beg = p_beg->p_predr ;
        }
    }
    else
    {
        _NodeLightPtr   p_lt_prev = p_lt_new->p_light_prev ;
        p_parent = p_lt_prev->p_heavy_predr ;
        p_lt_new->p_heavy_predr = p_parent ;
        ++( p_parent->m_subsz ) ;
    }

    if ( p_parent->m_subsz <= _max_degree() )
        return ;

    _NodeLightPtr    p_lt_mid = p_parent->_get_node_light( ) ;
    difference_type  n_move   = difference_type ( _min_degree() ) ;
    _move_level_ext ( p_lt_mid , n_move ) ;
    _split_node_external ( p_parent , p_lt_mid ) ;

    size_type       n_children  = 0 ;
    _NodeHeavyPtr   p_parent_up = p_parent->p_predr ;
    while ( p_parent_up != 0 )
    {
        n_children  = p_parent_up->m_subsz ;
        p_parent    = p_parent_up ;
        p_parent_up = p_parent->p_predr ;

        if ( n_children <= _max_degree() )
            break ;

        _NodeHeavyPtr   p_mid  = p_parent->p_succr ;
        difference_type n_move = difference_type ( _min_degree() ) ;
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

    --( p_parent->m_subsz ) ;

    if ( p_lt_erase == m_p_head_light->p_light_next )
    {
        _NodeHeavyPtr   p_beg = p_parent ;
        while ( p_beg )
        {
            p_beg->_set_node_light ( p_lt_after ) ;
            p_beg = p_beg->p_predr ;
        }
    }
    else
    {
        _NodeHeavyPtr   p_tmp = p_parent ;
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

    p_level_cur = _bottom_end() ;
    _find_two_groups ( p_level_cur , p_parent , p_left , p_mid , p_right ) ;

    if ( p_mid && ( p_left->m_subsz + p_mid->m_subsz <= _max_degree() ) )
    {
        _link_to_parent_ext ( p_left ) ;
        _merge_node_with    ( p_left ) ;
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

        n_ch_left  = p_left->m_subsz ;
        n_ch_right = p_mid ->m_subsz ;
        if ( ( n_ch_left + n_ch_right ) <= _max_degree() )
        {
            _link_to_parent  ( p_left ) ;
            _merge_node_with ( p_left ) ;
            continue ;
        }

        p_par_break = p_left ;
        break ;
    }

    _update_data ( p_par_break ) ;
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
void BP_TREE_TY::_merge_node_with ( _NodeHeavyPtr    p_left )
{
    _NodeHeavyPtr   p_mid   = p_left->p_next ;
    _NodeHeavyPtr   p_right = p_mid->p_next  ;
    size_type       cnt_mid = p_mid->m_subsz ;

    if ( p_mid->p_predr && p_mid->p_predr->p_succr==p_mid )
    {
        _NodeHeavyPtr   p_mid_par = p_mid->p_predr ;

        p_mid_par->p_succr = p_right ;
        --( p_mid_par->m_subsz ) ;
    }
    else
    {
        if ( p_left->p_predr != 0 )
        {
            --( p_left->p_predr->m_subsz) ;
        }
    }

    p_left->m_subsz += cnt_mid ;
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

    _NodeHeavyPtr   p_mid_new   = p_mid->p_succr ;
    _move_level ( p_mid_new , n_move ) ;

    _NodeHeavyPtr   p_tmp = p_mid ;

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

    do
    {
        p_tmp->_set_node_light ( p_mid_new->_get_node_light ( ) ) ;
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
void BP_TREE_TY::_balance_external ( const _NodeHeavyPtr  p_parent )
{
    if ( p_parent==0 )
        return ;

    const _NodeHeavyPtr     p_left  = p_parent ;
    const _NodeHeavyPtr     p_mid   = p_left->p_next ;

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

    do
    {
        p_tmp->_set_node_light ( p_lt_mid_new ) ;
        p_tmp = _move_to_parent ( p_tmp ) ;
    }
    while ( p_tmp != 0 ) ;
}


TEMPL_DECL
void BP_TREE_TY::_update_data ( const _NodeHeavyPtr   p_par_start )
{
    if ( ! p_par_start )
        return ;

    _NodeHeavyPtr   p_upd      = p_par_start->p_next ;
    _NodeLightPtr   p_lt_succr = p_upd->p_succr->_get_node_light( ) ;

    while ( p_upd )
    {
        p_upd->_set_node_light ( p_lt_succr )  ;
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
void BP_TREE_TY::_find_lower_bound ( const _Ty_Key &  key_x     ,
                                     _NodeLightPtr &  p_lt_posn ) const
{
    if ( m_p_head_light->p_light_next == m_p_head_light )
    {
        p_lt_posn = _external_end( ) ;
        return ;
    }

    _NodeLightPtr   p_lt_begin = _external_begin() ;
    _NodeLightPtr   p_lt_last  = _external_last () ;

    if      ( ! m_k_comp ( _KeyOfV( ) ( p_lt_begin->_elem() ) , key_x ) )
    {
        p_lt_posn = _external_begin( ) ;
    }
    else if (   m_k_comp ( _KeyOfV( ) ( p_lt_last->_elem() )  , key_x ) )
    {
        p_lt_posn = _external_end( ) ;
    }
    else
    {
        _NodeHeavyPtr   p_h_cur = _top_end( )->p_prev ;

        while ( p_h_cur )
        {
            while ( ! m_k_comp ( _KeyOfV( ) ( p_h_cur->_elem() ) , key_x ) )
            {
                p_h_cur = p_h_cur->p_prev  ;
            }

            if ( p_h_cur->p_succr == 0 )
                break ;

            p_h_cur = p_h_cur->p_next->p_succr->p_prev ;
        }

        p_h_cur   = p_h_cur->p_next ;
        p_lt_posn = p_h_cur->_get_node_light() ;
        p_lt_posn = p_lt_posn->p_light_prev ;
        while ( ! m_k_comp ( _KeyOfV( ) ( p_lt_posn->_elem() ) , key_x ) )
        {
            p_lt_posn = p_lt_posn->p_light_prev ;
        }
        p_lt_posn = p_lt_posn->p_light_next ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_find_upper_bound ( const _Ty_Key &  key_x     ,
                                     _NodeLightPtr &  p_lt_posn ) const
{
    if ( m_p_head_light->p_light_next == m_p_head_light )
    {
        p_lt_posn = _external_end( ) ;
        return ;
    }

    _NodeLightPtr   p_lt_beg  = _external_begin() ;
    _NodeLightPtr   p_lt_last = _external_last () ;

    if (    ! m_k_comp ( key_x , _KeyOfV( ) ( p_lt_last->_elem() ) ) )
    {
        p_lt_posn = _external_end( ) ;
    }
    else if ( m_k_comp ( key_x , _KeyOfV( ) ( p_lt_beg->_elem()  ) ) )
    {
        p_lt_posn = _external_begin( ) ;
    }
    else
    {
        _NodeHeavyPtr   p_h_cur = _top_end( )->p_prev ;

        while ( p_h_cur )
        {
            while ( m_k_comp ( key_x , _KeyOfV( ) ( p_h_cur->_elem() ) ) )
            {
                p_h_cur = p_h_cur->p_prev  ;
            }

            if ( p_h_cur->p_succr == 0 )
                break ;

            p_h_cur = p_h_cur->p_next->p_succr->p_prev ;
        }

        p_h_cur   = p_h_cur->p_next ;
        p_lt_posn = p_h_cur->_get_node_light() ;
        p_lt_posn = p_lt_posn->p_light_prev ;
        while ( m_k_comp ( key_x , _KeyOfV( ) ( p_lt_posn->_elem() ) ) )
        {
            p_lt_posn = p_lt_posn->p_light_prev ;
        }
        p_lt_posn = p_lt_posn->p_light_next ;
    }
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
    while ( pos_a != pos_b )
    {
        pos_a = erase ( pos_a ) ;
    }
    return pos_b ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::erase ( const _Ty_Key &  key_x )
{
    std::pair<iterator, iterator>
                eq_r  = equal_range ( key_x ) ;
    iterator    pos_a = eq_r.first  ;
    iterator    pos_b = eq_r.second ;
    size_type   cnt   = 0 ;

    while ( pos_a != pos_b )
    {
        pos_a = erase ( pos_a ) ;
        ++cnt ;
    }
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
    _NodeLightPtr       p_lt_posn = 0 ;
    _find_lower_bound  ( key_x , p_lt_posn ) ;
    return iterator ( p_lt_posn , this ) ;
}


TEMPL_DECL
typename BP_TREE_TY::const_iterator
BP_TREE_TY::lower_bound ( const _Ty_Key &  key_x ) const
{
    _NodeLightPtr       p_lt_posn = 0 ;
    _find_lower_bound ( key_x , p_lt_posn ) ;
    return  const_iterator ( p_lt_posn , this ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::upper_bound ( const _Ty_Key &  key_x )
{
    _NodeLightPtr       p_lt_posn = 0 ;
    _find_upper_bound ( key_x , p_lt_posn ) ;
    return  iterator ( p_lt_posn , this ) ;
}


TEMPL_DECL
typename BP_TREE_TY::const_iterator
BP_TREE_TY::upper_bound ( const _Ty_Key &  key_x ) const
{
    _NodeLightPtr       p_lt_posn = 0 ;
    _find_upper_bound ( key_x , p_lt_posn ) ;
    return  const_iterator ( p_lt_posn , this ) ;
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
    return size_type ( std::distance( eq_r.first , eq_r.second ) ) ;
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
typename BP_TREE_TY::mapped_type
BP_TREE_TY::accumulate ( const_iterator  it_start ,
                         const_iterator  it_end   ,
                         mapped_type     val_in   ) const
{
    _Ty_Map         sum_res  = val_in ;
    _NodeLightPtr   p_lt_cur = it_start._pointer() ;
    _NodeLightPtr   p_lt_end = it_end  ._pointer() ;

    while ( p_lt_cur != p_lt_end )
    {
        if ( p_lt_cur == m_p_head_light )
            throw std::range_error("accumulate: range error") ;

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

#endif  //  _BP_TREE_HPP

