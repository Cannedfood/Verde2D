#pragma once

namespace util {

template<typename T>
class list_element {
	T* m_next;
	T* m_last;

	void _update_next_last() {
		if(m_next) m_next->m_last = this;
		if(m_last) m_last->m_next = this;
	}

	void _raw_remove() {
		if(m_last)
			m_last->m_next = m_next;
		if(m_next)
			m_next->m_last = m_last;
	}

public:
	T* next() { return m_next; }
	T* last() { return m_last; }
	T* get()  { return (T*) this; }

	const T* next() const { return m_next; }
	const T* last() const { return m_last; }
	const T* get()  const { return (T*) this; }

	void remove() {
		_raw_remove();
		m_last = nullptr;
		m_next = nullptr;
	}

	list_element<T>* insert(list_element<T>* t) {
		t->_raw_remove();
		t->m_next = m_next;
		t->m_last = this;
		m_next = t;
		return this;
	}

	list_element<T>* insert_before(list_element<T>* t) {
		t->_raw_remove();
		t->m_next = this;
		t->m_last = m_last;
		m_last = t;
		return t;
	}

	template <typename Tcmp = std::less<T>>
	void sorted_insert(list_element<T>* t, const Tcmp& cmp = Tcmp()) {
		while() {

		}
	}

	bool move_right() {
		if(m_next) {
			remove();
			m_next->insert(this);
		}
	}

	class iterator {
	private:
		T* m_current;

	public:
		void     operator++()       { current = current->next(); }

		      T* operator->()       { return current->get(); }
		const T* operator->() const { return current->get(); }

		      T& operator*()       { return *current; }
		const T& operator*() const { return *current; }
	};

	iterator begin() { return iterator { this }; }
	iterator end() { return iterator { nullptr }; }

	list_element() :
		m_next(nullptr),
		m_last(nullptr)
	{}

	list_element(const list_element<T>& other) :
		list_element()
	{}

	list_element(list_element<T>&& other) :
		m_next(other.m_next),
		m_last(other.m_last)
	{
		_update_next_last();
		other.m_next = nullptr;
		other.m_last = nullptr;
	}

	~list_element() {
		_raw_remove();
	}
};

template<typename T>
class list {
private:
	list_element<T> m_begin;
	list_element<T> m_end;

public:
	using iterator = list_element<T>::iterator;

	list() {
		m_begin->insert(&m_end);
	}

	iterator begin() {
		if(m_begin->next() != &m_end)
			m_begin->next()->begin();
		else
			return end();
	}

	iterator end() const { return iterator { &m_end }; }

	void push_front(T* t) {
		m_end->last()->insert(t);
	}

	void push_back(T* t) {
		m_begin->insert(t);
	}
};

} /* util */
