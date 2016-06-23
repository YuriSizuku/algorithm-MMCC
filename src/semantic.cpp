#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <stack>
#include<sstream>
#include "base.h"
#include "algutil.h"
#include "ioutil.h"
#include "tinyxml.h"
using namespace std;
/* 
	The semantic analylize after syntax analysis
	by recursive decent method decendence
	suppose the syntaxtree is generated correctly
*/
const int TERM=SYNTAXNODE::TYPE_TERM;
const int NTERM=SYNTAXNODE::TYPE_NTERM;
const int END=SYNTAXNODE::TYPE_END;
const int EMPTY=SYNTAXNODE::TYPE_EMPTY;

//declearations
int gen_expr(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics);
int gen_etlist1(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics);
int gen_etlist2(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics);
int gen_etlist3(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics);
int gen_etlist4(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics);
int find_func(const vector<struct FUNCIC> &funcics,string func_name);
int find_var(const vector<struct SYMBOLNODE> &vars,string var_name);
int syntaxDirectedTrans(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics);

struct SLOG
{
	const struct SYNTAXTREE *ptree;
	bool isFirst;
	SLOG(){}
	SLOG(const struct SYNTAXTREE *ptree)
	{
		isFirst=true;
		this->ptree=ptree;
	}
};

int insert_funcdecl(vector<struct FUNCIC> &funcics,string func_name,int type)
{
	unsigned int i;
	struct FUNCIC funcic;
	for(i=0;i<funcics.size();i++)
	{
		if(funcics[i].func_name==func_name)
			return i;
	}
	funcic.func_name=func_name;
	funcic.ret_type=type;
	funcics.push_back(funcic);
	return i;
}
int insert_vardecl(vector<struct SYMBOLNODE> &vars,string var_name,int type,vector<int> *dims)
{
	unsigned int i;
	for(i=0;i<vars.size();i++)
	{
		if(vars[i].name==var_name)
			return i;
	}
	vars.push_back(struct SYMBOLNODE(type,var_name));
	if(dims!=NULL)
		vars[i].dims=*dims;
	return i;
}
int add_quat(vector<struct QUATERNION> &quats,int op,struct ARGNODE *arg1,struct ARGNODE *arg2,struct ARGNODE *result)
{
	struct QUATERNION quat;
	int addr;
	if(quats.size()==0)
		addr=0;
	else addr=quats[quats.size()-1].addr+1;
	quat.op=op;
	if(arg1!=NULL)
		quat.arg1=*arg1;
	if(arg2!=NULL)
		quat.arg2=*arg2;
	if(result!=NULL)
		quat.result=*result;
	quat.addr=quats.size();
	quats.push_back(quat);
	return 0;
}
int form_tmp(int num,struct ARGNODE &arg)
{
	stringstream ss;
	string str;
	arg.arr.clear();
	arg.type=ARGNODE::TYPE_TMP;
	arg.index=num;
	ss<<arg.index;
	ss>>str;
	arg.name="T"+str;
	return 0;
}
int form_consti(int num,struct ARGNODE &arg)
{
	stringstream ss;
	arg.type=ARGNODE::TYPE_INT;
	arg.vari=num;
	arg.arr.clear();
	ss<<arg.vari;
	ss>>arg.name;
	return 0;
}
int form_constf(float num,struct ARGNODE &arg)
{
	stringstream ss;
	arg.type=ARGNODE::TYPE_FLOAT;
	arg.varf=num;
	arg.arr.clear();
	ss<<arg.varf;
	ss>>arg.name;
	return 0;
}
int form_varref(string name,const vector<struct SYMBOLNODE> &vars,struct ARGNODE &arg)
{
	int index;
	arg.type=ARGNODE::TYPE_VAR;
	arg.name=name;
	arg.arr.clear();
	index=find_var(vars,name);
	arg.index=index;
	return index;
}
int form_arrayref(string name,vector<int> *arr,const vector<struct SYMBOLNODE> &vars,struct ARGNODE &arg)
{
	int index;
	arg.type=ARGNODE::TYPE_ARRAY;
	arg.name=name;
	if(arr!=NULL)
		arg.arr=*arr;
	else arg.arr.clear();
	index=find_var(vars,name);
	arg.index=index;
	return index;
}
int form_funcref(string name,vector<int> *func_args,const vector<struct FUNCIC> &funcics,struct ARGNODE &arg)
{
	int index;
	arg.type=ARGNODE::TYPE_FUNC;
	arg.name=name;
	index=find_func(funcics,name);
	arg.index=index;
	if(func_args!=NULL)
		arg.arr=*func_args;
	else arg.arr.clear();
	return index;
}
int form_label(int num,struct ARGNODE &arg)
{
	stringstream ss;
	string str;
	arg.type=ARGNODE::TYPE_LABEL;
	arg.index=num;
	ss<<arg.index;
	ss>>str;
	arg.name="L"+str;
	return 0;
}
int find_func(const vector<struct FUNCIC> &funcics,string func_name)//return index
{
	unsigned int i;
	for(i=0;i<funcics.size();i++)
	{
		if(funcics[i].func_name==func_name)
			break;
	}
	return i;
}
int find_var(const vector<struct SYMBOLNODE> &vars,string var_name)
{
	unsigned int i;
	for(i=0;i<vars.size();i++)
	{
		if(vars[i].name==var_name)
			break;
	}
	return i;
}
int get_type(const struct SYNTAXTREE *ptree)
{
	const struct TOKEN *ptoken;
	if(ptree->data.symbol!=SYNTAXSET::VN.TYPE)
		return SYNTAXNODE::TYPE_INVALID;
	ptoken=ptree->children[0]->data.ptoken;
	if(ptoken==NULL)
		return SYNTAXNODE::TYPE_INVALID;
	return getTokenType(*ptoken);
}
int get_opt(const struct SYNTAXTREE *ptree)
{
	int symbol=ptree->data.symbol;
	const struct TOKEN *ptoken;
	if(symbol!=SYNTAXSET::VN.OPTP1 && 
		symbol!=SYNTAXSET::VN.OPTP2 &&
		symbol!=SYNTAXSET::VN.OPTP3 && 
		symbol!=SYNTAXSET::VN.OPTP4)
		return SYNTAXNODE::TYPE_INVALID;
	if(ptree->children.size()<=0)
		return SYNTAXNODE::TYPE_INVALID;
	ptoken=ptree->children[0]->data.ptoken;
	if(ptoken==NULL)
		return SYNTAXNODE::TYPE_INVALID;
	return getTokenType(*ptoken);
}
string get_identifier(const struct SYNTAXTREE *ptree)
{
	if(ptree->data.symbol!=SYNTAXSET::VT.TKN_ID)
		return "";
	if(ptree->data.ptoken==NULL)
		return "";
	return ptree->data.ptoken->value;
}
int get_consti(const struct SYNTAXTREE *ptree,int *num)
{
	const struct TOKEN *ptoken;
	if(ptree->data.symbol!=SYNTAXSET::VT.TKN_CONSTI)
		return -1;
	ptoken=ptree->data.ptoken;
	if(ptoken==0)
		return -1;
	sscanf(ptoken->value.c_str(),"%d",num);
	return 0;
}
int get_constf(const struct SYNTAXTREE *ptree,float *num)
{
	const struct TOKEN *ptoken;
	if(ptree->data.symbol!=SYNTAXSET::VT.TKN_CONSTF)
		return -1;
	ptoken=ptree->data.ptoken;
	if(ptoken==0)
		return -1;
	sscanf(ptoken->value.c_str(),"%f",num);
	return 0;
}

int gen_funcdecl(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	int type=SYNTAXSET::VT.TKN_INT,symbol;
	unsigned int i,index=0;
	const struct SYNTAXTREE *ptree;
	string str;

	index=funcics.size()+2;
	if(syntaxtree.data.symbol!=SYNTAXSET::VN.FUNCTION)
		return -1;
	//FUNCTION -> TYPE，TKN_ID，TKN_LP，ARGS_DECL，TKN_RP，FUNC_BODY
	for(i=0;i<syntaxtree.children.size();i++)
	{
		symbol=syntaxtree.children[i]->data.symbol;
		ptree=syntaxtree.children[i];
		if(symbol==SYNTAXSET::VN.TYPE)
		{
			type=get_type(ptree);
			if(type==NULL) return -1;
		}
		else if(symbol==SYNTAXSET::VT.TKN_ID)
		{
			str=get_identifier(ptree);
			index=insert_funcdecl(funcics,str,type);
		}
		if(symbol==SYNTAXSET::VN.ARGS_DECL)
		{
			if(index != funcics.size()-1) 
				return -1;
			//ARGS_DECL -> TYPE，TKN_ID，ARGS_NDECL | $
			type=get_type(ptree->children[0]);
			str=get_identifier(ptree->children[1]);
			funcics[index].args.push_back(struct SYMBOLNODE(type,str));
			ptree=ptree->children[2];
			if(ptree->data.symbol!=SYNTAXSET::VN.ARGS_NDECL)
				return -1;
			while(ptree->children.size())
			{
				//ARGS_NDECL -> TKN_COMMA，TYPE，TKN_ID，ARGS_NDECL | $
				type=get_type(ptree->children[1]);
				str=get_identifier(ptree->children[2]);	
				funcics[index].args.push_back(struct SYMBOLNODE(type,str));
				ptree=ptree->children[3];
				if(ptree->data.symbol!=SYNTAXSET::VN.ARGS_NDECL)
					return -1;				
			}
		}
	}
	return 0;
}
int gen_dterm(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics,int type)
{
	int num;
	unsigned int index=0;
	const struct SYNTAXTREE *ptree;
	vector<int> dims;
	string str;

	if(syntaxtree.data.symbol!=SYNTAXSET::VN.DTERM)
			return -1;
	//DTERM -> IDENTIFIER，ARRAY_DECL
	str=get_identifier(syntaxtree.children[0]);
	if(str=="") return -1;
	ptree=syntaxtree.children[1];
	while(ptree->children.size())
	{
		if(ptree->data.symbol!=SYNTAXSET::VN.ARRAY_DECL && ptree->data.symbol!=SYNTAXSET::VN.ARRAY_NDEM)
			return -1;
		//ARRAY_DECL -> TKN_LB2，TKN_CONSTI，TKN_RB2，ARRAY_NDEM | $
		//ARRAY_NDEM -> TKN_LB2，TKN_CONSTI，TKN_RB2，ARRAY_NDEM | $
		if(get_consti(ptree->children[1],&num)!=0)
			return -1;
		dims.push_back(num);
		ptree=ptree->children[3];
	}
	insert_vardecl(funcics[funcics.size()-1].vars,str,type,&dims);
	return 0;
}
int gen_vardecl(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	int type=SYNTAXSET::VT.TKN_INT;
	unsigned int index=0;
	const struct SYNTAXTREE *ptree;
	int ret;


	if(syntaxtree.data.symbol!=SYNTAXSET::VN.DEF)
		return -1;
	//DEF -> TYPE，DTERM，DTLIST
	type=get_type(syntaxtree.children[0]);
	ret=gen_dterm(*syntaxtree.children[1],funcics,type);
	if(ret!=0) return ret;
	ptree=syntaxtree.children[2];
	while(ptree->children.size())
	{
		if(ptree->data.symbol!=SYNTAXSET::VN.DTLIST)
			return -1;
		//DTLIST -> TKN_COMMA，DTERM，DTLIST | $
		ret=gen_dterm(*ptree->children[1],funcics,type);
		if(ret!=0)	return ret;
		ptree=ptree->children[2];
	}
	return 0;
}
int gen_fargs(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics,string id_str)
{
	const struct SYNTAXTREE *ptree;
	struct FUNCIC *pfuncic;
	struct ARGNODE arg1,result;
	vector<int> arr;
	string str;
	int index,ret;
	
	if(syntaxtree.data.symbol!=SYNTAXSET::VN.FA_ARGS)
		return -1;
	//FA_ARGS -> TKN_LP，ARGS_REF，TKN_RP | ARRAY_REF | $
	pfuncic=&funcics[funcics.size()-1];
	if(syntaxtree.children.size()<=0)//id
	{
		index=form_varref(id_str,pfuncic->vars,pfuncic->last_result);
		if(index>=pfuncic->vars.size())
			return -1;
	}
	else if(syntaxtree.children[0]->data.symbol==SYNTAXSET::VN.ARRAY_REF)
	{
		ptree=syntaxtree.children[0];
		//ARRAY_REF     -> TKN_LB2，EXPR，TKN_RB2，ARRAY_NREF
		//ARRAY_NREF    -> TKN_LB2，EXPR，TKN_RB2，ARRAY_NREF |$
		while(ptree->children.size())
		{
			if(ptree->data.symbol!=SYNTAXSET::VN.ARRAY_REF && ptree->data.symbol!=SYNTAXSET::VN.ARRAY_NREF)
				return -1;
			ret=gen_expr(*ptree->children[1],funcics);
			if(ret!=0) return -1;
			if(pfuncic->last_result.type!=ARGNODE::TYPE_TMP)//to fullfil the array invoke quats
			{
				form_tmp(pfuncic->tmp_num++,result);
				add_quat(pfuncic->quats,SYNTAXSET::VT.TKN_ASIGN,&pfuncic->last_result,&result,NULL);
			}
			arr.push_back(pfuncic->tmp_num-1);
			ptree=ptree->children[3];
		}
		pfuncic->tmp_num-=arr.size();//release temp var
		if(pfuncic->tmp_num<0) return -1;
		form_tmp(pfuncic->tmp_num++,result);
		index=form_arrayref(id_str,&arr,pfuncic->vars,arg1);
		if(index>=pfuncic->vars.size())
			return -1;
		add_quat(pfuncic->quats,SYNTAXSET::VN.ARRAY_REF,&arg1,&result,NULL);
		pfuncic->last_result=result;
	}
	else if(syntaxtree.children[1]->data.symbol==SYNTAXSET::VN.ARGS_REF)
	{
		ptree=syntaxtree.children[1];
		//ARGS_REF -> EXPR，ARGS_NREF| $
		if(ptree->data.symbol!=SYNTAXSET::VN.ARGS_REF)
			return -1;
		if(ptree->children.size())
		{
			ret=gen_expr(*ptree->children[0],funcics);
			if(ret!=0) return -1;
			if(pfuncic->last_result.type!=ARGNODE::TYPE_TMP)//to fullfil the array invoke quats
			{
				form_tmp(pfuncic->tmp_num++,result);
				add_quat(pfuncic->quats,SYNTAXSET::VT.TKN_ASIGN,&pfuncic->last_result,NULL,&result);
			}
			arr.push_back(pfuncic->tmp_num-1);
			ptree=ptree->children[1];
			while(ptree->children.size())
			{
				//ARGS_NREF -> TKN_COMMA，EXPR，ARGS_NREF | $
				if(ptree->data.symbol!=SYNTAXSET::VN.ARGS_NREF)
					return -1;
				ret=gen_expr(*ptree->children[1],funcics);
				if(ret!=0) return -1;
				if(pfuncic->last_result.type!=ARGNODE::TYPE_TMP)//to fullfil the array invoke quats
				{
					form_tmp(pfuncic->tmp_num++,result);
					add_quat(pfuncic->quats,SYNTAXSET::VT.TKN_ASIGN,&arg1,NULL,&result);
				}
				arr.push_back(pfuncic->tmp_num-1);
				ptree=ptree->children[2];
			}
		}
		pfuncic->tmp_num-=arr.size();
		if(pfuncic->tmp_num<0) return -1;
		form_tmp(pfuncic->tmp_num++,result);
		index=form_funcref(id_str,&arr,funcics,arg1);
		if(index >= funcics.size())
			return -1;
		add_quat(pfuncic->quats,SYNTAXSET::VN.FUNCTION,&arg1,NULL,&result);
		pfuncic->last_result=result;
	}
	else
		return -1;
	return 0;
}
int gen_eterm(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	int symbol,ret;
	int num_i;
	float num_f;
	unsigned int index=0;
	struct FUNCIC *pfuncic;
	struct ARGNODE arg1,arg2,result;
	string str;

	if(syntaxtree.data.symbol!=SYNTAXSET::VN.ETERM)
		return -1;
	//ETERM  -> TKN_ID，FA_ARGS | CONST | TKN_LP，EXPR，TKN_RP
	pfuncic=&funcics[funcics.size()-1];
	symbol=syntaxtree.children[0]->data.symbol;
	if(symbol==SYNTAXSET::VT.TKN_ID)
	{
		str=get_identifier(syntaxtree.children[0]);
		if(str=="") return -1;
		ret=gen_fargs(*syntaxtree.children[1],funcics,str);
		if(ret!=0) return -1;
	}
	else if(symbol==SYNTAXSET::VN.CONST)
	{
		//CONST -> TKN_CONSTF | TKN_CONSTI
		if(syntaxtree.children[0]->children[0]->data.symbol==SYNTAXSET::VT.TKN_CONSTI)
		{
			if(get_consti(syntaxtree.children[0]->children[0],&num_i)!=0)
				return -1;
			form_consti(num_i,result);
			pfuncic->last_result=result;
		}
		else if(syntaxtree.children[0]->children[0]->data.symbol==SYNTAXSET::VT.TKN_CONSTF)
		{
			if(get_constf(syntaxtree.children[0]->children[0],&num_f)!=0)
				return -1;
			form_constf(num_f,result);
			pfuncic->last_result=result;
		}
		else
			return -1;
	}
	else
	{
		ret=gen_expr(*syntaxtree.children[1],funcics);
		if(ret!=0) return -1;
	}
	return 0;
}
int gen_etlist1(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	int ret;
	int opt;
	const struct SYNTAXTREE *ptree;
	struct FUNCIC *pfuncic;
	struct ARGNODE arg1,arg2,result;
	if(syntaxtree.data.symbol!=SYNTAXSET::VN.ETLIST1)
		return -1;

	//ETLIST1 -> ETLIST2，ETLIST1_C	
	pfuncic=&funcics[funcics.size()-1];
	ret=gen_etlist2(*syntaxtree.children[0],funcics);
	if(ret!=0) return -1;
	arg1=pfuncic->last_result;
	ptree=syntaxtree.children[1];
	while(ptree->children.size())
	{
		//ETLIST1_C -> OPTP1，ETLIST2，ETLIST1_C | $ 
		if(ptree->data.symbol!=SYNTAXSET::VN.ETLIST1_C)
			return -1;
		opt=get_opt(ptree->children[0]);
		ret=gen_etlist2(*ptree->children[1],funcics);
		if(ret!=0) return -1;
		arg2=pfuncic->last_result;
		if(arg1.type==ARGNODE::TYPE_TMP)
			pfuncic->tmp_num--;
		if(arg2.type==ARGNODE::TYPE_TMP)
			pfuncic->tmp_num--;
		form_tmp(pfuncic->tmp_num++,result);
		add_quat(pfuncic->quats,opt,&arg1,&arg2,&result);
		arg1=result;
		pfuncic->last_result=result;
		ptree=ptree->children[2];
	}
	return 0;
}
int gen_etlist2(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	int ret;
	int opt;
	const struct SYNTAXTREE *ptree;
	struct FUNCIC *pfuncic;
	struct ARGNODE arg1,arg2,result;
	if(syntaxtree.data.symbol!=SYNTAXSET::VN.ETLIST2)
		return -1;

	//ETLIST2 -> ETLIST3，ETLIST3_C 
	pfuncic=&funcics[funcics.size()-1];
	ret=gen_etlist3(*syntaxtree.children[0],funcics);
	if(ret!=0) return -1;
	arg1=pfuncic->last_result;
	ptree=syntaxtree.children[1];
	while(ptree->children.size())
	{
		//ETLIST2_C -> OPTP2，ETLIST3，ETLIST2_C | $
		if(ptree->data.symbol!=SYNTAXSET::VN.ETLIST2_C)
			return -1;
		opt=get_opt(ptree->children[0]);
		ret=gen_etlist3(*ptree->children[1],funcics);
		if(ret!=0) return -1;
		arg2=pfuncic->last_result;
		if(arg1.type==ARGNODE::TYPE_TMP)
			pfuncic->tmp_num--;
		if(arg2.type==ARGNODE::TYPE_TMP)
			pfuncic->tmp_num--;
		form_tmp(pfuncic->tmp_num++,result);
		add_quat(pfuncic->quats,opt,&arg1,&arg2,&result);
		arg1=result;
		pfuncic->last_result=result;
		ptree=ptree->children[2];
	}
	return 0;
}
int gen_etlist3(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	int ret;
	int opt;
	const struct SYNTAXTREE *ptree;
	struct FUNCIC *pfuncic;
	struct ARGNODE arg1,arg2,result;
	if(syntaxtree.data.symbol!=SYNTAXSET::VN.ETLIST3)
		return -1;

	//ETLIST3 -> ETLIST4，ETLIST3_C
	pfuncic=&funcics[funcics.size()-1];
	ret=gen_etlist4(*syntaxtree.children[0],funcics);
	if(ret!=0) return -1;
	arg1=pfuncic->last_result;
	ptree=syntaxtree.children[1];
	while(ptree->children.size())
	{
		//ETLIST3_C -> OPTP3，ETLIST4，ETLIST3_C | $
		if(ptree->data.symbol!=SYNTAXSET::VN.ETLIST3_C)
			return -1;
		opt=get_opt(ptree->children[0]);
		ret=gen_etlist4(*ptree->children[1],funcics);
		if(ret!=0) return -1;
		arg2=pfuncic->last_result;
		if(arg1.type==ARGNODE::TYPE_TMP)
			pfuncic->tmp_num--;
		if(arg2.type==ARGNODE::TYPE_TMP)
			pfuncic->tmp_num--;
		form_tmp(pfuncic->tmp_num++,result);
		add_quat(pfuncic->quats,opt,&arg1,&arg2,&result);
		arg1=result;
		pfuncic->last_result=result;
		ptree=ptree->children[2];
	}
	return 0;
}
int gen_etlist4(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	int opt,ret;
	const struct SYNTAXTREE *ptree;
	struct FUNCIC *pfuncic;
	struct ARGNODE arg1,arg2,result;
	if(syntaxtree.data.symbol!=SYNTAXSET::VN.ETLIST4)
		return -1;

	//ETLIST4 -> ETERM，ETLIST4_C
	pfuncic=&funcics[funcics.size()-1];
	ret=gen_eterm(*syntaxtree.children[0],funcics);
	if(ret!=0) return -1;
	arg1=pfuncic->last_result;
	ptree=syntaxtree.children[1];
	while(ptree->children.size())
	{
		//ETLIST4_C -> OPTP4，ETERM，ETLIST4_C | $
		if(ptree->data.symbol!=SYNTAXSET::VN.ETLIST4_C)
			return -1;
		opt=get_opt(ptree->children[0]);
		ret=gen_eterm(*ptree->children[1],funcics);
		if(ret!=0) return -1;
		arg2=pfuncic->last_result;
		if(arg1.type==ARGNODE::TYPE_TMP)
			pfuncic->tmp_num--;
		if(arg2.type==ARGNODE::TYPE_TMP)
			pfuncic->tmp_num--;
		form_tmp(pfuncic->tmp_num++,result);
		add_quat(pfuncic->quats,opt,&arg1,&arg2,&result);
		arg1=result;
		pfuncic->last_result=result;
		ptree=ptree->children[2];
	}
	return 0;
}
int gen_expr(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	int ret;
	if(syntaxtree.data.symbol!=SYNTAXSET::VN.EXPR)
		return -1;
	//EXPR  -> ETLIST1  
	ret=gen_etlist1(*syntaxtree.children[0],funcics);
	return ret;
}
int gen_codeblock(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	if(syntaxtree.data.symbol!=SYNTAXSET::VN.CODE_BLOAK)
		return -1;
	//CODE_BLOAK -> STMT | FUNC_BODY
	return syntaxDirectedTrans(syntaxtree,funcics);
}
int gen_if(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	const struct SYNTAXTREE *ptree;
	struct FUNCIC *pfuncic;
	int idx_qj1,idx_qj2;
	struct ARGNODE arg1,arg2,result;
	if(syntaxtree.data.symbol!=SYNTAXSET::VN.IF_STMT)
		return -1;
	//IF_STMT -> TKN_IF，TKN_LP，EXPR，TKN_RP，CODE_BLOAK，ELSE_STMT
	pfuncic=&funcics[funcics.size()-1];
	gen_expr(*syntaxtree.children[2],funcics);
	add_quat(pfuncic->quats,SYNTAXSET::VT.TKN_JNE,&pfuncic->last_result,NULL,NULL);
	idx_qj1=pfuncic->quats.size()-1;
	gen_codeblock(*syntaxtree.children[4],funcics);
	ptree=syntaxtree.children[5];	
	//ELSE_STMT -> TKN_ELSE，CODE_BLOAK | $
	if(ptree->children.size())
	{
		add_quat(pfuncic->quats,SYNTAXSET::VT.TKN_JMP,NULL,NULL,NULL);
		
		idx_qj2=pfuncic->quats.size()-1;
		form_label(pfuncic->label_num++,arg2);
		pfuncic->quats[idx_qj1].arg2=arg2;	
		add_quat(pfuncic->quats,ARGNODE::TYPE_LABEL,NULL,NULL,&arg2);//add lable
		
		gen_codeblock(*ptree->children[1],funcics);
		form_label(pfuncic->label_num++,arg2);
		pfuncic->quats[idx_qj2].arg2=arg2;
		add_quat(pfuncic->quats,ARGNODE::TYPE_LABEL,NULL,NULL,&arg2);//add lable
	}
	else
	{
		form_label(pfuncic->label_num++,arg2);
		pfuncic->quats[idx_qj1].arg2=arg2;	//fulfill jmp
		add_quat(pfuncic->quats,ARGNODE::TYPE_LABEL,NULL,NULL,&arg2);//add lable
	}
	return 0;
}
int gen_while(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	const struct SYNTAXTREE *ptree;
	struct FUNCIC *pfuncic;
	vector<int> idx_break;
	struct ARGNODE start,end;
	unsigned int i;
	if(syntaxtree.data.symbol!=SYNTAXSET::VN.WHILE_STMT)
		return -1;
	//WHILE_STMT -> TKN_WHILE，TKN_LP，EXPR，TKN_RP，WHILE_BODY
	pfuncic=&funcics[funcics.size()-1];
	form_label(pfuncic->label_num++,start);
	add_quat(pfuncic->quats,ARGNODE::TYPE_LABEL,NULL,NULL,&start);//add start lable
	
	gen_expr(*syntaxtree.children[2],funcics);
	add_quat(pfuncic->quats,SYNTAXSET::VT.TKN_JNE,&pfuncic->last_result,NULL,NULL);
	idx_break.push_back(pfuncic->quats.size()-1);
	ptree=syntaxtree.children[4];
	//WHILE_BODY -> EXPR_STMT | TKN_LB，WHILE_BLOAK，TKN_RB
	if(ptree->children[0]->data.symbol==SYNTAXSET::VN.EXPR_STMT)
	{
		gen_expr(*ptree->children[0]->children[0],funcics);
	}
	else if(ptree->children.size() ==3 &&
		ptree->children[1]->data.symbol==SYNTAXSET::VN.WHILE_BLOAK)
	{
		ptree=ptree->children[1];
		while(ptree->children.size())
		{
			//WHILE_BLOAK -> STMTS，BRE_CONTI，WHILE_BLOAK | $
			if(ptree->data.symbol!=SYNTAXSET::VN.WHILE_BLOAK)
				return -1;
			syntaxDirectedTrans(*ptree->children[0],funcics);
			if(ptree->children[1]->children.size())
			{
				//BRE_CONTI -> TKN_BREAK，TKN_SEMI | TKN_CONTINUE，TKN_SEMI | $
				if(ptree->children[1]->children[0]->data.symbol==SYNTAXSET::VT.TKN_BREAK)
				{
					add_quat(pfuncic->quats,SYNTAXSET::VT.TKN_JMP,NULL,NULL,NULL);
					idx_break.push_back(pfuncic->quats.size()-1);
				}
				else if(ptree->children[1]->children[0]->data.symbol==SYNTAXSET::VT.TKN_CONTINUE)
				{
					add_quat(pfuncic->quats,SYNTAXSET::VT.TKN_JMP,NULL,&start,NULL);
				}
			}
			ptree=ptree->children[2];
		}
	}
	else 
		return -1;
	form_label(pfuncic->label_num++,end);
	add_quat(pfuncic->quats,ARGNODE::TYPE_LABEL,NULL,NULL,&end);//add end lable
	for(i=0;i<idx_break.size();i++)
	{
		pfuncic->quats[idx_break[i]].arg2=end;
	}
	return 0;
}
int gen_return(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	if(syntaxtree.data.symbol!=SYNTAXSET::VN.RET_STMT)
		return -1;
	//RET_STMT -> TKN_RET，EXPR_STMT 
	gen_expr(*syntaxtree.children[1]->children[0],funcics);
	add_quat(funcics[funcics.size()-1].quats,SYNTAXSET::VT.TKN_RET,NULL,NULL,&funcics[funcics.size()-1].last_result);
	return 0;
}
int syntaxDirectedTrans(const struct SYNTAXTREE &syntaxtree,vector<struct FUNCIC> &funcics)
{
	struct SLOG pnode;
	stack<struct SLOG> s;
	int type,symbol;
	int i;
	int ret;

	s.push(struct SLOG(&syntaxtree));
	while(!s.empty())
	{
		pnode=s.top();
		symbol=pnode.ptree->data.symbol;
		if(pnode.isFirst)
		{
			s.pop();
			pnode.isFirst=false;
			s.push(pnode);
			//FUNCTION -> TYPE，TKN_ID，TKN_LP，ARGS_DECL，TKN_RP，FUNC_BODY
			if(symbol==SYNTAXSET::VN.FUNCTION)
			{
				ret=gen_funcdecl(*pnode.ptree,funcics);
				if(ret!=0) 
				{
					cout<<"#error in function!"<<endl;
					return -1;
				}
				for(i=pnode.ptree->children.size()-1;i>=0;i--)
				{
					if(pnode.ptree->children[i]->data.symbol==SYNTAXSET::VN.FUNC_BODY)
						s.push(struct SLOG(pnode.ptree->children[i]));
				}
			}
			//DEF -> TYPE，DTERM，DTLIST
			else if(symbol==SYNTAXSET::VN.DEF)
			{
				ret=gen_vardecl(*pnode.ptree,funcics);
				if(ret!=0)
				{
					cout<<"#error def"<<endl;
					return -1;	
				}
			}
			//EXPR -> ETERM，ETLIST1
			else if(symbol==SYNTAXSET::VN.EXPR)
			{
				ret=gen_expr(*pnode.ptree,funcics);
				if(ret!=0)
				{
					cout<<"#error expr"<<endl;
					return -1;	
				}
			}
			//IF_STMT -> TKN_IF，TKN_LP，EXPR，TKN_RP，CODE_BLOAK，ELSE_STMT
			else if(symbol==SYNTAXSET::VN.IF_STMT)
			{
				ret=gen_if(*pnode.ptree,funcics);
				if(ret!=0) return -1;
			}
			//WHILE_STMT -> TKN_WHILE，TKN_LP，EXPR，TKN_RP，WHILE_BODY
			else if(symbol==SYNTAXSET::VN.WHILE_STMT)
			{
				ret=gen_while(*pnode.ptree,funcics);
				if(ret!=0) return -1;
			}
			//RET_STMT -> TKN_RET，EXPR_STMT 
			else if(symbol==SYNTAXSET::VN.RET_STMT)
			{
				ret=gen_return(*pnode.ptree,funcics);
				if(ret!=0) return -1;
			}
			else
			{
				if(symbol==SYNTAXSET::VN.EXPR_STMT)
				{
					funcics[funcics.size()-1].tmp_num=0;
				}
				for(i=pnode.ptree->children.size()-1;i>=0;i--)
				{
					s.push(struct SLOG(pnode.ptree->children[i]));
				}
			}
		}
		else
		{
			s.pop();
		}
	}
	return 0;
}
int semeticAnalyze(const char *inpath_tree,const char *inpath_token,const char *outpath)
{
	vector<struct TOKEN> tokens;
	struct SYNTAXTREE *syntaxtree=new struct SYNTAXTREE;
	vector<struct FUNCIC> funcics;

	readTokenXml(inpath_token,tokens);//tokens can't be change after that
	readSyntaxTreeXml(inpath_tree,*syntaxtree,tokens);
	if(tokens.size()==9 || syntaxtree==NULL)
		return -1;
	syntaxDirectedTrans(*syntaxtree->children[0],funcics);
	//writeSyntaxTreeXml("d:\\132.xml","test",*syntaxtree->children[0]);//only for debug
	writeIcCodeXml(outpath,outpath,funcics);
	DeleteSyntaxTree(syntaxtree);
	return 0;
}
int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		cout<<"semantic error!"<<endl;
		return -1;
	}
	string str=argv[1];
	int pos;
	pos=str.rfind(".");
	pos=str.rfind(".",pos-1);
	if(pos==string::npos) return -1;
	str=str.substr(0,pos);
	str+=".token.xml";
	semeticAnalyze(argv[1],str.c_str(),argv[2]);
	return 0;
}