#pragma once

class CUndoBuffer
{
	public:
		class IUndoData
		{
			public:
				IUndoData() {}
				virtual ~IUndoData() {}

				virtual void Backup() = 0;
				virtual void Restore() = 0;

				virtual void BackupStatement() = 0;
				virtual void RestoreStatement() = 0;
		};
		typedef std::deque<IUndoData*>		TUndoDataDeque;
		typedef TUndoDataDeque::iterator	TUndoDataIterator;

	public:
		CUndoBuffer();
		~CUndoBuffer();

		void ClearTail (DWORD dwIndex);

		void Backup (IUndoData * pData);
		void BackupCurrent (IUndoData * pData);

		void Undo();
		void Redo();

	protected:
		bool GetUndoData (DWORD dwIndex, IUndoData** ppUndoData);

	protected:
		DWORD m_dwCurrentStackPosition;
		TUndoDataDeque m_UndoDataDeque;

		IUndoData* m_pTopData;

		const unsigned char m_ucMAXBufferCount;
};