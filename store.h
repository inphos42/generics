#ifndef GENERICS_STORE_H
#define GENERICS_STORE_H

#include "store_fwd.h"

#include <unordered_map>
#include <map>
#include <deque>
#include <string>

namespace generics {

	template< typename T, typename ID >
	class Store {
	public:
		struct StoreEntry {
			T value;
			int references;

			StoreEntry(const T & val, int refs = 0) : value(val), references(refs) {}
		};

		typedef typename std::unordered_map< ID, StoreEntry * >::const_iterator const_iterator;

		Store() : m_IdCounter(1) {}
		virtual ~Store() { clear(); }

		ID insert(const T & value);

		void remove(ID id);
		void remove(const T & value);
		void remove(StoreEntry * entry);

		inline bool contains(const T & value) const { return m_IdMap.count(value); }

		void clear();

		inline ID id(const T & value) const  {
			id_const_iterator target = m_IdMap.find(value);
			return target == m_IdMap.end() ? 0 : target->second;
		}

		inline const_iterator cbegin() const { return m_Entries.cbegin(); }
		inline const_iterator cend() const { return m_Entries.cend(); }

		inline const T & query(ID id) const { return m_Entries.at(id)->value; }
		inline const T & operator[](ID id) const { return query(id); }

		inline std::size_t size() const { return m_Entries.size(); }

		inline ID maxId() const { return m_IdCounter; }

	protected:
		typedef typename std::unordered_map< ID, StoreEntry * >::iterator iterator;

		typedef typename std::map< T, ID >::const_iterator id_const_iterator;
		typedef typename std::map< T, ID >::iterator id_iterator;

		std::unordered_map< ID, StoreEntry * > m_Entries;
		std::map< T, ID > m_IdMap;
		std::deque< ID > m_FreeIds;

		ID m_IdCounter;

	private:
		Store(const Store & other);
		Store & operator=(const Store & other);
	};

	template< typename T, typename ID >
	ID Store< T, ID >::insert(const T & value)  {
		id_const_iterator target = m_IdMap.find(value);
		ID result;
		StoreEntry * entry;

		if (target == m_IdMap.end()) {
			if (m_FreeIds.empty()) {
				result = m_IdCounter;
				++m_IdCounter;
			}
			else {
				result = m_FreeIds.front();
				m_FreeIds.pop_front();
			}

			entry = new StoreEntry(value, 0);

			m_Entries.insert(std::pair< ID, StoreEntry * >(result, entry));
			m_IdMap.insert(std::pair< T, ID >(value, m_Entries.size()));
		}
		else {
			result = target->second;
			entry = m_Entries[result];
		}

		entry->references++;

		return result;
	}

	template< typename T, typename ID >
	void Store< T, ID >::remove(ID id) {
		iterator entryIt = m_Entries.find(id);
		if (entryIt == m_Entries.end())
			return;

		StoreEntry * entry = entryIt->second;

		entry->references--;

		if (entry->references < 1) {
			m_FreeIds.push_back(id);
			m_IdMap.erase(entry->value);
			m_Entries.erase(entryIt);
			delete entry;
		}
	}

	template< typename T, typename ID >
	void Store< T, ID >::remove(const T & value) {
		id_iterator target = m_IdMap.find(value);
		if (target == m_IdMap.end())
			return;

		iterator entryIt = m_Entries.find(target->second);
		StoreEntry * entry = entryIt->second;

		entry->references--;

		if (entry->references < 1) {
			m_FreeIds.push_back(entryIt->first);
			m_IdMap.erase(target);
			m_Entries.erase(entryIt);
			delete entry;
		}
	}

	template< typename T, typename ID >
	void Store< T, ID >::remove(StoreEntry * entry) {
		id_iterator target = m_IdMap.find(entry->value);
		if (target == m_IdMap.end())
			return;

		iterator entryIt = m_Entries.find(target->second);

		m_FreeIds.push_back(entryIt->first);
		m_IdMap.erase(target);
		m_Entries.erase(entryIt);
		delete entry;
	}

	template< typename T, typename ID >
	void Store< T, ID >::clear() {
		const_iterator stringIt = m_Entries.cbegin();
		while (stringIt != m_Entries.cend()) {
			delete stringIt->second;
			++stringIt;
		}

		m_Entries.clear();
		m_IdMap.clear();
		m_FreeIds.clear();
		m_IdCounter = 1;
	}

}
#endif
