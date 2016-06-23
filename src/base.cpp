#include <iostream>
#include <stdarg.h>  
#include <string>
#include "base.h"
using namespace std;
//error c2784 :    *****need #include<string>

/*WORDSET*/
std::map<std::string,int> WORDSET::table;//must defined outside class...
std::map<int,std::string> WORDSET::table_r;
std::set<std::string> WORDSET::keywords;
std::set<std::string> WORDSET::seperators;
std::set<std::string> WORDSET::operators;
bool WORDSET::isIni=false;
void WORDSET::iniTable() //only 
{
	table["identifier"] =IDENTIFIER;	table_r[IDENTIFIER]="identifier";
	table["keyword"]     =KEYWORD;		table_r[KEYWORD]="keyword";
	table["seperator"]   =SEPERATOR;	table_r[SEPERATOR]="seperator";
	table["operator"]    =OPERATOR;		table_r[OPERATOR]="operator";
	table["const_c"]  =CONST_CHAR;      table_r[CONST_CHAR]="const_c";
	table["const_s"]  =CONST_STRING;     table_r[CONST_STRING]="const_s";
	table["const_i"]  =CONST_INT;       table_r[CONST_INT]="const_i";
	table["const_f"]  =CONST_FLOAT;     table_r[CONST_FLOAT]="const_f";
	table["undefined"]   =UNDEFINED;	table_r[UNDEFINED]="undefined";
}
void WORDSET::iniKeywords()
{
	keywords.insert("void");
	keywords.insert("const");
	keywords.insert("int");
	keywords.insert("long");
	keywords.insert("float");
	keywords.insert("double");
	keywords.insert("if");
	keywords.insert("else");
	keywords.insert("do");
	keywords.insert("while");
	keywords.insert("for");
	keywords.insert("goto");
	keywords.insert("break");
	keywords.insert("continue");
	keywords.insert("return");
	keywords.insert("struct");//may be not consider now
}
void WORDSET::iniSeperators()
{
	seperators.insert("(");
	seperators.insert(")");
	seperators.insert("{");
	seperators.insert("}");
	seperators.insert("[");
	seperators.insert("]");
	seperators.insert("\"");
	seperators.insert("'");
	seperators.insert(";");
	seperators.insert(",");
}
void WORDSET::iniOperators()
{
	operators.insert("+");
	operators.insert("-");
	operators.insert("++");
	operators.insert("--");
	operators.insert("*");
	operators.insert("/");
	operators.insert("%");
	operators.insert(">");
	operators.insert("<");
	operators.insert(">=");
	operators.insert("<=");
	operators.insert("!=");
	operators.insert("=");
	operators.insert("==");
	operators.insert("&&");
	operators.insert("||");
	operators.insert("!");
	operators.insert("|");
	operators.insert("&");
	operators.insert("~");
}
WORDSET::WORDSET()
{
	if(!isIni)
	{
		iniTable();
		iniKeywords();
		iniSeperators();
		iniOperators();
		isIni=true;
	}
}
std::string WORDSET::typeStr(int id)
{
	WORDSET();
	return table_r[id];
}
int WORDSET::typeID(std::string str)
{
	WORDSET();
	return table[str];
}
bool WORDSET::isKeyword(std::string str)
{
	WORDSET();
	std::set<std::string>::iterator iter;
	iter=keywords.find(str);
	if(iter!=keywords.end())
		return true;
	else return false;
}
bool WORDSET::isSeperator(std::string str)
{
	WORDSET();
	std::set<std::string>::iterator iter;
	iter=seperators.find(str);
	if(iter!=seperators.end())
		return true;
	else return false;
}
bool WORDSET::isOperator(std::string str)
{
	WORDSET();
	std::set<std::string>::iterator iter;
	iter=operators.find(str);
	if(iter!=operators.end())
		return true;
	else return false;
}

/*STNTAXSET*/
bool SYNTAXSET::isIni=false;
std::map<int,std::string> SYNTAXSET::terms; //terminators
std::map<std::string,int> SYNTAXSET::terms_r;
std::map<int,std::string> SYNTAXSET::nterms;//non_terminstors;
std::map<std::string,int> SYNTAXSET::nterms_r;
std::vector<PRODNODE> SYNTAXSET::prods;//productions
LL1STABLE SYNTAXSET::LL1reserves;
SYNTAXSET::SYNTAXSET()
{
	if(isIni) return;
	else isIni=true;
	iniTearms();
	iniNterms();
	iniProds();
	iniLL1reserves();
}

struct PRODNODE* SYNTAXSET::addProdLeft(int left)//if error return NULL
{
	struct PRODNODE prodNode;
	prodNode.left=left;
	prods.push_back(prodNode);
	return &prods[prods.size()-1];
}
int SYNTAXSET::addProdRight(struct PRODNODE *pprod,int r1,...)
{    
	va_list argptr;
	va_start(argptr,pprod); //stdarg.h
	if(pprod==NULL) 
	{
		va_end(argptr);
		return SYNTAXNODE::TYPE_INVALID;
	}
	struct SYNTAXNODE sytaxnode={NULL,0};
	vector<struct SYNTAXNODE> prodRight;
	int count=0;
	while(1)
	{
		r1 = va_arg(argptr,int);
		if(r1==0) break;
		sytaxnode.symbol=r1;
		if(symType(r1)==SYNTAXNODE::TYPE_TERM)
		{
			if(sytaxnode.symbol!=VT.TKN_EMPTY)
				sytaxnode.symType=SYNTAXNODE::TYPE_TERM;
			else sytaxnode.symType=SYNTAXNODE::TYPE_EMPTY;
		}
		else
		{
			sytaxnode.symType=SYNTAXNODE::TYPE_NTERM;
		}
		prodRight.push_back(sytaxnode);
		count++;
	}
	pprod->right.push_back(prodRight);
	va_end(argptr);
	return count;
}
void SYNTAXSET::iniTearms()
{
	terms[VT.TKN_INT]="int";
	terms[VT.TKN_FLOAT]="float";
	terms[VT.TKN_ID]="identifier";
	terms[VT.TKN_CONSTF]="const_f";
	terms[VT.TKN_CONSTI]="const_i";
	terms[VT.TKN_RET]="return";
	terms[VT.TKN_IF]="if";
	terms[VT.TKN_ELSE]="else";
	terms[VT.TKN_WHILE]="while";
	terms[VT.TKN_BREAK]="break";
	terms[VT.TKN_CONTINUE]="continue";
	terms[VT.TKN_LP]="(";
	terms[VT.TKN_RP]=")";
	terms[VT.TKN_LB]="{";
	terms[VT.TKN_RB]="}";
	terms[VT.TKN_LB2]="[";
	terms[VT.TKN_RB2]="]";
	terms[VT.TKN_COMMA]=",";
	terms[VT.TKN_SEMI]=";";
	terms[VT.TKN_ASIGN]="=";
	terms[VT.TKN_ADD]="+";
	terms[VT.TKN_SUB]="-";
	terms[VT.TKN_MUL]="*";
	terms[VT.TKN_DIV]="/";
	terms[VT.TKN_EQU]="==";
	terms[VT.TKN_NEQU]="!=";
	terms[VT.TKN_G]=">";
	terms[VT.TKN_L]="<";
	terms[VT.TKN_GE]=">=";
	terms[VT.TKN_LE]="<=";
	terms[VT.TKN_END]="#";
	terms[VT.TKN_EMPTY]="$";


	terms[VT.TKN_GOTO]="goto";
	terms[VT.TKN_JMP]="jmp";
	terms[VT.TKN_JE]="je";
	terms[VT.TKN_JNE]="jne";
	terms[VT.TKN_INVOKE]="invoke";
	terms[VT.TKN_LABLE]="lable";
	iniTerms_r();
}
void SYNTAXSET::iniNterms()
{
	nterms[VN.PROGRAM]="PROGRAM";
	nterms[VN.FUNCTIONS]="FUNCTIONS";
	nterms[VN.FUNCTION]="FUNCTION";
	nterms[VN.FLIST]="FLIST";
	nterms[VN.ARGS_DECL]="ARGS_DECL";
	nterms[VN.ARGS_NDECL]="ARGS_NDECL";
	nterms[VN.FUNC_BODY]="FUNC_BODY";
	nterms[VN.STMTS]="STMTS";
	nterms[VN.STMT]="STMT";
	nterms[VN.EXPR_STMT]="EXPR_STMT";
	nterms[VN.DEF_STMT]="DEF_STMT";
	nterms[VN.IF_STMT]="IF_STMT";
	nterms[VN.RET_STMT]="RET_STMT";
	nterms[VN.CODE_BLOAK]="CODE_BLOAK";
	nterms[VN.ELSE_STMT]="ELSE_STMT";
	nterms[VN.WHILE_STMT]="WHILE_STMT";
	nterms[VN.WHILE_BODY]="WHILE_BODY";
	nterms[VN.WHILE_BLOAK]="WHILE_BLOAK";
	nterms[VN.EXPR]="EXPR";
	nterms[VN.ETERM]="ETERM";
	nterms[VN.FA_ARGS]="FA_ARGS";
	nterms[VN.ARGS_REF]="ARGS_REF";
	nterms[VN.ARGS_NREF]="ARGS_NREF";
	nterms[VN.ARRAY_REF]="ARRAY_REF";
	nterms[VN.ARRAY_NREF]="ARRAY_NREF";
	nterms[VN.ETLIST1]="ETLIST1";
	nterms[VN.ETLIST2]="ETLIST2";
	nterms[VN.ETLIST3]="ETLIST3";
	nterms[VN.ETLIST4]="ETLIST4";
	nterms[VN.ETLIST1_C]="ETLIST1_C";
	nterms[VN.ETLIST2_C]="ETLIST2_C";
	nterms[VN.ETLIST3_C]="ETLIST3_C";
	nterms[VN.ETLIST4_C]="ETLIST4_C";
	nterms[VN.DEF]="DEF";
	nterms[VN.DTERM]="DTERM";
	nterms[VN.DTLIST]="DTLIST";
	nterms[VN.ARRAY_DECL]="ARRAY_DECL";
	nterms[VN.ARRAY_NDEM]="ARRAY_NDEM";
	nterms[VN.TYPE]="TYPE";
	nterms[VN.CONST]="CONST";
	nterms[VN.BRE_CONTI]="BRE_CONTI";
	nterms[VN.OPTP1]="OPTP1";
	nterms[VN.OPTP2]="OPTP2";
	nterms[VN.OPTP3]="OPTP3";
	nterms[VN.OPTP4]="OPTP4";
	iniNterms_r();
}
void SYNTAXSET::iniTerms_r()
{
	map<int,string>::iterator iter;
	for(iter=terms.begin();iter!=terms.end();iter++)
		terms_r[iter->second]=iter->first;
}
void SYNTAXSET::iniNterms_r()
{
	map<int,string>::iterator iter;
	for(iter=nterms.begin();iter!=nterms.end();iter++)
		nterms_r[iter->second]=iter->first;	
}
void SYNTAXSET::iniProds()
{
	struct PRODNODE *pprod;
	//PROGRAM       -> FUNCTIONS
	pprod=addProdLeft(VN.PROGRAM);
	addProdRight(pprod,VN.FUNCTIONS,0);
	//FUNCTIONS     -> FUNCTION，FLIST
	pprod=addProdLeft(VN.FUNCTIONS);
	addProdRight(pprod,VN.FUNCTION,VN.FLIST,0);
	//FLIST         -> FUNCTION，FLIST | $
	pprod=addProdLeft(VN.FLIST);
	addProdRight(pprod,VN.FUNCTION,VN.FLIST,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//FUNCTION      -> TYPE，TKN_ID，TKN_LP，ARGS_DECL，TKN_RP，FUNC_BODY
	pprod=addProdLeft(VN.FUNCTION);
	addProdRight(pprod,VN.TYPE,VT.TKN_ID,VT.TKN_LP,VN.ARGS_DECL,VT.TKN_RP,VN.FUNC_BODY,0);
	//ARGS_DECL    ->  TYPE，TKN_ID，ARGS_NDECL | $
	pprod=addProdLeft(VN.ARGS_DECL);
	addProdRight(pprod,VN.TYPE,VT.TKN_ID,VN.ARGS_NDECL,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//ARGS_NDECL   ->  TKN_COMMA，TYPE，TKN_ID，ARGS_NDECL | $
	pprod=addProdLeft(VN.ARGS_NDECL);
	addProdRight(pprod,VT.TKN_COMMA,VN.TYPE,VT.TKN_ID,VN.ARGS_NDECL,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//FUNC_BODY     -> TKN_LB，STMTS，TKN_RB
	pprod=addProdLeft(VN.FUNC_BODY);
	addProdRight(pprod,VT.TKN_LB,VN.STMTS,VT.TKN_RB,0);
	//STMTS         -> STMT，STMTS | $
	pprod=addProdLeft(VN.STMTS);
	addProdRight(pprod,VN.STMT,VN.STMTS,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//STMT          -> EXPR_STMT | DEF_STMT | IF_STMT | WHILE_STMT | RET_STMT
	pprod=addProdLeft(VN.STMT);
	addProdRight(pprod,VN.EXPR_STMT,0);
	addProdRight(pprod,VN.DEF_STMT,0);
	addProdRight(pprod,VN.IF_STMT,0);
	addProdRight(pprod,VN.WHILE_STMT,0);
	addProdRight(pprod,VN.RET_STMT,0);
	//RET_STMT      -> TKN_RET，EXPR_STMT 
	pprod=addProdLeft(VN.RET_STMT);
	addProdRight(pprod,VT.TKN_RET,VN.EXPR_STMT,0);
	//EXPR_STMT     -> EXPR，TKN_SEMI
	pprod=addProdLeft(VN.EXPR_STMT);
	addProdRight(pprod,VN.EXPR,VT.TKN_SEMI,0);
	//EXPR          -> ETLIST1  
	pprod=addProdLeft(VN.EXPR);
	addProdRight(pprod,VN.ETLIST1,0);
	//ETLIST1       -> ETLIST2，ETLIST1_C
	pprod=addProdLeft(VN.ETLIST1);
	addProdRight(pprod,VN.ETLIST2,VN.ETLIST1_C,0);
	//ETLIST1_C     -> OPTP1，ETLIST2，ETLIST1_C | $ 
	pprod=addProdLeft(VN.ETLIST1_C);
	addProdRight(pprod,VN.OPTP1,VN.ETLIST2,VN.ETLIST1_C,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//ETLIST2       -> ETLIST3，ETLIST2_C
	pprod=addProdLeft(VN.ETLIST2);
	addProdRight(pprod,VN.ETLIST3,VN.ETLIST2_C,0);
	//ETLIST2_C     -> OPTP2，ETLIST3，ETLIST2_C | $ 
	pprod=addProdLeft(VN.ETLIST2_C);
	addProdRight(pprod,VN.OPTP2,VN.ETLIST3,VN.ETLIST2_C,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//ETLIST3       -> ETLIST4，ETLIST3_C
	pprod=addProdLeft(VN.ETLIST3);
	addProdRight(pprod,VN.ETLIST4,VN.ETLIST3_C,0);
	//ETLIST3_C     -> OPTP3，ETLIST4，ETLIST3_C | $ 
	pprod=addProdLeft(VN.ETLIST3_C);
	addProdRight(pprod,VN.OPTP3,VN.ETLIST4,VN.ETLIST3_C,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//ETLIST4       -> ETERM，ETLIST4_C
	pprod=addProdLeft(VN.ETLIST4);
	addProdRight(pprod,VN.ETERM,VN.ETLIST4_C,0);
	//ETLIST4_C     -> OPTP4，ETERM，ETLIST4_C | $ 
	pprod=addProdLeft(VN.ETLIST4_C);
	addProdRight(pprod,VN.OPTP4,VN.ETERM,VN.ETLIST4_C,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//ETERM         -> TKN_ID，FA_ARGS | CONST | TKN_LP，EXPR，TKN_RP
	pprod=addProdLeft(VN.ETERM);
	addProdRight(pprod,VT.TKN_ID,VN.FA_ARGS,0);
	addProdRight(pprod,VN.CONST,0);
	addProdRight(pprod,VT.TKN_LP,VN.EXPR,VT.TKN_RP,0);
	//FA_ARGS       -> TKN_LP，ARGS_REF，TKN_RP | ARRAY_REF | $
	pprod=addProdLeft(VN.FA_ARGS);
	addProdRight(pprod,VT.TKN_LP,VN.ARGS_REF,VT.TKN_RP,0);
	addProdRight(pprod,VN.ARRAY_REF,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//ARGS_REF     -> EXPR，ARGS_NREF| $
	pprod=addProdLeft(VN.ARGS_REF);
	addProdRight(pprod,VN.EXPR,VN.ARGS_NREF,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//ARGS_NREF     -> TKN_COMMA，EXPR，ARGS_NREF | $
	pprod=addProdLeft(VN.ARGS_NREF);
	addProdRight(pprod,VT.TKN_COMMA,VN.EXPR,VN.ARGS_NREF,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//ARRAY_REF     -> TKN_LB2，EXPR，TKN_RB2，ARRAY_NREF
	pprod=addProdLeft(VN.ARRAY_REF);
	addProdRight(pprod,VT.TKN_LB2,VN.EXPR,VT.TKN_RB2,VN.ARRAY_NREF,0);
	//ARRAY_NREF    -> TKN_LB2，EXPR，TKN_RB2，ARRAY_NREF |$
	pprod=addProdLeft(VN.ARRAY_NREF);
	addProdRight(pprod,VT.TKN_LB2,VN.EXPR,VT.TKN_RB2,VN.ARRAY_NREF,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//DEF_STMT      -> DEF，TKN_SEMI
	pprod=addProdLeft(VN.DEF_STMT);
	addProdRight(pprod,VN.DEF,VT.TKN_SEMI,0);
	//DEF           -> TYPE，DTERM，DTLIST
	pprod=addProdLeft(VN.DEF);
	addProdRight(pprod,VN.TYPE,VN.DTERM,VN.DTLIST,0);
	//DTERM         -> IDENTIFIER，ARRAY_DECL
	pprod=addProdLeft(VN.DTERM);
	addProdRight(pprod,VT.TKN_ID,VN.ARRAY_DECL,0);
	//ARRAY_DECL    -> TKN_LB2，TKN_CONSTI，TKN_RB2，ARRAY_NDEM | $
	pprod=addProdLeft(VN.ARRAY_DECL);
	addProdRight(pprod,VT.TKN_LB2,VT.TKN_CONSTI,VT.TKN_RB2,VN.ARRAY_NDEM,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//ARRAY_NDEM    -> TKN_LB2，TKN_CONSTI，TKN_RB2，ARRAY_NDEM | $
	pprod=addProdLeft(VN.ARRAY_NDEM);
	addProdRight(pprod,VT.TKN_LB2,VT.TKN_CONSTI,VT.TKN_RB2,VN.ARRAY_NDEM,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//DTLIST        -> TKN_COMMA，DTERM，DTLIST | $
	pprod=addProdLeft(VN.DTLIST);
	addProdRight(pprod,VT.TKN_COMMA,VN.DTERM,VN.DTLIST,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//IF_STMT       -> TKN_IF，TKN_LP，EXPR，TKN_RP，CODE_BLOAK，ELSE_STMT
	pprod=addProdLeft(VN.IF_STMT);
	addProdRight(pprod,VT.TKN_IF,VT.TKN_LP,VN.EXPR,VT.TKN_RP,VN.CODE_BLOAK,VN.ELSE_STMT,0);
	//CODE_BLOAK    -> STMT | FUNC_BODY
	pprod=addProdLeft(VN.CODE_BLOAK);
	addProdRight(pprod,VN.STMT,0);
	addProdRight(pprod,VN.FUNC_BODY,0);
	//ELSE_STMT     -> TKN_ELSE，CODE_BLOAK | $
	pprod=addProdLeft(VN.ELSE_STMT);
	addProdRight(pprod,VT.TKN_ELSE,VN.CODE_BLOAK,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//WHILE_STMT    -> TKN_WHILE，TKN_LP，EXPR，TKN_RP，WHILE_BODY
	pprod=addProdLeft(VN.WHILE_STMT);
	addProdRight(pprod,VT.TKN_WHILE,VT.TKN_LP,VN.EXPR,VT.TKN_RP,VN.WHILE_BODY,0);
	//WHILE_BODY    -> EXPR_STMT | TKN_LB，WHILE_BLOAK，TKN_RB
	pprod=addProdLeft(VN.WHILE_BODY);
	addProdRight(pprod,VN.EXPR_STMT,0);
	addProdRight(pprod,VT.TKN_LB,VN.WHILE_BLOAK,VT.TKN_RB,0);
	//WHILE_BLOAK   -> STMTS，BRE_CONTI，WHILE_BLOAK | $
	pprod=addProdLeft(VN.WHILE_BLOAK);
	addProdRight(pprod,VN.STMTS,VN.BRE_CONTI,VN.WHILE_BLOAK,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
	//CONST         -> TKN_CONSTF | TKN_CONSTI
	pprod=addProdLeft(VN.CONST);
	addProdRight(pprod,VT.TKN_CONSTF,0);
	addProdRight(pprod,VT.TKN_CONSTI,0);
	//TYPE          -> TKN_INT | TKN_FLOAT
	pprod=addProdLeft(VN.TYPE);
	addProdRight(pprod,VT.TKN_INT,0);
	addProdRight(pprod,VT.TKN_FLOAT,0);
	//OPTP1         -> TKN_ASIGN
	pprod=addProdLeft(VN.OPTP1);
	addProdRight(pprod,VT.TKN_ASIGN,0);
	//OPTP2         -> TKN_EQU | TKN_NEQU | TKN_G | TKN_L | TKN_GE | TKN_LE	
	pprod=addProdLeft(VN.OPTP2);
	addProdRight(pprod,VT.TKN_EQU,0);
	addProdRight(pprod,VT.TKN_NEQU,0);
	addProdRight(pprod,VT.TKN_G,0);
	addProdRight(pprod,VT.TKN_L,0);
	addProdRight(pprod,VT.TKN_GE,0);
	addProdRight(pprod,VT.TKN_LE,0);
	//OPTP3		  -> TKN_ADD | TKN_SUB
	pprod=addProdLeft(VN.OPTP3);
	addProdRight(pprod,VT.TKN_ADD,0);
	addProdRight(pprod,VT.TKN_SUB,0);
	//OPTP4         -> TKN_MUL | TKN_DIV
	pprod=addProdLeft(VN.OPTP4);
	addProdRight(pprod,VT.TKN_MUL,0);
	addProdRight(pprod,VT.TKN_DIV,0);
	//BRE_CONTI     -> TKN_BREAK，TKN_SEMI | TKN_CONTINUE，TKN_SEMI | $
	pprod=addProdLeft(VN.BRE_CONTI);
	addProdRight(pprod,VT.TKN_BREAK,VT.TKN_SEMI,0);
	addProdRight(pprod,VT.TKN_CONTINUE,VT.TKN_SEMI,0);
	addProdRight(pprod,VT.TKN_EMPTY,0);
}
void SYNTAXSET::iniLL1reserves()
{
	//WHILE_BLOAK -> STMTS，BRE_CONTI，WHILE_BLOAK | $
	LL1reserves[pair<int,int>(VN.WHILE_BLOAK,VT.TKN_RB)]=1;
	//BRE_CONTI -> TKN_BREAK，TKN_SEMI | TKN_CONTINUE，TKN_SEMI | $
	LL1reserves[pair<int,int>(VN.BRE_CONTI,VT.TKN_BREAK)]=0;
	LL1reserves[pair<int,int>(VN.BRE_CONTI,VT.TKN_CONTINUE)]=1;
}
const std::vector<struct PRODNODE>& SYNTAXSET::getProds()
{
	SYNTAXSET();
	return prods;
}
const LL1STABLE& SYNTAXSET::getLL1reserves()
{
	SYNTAXSET();
	return LL1reserves;
}
int SYNTAXSET::symType(int id)
{
	SYNTAXSET();
	if(terms.find(id)!=terms.end())
		return SYNTAXNODE::TYPE_TERM;
	else if(nterms.find(id)!=nterms.end())
		return SYNTAXNODE::TYPE_NTERM;
	else return SYNTAXNODE::TYPE_INVALID;
}
int SYNTAXSET::symType(std::string str)
{
	SYNTAXSET();
	if(terms_r.find(str)!=terms_r.end())
		return SYNTAXNODE::TYPE_TERM;
	else if(nterms_r.find(str)!=nterms_r.end())
		return SYNTAXNODE::TYPE_NTERM;
	else return SYNTAXNODE::TYPE_INVALID;
}
std::string SYNTAXSET::symStr(int id)
{
	SYNTAXSET();
	if(terms.find(id)!=terms.end())
		return terms[id];
	else if(nterms.find(id)!=nterms.end())
		return nterms[id];
	else return "";
}
int SYNTAXSET::symId(std::string str)
{
	SYNTAXSET();
	if(terms_r.find(str)!=terms_r.end())
		return terms_r[str];
	else if(nterms_r.find(str)!=nterms_r.end())
		return nterms_r[str];
	else return SYNTAXNODE::TYPE_INVALID;
}

void SYNTAXSET::printProds()
{
	SYNTAXSET();
	for(unsigned int i=0;i<prods.size();i++)
	{
		cout<<SYNTAXSET::symStr(prods[i].left)<<"->";
		for(unsigned int j=0;j<prods[i].right.size();j++)
		{
			for(unsigned int k=0;k<prods[i].right[j].size();k++)
			{
				cout<<SYNTAXSET::symStr(prods[i].right[j][k].symbol);
				if(k<prods[i].right[j].size()-1)
					cout<<"，";
			}
			if(j<prods[i].right.size()-1) cout<<" | ";
		}
		cout<<endl;
	}
}
void SYNTAXSET::printProdsNum()
{
	SYNTAXSET();
	for(unsigned int i=0;i<prods.size();i++)
	{
		cout<<prods[i].left;
		cout<<"->";
		for(unsigned int j=0;j<prods[i].right.size();j++)
		{
			for(unsigned int k=0;k<prods[i].right[j].size();k++)
			{
				cout<<(prods[i].right[j][k].symbol);
				if(k<prods[i].right[j].size()-1)
					cout<<"，";
			}
			if(j<prods[i].right.size()-1) cout<<" | ";
		}
		cout<<endl;
	}
}
void SYNTAXSET::printFirst(FIRST &first)
{
	SYNTAXSET();
	FIRST::iterator iter;
	set<int>::iterator iter2;
	string str;
	int count=0;
	for(iter=first.begin();iter!=first.end();iter++)
	{
		str=symStr(iter->first);
		cout<<str<<"={";
		for(iter2=iter->second.begin();iter2!=iter->second.end();iter2++)
		{
			if(count!=0) cout<<"  ";
			cout<<symStr(*iter2);
			count++;
		}
		cout<<" }"<<endl;
	}
}
void SYNTAXSET::printFirstNum(FIRST &first)
{
	SYNTAXSET();
	FIRST::iterator iter;
	set<int>::iterator iter2;
	string str;
	for(iter=first.begin();iter!=first.end();iter++)
	{
		cout<<iter->first<<"={";
		for(iter2=iter->second.begin();iter2!=iter->second.end();iter2++)
		{
			cout<<" "<<*iter2;
		}
		cout<<" }"<<endl;
	}
}
void SYNTAXSET::printFollow(FOLLOW &follow)
{
	printFirst(follow);
}
void SYNTAXSET::printFollowNum(FOLLOW &follow)
{
	printFirstNum(follow);
}
void SYNTAXSET::printLL1Table(LL1TABLE LL1table)
{
	set<int> terms;
	set<int> nterms;
	set<int>::iterator iter,iter2,iter3;
	struct SYNTAXNODE *synode;
	int i,j,k,left;
	const int TERM=SYNTAXNODE::TYPE_TERM;
	const int NTERM=SYNTAXNODE::TYPE_NTERM;
	const int END=SYNTAXNODE::TYPE_END;
	const int EMPTY=SYNTAXNODE::TYPE_EMPTY;
	
	terms.insert(END);
	for(unsigned i=0;i<prods.size();i++)
	{
		left=prods[i].left;
		nterms.insert(left);
		for(unsigned j=0;j<prods[i].right.size();j++)
		{
			synode=&(prods[i].right[j][0]);
			for(unsigned k=0;k<prods[i].right[j].size();k++)
			{
				if(synode[k].symType==TERM)
					terms.insert(synode[k].symbol);
				else if(synode[k].symType==NTERM)
					nterms.insert(synode[k].symbol);
			}
		}
	}

	for(iter=nterms.begin();iter!=nterms.end();iter++)
	{
		for(iter2=terms.begin();iter2!=terms.end();iter2++)
		{
			if(LL1table[pair<int,int>(*iter,*iter2)].size()>0)
			{
				cout<<"<"<<symStr(*iter)<<","<<symStr(*iter2)<<">=";
				for(iter3=LL1table[pair<int,int>(*iter,*iter2)].begin();
					iter3!=LL1table[pair<int,int>(*iter,*iter2)].end();iter3++)
				{
					cout<<" "<<*iter3;
				}
				cout<<" "<<endl;
			}
		}
	}
}
void SYNTAXSET::printLL1STable(LL1STABLE LL1Stable)
{
	LL1STABLE::iterator iter;
	for(iter=LL1Stable.begin();iter!=LL1Stable.end();iter++)
	{
		cout<<"<"<<symStr(iter->first.first)<<","<<symStr(iter->first.second)<<">=";
		cout<<" "<<iter->second<<" "<<endl;
	}
}