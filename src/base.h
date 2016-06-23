#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <iterator>
#include <string>
/*
	1.the compiler's basic data structures 
	2.some useful functions about it
	3.The class is only package of the struct and doesn't provide parsing fuctions
*/
#ifndef _BASE_H
#define _BASE_H
#define VALUE_END 1950
#define VALUE_EMPTY 1955

//scanning
struct TOKEN
{
	int num;
	std::string value;
	std::string type;
	int line;
	bool valid;
};
class WORDSET
{
	static bool isIni;
	static std::map<std::string,int> table;
	static std::map<int,std::string> table_r;
	static std::set<std::string> keywords;
	static std::set<std::string> seperators;
	static std::set<std::string> operators;
	static void iniTable();
	static void iniKeywords();
	static void iniSeperators();
	static void iniOperators();
public:
	static const int IDENTIFIER=0;
	static const int KEYWORD=1;
	static const int SEPERATOR=2;
	static const int OPERATOR=3;
	static const int CONST_INT=4;
	static const int CONST_FLOAT=5;
	static const int CONST_CHAR=6;
	static const int CONST_STRING=7;
	static const int UNDEFINED=100;
	WORDSET();
	static std::string typeStr(int id);
	static int typeID(std::string str);
	static bool isKeyword(std::string str);
	static bool isSeperator(std::string str);
	static bool isOperator(std::string str);
};

//parsing
struct SYNTAXNODE
{
	union
	{
		const struct TOKEN  *ptoken;//pToken==NULL non-terminator
		int symType;
	};
	int symbol;//the symbol must >=1000
	static const int TYPE_TERM=501;
	static const int TYPE_NTERM=502;
	static const int TYPE_END=VALUE_END;
	static const int TYPE_EMPTY=VALUE_EMPTY;
	static const int TYPE_INVALID=0;
};
struct SYNTAXTREE;
struct SYNTAXTREE
{
	SYNTAXNODE data;
	SYNTAXTREE* father;
	std::vector<SYNTAXTREE *> children;
};
typedef std::vector<struct PRODNODE>
		PRODUCTIONS;
typedef std::map<int,std::set<int>> 
	FIRST,FOLLOW;
typedef std::map<std::pair<int,int>,std::set<int>>
	LL1TABLE;
typedef std::map<std::pair<int,int>,int>
	LL1STABLE;//single table
struct PRODNODE
{
	int left;
	std::vector<std::vector<struct SYNTAXNODE>> right; 
};
struct TERMSET //termination set
{
	static const int TKN_INT=1001;
	static const int TKN_FLOAT=1002;
	static const int TKN_ID=1003;
	static const int TKN_CONSTF=1004;
	static const int TKN_CONSTI=1005;
	static const int TKN_RET=1006;
	static const int TKN_IF=1007;
	static const int TKN_ELSE=1008;
	static const int TKN_WHILE=1009;
	static const int TKN_BREAK=1010;
	static const int TKN_CONTINUE=1011;
	static const int TKN_LP=1012;
	static const int TKN_RP=1013;
	static const int TKN_LB=1014;
	static const int TKN_RB=1015;
	static const int TKN_LB2=1016;
	static const int TKN_RB2=1017;
	static const int TKN_COMMA=1018;
	static const int TKN_SEMI=1019;
	static const int TKN_ASIGN=1020;
	static const int TKN_ADD=1021;
	static const int TKN_SUB=1022;
	static const int TKN_MUL=1023;
	static const int TKN_DIV=1024;
	static const int TKN_EQU=1025;
	static const int TKN_NEQU=1026;
	static const int TKN_G=1027;
	static const int TKN_L=1028;
	static const int TKN_GE=1029;
	static const int TKN_LE=1030;
	static const int TKN_END=VALUE_END;
	static const int TKN_EMPTY=VALUE_EMPTY;

	//this values are used in semetic analyze
	static const int TKN_GOTO=1080;
	static const int TKN_JMP=1081;
	static const int TKN_INVOKE=1082;
	static const int TKN_JE=1083;
	static const int TKN_JNE=1084;
	static const int TKN_LABLE=1085;
};
struct NTERMSET //non termination set
{
	static const int PROGRAM=2000;
	static const int FUNCTIONS=2001;
	static const int FUNCTION=2002;
	static const int FLIST=2003;
	static const int ARGS_DECL=2004;
	static const int ARGS_NDECL=2005;
	static const int FUNC_BODY=2006;
	static const int STMTS=2007;
	static const int STMT=2008;
	static const int EXPR_STMT=2009;
	static const int DEF_STMT=2010;
	static const int IF_STMT=2011;
	static const int RET_STMT=2012;
	static const int CODE_BLOAK=2013;
	static const int ELSE_STMT=2014;
	static const int WHILE_STMT=2015;
	static const int WHILE_BODY=2016;
	static const int WHILE_BLOAK=2017;
	static const int EXPR=2018;
	static const int ETERM=2019;
	static const int FA_ARGS=2020;
	static const int ARGS_REF=2021;
	static const int ARGS_NREF=2022;
	static const int ARRAY_REF=2023;
	static const int ARRAY_NREF=2024;
	static const int ETLIST1=2025;
	static const int ETLIST1_C=2026;
	static const int ETLIST2=2027;
	static const int ETLIST2_C=2028;
	static const int ETLIST3=2029;
	static const int ETLIST3_C=2030;
	static const int ETLIST4=2031;
	static const int ETLIST4_C=2032;
	static const int DEF=2033;
	static const int DTERM=2034;
	static const int DTLIST=2035;
	static const int ARRAY_DECL=2036;
	static const int ARRAY_NDEM=2037;
	static const int TYPE=2038;
	static const int CONST=2039;
	static const int OPTP1=2040;
	static const int OPTP2=2041;
	static const int OPTP3=2042;
	static const int OPTP4=2043;
	static const int BRE_CONTI=2044;
};
class SYNTAXSET
{
	static bool isIni;
	static std::map<int,std::string> terms; //terminators
	static std::map<std::string,int> terms_r;
	static std::map<int,std::string> nterms;//non_terminstors;
	static std::map<std::string,int> nterms_r;
	static std::vector<struct PRODNODE> prods;//productions
	static LL1STABLE LL1reserves;

	static struct PRODNODE* addProdLeft(int left);//if error return NULL
	static int addProdRight(struct PRODNODE *prod,int r1,...);//the end is 0
	static void iniTearms();
	static void iniTerms_r();
	static void iniNterms();
	static void iniNterms_r();
	static void iniLL1reserves();
	static void iniProds();

public:
	static const struct TERMSET VT;
	static const struct NTERMSET VN;
    SYNTAXSET();

	static const std::vector<struct PRODNODE>& getProds();
	static const LL1STABLE& getLL1reserves();
	static int symType(int id);//symbol type
	static int symType(std::string str);
	static std::string symStr(int id);
	static int symId(std::string str);

	//debug
	static void printProds();
	static void printProdsNum();
	static void printFirst(FIRST &first);
	static void printFirstNum(FIRST &first);
	static void printFollow(FOLLOW &follow);
	static void printFollowNum(FOLLOW &follow);
	static void printLL1Table(LL1TABLE LL1table);
	static void printLL1STable(LL1STABLE LL1Stable);
};

//semantic
struct ARGNODE
{
	std::string name;
	union
	{
		int index;
		float varf;
		int vari;
	};
	int type;
	std::vector<int> arr;//the array position coordinate or function args (the number of the temp var)
	ARGNODE()
	{
		type=TYPE_EMPTY;
	}
	static const int TYPE_INT=TERMSET::TKN_CONSTI;
	static const int TYPE_FLOAT=TERMSET::TKN_CONSTF;
	static const int TYPE_ARRAY=NTERMSET::ARRAY_REF;
	static const int TYPE_TMP=NTERMSET::EXPR;
	static const int TYPE_VAR=TERMSET::TKN_ID;
	static const int TYPE_FUNC=NTERMSET::FUNCTION;
	static const int TYPE_EMPTY=TERMSET::TKN_EMPTY;
	static const int TYPE_LABEL=TERMSET::TKN_LABLE;
};
struct SYMBOLNODE
{
	int type;
	std::string name;
	std::vector<int> dims;//demention size;if null mains virables else mains arrays
	SYMBOLNODE(){}
	SYMBOLNODE(int type,std::string name)
	{
		this->type=type;
		this->name=name;
	}
};
struct QUATERNION
{
	int op;
	struct ARGNODE arg1;
	struct ARGNODE arg2;
	struct ARGNODE result;
	int addr;
};
struct FUNCIC//function intermediate code
{
	std::string func_name;
	int ret_type;
	std::vector<struct SYMBOLNODE> args;
	std::vector<struct SYMBOLNODE> vars;
	std::vector<struct QUATERNION> quats;//quaternions
	
	//intermediate values
	int tmp_num;//temp var number
	int label_num;
	struct ARGNODE last_result;
	FUNCIC()
	{
		tmp_num=0;
		label_num=0;
	}
};
#endif