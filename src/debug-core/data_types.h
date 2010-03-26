#ifndef DATA_TYPES_H
#define DATA_TYPES_H


/** Base class for aall data types usd to build data structure hirarchies
	out of the set of base types.
*/
class DataType
{
public:
	DataType();
	~DataType();
	
	uint16_t Size()	{ return size; }
	string descr()	{ return "Unknown"; }
protected:
	uint16_t m_size;
};


typedef vector<DataType> DATA_TYPE_VECTOR;

class BaseType
{
public:
	typedef enum 
	{
		SHORT=0,
		LONG,
		NONE
	} MODIFIER;
	string ModifierStr[] = { "short","long","" };
	
	MODIFIER Modifier()		{ return m_Modifier; }
	bool IsSigned()			{ return m_bSigned; }
	string descr()			{ return "Base Type unknown"; }
	
protected:
	MODIFIER	m_modifier;
	bool		m_bSigned;
};

class BaseTypeInt
{
	BaseTypeInt( MODIFIER m, bool bSigned )
	{
		m_bSigned = bSigned;
		m_Modifier = m;
		switch(m)
		{
		case SHORT:	size = 1; break;
		case NONE:	size = 2; break;
		case LONG:	size = 4; break;
		}
	}
	string descr()
	{
		string r;
		r += m_bSigned ? "" : "unsigned ";
		r += ModifierStr[m_Modifier];
		r += " int"
		return r;
	}
};

class BaseTypeChar
{
	BaseTypeChar( MODIFIER m, bool bSigned )
	{
		m_bSigned = bSigned;
		m_Modifier = m;
		switch(m)
		{
			case SHORT:	size = 1; break;
			case NONE:	size = 1; break;
			case LONG:	size = 1; break;
		}
	}
	string descr()
	{
		string r;
		r += m_bSigned ? "" : "unsigned ";
		r += ModifierStr[m_Modifier];
		r += " char"
		return r;
	}
};

class BaseTypeFloat
{
	// @FIXME: floats don't support signed/unsigned or short/long modifications...
	//  this class needs rewriting.
	BaseTypeFloat( MODIFIER m, bool bSigned )
	{
		m_bSigned = bSigned;
		m_Modifier = m;
		switch(m)
		{
			case SHORT:	size = 1; break;	// ILLEGAL
			case NONE:	size = 1; break;
			case LONG:	size = 1; break;	// ILLEGAL
		}
	}
	string descr()
	{
		string r;
		r += m_bSigned ? "" : "unsigned ";
		r += ModifierStr[m_Modifier];
		r += " float"
		return r;
	}
};

/** used to build an array of any data type.
*/
class ArrayType : public DataType
{
public:
	ArrayType( DataType dt, int NumElements )
	{
	}
	string descr()	{ return "array"; }
protected:
	int m_NumElements;
	DataType m_Type;
};

class StructType : public DataType
{
public:
	StructType()	{}
	~StructType()	{}
	string descr()	{ return "struct"; }
protected:
	DATA_TYPE_VECTOR m_Elements;
	vector<string> mElementNames;
};



#endif // DATA_TYPES_H
