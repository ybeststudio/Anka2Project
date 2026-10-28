#ifndef __CSVFILE_H__
#define __CSVFILE_H__

#include <string>
#include <vector>

#if _MSC_VER
    //#include <hash_map>
    #include <unordered_map>
#else
    #include <map>
#endif

////////////////////////////////////////////////////////////////////////////////
/// \class cCsvAlias
/// \brief CSV 파일을 수정했을 때 발생하는 인덱스 문제를 줄이기 위한 
/// 별명 객체.
///
/// 예를 들어 0번 컬럼이 A에 관한 내용을 포함하고, 1번 컬럼이 B에 관한 내용을 
/// 포함하고 있었는데...
///
/// <pre>
/// int a = row.AsInt(0);
/// int b = row.AsInt(1);
/// </pre>
///
/// 그 사이에 C에 관한 내용을 포함하는 컬럼이 끼어든 경우, 하드코딩되어 있는 
/// 1번을 찾아서 고쳐야 하는데, 상당히 에러가 발생하기 쉬운 작업이다. 
///
/// <pre>
/// int a = row.AsInt(0);
/// int c = row.AsInt(1);
/// int b = row.AsInt(2); <-- 이 부분을 일일이 신경써야 한다.
/// </pre>
///
/// 이 부분을 문자열로 처리하면 유지보수에 들어가는 수고를 약간이나마 줄일 수 
/// 있다.
////////////////////////////////////////////////////////////////////////////////

class cCsvAlias
{
private:
#if _MSC_VER
    typedef std::unordered_map<std::string, size_t> NAME2INDEX_MAP;
    typedef std::unordered_map<size_t, std::string> INDEX2NAME_MAP;
#else
    typedef std::map<std::string, size_t> NAME2INDEX_MAP;
    typedef std::map<size_t, std::string> INDEX2NAME_MAP;
#endif

    NAME2INDEX_MAP m_Name2Index;  ///< 셀 인덱스 대신으로 사용하기 위한 이름들
    INDEX2NAME_MAP m_Index2Name;  ///< 잘못된 alias를 검사하기 위한 추가적인 맵


public:
    cCsvAlias() {}

    virtual ~cCsvAlias() {}


public:
    void AddAlias(const char* name, size_t index);

    void Destroy();

    const char* operator [] (size_t index) const;

    size_t operator [] (const char* name) const;


private:
    cCsvAlias(const cCsvAlias&) {}

    const cCsvAlias& operator = (const cCsvAlias&) { return *this; }
};

////////////////////////////////////////////////////////////////////////////////
/// \class cCsvRow 
/// \brief CSV 파일의 한 행을 캡슐화한 클래스
///
/// CSV의 기본 포맷은 엑셀에서 보이는 하나의 셀을 ',' 문자로 구분한 것이다.
/// 하지만, 셀 안에 특수 문자로 쓰이는 ',' 문자나 '"' 문자가 들어갈 경우, 
/// 모양이 약간 이상하게 변한다. 다음은 그 변화의 예이다.
///
/// <pre>
/// 엑셀에서 보이는 모양 | 실제 CSV 파일에 들어가있는 모양
/// ---------------------+----------------------------------------------------
/// ItemPrice            | ItemPrice
/// Item,Price           | "Item,Price"
/// Item"Price           | "Item""Price"
/// "ItemPrice"          | """ItemPrice"""
/// "Item,Price"         | """Item,Price"""
/// Item",Price          | "Item"",Price"
/// </pre>
/// 
/// 이 예로서 다음과 같은 사항을 알 수 있다.
/// - 셀 내부에 ',' 또는 '"' 문자가 들어갈 경우, 셀 좌우에 '"' 문자가 생긴다.
/// - 셀 내부의 '"' 문자는 2개로 치환된다.
///
/// \sa cCsvFile
////////////////////////////////////////////////////////////////////////////////

class cCsvRow : public std::vector<std::string>
{
public:
    cCsvRow() {}

    ~cCsvRow() {}


public:
    int AsInt(size_t index) const { return atoi(at(index).c_str()); }

    double AsDouble(size_t index) const { return atof(at(index).c_str()); }

    const char* AsString(size_t index) const { return at(index).c_str(); }

    int AsInt(const char* name, const cCsvAlias& alias) const {
        return atoi( at(alias[name]).c_str() ); 
    }

    double AsDouble(const char* name, const cCsvAlias& alias) const {
        return atof( at(alias[name]).c_str() ); 
    }

    const char* AsString(const char* name, const cCsvAlias& alias) const { 
        return at(alias[name]).c_str(); 
    }


private:
    cCsvRow(const cCsvRow&) {}

    const cCsvRow& operator = (const cCsvRow&) { return *this; }
};


////////////////////////////////////////////////////////////////////////////////
/// \class cCsvFile
/// \brief CSV(Comma Seperated Values) 파일을 read/write하기 위한 클래스
///
/// <b>sample</b>
/// <pre>
/// cCsvFile file;
///
/// cCsvRow row1, row2, row3;
/// row1.push_back("ItemPrice");
/// row1.push_back("Item,Price");
/// row1.push_back("Item\"Price");
///
/// row2.reserve(3);
/// row2[0] = "\"ItemPrice\"";
/// row2[1] = "\"Item,Price\"";
/// row2[2] = "Item\",Price\"";
///
/// row3 = "\"ItemPrice\"\"Item,Price\"Item\",Price\"";
///
/// file.add(row1);
/// file.add(row2);
/// file.add(row3);
/// file.save("test.csv", false);
/// </pre>
///
/// \todo 파일에서만 읽어들일 것이 아니라, 메모리 소스로부터 읽는 함수도 
/// 있어야 할 듯 하다.
////////////////////////////////////////////////////////////////////////////////

class cCsvFile
{
private:
    typedef std::vector<cCsvRow*> ROWS;

    ROWS m_Rows; ///< 행 컬렉션


public:
    cCsvFile() {}

    /// \brief 소멸자
    virtual ~cCsvFile() { Destroy(); }


public:
    bool Load(const char* fileName, const char seperator=',', const char quote='"');

    bool Save(const char* fileName, bool append=false, char seperator=',', char quote='"') const;

    void Destroy();

    cCsvRow* operator [] (size_t index);

    const cCsvRow* operator [] (size_t index) const;

    size_t GetRowCount() const { return m_Rows.size(); }


private:
    cCsvFile(const cCsvFile&) {}

    const cCsvFile& operator = (const cCsvFile&) { return *this; }
};


////////////////////////////////////////////////////////////////////////////////
/// \class cCsvTable
/// \brief CSV 파일을 이용해 테이블 데이터를 로드하는 경우가 많은데, 이 클래스는 
/// 그 작업을 좀 더 쉽게 하기 위해 만든 유틸리티 클래스다.
///
/// CSV 파일을 로드하는 경우, 숫자를 이용해 셀을 액세스해야 하는데, CSV 
/// 파일의 포맷이 바뀌는 경우, 이 숫자들을 변경해줘야한다. 이 작업이 꽤 
/// 신경 집중을 요구하는 데다가, 에러가 발생하기 쉽다. 그러므로 숫자로 
/// 액세스하기보다는 문자열로 액세스하는 것이 약간 느리지만 낫다고 할 수 있다.
///
/// <b>sample</b>
/// <pre>
/// cCsvTable table;
///
/// table.alias(0, "ItemClass");
/// table.alias(1, "ItemType");
///
/// if (table.load("test.csv"))
/// {
///     while (table.next())
///     {
///         std::string item_class = table.AsString("ItemClass");
///         int         item_type  = table.AsInt("ItemType"); 
///     }
/// }
/// </pre>
////////////////////////////////////////////////////////////////////////////////

class cCsvTable
{
public :
    cCsvFile  m_File;   ///< CSV 파일 객체
private:
    cCsvAlias m_Alias;  ///< 문자열을 셀 인덱스로 변환하기 위한 객체
    int       m_CurRow; ///< 현재 횡단 중인 행 번호


public:
    cCsvTable();

    virtual ~cCsvTable();


public:
    bool Load(const char* fileName, const char seperator=',', const char quote='"');

    void AddAlias(const char* name, size_t index) { m_Alias.AddAlias(name, index); }

    bool Next();

    size_t ColCount() const;

    int AsInt(size_t index) const;

    double AsDouble(size_t index) const;

    const char* AsStringByIndex(size_t index) const;

    int AsInt(const char* name) const { return AsInt(m_Alias[name]); }

    double AsDouble(const char* name) const { return AsDouble(m_Alias[name]); }

    const char* AsString(const char* name) const { return AsStringByIndex(m_Alias[name]); }

    void Destroy();


private:
    const cCsvRow* const CurRow() const;

    cCsvTable(const cCsvTable&) {}

    const cCsvTable& operator = (const cCsvTable&) { return *this; }
};

#endif //__CSVFILE_H__
