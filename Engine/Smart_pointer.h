/*-----------------------------------------------------------------
----- © 2014 DigiPen Corporation (USA), All Rights Reserved -----
-----------------------------------------------------------------
-* Original Author: Kai Chuan Hsiao
-* Physics Engine Kai
-* See Footer for Revisions                                      */

#pragma once
namespace smart_ptr_internal
{
	// for pooled ref counter
	// not including Pool to avoid circular dependency
	unsigned *newRefCounter(unsigned value);
	void deleteRefCounter(unsigned *counter);
}

// scoped pointer (object deleted upon destruction)
template <typename T>
class scoped_ptr : private NonCopyable
{
public:

	scoped_ptr(void)
		: m_ptr(nullptr)
	{ }

	template <typename U>
	scoped_ptr(U *ptr)
		: m_ptr(nullptr)
	{
		reset(ptr);
	}

	scoped_ptr(T *ptr)
		: m_ptr(nullptr)
	{
		reset(ptr);
	}

	~scoped_ptr(void)
	{
		delete m_ptr;
	}

	void reset(void)
	{
		delete m_ptr;
		m_ptr = nullptr;
	}

	template <typename U>
	void reset(U *ptr)
	{
		// early out
		if (m_ptr == static_cast<T *>(ptr))
			return;

		delete m_ptr;
		m_ptr = ptr;
	}

	__forceinline operator bool(void) const { return m_ptr != nullptr; }

	__forceinline T *get(void) const  { return  m_ptr; }
	__forceinline T &operator* (void) const { return *m_ptr; }
	__forceinline T *operator->(void) const { return  m_ptr; }

private:

	T *m_ptr;
};

// scoped array (object deleted upon destruction)
template <typename T>
class scoped_array : private NonCopyable
{
public:

	scoped_array(void)
		: m_ptr(nullptr)
	{ }

	template <typename U>
	scoped_array(U *ptr)
		: m_ptr(nullptr)
	{
		reset(ptr);
	}

	scoped_array(T *ptr)
		: m_ptr(nullptr)
	{
		reset(ptr);
	}

	~scoped_array(void)
	{
		delete[] m_ptr;
	}

	void reset(void)
	{
		delete[] m_ptr;
		m_ptr = nullptr;
	}

	template <typename U>
	void reset(U *ptr)
	{
		// early out
		if (m_ptr == static_cast<T *>(ptr))
			return;

		delete[] m_ptr;
		m_ptr = ptr;
	}

	__forceinline operator bool(void) const { return m_ptr != nullptr; }

	__forceinline T *get(void) const  { return  m_ptr; }
	__forceinline T &operator* (void) const { return *m_ptr; }
	__forceinline T *operator->(void) const { return  m_ptr; }
	__forceinline T &operator[](unsigned i) { return m_ptr[i]; }
	__forceinline const T &operator[](unsigned i) const { return m_ptr[i]; }

private:

	T *m_ptr;
};

// unique pointer (object deleted upon destruction, transferable ownership)
template <typename T>
class unique_ptr_phy
{
public:

	unique_ptr_phy(void)
		: m_ptr(nullptr)
	{ }

	template <typename U>
	unique_ptr_phy(U *ptr)
		: m_ptr(nullptr)
	{
		reset(ptr);
	}

	unique_ptr_phy(T *ptr)
		: m_ptr(nullptr)
	{
		reset(ptr);
	}

	unique_ptr_phy(unique_ptr_phy &rhs)
		: m_ptr(nullptr)
	{
		reset(rhs.release());
	}

	template <typename U>
	unique_ptr_phy(unique_ptr_phy<U> &rhs)
		: m_ptr(nullptr)
	{
		reset(rhs.release());
	}

	~unique_ptr_phy(void)
	{
		delete m_ptr;
	}

	void reset(void)
	{
		delete m_ptr;
		m_ptr = nullptr;
	}

	template <typename U>
	void reset(U *ptr)
	{
		// early out
		if (m_ptr == static_cast<T *>(ptr))
			return;

		delete m_ptr;
		m_ptr = ptr;
	}

	T *release(void)
	{
		T *ptr = m_ptr;
		m_ptr = nullptr;
		return ptr;
	}

	template <typename U>
	unique_ptr_phy &operator=(unique_ptr_phy<U> &rhs)
	{
		if (this == &rhs)
			return *this;

		reset(rhs.release());
		return *this;
	}

	unique_ptr_phy &operator=(unique_ptr_phy &rhs)
	{
		return operator=<T>(rhs);
	}

	__forceinline operator bool(void) const { return m_ptr != nullptr; }

	__forceinline T *get(void) const  { return  m_ptr; }
	__forceinline T &operator* (void) const { return *m_ptr; }
	__forceinline T *operator->(void) const { return  m_ptr; }

private:

	T *m_ptr;
};

// unique array (object deleted upon destruction, transferable ownership)
template <typename T>
class unique_array
{
public:

	unique_array(void)
		: m_ptr(nullptr)
	{ }

	template <typename U>
	unique_array(U *ptr)
		: m_ptr(nullptr)
	{
		reset(ptr);
	}

	unique_array(T *ptr)
		: m_ptr(nullptr)
	{
		reset(ptr);
	}

	unique_array(unique_array &rhs)
		: m_ptr(nullptr)
	{
		reset(rhs.release());
	}

	template <typename U>
	unique_array(unique_array<U> &rhs)
		: m_ptr(nullptr)
	{
		reset(rhs.release());
	}

	~unique_array(void)
	{
		delete[] m_ptr;
	}

	void reset(void)
	{
		delete[] m_ptr;
		m_ptr = nullptr;
	}

	template <typename U>
	void reset(U *ptr)
	{
		// early out
		if (m_ptr == static_cast<T *>(ptr))
			return;

		delete[] m_ptr;
		m_ptr = ptr;
	}

	T *release(void)
	{
		T *ptr = m_ptr;
		m_ptr = nullptr;
		return ptr;
	}

	template <typename U>
	unique_array &operator=(unique_array<U> &rhs)
	{
		if (this == &rhs)
			retur *this;

		reset(rhs.release());
		return *this;
	}

	unique_array &operator=(unique_array &rhs)
	{
		return operator<T>(rhs);
	}

	__forceinline operator bool(void) const { return m_ptr != nullptr; }

	__forceinline T *get(void) const  { return  m_ptr; }
	__forceinline T &operator* (void) const { return *m_ptr; }
	__forceinline T *operator->(void) const { return  m_ptr; }
	__forceinline T &operator[](unsigned i) { return m_ptr[i]; }
	__forceinline const T &operator[](unsigned i) const { return m_ptr[i]; }

private:

	T *m_ptr;
};

// shared pointer (object deleted upon zero ref count)
template <typename T>
class shared_ptr_phy
{
public:

	shared_ptr_phy(void)
		: m_refCounter(nullptr)
		, m_ptr(nullptr)
	{ }

	template <typename U>
	shared_ptr_phy(U *ptr)
		: m_refCounter(nullptr)
		, m_ptr(nullptr)
	{
		reset(ptr);
	}

	shared_ptr_phy(T *ptr)
		: m_refCounter(nullptr)
		, m_ptr(nullptr)
	{
		reset(ptr);
	}

	template <typename U>
	shared_ptr_phy(shared_ptr_phy<U> &clone)
		: m_refCounter(nullptr)
		, m_ptr(nullptr)
	{
		reset(clone.m_ptr);
		m_refCounter = clone.m_refCounter;
		if (m_refCounter)
			++(*m_refCounter);
	}

	template <typename U>
	shared_ptr_phy(const shared_ptr_phy<U> &clone)
		: m_refCounter(nullptr)
		, m_ptr(nullptr)
	{
		reset(clone.m_ptr);
		m_refCounter = clone.m_refCounter;
		if (m_refCounter)
			++(*m_refCounter);
	}

	~shared_ptr_phy(void)
	{
		reset();
	}

	void reset(void)
	{
		if (m_refCounter && !(--*m_refCounter))
		{
			// zero ref count, delete everything
			smart_ptr_internal::deleteRefCounter(m_refCounter);
			delete m_ptr;
			m_refCounter = nullptr;
			m_ptr = nullptr;
		}
		else
		{
			// non-zero ref count, just point to null
			m_refCounter = nullptr;
			m_ptr = nullptr;
		}
	}

	template <typename U>
	void reset(U *ptr)
	{
		// early out
		if (m_ptr == static_cast<T *>(ptr))
			return;

		// nullptr
		if (!ptr)
		{
			reset();
			return;
		}

		if (m_refCounter && !(--*m_refCounter))
		{
			// zero ref count, delete everything
			smart_ptr_internal::deleteRefCounter(m_refCounter);
			delete m_ptr;
		}

		m_refCounter = smart_ptr_internal::newRefCounter(1);
		m_ptr = ptr;
	}

	__forceinline operator bool(void) const { return m_ptr != nullptr; }

	unsigned use_count(void) const
	{
		if (m_refCounter)
			return *m_refCounter;
		else
			return 0u;
	}

	template <typename U>
	shared_ptr_phy &operator=(const shared_ptr_phy<U> &rhs)
	{
		if (this == &rhs)
			return *this;

		reset(rhs.m_ptr);
		m_refCounter = rhs.m_refCounter;
		if (m_refCounter)
			++(*m_refCounter);
		return *this;
	}

	shared_ptr_phy &operator=(const shared_ptr_phy &rhs)
	{
		return operator=<T>(rhs);
	}

	__forceinline T *get(void) const  { return  m_ptr; }
	__forceinline T &operator* (void) const { return *m_ptr; }
	__forceinline T *operator->(void) const { return  m_ptr; }

private:
	mutable unsigned *m_refCounter;
	T *m_ptr;
};

// shared array (object deleted upon zero ref count)
template <typename T>
class shared_array
{
public:

	shared_array(void)
		: m_refCounter(nullptr)
		, m_ptr(nullptr)
	{ }

	template <typename U>
	shared_array(U *ptr)
		: m_refCounter(nullptr)
		, m_ptr(nullptr)
	{
		reset(ptr);
	}

	shared_array(T *ptr)
		: m_refCounter(nullptr)
		, m_ptr(nullptr)
	{
		reset(ptr);
	}

	template <typename U>
	shared_array(shared_array<U> &clone)
		: m_refCounter(nullptr)
		, m_ptr(nullptr)
	{
		reset(clone.m_ptr);
		m_refCounter = clone.m_refCounter;
		if (m_refCounter)
			++(*m_refCounter);
	}

	template <typename U>
	shared_array(const shared_array<U> &clone)
		: m_refCounter(nullptr)
		, m_ptr(nullptr)
	{
		reset(clone.m_ptr);
		m_refCounter = clone.m_refCounter;
		if (m_refCounter)
			++(*m_refCounter);
	}

	~shared_array(void)
	{
		reset();
	}

	void reset(void)
	{
		if (m_refCounter && !(--*m_refCounter))
		{
			// zero ref count, delete everything
			smart_ptr_internal::deleteRefCounter(m_refCounter);
			delete[] m_ptr;
			m_refCounter = nullptr;
			m_ptr = nullptr;
		}
		else
		{
			// non-zero ref count, just point to null
			m_refCounter = nullptr;
			m_ptr = nullptr;
		}
	}

	template <typename U>
	void reset(U *ptr)
	{
		//early out
		if (m_ptr == static_cast<T *>(ptr))
			return;

		// nullptr
		if (!ptr)
		{
			reset();
			return;
		}

		if (m_refCounter && !(--*m_refCounter))
		{
			// zero ref count, delete everything
			smart_ptr_internal::deleteRefCounter(m_refCounter);
			delete[] m_ptr;
		}

		m_refCounter = smart_ptr_internal::newRefCounter(1);
		m_ptr = ptr;
	}

	__forceinline operator bool(void) const { return m_ptr != nullptr; }

	unsigned use_count(void) const
	{
		if (m_refCounter)
			return *m_refCounter;
		else
			return 0;
	}

	template <typename U>
	shared_array &operator=(const shared_array<U> &rhs)
	{
		if (this == &rhs)
			retur *this;

		reset(rhs.m_ptr);
		m_refCounter = rhs.m_refCounter;
		if (m_refCounter)
			++(*m_refCounter);
		return *this;
	}

	shared_array &operator=(const shared_array &rhs)
	{
		return operator=<T>(rhs);
	}

	__forceinline T *get(void) const  { return  m_ptr; }
	__forceinline T &operator* (void) const { return *m_ptr; }
	__forceinline T *operator->(void) const { return  m_ptr; }
	__forceinline T &operator[](unsigned i) { return m_ptr[i]; }
	__forceinline const T &operator[](unsigned i) const { return m_ptr[i]; }

private:
	mutable unsigned *m_refCounter;
	T *m_ptr;
};

