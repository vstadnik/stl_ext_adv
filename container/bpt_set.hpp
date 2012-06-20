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

#ifndef _BPT_SET_HPP
#define _BPT_SET_HPP

#include <functional>
#include "bp_tree_idx.hpp"


_BPT_OPEN

template
<
    class _K                       ,
    class _Pr = std::less<_K>      ,
    class _A  = std::allocator<_K> ,
    template < class , class , class , class , class , class , class >
    class _BPTree = bp_tree_idx
>
class set
{
    typedef _BPTree < _K , _K , _K , GetSelf<_K> , GetSelf<_K> , _Pr , _A >
                                                        _BPTreeType     ;

public:
    typedef set<_K, _Pr, _A, _BPTree>                   _Ty_This        ;
    typedef _K                                          value_type      ;

    typedef _Pr                                         value_compare   ;
    typedef _K                                          key_type        ;
    typedef _Pr                                         key_compare     ;
    typedef _A                                          allocator_type  ;

    typedef typename allocator_type::size_type          size_type       ;
    typedef typename allocator_type::difference_type    difference_type ;
    typedef typename allocator_type::pointer            pointer         ;
    typedef typename allocator_type::const_pointer      const_pointer   ;
    typedef typename allocator_type::reference          reference       ;
    typedef typename allocator_type::const_reference    const_reference ;

    typedef typename _BPTreeType::iterator              iterator        ;
    typedef typename _BPTreeType::const_iterator        const_iterator  ;
    typedef std::reverse_iterator< iterator >           reverse_iterator;
    typedef std::reverse_iterator< const_iterator >     const_reverse_iterator;

    explicit
    set ( const key_compare &    pred=key_compare()    ,
          const allocator_type & alr =allocator_type() ) :
        m_contr ( pred, false, true, alr ) { }

    template < class _InpIter >
    set ( _InpIter               pos_a ,
          _InpIter               pos_b ,
          const key_compare &    pred=key_compare()    ,
          const allocator_type & alr =allocator_type() ) :
        m_contr ( pred, false, true, alr )
        { m_contr.insert_set ( pos_a, pos_b ) ; }

    set        ( const _Ty_This &  that ) : m_contr ( that.m_contr ) { }
    _Ty_This &
    operator = ( const _Ty_This &  that )
    { m_contr = that.m_contr ; return *this ; }

    const_iterator          begin ( ) const { return m_contr.begin() ; }
    iterator                begin ( )       { return m_contr.begin() ; }
    const_iterator          end   ( ) const { return m_contr.end()   ; }
    iterator                end   ( )       { return m_contr.end()   ; }
    const_reverse_iterator  rbegin( ) const
                            { return const_reverse_iterator (end()  ); }
    reverse_iterator        rbegin( )
                            { return reverse_iterator       (end()  ); }
    const_reverse_iterator  rend  ( ) const
                            { return const_reverse_iterator (begin()); }
    reverse_iterator        rend  ( )
                            { return reverse_iterator       (begin()); }

    size_type               size     ( ) const { return m_contr.size()    ; }
    size_type               max_size ( ) const { return m_contr.max_size(); }
    bool                    empty    ( ) const { return m_contr.empty()   ; }

    allocator_type  get_allocator ( ) const { return m_contr.get_allocator(); }
    key_compare     key_comp      ( ) const { return m_contr.key_comp()     ; }
    value_compare   value_comp    ( ) const { return m_contr.key_comp()     ; }

    std::pair<iterator, bool>
                insert ( const value_type &  val_x )
                { return m_contr.insert(val_x) ; }
    iterator    insert ( iterator  pos , const value_type &  val_x )
                { return m_contr.insert(pos, val_x) ; }
    template < class _InpIter >
    void        insert ( _InpIter  pos_a , _InpIter  pos_b )
                { m_contr.insert_set(pos_a, pos_b) ; }

    void      clear ( )
                    { m_contr.clear() ; }
    iterator  erase ( iterator  pos )
                    { return m_contr.erase(pos) ; }
    iterator  erase ( iterator  pos_a, iterator  pos_b )
                    { return m_contr.erase( pos_a, pos_b) ; }
    size_type erase ( const _K &  key_x )
                    { return m_contr.erase(key_x) ; }

    size_type       count       ( const _K &  key_x ) const
                                { return m_contr.count(key_x); }
    const_iterator  find        ( const _K &  key_x ) const
                                { return m_contr.find(key_x) ; }
    iterator        find        ( const _K &  key_x )
                                { return m_contr.find(key_x) ; }
    const_iterator  lower_bound ( const _K &  key_x ) const
                                { return m_contr.lower_bound(key_x) ; }
    iterator        lower_bound ( const _K &  key_x )
                                { return m_contr.lower_bound(key_x) ; }
    const_iterator  upper_bound ( const _K &  key_x ) const
                                { return m_contr.upper_bound(key_x) ; }
    iterator        upper_bound ( const _K &  key_x )
                                { return m_contr.upper_bound(key_x) ; }
    std::pair<const_iterator, const_iterator>
                    equal_range ( const _K &  key_x ) const
                                { return m_contr.equal_range(key_x) ; }
    std::pair<iterator, iterator>
                    equal_range ( const _K &  key_x )
                                { return m_contr.equal_range(key_x) ; }

    void        swap ( _Ty_This &  ctr_x )   { m_contr.swap(ctr_x.m_contr) ; }

    value_type  accumulate ( const_iterator  it_start ,
                             const_iterator  it_end   ,
                             value_type      val_in   ) const
                { return m_contr.accumulate ( it_start, it_end, val_in ) ; }

    std::pair<iterator, bool>
                write ( iterator  pos, const value_type &  val_new )
    {
        if ( !_Pr()( *pos , val_new ) && !_Pr()( val_new , *pos ) )
            return std::pair<iterator, bool> ( pos , false ) ;

        iterator  lb = lower_bound ( val_new ) ;
        bool      contains = ( ( lb!=end() ) && !_Pr()( val_new, *lb ) ) ;
        if ( contains )
            return std::pair<iterator, bool> ( lb , false ) ;

        iterator  it_res = m_contr.write_deep ( pos, val_new ) ;
        return std::pair<iterator, bool> ( it_res , true ) ;
    }

protected:
    _BPTreeType     m_contr ;

} ;


#define TEMPL_DECL_SET  template < class _K , class _Pr , class _A , \
        template < class , class , class , class , class , class , class > \
        class _BPTree > inline
#define SET_BPT         set < _K , _Pr , _A , _BPTree >


TEMPL_DECL_SET
void swap ( SET_BPT &  ctr_x , SET_BPT &  ctr_y )
{
    ctr_x . swap ( ctr_y ) ;
}

TEMPL_DECL_SET
bool operator == ( const SET_BPT &  ctr_x ,
                   const SET_BPT &  ctr_y   )
{
    return ( (ctr_x.size()==ctr_y.size()) &&
             std::equal ( ctr_x.begin(), ctr_x.end(), ctr_y.begin() ) ) ;
}

TEMPL_DECL_SET
bool operator != ( const SET_BPT &  ctr_x ,
                   const SET_BPT &  ctr_y   )
{
    return (!(ctr_x == ctr_y)) ;
}

TEMPL_DECL_SET
bool operator < ( const SET_BPT &  ctr_x ,
                  const SET_BPT &  ctr_y   )
{
    return std::lexicographical_compare(ctr_x.begin(), ctr_x.end(),
                                        ctr_y.begin(), ctr_y.end() ) ;
}

TEMPL_DECL_SET
bool operator > ( const SET_BPT &  ctr_x ,
                  const SET_BPT &  ctr_y   )
{
    return (ctr_y < ctr_x) ;
}

TEMPL_DECL_SET
bool operator <= ( const SET_BPT &  ctr_x ,
                   const SET_BPT &  ctr_y   )
{
    return (!(ctr_y < ctr_x)) ;
}

TEMPL_DECL_SET
bool operator >= ( const SET_BPT &  ctr_x ,
                   const SET_BPT &  ctr_y   )
{
    return (!(ctr_x < ctr_y)) ;
}


#undef TEMPL_DECL_SET
#undef SET_BPT


template
<
    class _K                       ,
    class _Pr = std::less<_K>      ,
    class _A  = std::allocator<_K> ,
    template < class , class , class , class , class , class , class >
    class _BPTree = bp_tree_idx
>
class multiset
{
    typedef _BPTree < _K , _K , _K , GetSelf<_K> , GetSelf<_K> , _Pr , _A >
                                                        _BPTreeType     ;

public:
    typedef multiset<_K, _Pr, _A, _BPTree>              _Ty_This        ;
    typedef _K                                          value_type      ;

    typedef _Pr                                         value_compare   ;
    typedef _K                                          key_type        ;
    typedef _Pr                                         key_compare     ;
    typedef _A                                          allocator_type  ;

    typedef typename allocator_type::size_type          size_type       ;
    typedef typename allocator_type::difference_type    difference_type ;
    typedef typename allocator_type::pointer            pointer         ;
    typedef typename allocator_type::const_pointer      const_pointer   ;
    typedef typename allocator_type::reference          reference       ;
    typedef typename allocator_type::const_reference    const_reference ;

    typedef typename _BPTreeType::iterator              iterator        ;
    typedef typename _BPTreeType::const_iterator        const_iterator  ;
    typedef std::reverse_iterator< iterator >           reverse_iterator;
    typedef std::reverse_iterator< const_iterator >     const_reverse_iterator;

    explicit
    multiset ( const key_compare &     pred=key_compare()    ,
               const allocator_type &  alr =allocator_type() ) :
        m_contr  ( pred, true, true, alr ) { }

    template < class _InpIter >
    multiset ( _InpIter                pos_a ,
               _InpIter                pos_b ,
               const key_compare &     pred=key_compare()    ,
               const allocator_type &  alr =allocator_type() ) :
        m_contr ( pred, true, true, alr )
        { m_contr.insert_set ( pos_a, pos_b ) ; }

    multiset     ( const _Ty_This &  that ) : m_contr ( that.m_contr ) { }
    _Ty_This &
    operator =   ( const _Ty_This &  that )
    { m_contr = that.m_contr ; return *this ; }

    const_iterator          begin ( ) const { return m_contr.begin() ; }
    iterator                begin ( )       { return m_contr.begin() ; }
    const_iterator          end   ( ) const { return m_contr.end()   ; }
    iterator                end   ( )       { return m_contr.end()   ; }
    const_reverse_iterator  rbegin( ) const
                            { return const_reverse_iterator (end()  ); }
    reverse_iterator        rbegin( )
                            { return reverse_iterator       (end()  ); }
    const_reverse_iterator  rend  ( ) const
                            { return const_reverse_iterator (begin()); }
    reverse_iterator        rend  ( )
                            { return reverse_iterator       (begin()); }

    size_type               size     ( ) const { return m_contr.size()     ; }
    size_type               max_size ( ) const { return m_contr.max_size() ; }
    bool                    empty    ( ) const { return m_contr.empty()    ; }

    allocator_type  get_allocator ( ) const { return m_contr.get_allocator(); }
    key_compare     key_comp      ( ) const { return m_contr.key_comp()     ; }
    value_compare   value_comp    ( ) const { return m_contr.key_comp()     ; }

    iterator    insert ( const value_type &  val_x )
                { return m_contr.insert(val_x).first; }
    iterator    insert ( iterator  pos, const value_type &  val_x )
                { return m_contr.insert(pos, val_x) ; }
    template < class _InpIter >
    void        insert ( _InpIter  pos_a, _InpIter  pos_b )
                { m_contr.insert_set ( pos_a , pos_b ) ; }

    void        clear ( )
                { m_contr.clear() ; }
    iterator    erase ( iterator  pos )
                { return m_contr.erase(pos) ; }
    iterator    erase ( iterator  pos_a, iterator  pos_b )
                { return m_contr.erase( pos_a, pos_b) ; }
    size_type   erase ( const _K &  key_x )
                { return m_contr.erase(key_x) ; }

    size_type       count       ( const _K &  key_x ) const
                                { return m_contr.count(key_x); }
    const_iterator  find        ( const _K &  key_x ) const
                                { return m_contr.find(key_x) ; }
    iterator        find        ( const _K &  key_x )
                                { return m_contr.find(key_x) ; }
    const_iterator  lower_bound ( const _K &  key_x ) const
                                { return m_contr.lower_bound(key_x) ; }
    iterator        lower_bound ( const _K &  key_x )
                                { return m_contr.lower_bound(key_x) ; }
    const_iterator  upper_bound ( const _K &  key_x ) const
                                { return m_contr.upper_bound(key_x) ; }
    iterator        upper_bound ( const _K &  key_x )
                                { return m_contr.upper_bound(key_x) ; }
    std::pair<const_iterator, const_iterator>
                    equal_range ( const _K &  key_x ) const
                                { return m_contr.equal_range(key_x) ; }
    std::pair<iterator, iterator>
                    equal_range ( const _K &  key_x )
                                { return m_contr.equal_range(key_x) ; }

    void        swap ( _Ty_This &  ctr_x )   { m_contr.swap(ctr_x.m_contr); }

    value_type  accumulate ( const_iterator  it_start ,
                             const_iterator  it_end   ,
                             value_type      val_in   ) const
                { return m_contr.accumulate ( it_start, it_end, val_in ) ; }

    std::pair<iterator, bool>
                write ( iterator  pos, const value_type &  val_new )
    {
        if ( !_Pr()( *pos, val_new ) && !_Pr()( val_new, *pos ) )
            return std::pair<iterator, bool> ( pos , false ) ;

        iterator  it_res = m_contr.write_deep ( pos, val_new ) ;
        return std::pair<iterator, bool> ( it_res , true ) ;
    }

protected:
    _BPTreeType     m_contr ;

} ;


#define TEMPL_DECL_M_SET  template < class _K , class _Pr , class _A , \
        template < class , class , class , class , class , class , class > \
        class _BPTree > inline
#define M_SET_BPT         multiset < _K , _Pr , _A , _BPTree >


TEMPL_DECL_M_SET
void swap ( M_SET_BPT &  ctr_x , M_SET_BPT &  ctr_y )
{
     ctr_x . swap ( ctr_y ) ;
}

TEMPL_DECL_M_SET
bool operator == ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return ( (ctr_x.size()==ctr_y.size()) &&
             std::equal ( ctr_x.begin() , ctr_x.end() , ctr_y.begin() ) ) ;
}

TEMPL_DECL_M_SET
bool operator != ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return (!(ctr_x == ctr_y));
}

TEMPL_DECL_M_SET
bool operator <  ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return std::lexicographical_compare(ctr_x.begin(), ctr_x.end(),
                                        ctr_y.begin(), ctr_y.end()  ) ;
}

TEMPL_DECL_M_SET
bool operator >  ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return (ctr_y < ctr_x);
}

TEMPL_DECL_M_SET
bool operator <= ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return (!(ctr_y < ctr_x));
}

TEMPL_DECL_M_SET
bool operator >= ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return (!(ctr_x < ctr_y));
}


#undef TEMPL_DECL_M_SET
#undef M_SET_BPT


_BPT_CLOSE


#endif  //  _BPT_SET_HPP









