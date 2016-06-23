#include <stack>
#include <iterator>
#include <algorithm>
#include <sstream>
#include "base.h"
#include "ioutil.h"
#include "algutil.h"
using namespace std;
const struct TOKEN* getToken(const vector<TOKEN> &tokens,unsigned int num)
{
	if(num>tokens.size() || num<0)
		return NULL;
	if(num==tokens[num-1].num)
		return &tokens[num-1];
	for(unsigned int i=0;i<tokens.size();i++)
	{
		if(num==tokens[i].num)
			return &tokens[i];
	}
	return NULL;
}
int writeTokenXml(const char *outpath,const char *name,const vector<TOKEN> &tokens)
{
	char tmp[200];
	TiXmlDocument *doc = new TiXmlDocument();
	TiXmlDeclaration *decl=new TiXmlDeclaration("1.0","UTF-8","");
    TiXmlElement *eleRoot = new TiXmlElement("project");
	eleRoot->SetAttribute("name",name);
	doc->LinkEndChild(decl);
    doc->LinkEndChild(eleRoot);
    TiXmlElement *eleTokens = new TiXmlElement("tokens");
    eleRoot->LinkEndChild(eleTokens);
	for(unsigned int i=0;i<tokens.size();i++)
	{
        TiXmlElement *eleToken = new TiXmlElement("token");
		
		TiXmlElement *eleNUM   = new TiXmlElement("number");
		TiXmlElement *eleVal   = new TiXmlElement("value");
		TiXmlElement *eleType  = new TiXmlElement("type");
		TiXmlElement *eleLine  = new TiXmlElement("line");
		TiXmlElement *eleValid = new TiXmlElement("valid");
		sprintf(tmp,"%d",tokens[i].num);
		TiXmlText *textNUM     = new TiXmlText(tmp);
		TiXmlText *textVal     = new TiXmlText(tokens[i].value.c_str());
		TiXmlText *textType    = new TiXmlText(tokens[i].type.c_str());
		sprintf(tmp,"%d",tokens[i].line);
		TiXmlText *textLine    = new TiXmlText(tmp);
		tokens[i].valid==true ? strcpy(tmp,"true") : strcpy(tmp,"false");
		TiXmlText *textValid   = new TiXmlText(tmp);
		
		eleNUM->LinkEndChild(textNUM);
		eleVal->LinkEndChild(textVal);
		eleType->LinkEndChild(textType);
		eleLine->LinkEndChild(textLine);
		eleValid->LinkEndChild(textValid);
		
		eleToken->LinkEndChild(eleNUM);
		eleToken->LinkEndChild(eleVal);
		eleToken->LinkEndChild(eleType);
		eleToken->LinkEndChild(eleLine);
		eleToken->LinkEndChild(eleValid);

		eleTokens->LinkEndChild(eleToken);
	}
	doc->SaveFile(outpath);
	return 0;
}
int readTokenXml(const char *inpath,vector<TOKEN> &tokens)
{
	struct TOKEN token;
	TiXmlDocument doc;
	if(!doc.LoadFile(inpath))
		return -1;
	TiXmlElement *eleRoot=doc.FirstChildElement();
	if(eleRoot==NULL) return -1;
	TiXmlElement *eleTokens=eleRoot->FirstChildElement();
	TiXmlElement *eleToken=eleTokens->FirstChildElement();
	for(;eleToken!=NULL;eleToken=eleToken->NextSiblingElement())
	{
		for(TiXmlElement *elem=eleToken->FirstChildElement();
			elem!=NULL;elem=elem->NextSiblingElement())
		{
			string name=elem->Value();
			string text=elem->GetText();
			if(name=="number")
				sscanf(text.c_str(),"%d",&token.num);
			else if(name=="value")
				token.value=text;
			else if(name=="type")
				token.type=text;
			else if(name=="line")
				sscanf(text.c_str(),"%d",&token.line);
			else if(name=="valid")
				token.valid = text=="true" ? true :false;
		}
		tokens.push_back(token);
	}
	return 0;
}
int writeSyntaxTreeXml(char *outpath,const char *name,const struct SYNTAXTREE &syntaxtree)
{
	struct SLOG
	{
		bool isFirst;
		const struct SYNTAXTREE  *ptree;
		TiXmlElement* pxml;
		SLOG(const SYNTAXTREE *ptree,TiXmlElement* pxml)
		{
			this->ptree=ptree;
			isFirst=true;
			this->pxml=pxml;
		}
		SLOG(){}
	};
	stack<struct SLOG> s;
	struct SLOG pnode;
	int i,num;
	string text;

	const int TERM=SYNTAXNODE::TYPE_TERM;
	const int NTERM=SYNTAXNODE::TYPE_NTERM;
	const int END=SYNTAXNODE::TYPE_END;
	const int EMPTY=SYNTAXNODE::TYPE_EMPTY;

	TiXmlDocument *doc = new TiXmlDocument();
	TiXmlDeclaration *decl=new TiXmlDeclaration("1.0","UTF-8","");
    TiXmlElement *eleRoot = new TiXmlElement("PaserTree");
	TiXmlElement *pxnode;
	TiXmlText    *pxtext;
	eleRoot->SetAttribute("name",name);
	text=SYNTAXSET::symStr(syntaxtree.data.symbol);
	pxnode=new TiXmlElement(text.c_str());
	eleRoot->LinkEndChild(pxnode);
	doc->LinkEndChild(decl);
    doc->LinkEndChild(eleRoot);
	
	s.push(struct SLOG(&syntaxtree,pxnode));
	while(!s.empty())
	{
		pnode=s.top();
		if(pnode.isFirst)
		{
			s.pop();
			pnode.isFirst=false;//being the active node only once 
			s.push(pnode);
			num=pnode.ptree->children.size();
			for(i=0;i<num;i++)
			{
				if(pnode.ptree->children[i]->data.ptoken==NULL)//nterms
				{
					text=SYNTAXSET::symStr(pnode.ptree->children[i]->data.symbol);
					pxnode=new TiXmlElement(text.c_str());
				}
				else
				{
					text=pnode.ptree->children[i]->data.ptoken->type;
					pxnode=new TiXmlElement(text.c_str());
					stringstream ss;
					ss<<pnode.ptree->children[i]->data.ptoken->line;
					ss>>text;
					pxnode->SetAttribute("line",text.c_str());
					ss.clear();
					ss<<pnode.ptree->children[i]->data.ptoken->num;
					ss>>text;
					pxnode->SetAttribute("num",text.c_str());
					text=pnode.ptree->children[i]->data.ptoken->value;
					pxtext=new TiXmlText(text.c_str());
					pxnode->LinkEndChild(pxtext);
				}
				pnode.pxml->LinkEndChild(pxnode);
				s.push(struct SLOG(pnode.ptree->children[i],pxnode));
			}
		}
		else
		{
			s.pop();
		}
	}
	doc->SaveFile(outpath);
	return 0;
}
int readSyntaxTreeXml(const char *inpath,struct SYNTAXTREE &syntaxtree,const vector<TOKEN> &tokens)
{
	struct SLOG
	{
		bool isFirst;
		struct SYNTAXTREE  *ptree;
		TiXmlElement* pxml;
		SLOG(SYNTAXTREE *ptree,TiXmlElement* pxml)
		{
			this->ptree=ptree;
			isFirst=true;
			this->pxml=pxml;
		}
		SLOG(){}
	};
	stack<struct SLOG> s;
	struct SLOG pnode;
	struct SYNTAXTREE *ptree;
	string text;
	const struct TOKEN *ptoken;
	const int TERM=SYNTAXNODE::TYPE_TERM;
	const int NTERM=SYNTAXNODE::TYPE_NTERM;
	const int END=SYNTAXNODE::TYPE_END;
	const int EMPTY=SYNTAXNODE::TYPE_EMPTY;

	TiXmlDocument doc;
	if(!doc.LoadFile(inpath))
		return -1;
	TiXmlElement *eleRoot=doc.FirstChildElement();
	TiXmlElement *pxnode;
	if(eleRoot==NULL) return -1;

	syntaxtree.father=NULL;
	s.push(struct SLOG(&syntaxtree,eleRoot));
	while(!s.empty())
	{
		pnode=s.top();
		if(pnode.isFirst)
		{
			s.pop();
			pnode.isFirst=false;//being the active node only once 
			s.push(pnode);
			for(pxnode=pnode.pxml->FirstChildElement();pxnode!=NULL;pxnode=pxnode->NextSiblingElement())
			{
				text=pxnode->Value();
				ptree=new struct SYNTAXTREE;
				ptree->father=pnode.ptree;
				if(SYNTAXSET::symType(text)==NTERM)
				{
					ptree->data.symbol=SYNTAXSET::symId(text);
					ptree->data.ptoken=NULL;
				}
				else
				{
					//text=pxnode->GetText(); the xml text is useless there
					text=pxnode->Attribute("num");
					int num;
					sscanf(text.c_str(),"%d",&num);
					ptoken=getToken(tokens,num);
					if(ptoken==NULL)
						ptree->data.symbol=SYNTAXNODE::TYPE_INVALID;	
					else
						ptree->data.symbol=getTokenType(*ptoken);
					ptree->data.ptoken=ptoken;
				}
				pnode.ptree->children.push_back(ptree);
				s.push(struct SLOG(ptree,pxnode));
			}
		}
		else
		{
			s.pop();
		}
	}
	return 0;
}
int writeIcCodeXml(const char *outpath,const char *name,const std::vector<struct FUNCIC> &funcics)
{
	const struct FUNCIC *pfuncic;
	const struct ARGNODE *parg;
	stringstream ss;
	string str,tstr;

	TiXmlDocument *doc = new TiXmlDocument();
	TiXmlDeclaration *decl=new TiXmlDeclaration("1.0","UTF-8","");
    TiXmlElement *eleRoot = new TiXmlElement("IC");
	TiXmlElement *eleFuncs =  new TiXmlElement("functions");;
	TiXmlElement *eleFunc;
	TiXmlElement *eleArgs;
	TiXmlElement *eleVars;
	TiXmlElement *eleQuats;
	TiXmlElement *eleArg;
	TiXmlElement *eleVar;
	TiXmlElement *eleQuat;
	TiXmlElement *eleOp;
	TiXmlElement *eleArg1;
	TiXmlElement *eleArg2;
	TiXmlElement *eleResult;
	TiXmlText *pxtext; 

	eleRoot->SetAttribute("name",name);
	doc->LinkEndChild(decl);
    doc->LinkEndChild(eleRoot);
    eleRoot->LinkEndChild(eleFuncs);
	
	for(unsigned int i=0;i<funcics.size();i++)
	{
		//function
		pfuncic=&funcics[i];
		eleFunc=new TiXmlElement("function");
		eleFunc->SetAttribute("name",pfuncic->func_name.c_str());
		eleFunc->SetAttribute("ret_type",SYNTAXSET::symStr(pfuncic->ret_type).c_str());
		eleFuncs->LinkEndChild(eleFunc);
		
		//args
		eleArgs=new TiXmlElement("args");
		ss<<pfuncic->args.size();
		ss>>str;
		eleArgs->SetAttribute("size",str.c_str());
		for(unsigned int j=0;j<pfuncic->args.size();j++)
		{
			eleArg=new TiXmlElement("arg");
			eleArg->SetAttribute("type",SYNTAXSET::symStr(pfuncic->args[j].type).c_str());
			eleArg->SetAttribute("name",pfuncic->args[j].name.c_str());
			if(pfuncic->args[j].dims.size()>0)
			{
				str.clear();
				ss.clear();
				for(unsigned int k=0;k<pfuncic->args[j].dims.size();k++)
				{
					str+="[";
					ss.clear();
					ss<<pfuncic->args[j].dims[k];
					ss>>tstr;
					str+=tstr+"]";
				}
				eleArg->SetAttribute("dims",str.c_str());
			}
			eleArgs->LinkEndChild(eleArg);
		}
		eleFunc->LinkEndChild(eleArgs);
		
		//vars
		eleVars=new TiXmlElement("vars");
		ss<<pfuncic->vars.size();
		ss>>str;
		eleVars->SetAttribute("size",str.c_str());
		for(unsigned int j=0;j<pfuncic->vars.size();j++)
		{
			eleVar=new TiXmlElement("var");
			eleVar->SetAttribute("type",SYNTAXSET::symStr(pfuncic->vars[j].type).c_str());
			eleVar->SetAttribute("name",pfuncic->vars[j].name.c_str());
			if(pfuncic->vars[j].dims.size()>0)
			{
				str.clear();
				ss.clear();
				for(unsigned int k=0;k<pfuncic->vars[j].dims.size();k++)
				{
					str+="[";
					ss.clear();
					ss<<pfuncic->vars[j].dims[k];
					ss>>tstr;
					str+=tstr+"]";
				}
				eleVar->SetAttribute("dims",str.c_str());
			}
			eleVars->LinkEndChild(eleVar);
		}
		eleFunc->LinkEndChild(eleVars);
		
		//quats
		eleQuats=new TiXmlElement("quats");
		ss.clear();
		ss<<pfuncic->quats.size();
		ss>>str;
		eleQuats->SetAttribute("size",str.c_str());
		for(unsigned int j=0;j<pfuncic->quats.size();j++)
		{
			eleQuat=new TiXmlElement("quat");
			ss.clear();
			ss<<pfuncic->quats[j].addr;
			ss>>str;
			eleQuat->SetAttribute("addr",str.c_str());
			//op
			eleOp=new TiXmlElement("op");
			str=SYNTAXSET::symStr(pfuncic->quats[j].op);
			if(str!="")
			{
				pxtext=new TiXmlText(str.c_str());
				eleOp->LinkEndChild(pxtext);
			}
			eleQuat->LinkEndChild(eleOp);
			//arg1
			parg=&pfuncic->quats[j].arg1;
			eleArg1=new TiXmlElement("arg1");
			eleArg1->SetAttribute("type",SYNTAXSET::symStr(parg->type).c_str());
			str=parg->name;
			if(parg->type==ARGNODE::TYPE_ARRAY)
			{
				for(unsigned int k=0;k<parg->arr.size();k++)
				{
					str+="[T";
					ss.clear();
					ss<<parg->arr[k];
					ss>>tstr;
					str+=tstr+"]";
				}
			}
			else if(parg->type==ARGNODE::TYPE_FUNC)
			{
				str+="(";
				for(unsigned int k=0;k<parg->arr.size();k++)
				{
					ss.clear();
					ss<<parg->arr[k];
					ss>>tstr;
					str+="T"+tstr;
					if(k<parg->arr.size()-1)
						str+=",";
					
				}
				str+=")";
			}
			if(str!="")
			{
				pxtext=new TiXmlText(str.c_str());
				eleArg1->LinkEndChild(pxtext);
			}
			eleQuat->LinkEndChild(eleArg1);
			//arg2
			parg=&pfuncic->quats[j].arg2;
			eleArg2=new TiXmlElement("arg2");
			eleArg2->SetAttribute("type",SYNTAXSET::symStr(parg->type).c_str());
			str=parg->name;
			if(parg->type==ARGNODE::TYPE_ARRAY)
			{
				for(unsigned int k=0;k<parg->arr.size();k++)
				{
					str+="[T";
					ss.clear();
					ss<<parg->arr[k];
					ss>>tstr;
					str+=tstr+"]";
				}
			}
			else if(parg->type==ARGNODE::TYPE_FUNC)
			{
				str+="(";
				for(unsigned int k=0;k<parg->arr.size();k++)
				{
					ss.clear();
					ss<<parg->arr[k];
					ss>>tstr;
					str+=tstr;
					if(k<parg->arr.size()-1)
						str+=",";
					
				}
				str+=")";
			}	
			if(str!="")
			{
				pxtext=new TiXmlText(str.c_str());
				eleArg2->LinkEndChild(pxtext);
			}
			eleQuat->LinkEndChild(eleArg2);
			//result
			parg=&pfuncic->quats[j].result;
			eleResult=new TiXmlElement("result");
			eleResult->SetAttribute("type",SYNTAXSET::symStr(parg->type).c_str());
			str=parg->name;
			if(parg->type==ARGNODE::TYPE_ARRAY)
			{
				for(unsigned int k=0;k<parg->arr.size();k++)
				{
					str+="[T";
					ss.clear();
					ss<<parg->arr[k];
					ss>>tstr;
					str+=tstr+"]";
				}
			}
			else if(parg->type==ARGNODE::TYPE_FUNC)
			{
				str+="(";
				for(unsigned int k=0;k<parg->arr.size();k++)
				{
					ss.clear();
					ss<<parg->arr[k];
					ss>>tstr;
					str+="T"+tstr;
					if(k<parg->arr.size()-1)
						str+=",";
					
				}
				str+=")";
			}
			if(str!="")
			{
				pxtext=new TiXmlText(str.c_str());
				eleResult->LinkEndChild(pxtext);
			}
			eleQuat->LinkEndChild(eleResult);
			eleQuats->LinkEndChild(eleQuat);
		}
		eleFunc->LinkEndChild(eleQuats);
	}
	doc->SaveFile(outpath);
	return 0;
}