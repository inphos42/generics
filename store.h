#ifndef OSMVMAP_CREATE_STORE_H
#define OSMVMAP_CREATE_STORE_H

#include <unordered_map>
#include <map>
#include <deque>
#include <string>
#include <cstdint>

namespace generics {

	template< typename T >
	class Store {
	public:
		typedef uint32_t ID;

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

		inline const T & query(uint32_t id) const { return m_Entries.at(id)->value; }
		inline const T & operator[](uint32_t id) const { return query(id); }

		inline std::size_t size() const { return m_Entries.size(); }

		inline uint32_t maxId() const { return m_IdCounter; }

	protected:
		typedef typename std::unordered_map< uint32_t, StoreEntry * >::iterator iterator;

		typedef typename std::map< T, uint32_t >::const_iterator id_const_iterator;
		typedef typename std::map< T, uint32_t >::iterator id_iterator;

		std::unordered_map< uint32_t, StoreEntry * > m_Entries;
		std::map< T, uint32_t > m_IdMap;
		std::deque< uint32_t > m_FreeIds;

		uint32_t m_IdCounter;

	private:
		Store(const Store & other);
		Store & operator=(const Store & other);
	};

	template< typename T >
	uint32_t Store< T >::insert(const T & value)  {
		id_const_iterator target = m_IdMap.find(value);
		uint32_t result;
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

			m_Entries.insert(std::pair< uint32_t, StoreEntry * >(result, entry));
			m_IdMap.insert(std::pair< T, uint32_t >(value, m_Entries.size()));
		}
		else {
			result = target->second;
			entry = m_Entries[result];
		}

		entry->references++;

		return result;
	}

	template< typename T >
	void Store< T >::remove(uint32_t id) {
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

	template< typename T >
	void Store< T >::remove(const T & value) {
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

	template< typename T >
	void Store< T >::remove(StoreEntry * entry) {
		id_iterator target = m_IdMap.find(entry->value);
		if (target == m_IdMap.end())
			return;

		iterator entryIt = m_Entries.find(target->second);

		m_FreeIds.push_back(entryIt->first);
		m_IdMap.erase(target);
		m_Entries.erase(entryIt);
		delete entry;
	}

	template< typename T >
	void Store< T >::clear() {
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
