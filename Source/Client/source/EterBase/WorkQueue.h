#pragma once
#include <Windows.h>
#include <mutex>
#include <deque>

template <typename T>
class CQueue
{
	typedef std::function<void(T)> TCustomCallback;

	public:
		CQueue() : m_pCallback(nullptr) { }
		~CQueue() = default;

		void BindCallback(TCustomCallback pFunc)
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);
			
			m_pCallback = pFunc;
		}

		size_t GetWorkObjectsSize()
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			return m_kWorkList.size();
		}

		bool HasWorkObject()
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			return (m_kWorkList.size() > 0);
		}

		template <class cData>
		bool InsertObject(cData data)
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			// If already queue'd object, dont add duplicate
			if (m_kWorkList.size() > 0 && std::find(m_kWorkList.begin(), m_kWorkList.end(), data) != m_kWorkList.end())
				return false;

			if (m_kCheckedList.size() > 0 && std::find(m_kCheckedList.begin(), m_kCheckedList.end(), data) != m_kCheckedList.end())
				return false;

			// Add to queue
			m_kWorkList.push_back(data);
			return true;
		}

		void ProcessFirstObject()
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

#ifdef _DEBUG
			if (!m_pCallback)
			{
//				g_nmApp->LogHelperInstance()->sys_log(__FUNCTION__, LL_CRI, "Null callback ptr!!!");
				return;
			}
#endif

			if (!m_kWorkList.empty())
			{
				m_pCallback(m_kWorkList.front());
				m_kCheckedList.push_back(m_kWorkList.front());
				m_kWorkList.pop_front();
			}
		}

		T GetFirstObject()
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			return m_kWorkList.front();
		}

		void RemoveFirstObject()
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			m_kCheckedList.push_back(m_kWorkList.front());
			m_kWorkList.pop_front();
		}

		template <class cData>
		bool HasListed(cData data)
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			if (m_kWorkList.size() > 0 && std::find(m_kWorkList.begin(), m_kWorkList.end(), data) != m_kWorkList.end())
				return true;

			return false;
		}

		template <class cData>
		bool HasProcessed(cData data)
		{
			std::lock_guard <std::recursive_mutex> __lock(m_Mutex);

			if (m_kCheckedList.size() > 0 && std::find(m_kCheckedList.begin(), m_kCheckedList.end(), data) != m_kCheckedList.end())
				return true;

			return false;
		}

	private:
		mutable std::recursive_mutex m_Mutex;

		std::deque <T> m_kWorkList;
		std::deque <T> m_kCheckedList;

		TCustomCallback m_pCallback;
};
