#pragma once

#include <map>
#include <vector>
#include "Utils.h"

namespace Simulator
{
	template<class KeyType>
	inline unsigned int hashFunction(const KeyType &key)
	{
		return 0;
	}

	template<class KeyType, class ValueType>
	class HashMap
	{
	public:
		typedef typename std::map<unsigned int, ValueType> KeyValueMap;

	private:
		unsigned int m_bucketCount;
		unsigned int m_moduloValue;
		std::vector<KeyValueMap*> m_hashMap;

	public:

		HashMap(const unsigned int &bucketCount)
		{
			unsigned int val = bucketCount;
			unsigned int powerOfTwo = 1;
			while (powerOfTwo < val)
				powerOfTwo <<= 1;
			m_bucketCount = powerOfTwo;
			m_moduloValue = powerOfTwo - 1;
			initialize();
		}

		~HashMap()
		{
			cleanup();
		}

		void clear()
		{
			cleanup();
			initialize();
		}

		KeyValueMap *getKeyValueMap(const unsigned int &index)
		{
			return m_hashMap[index];
		}

		unsigned int bucketCount() const
		{
			return m_bucketCount;
		}

		ValueType *find(const KeyType &key)
		{
			const unsigned int hashValue = hashFunction<KeyType>(key);
			const unsigned int mapIndex = hashValue & m_moduloValue;
			if (m_hashMap[mapIndex] != nullptr)
			{
				typename KeyValueMap::iterator &iter = m_hashMap[mapIndex]->find(hashValue);
				if (iter != m_hashMap[mapIndex]->end())
					return &iter->second;
			}
			return nullptr;
		}

		void insert(const KeyType &key, const ValueType &value)
		{
			const unsigned int hashValue = hashFunction<KeyType>(key);
			const unsigned int mapIndex = hashValue & m_moduloValue;
			if (m_hashMap[mapIndex] == nullptr)
				m_hashMap[mapIndex] = new KeyValueMap();
			(*m_hashMap[mapIndex])[hashValue] = value;
		}

		void remove(const KeyType &key)
		{
			const unsigned int hashValue = hashFunction<KeyType>(key);
			const unsigned int mapIndex = hashValue & m_moduloValue;
			if (m_hashMap[mapIndex] != nullptr)
			{
				m_hashMap[mapIndex]->erase(hashValue);
				if (m_hashMap[mapIndex]->size() == 0)
				{
					delete m_hashMap[mapIndex];
					m_hashMap[mapIndex] = nullptr;
				}
			}
		}

		ValueType &operator[](const KeyType &key)
		{
			const int hashValue = hashFunction<KeyType>(key);
			const unsigned int mapIndex = hashValue & m_moduloValue;
			if (m_hashMap[mapIndex] == nullptr)
			{
				m_hashMap[mapIndex] = new KeyValueMap();
			}
			return (*m_hashMap[mapIndex])[hashValue];
		}
		
		ValueType* query(const KeyType &key)
		{
			const unsigned int hashValue = hashFunction<KeyType>(key);
			const unsigned int mapIndex = hashValue & m_moduloValue;
			if (m_hashMap[mapIndex] == nullptr)
				return nullptr;
			const typename KeyValueMap::iterator &it = m_hashMap[mapIndex]->find(hashValue);
			if (it != m_hashMap[mapIndex]->end())
				return &it->second;
			return nullptr;
		}

		const ValueType* query(const KeyType &key) const
		{
			const unsigned int hashValue = hashFunction<KeyType>(key);
			const unsigned int mapIndex = hashValue & m_moduloValue;
			if (m_hashMap[mapIndex] == nullptr)
				return nullptr;
			const typename KeyValueMap::iterator &it = m_hashMap[mapIndex]->find(hashValue);
			if (it != m_hashMap[mapIndex]->end())
				return &it->second;
			return nullptr;
		}

	protected:

		void initialize()
		{
			m_hashMap = std::vector<KeyValueMap*>(m_bucketCount, nullptr);
		}

		void cleanup()
		{
			for (unsigned int x = 0; x < m_bucketCount; ++x)
			{
				if (m_hashMap[x] != nullptr)
				{
					m_hashMap[x]->clear();
					delete m_hashMap[x];
				}
			}
			std::vector<KeyValueMap*>().swap(m_hashMap);
		}
	};
}