/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once

#include "List_assert.h"

#define IN_LIST_LINK(HostType) InListLink<HostType>
#define IN_LIST(LinkHostType, LinkHostName) InListDecl<LinkHostType, offsetof(LinkHostType, LinkHostName)>


  // interface
  //---------------------------------------------------------------------------

  template <typename T>
  class InList;

  template <typename T>
  struct InListIterator;

  template <typename T>
  struct InListConstIterator;

  template <typename T>
  struct InListLink 
  {
    friend class InList<T>;
    friend struct InListIterator<T>;
    friend struct InListConstIterator<T>;
    public:
      
      static InListLink *GetLinkFromObj(T *obj, unsigned linkOffset);
      static T *GetObjFromLink(InListLink *link, unsigned linkOffset);

      InListLink(void);
      ~InListLink(void);

      void Link(InListLink *prev, InListLink *next);
      void Unlink(void);

      InListLink *Prev(void);
      InListLink *Next(void);
      const InListLink *Prev(void) const;
      const InListLink *Next(void) const;

      const InList<T> *List(void) const;
      InList<T> *List(void);

    private:

      InListLink *m_prev;
      InListLink *m_next;
      InList<T>  *m_list;
  };

  template <typename T>
  struct InListIterator
  {
    friend class InList<T>;
    public:
          
      InListIterator(InListLink<T> *link = nullptr);

      T &operator* (void) const;
      T *operator->(void) const;
      InListIterator &operator++(void);
      InListIterator &operator--(void);
      InListIterator  operator++(int);
      InListIterator  operator--(int);

      bool operator==(const InListIterator &rhs) const;
      bool operator!=(const InListIterator &rhs) const;

    private:
          
      InListLink<T> *m_link;
  };

  template <typename T>
  struct InListConstIterator
  {
    friend class InList<T>;
    public:

      InListConstIterator(InListLink<T> *link = nullptr);

      const T &operator* (void) const;
      const T *operator->(void) const;
      InListConstIterator &operator++(void);
      InListConstIterator &operator--(void);
      InListConstIterator  operator++(int);
      InListConstIterator  operator--(int);

      bool operator==(const InListConstIterator &rhs) const;
      bool operator!=(const InListConstIterator &rhs) const;

    private:

      InListLink<T> *m_link;
  };

  template <typename T>
  class InList 
  {
    friend struct InListLink<T>;
    friend struct InListIterator<T>;
    friend struct InListConstIterator<T>;
    public:
      
      typedef InListIterator<T> iterator;
      typedef InListConstIterator<T> const_iterator;

      InList(unsigned linkOffset);
      ~InList(void);

      void push_back (T *obj);
      void push_front(T *obj);
      void push_back (InListLink<T> *link);
      void push_front(InListLink<T> *link);
      void insert_before(T *inserted, T *beforeThis);
      void insert_after (T *inserted, T *afterThis );
      void insert_before(T *inserted, iterator beforeThis);
      void insert_after (T *inserted, iterator afterThis );

      T *pop_back (void);
      T *pop_front(void);
      iterator erase(T *obj);
      iterator erase(iterator iter);
      void clear(void);

      unsigned size(void) const;
      bool empty(void) const;

      iterator begin(void);
      iterator end  (void);
      const_iterator begin (void) const;
      const_iterator end   (void) const;
      const_iterator cbegin(void) const;
      const_iterator cend  (void) const;

    private:

      const unsigned k_linkOffset;

      InListLink<T> m_head;
      InListLink<T> m_tail;

      unsigned m_size;
  };

  // for declarations (forwarding down link offsets)
  template <typename T, unsigned LinkOffset>
  class InListDecl : public InList<T>
  {
    public:
      __forceinline InListDecl(void) : InList(LinkOffset) { }
  };

  //---------------------------------------------------------------------------
  // end of interface


  // InList::InListIterator implementation
  //---------------------------------------------------------------------------

  template <typename T>
  __forceinline InListIterator<T>::InListIterator(InListLink<T> *link)
    : m_link(link)
  { }

  template <typename T>
  __forceinline T &InListIterator<T>::operator*(void) const
  {
    return *InListLink<T>::GetObjFromLink(m_link, m_link->m_list->k_linkOffset);
  }

  template <typename T>
  __forceinline T *InListIterator<T>::operator->(void) const
  {
    return InListLink<T>::GetObjFromLink(m_link, m_link->m_list->k_linkOffset);
  }

  template <typename T>
  __forceinline InListIterator<T> &InListIterator<T>::operator++(void)
  {
    GV_ASSERT_MESSAGE(m_link != nullptr, "Invalid in-list InListIterator increment.");
    m_link = m_link->m_next;
    return *this;
  }

  template <typename T>
  __forceinline InListIterator<T> &InListIterator<T>::operator--(void)
  {
    GV_ASSERT_MESSAGE(m_link != nullptr, "Invalid in-list InListIterator decrement.");
    m_link = m_link->m_prev;
    return *this;
  }

  template <typename T>
  __forceinline InListIterator<T> InListIterator<T>::operator++(int)
  {
    InListIterator copy = *this;
    ++*this;
    return copy;
  }

  template <typename T>
  __forceinline InListIterator<T> InListIterator<T>::operator--(int)
  {
    InListIterator copy = *this;
    --*this;
    return copy;
  }

  template <typename T>
  __forceinline bool InListIterator<T>::operator==(const InListIterator &rhs) const
  {
    return m_link == rhs.m_link;
  }

  template <typename T>
  __forceinline bool InListIterator<T>::operator!=(const InListIterator &rhs) const
  {
    return !operator==(rhs);
  }

  //---------------------------------------------------------------------------
  // end of InList::InListIterator implementation


  // InList::InListConstIterator implementation
  //---------------------------------------------------------------------------

  template <typename T>
  __forceinline InListConstIterator<T>::InListConstIterator(InListLink<T> *link)
    : m_link(link)
  { }

  template <typename T>
  __forceinline const T &InListConstIterator<T>::operator*(void) const
  {
    return *InListLink<T>::GetObjFromLink(m_link, m_link->m_list->k_linkOffset);
  }

  template <typename T>
  __forceinline const T *InListConstIterator<T>::operator->(void) const
  {
    return InListLink<T>::GetObjFromLink(m_link, m_link->m_list->k_linkOffset);
  }

  template <typename T>
  __forceinline InListConstIterator<T> &InListConstIterator<T>::operator++(void)
  {
    GV_ASSERT_MESSAGE(m_link != nullptr, "Invalid in-list InListIterator increment.");
    m_link = m_link->m_next;
    return *this;
  }

  template <typename T>
  __forceinline InListConstIterator<T> &InListConstIterator<T>::operator--(void)
  {
    GV_ASSERT_MESSAGE(m_link != nullptr, "Invalid in-list InListIterator decrement.");
    m_link = m_link->m_prev;
    return *this;
  }

  template <typename T>
  __forceinline InListConstIterator<T> InListConstIterator<T>::operator++(int)
  {
    InListConstIterator copy = *this;
    ++*this;
    return copy;
  }

  template <typename T>
  __forceinline InListConstIterator<T> InListConstIterator<T>::operator--(int)
  {
    InListConstIterator copy = *this;
    --*this;
    return copy;
  }

  template <typename T>
  __forceinline bool InListConstIterator<T>::operator==(const InListConstIterator &rhs) const
  {
    return m_link == rhs.m_link;
  }

  template <typename T>
  __forceinline bool InListConstIterator<T>::operator!=(const InListConstIterator &rhs) const
  {
    return !operator==(rhs);
  }

  //---------------------------------------------------------------------------
  // end of InList::InListConstIterator implementation


  // InList implementation
  //---------------------------------------------------------------------------

  template <typename T>
  __forceinline InList<T>::InList(unsigned linkOffset)
    : k_linkOffset(linkOffset)
    , m_size(0)
  {
    m_head.m_next = &m_tail;
    m_tail.m_prev = &m_head;
    m_head.m_list = m_tail.m_list = this;
  }

  template <typename T>
  __forceinline InList<T>::~InList(void)
  {
    clear();
  }

  template <typename T>
  __forceinline void InList<T>::push_back(T *obj)
  {
    push_back(InListLink<T>::GetLinkFromObj(obj, k_linkOffset));
  }

  template <typename T>
  __forceinline void InList<T>::push_front(T *obj)
  {
    push_front(InListLink<T>::GetLinkFromObj(obj, k_linkOffset));
  }

  template <typename T>
  __forceinline void InList<T>::push_back(InListLink<T> *link)
  {
    link->Link(m_tail.m_prev, &m_tail);
  }

  template <typename T>
  __forceinline void InList<T>::push_front(InListLink<T> *link)
  {
    link->Link(&m_head, m_head.m_next);
  }

  template <typename T>
  __forceinline T *InList<T>::pop_back(void)
  {
    T *popped = InListLink<T>::GetObjFromLink(m_tail.m_prev, k_linkOffset);
    m_tail.m_prev->Unlink();
    return popped;
  }

  template <typename T>
  __forceinline T *InList<T>::pop_front(void)
  {
    T *popped = InListLink<T>::GetObjFromLink(m_head.m_next, k_linkOffset);
    m_head.m_next->Unlink();
    return popped;
  }

  template <typename T>
  __forceinline typename InList<T>::iterator InList<T>::erase(T *obj)
  {
    InListLink<T> *link = InListLink<T>::GetLinkFromObj(obj, k_linkOffset);

    iterator next(link->m_next);
    link->Unlink();
    return next;
  }

  template <typename T>
  __forceinline typename InList<T>::iterator InList<T>::erase(typename InList<T>::iterator iter)
  {
    InListLink<T> *link = iter.m_link;

    iterator next(link->m_next);
    link->Unlink();
    return next;
  }

  template <typename T>
  __forceinline void InList<T>::clear(void)
  {
    while (m_head.m_next != &m_tail)
      m_head.m_next->Unlink();

    GV_ASSERT_MESSAGE(m_size == 0, "Something's wrong: the list size is not 0 after being cleared.");
  }

  template <typename T>
  __forceinline void InList<T>::insert_before(T *inserted, T *beforeThis)
  {
    InListLink<T> *link = InListLink<T>::GetLinkFromObj(inserted, k_linkOffset);
    InListLink<T> *next = InListLink<T>::GetLinkFromObj(beforeThis, k_linkOffset);
    link->Link(next->m_prev, next);
  }

  template <typename T>
  __forceinline void InList<T>::insert_after(T *inserted, T *afterThis)
  {
    InListLink<T> *prev = InListLink<T>::GetLinkFromObj(afterThis, k_linkOffset);
    InListLink<T> *link = InListLink<T>::GetLinkFromObj(inserted, k_linkOffset);
    link->Link(prev, prev->m_next);
  }

  template <typename T>
  __forceinline void InList<T>::insert_before(T *inserted, typename InList<T>::iterator beforeThis)
  {
    InListLink<T> *link = InListLink<T>::GetLinkFromObj(inserted, k_linkOffset);
    InListLink<T> *next = beforeThis.m_link;
    link->Link(next->m_prev, next);
  }

  template <typename T>
  __forceinline void InList<T>::insert_after(T *inserted, typename InList<T>::iterator afterThis)
  {
    InListLink<T> *prev = afterThis.m_link;
    InListLink<T> *link = InListLink<T>::GetLinkFromObj(inserted, k_linkOffset);
    link->Link(prev, prev->m_next);
  }

  template <typename T>
  __forceinline unsigned InList<T>::size(void) const { return m_size; }

  template <typename T>
  __forceinline bool InList<T>::empty(void) const { return m_head.m_next == &m_tail; }

  template <typename T>
  __forceinline typename InList<T>::iterator InList<T>::begin(void)
  {
    return iterator(m_head.m_next);
  }

  template <typename T>
  __forceinline typename InList<T>::iterator InList<T>::end(void)
  {
    return iterator(&m_tail);
  }

  template <typename T>
  __forceinline typename InList<T>::const_iterator InList<T>::begin(void) const
  {
    return const_iterator(const_cast<InListLink<T> *>(m_head.m_next));
  }

  template <typename T>
  __forceinline typename InList<T>::const_iterator InList<T>::end(void) const
  {
    return const_iterator(const_cast<InListLink<T> *>(&m_tail));
  }

  template <typename T>
  __forceinline typename InList<T>::const_iterator InList<T>::cbegin(void) const
  {
    return const_iterator(const_cast<InListLink<T> *>(m_head.m_next));
  }

  template <typename T>
  __forceinline typename InList<T>::const_iterator InList<T>::cend(void) const
  {
    return const_iterator(const_cast<InListLink<T> *>(&m_tail));
  }

  //---------------------------------------------------------------------------
  // end of InList implementation


  // InListLink implementation
  //---------------------------------------------------------------------------

  template <typename T>
  __forceinline InListLink<T> *InListLink<T>::GetLinkFromObj(T *obj, unsigned linkOffset)
  {
    return 
      reinterpret_cast<InListLink<T> *>
      (
        reinterpret_cast<char *>(obj) + linkOffset
      );
  }

  template <typename T>
  __forceinline T *InListLink<T>::GetObjFromLink(InListLink *link, unsigned linkOffset)
  {
    return 
      reinterpret_cast<T *>
      (
        reinterpret_cast<char *>(link) - linkOffset
      );
  }

  template <typename T>
  __forceinline InListLink<T>::InListLink(void)
    : m_prev(nullptr)
    , m_next(nullptr)
    , m_list(nullptr)
  {
    m_prev = m_next = this;
  }

  template <typename T>
  __forceinline InListLink<T>::~InListLink(void)
  {
    Unlink();
  }

  template <typename T>
  __forceinline void InListLink<T>::Link(InListLink *prev, InListLink *next)
  {
    GV_ASSERT_MESSAGE(prev->m_list == next->m_list, "Prev & next belong to different lists.");

    // update prev link
    prev->m_next = this;
    this->m_prev = prev;

    // update next link
    next->m_prev = this;
    this->m_next = next;

    // update list pointer
    m_list = next->m_list;

    // update list size
    ++m_list->m_size;
  }

  template <typename T>
  __forceinline void InListLink<T>::Unlink(void)
  {
    if (m_list)
    {
      // update links
      m_prev->m_next = m_next;
      m_next->m_prev = m_prev;

      // update list size
      --m_list->m_size;

      // clean up pointers
      m_list = nullptr;
      m_prev = m_next = nullptr;
    }
  }

  template <typename T>
  __forceinline InListLink<T> *InListLink<T>::Prev(void)
  {
    return m_prev;
  }

  template <typename T>
  __forceinline InListLink<T> *InListLink<T>::Next(void)
  {
    return m_next;
  }

  template <typename T>
  __forceinline const InListLink<T> *InListLink<T>::Prev(void) const
  {
    return m_prev;
  }

  template <typename T>
  __forceinline const InListLink<T> *InListLink<T>::Next(void) const
  {
    return m_next;
  }

  template <typename T>
  __forceinline const InList<T> *InListLink<T>::List(void) const
  {
    return m_list;
  }

  template <typename T>
  __forceinline InList<T> *InListLink<T>::List(void)
  {
    return m_list;
  }

  //---------------------------------------------------------------------------
  // end of InListLink implementation


