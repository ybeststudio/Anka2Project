#ifndef _EL_CPOSTIT_H_
#define _EL_CPOSTIT_H_

// _CPostItMemoryBlock is defined in CPostIt.cpp
class _CPostItMemoryBlock;


class CPostIt
{
public:

	explicit CPostIt( LPCSTR szAppName );

	/**
	 * @brief	CPostIt destructor
	 */
	~CPostIt( void );


	BOOL	Flush( void );


	void	Empty( void );


	BOOL	Get( LPCSTR lpszKeyName, LPSTR lpszData, DWORD nSize );


	BOOL	Set( LPCSTR lpszData );


	BOOL	Set( LPCSTR lpszKeyName, LPCSTR lpszData );


	BOOL	Set( LPCSTR lpszKeyName, DWORD dwValue );


	BOOL	CopyTo( CPostIt *pPostIt, LPCSTR lpszKeyName );

protected:
	BOOL					Init( LPCSTR szAppName );
	void					Destroy( void );

protected:
	BOOL					m_bModified;
	CHAR					m_szClipFormatName[_MAX_PATH];
	_CPostItMemoryBlock*	m_pMemoryBlock;
};

#endif /* _EL_CPOSTIT_H_ */
