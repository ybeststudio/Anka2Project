#pragma once

#include "../eterBase/Debug.h"

template<typename T>
class CDynamicPool
{
	public:
		CDynamicPool()
		{
			m_uInitCapacity=0;
			m_uUsedCapacity=0;
		}
		virtual ~CDynamicPool()
		{
			assert(m_kVct_pkData.empty());
		}

		void SetName(const char *c_szName) {}

		void Clear()
		{
			Destroy();
		}

		void Destroy()
		{
			for (auto v : m_kVct_pkData)
				Delete(v);
			m_kVct_pkData.clear();
			m_kVct_pkFree.clear();
		}

		void Create(UINT uCapacity)
		{
			m_uInitCapacity=uCapacity;
			m_kVct_pkData.reserve(uCapacity);
			m_kVct_pkFree.reserve(uCapacity);
		}
		T* Alloc()
		{
			if (m_kVct_pkFree.empty())
			{
				T* pkNewData=new T;
				m_kVct_pkData.push_back(pkNewData);
				++m_uUsedCapacity;
				return pkNewData;
			}

			T* pkFreeData=m_kVct_pkFree.back();
			m_kVct_pkFree.pop_back();
			return pkFreeData;
		}
		void Free(T* pkData)
		{
			m_kVct_pkFree.push_back(pkData);
		}
		void FreeAll()
		{
			m_kVct_pkFree=m_kVct_pkData;
		}

		DWORD GetCapacity()
		{
			return m_kVct_pkData.size();
		}

	protected:
		bool __IsValidData(T* pkData)
		{
			if (m_kVct_pkData.end()==std::find(m_kVct_pkData.begin(), m_kVct_pkData.end(), pkData))
				return false;
			return true;
		}
		bool __IsFreeData(T* pkData)
		{
			if (m_kVct_pkFree.end()==std::find(m_kVct_pkFree.begin(), m_kVct_pkFree.end(), pkData))
				return false;

			return true;
		}

		static void Delete(T* pkData)
		{
			delete pkData;
		}

	protected:
		std::vector<T*> m_kVct_pkData;
		std::vector<T*> m_kVct_pkFree;

		UINT m_uInitCapacity;
		UINT m_uUsedCapacity;
};

template<typename T>
class CDynamicPoolEx
{
	public:
		CDynamicPoolEx()
		{
			m_uInitCapacity=0;
			m_uUsedCapacity=0;
		}
		virtual ~CDynamicPoolEx()
		{
			assert(m_kVct_pkFree.size()==m_kVct_pkData.size());
			Destroy();

#ifdef _DEBUG
			char szText[256];
			sprintf(szText, "--------------------------------------------------------------------- %s Pool Capacity %d\n", typeid(T).name(), m_uUsedCapacity);
			OutputDebugString(szText);
			printf(szText);
#endif
		}

		void Clear()
		{
			Destroy();
		}

		void Destroy()
		{
#ifdef _DEBUG
			if (!m_kVct_pkData.empty())
			{
				char szText[256];
				sprintf(szText, "--------------------------------------------------------------------- %s Pool Destroy\n", typeid(T).name());
				OutputDebugString(szText);
				printf(szText);
			}
#endif
			for (auto v : m_kVct_pkData)
				Delete(v);
			m_kVct_pkData.clear();
			m_kVct_pkFree.clear();
		}

		void Create(UINT uCapacity)
		{
			m_uInitCapacity=uCapacity;
			m_kVct_pkData.reserve(uCapacity);
			m_kVct_pkFree.reserve(uCapacity);
		}
		T* Alloc()
		{
			if (m_kVct_pkFree.empty())
			{
				T* pkNewData=New();
				m_kVct_pkData.push_back(pkNewData);
				++m_uUsedCapacity;
				return pkNewData;
			}

			T* pkFreeData=m_kVct_pkFree.back();
			m_kVct_pkFree.pop_back();
			return pkFreeData;
		}
		void Free(T* pkData)
		{
			m_kVct_pkFree.push_back(pkData);
		}
		void FreeAll()
		{
			m_kVct_pkFree=m_kVct_pkData;
		}

		DWORD GetCapacity()
		{
			return m_kVct_pkData.size();
		}

	protected:
		bool __IsValidData(T* pkData)
		{
			if (m_kVct_pkData.end()==std::find(m_kVct_pkData.begin(), m_kVct_pkData.end(), pkData))
				return false;
			return true;
		}
		bool __IsFreeData(T* pkData)
		{
			if (m_kVct_pkFree.end()==std::find(m_kVct_pkFree.begin(), m_kVct_pkFree.end(), pkData))
				return false;

			return true;
		}

		static T* New()
		{
			return (T*)::operator new(sizeof(T));
		}
		static void Delete(T* pkData)
		{
			::operator delete(pkData);
		}

	protected:
		std::vector<T*> m_kVct_pkData;
		std::vector<T*> m_kVct_pkFree;

		UINT m_uInitCapacity;
		UINT m_uUsedCapacity;
};

template <class T>
class CPooledObject
{
	public:
		CPooledObject() {}
		virtual ~CPooledObject() {}

		void * operator new(size_t /*mem_size*/)
		{
			return ms_kPool.Alloc();
		}

		void operator delete(void* pT)
		{
			ms_kPool.Free((T*)pT);
		}

		static void DestroySystem()
		{
			ms_kPool.Destroy();
		}

		static void DeleteAll()
		{
			ms_kPool.FreeAll();
		}

	protected:
		static CDynamicPoolEx<T> ms_kPool;
};

template <class T> CDynamicPoolEx<T> CPooledObject<T>::ms_kPool;
