#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include<sstream>
#include "tinyxml.h"
#include "base.h"
#include "algutil.h"
#include "ioutil.h"
using namespace std;
/* 
	The syntex analylize after lex analysis
*/
int LL1error(const vector<TOKEN> &tokens,const PRODUCTIONS &prods,LL1STABLE &LL1Stable,struct SYNTAXTREE &syntaxtree,
			stack<int> &s,int cur_stack,int cur_token)
{
	cout<<"#error "<<SYNTAXSET::symStr(cur_stack)<<" "<<SYNTAXSET::symStr(cur_token)<<endl;
	return 0;
}
int LL1Analyze(const vector<TOKEN> &tokens,const PRODUCTIONS &prods,LL1STABLE &LL1Stable,struct SYNTAXTREE &syntaxtree)
{
	stack<int> s;
	stack<struct SYNTAXTREE*> st;
	int i,j;
	int cur_stack,cur_token,num,index,tmp,symbol;
	bool isError;
	struct SYNTAXTREE *ptnode;
	struct SYNTAXTREE *ptree;
	LL1STABLE::iterator iter;
	const int TERM=SYNTAXNODE::TYPE_TERM;
	const int NTERM=SYNTAXNODE::TYPE_NTERM;
	const int END=SYNTAXNODE::TYPE_END;
	const int EMPTY=SYNTAXNODE::TYPE_EMPTY;
	
	s.push(END);
	s.push(prods[0].left);
	syntaxtree.data.symbol=prods[0].left;
	syntaxtree.data.ptoken=NULL;
	syntaxtree.father=NULL;
	ptree=&syntaxtree;
	st.push(NULL);
	st.push(ptree);
	i=0;
	num=tokens.size();
	isError=false;
	while(1)
	{
		cur_stack=s.top();
		if(i<num)
			cur_token=getTokenType(tokens[i]);
		else
			cur_token=END;
		ptree=st.top();
		if(SYNTAXSET::symType(cur_stack)==TERM)
		{
			if(cur_stack==END && cur_token==END)
				break;
			if(cur_stack==cur_token)
			{
				s.pop();
				st.pop();
				ptree->data.ptoken=&tokens[i];
				i++;
			}
			else//error
			{
				LL1error(tokens,prods,LL1Stable,syntaxtree,s,cur_stack,cur_token);
				isError=true;
			}
		}
		else
		{
			iter=LL1Stable.find(pair<int,int>(cur_stack,cur_token));
			if(iter==LL1Stable.end())//error
			{
				LL1error(tokens,prods,LL1Stable,syntaxtree,s,cur_stack,cur_token);
				isError=true;
			}
			else
			{
				index=getProdsIndex(prods,iter->first.first);
				if(index >= num) return -1;
				s.pop();//prepare to add tree node
				st.pop();
				if(prods[index].right[iter->second][0].symType!=EMPTY)
				{
					for(j=0;j<prods[index].right[iter->second].size();j++)//add to tree
					{
						symbol=prods[index].right[iter->second][j].symbol;
						ptnode=new struct SYNTAXTREE;
						ptnode->father=ptree;
						ptnode->data.symbol=symbol;
						ptnode->data.ptoken=NULL;
						ptree->children.push_back(ptnode);
					}
					for(j=prods[index].right[iter->second].size()-1;j>=0;j--) //add to stack (inverse)
					{
						symbol=prods[index].right[iter->second][j].symbol;
						s.push(symbol);
						st.push(ptree->children[j]);
					}
				}
				else//reserve empty non-terminator
				{
					//ptree->data.symbol=EMPTY;
				}
			}
		}
	}
	return isError==true ? 0:-1;
}
int syntaxAnalyze(char *inpath,char *outpath)
{
	vector<TOKEN> tokens;
	FIRST first;
	FOLLOW follow;
	LL1TABLE LL1table;
	LL1STABLE LL1Stable;
	struct SYNTAXTREE *syntaxtree;
	
	syntaxtree=new struct SYNTAXTREE;
	readTokenXml(inpath,tokens);//tokens can't be change after that
	CreateFirstSet(SYNTAXSET::getProds(),first);
	CreateFollowSet(SYNTAXSET::getProds(),first,follow);
	CreateLL1Table(SYNTAXSET::getProds(),first,follow,LL1table);
	CreateLL1SingleTable(LL1table,SYNTAXSET::getLL1reserves(),LL1Stable);
	LL1Analyze(tokens,SYNTAXSET::getProds(),LL1Stable,*syntaxtree);
	writeSyntaxTreeXml(outpath,outpath,*syntaxtree);
	DeleteSyntaxTree(syntaxtree);

	return 0;
}
void showall()
{
	FIRST first;
	FOLLOW follow;
	SYNTAXSET();
	CreateFirstSet(SYNTAXSET::getProds(),first);
	CreateFollowSet(SYNTAXSET::getProds(),first,follow);
	cout<<"------------------------PRODS-------------------------"<<endl;
	SYNTAXSET::printProds();
	SYNTAXSET::printProdsNum();
	cout<<"------------------------FIRST-------------------------"<<endl;
	SYNTAXSET::printFirst(first);
	SYNTAXSET::printFirstNum(first);
	cout<<"------------------------FOLLOW-------------------------"<<endl;
	SYNTAXSET::printFollow(follow);	
	SYNTAXSET::printFollowNum(follow);
}
void showLL1()
{
	FIRST first;
	FOLLOW follow;
	LL1TABLE LL1table;
	LL1STABLE LL1Stable;
	SYNTAXSET();
	CreateFirstSet(SYNTAXSET::getProds(),first);
	CreateFollowSet(SYNTAXSET::getProds(),first,follow);
	CreateLL1Table(SYNTAXSET::getProds(),first,follow,LL1table);
	CreateLL1SingleTable(LL1table,SYNTAXSET::getLL1reserves(),LL1Stable);
	cout<<"------------------------Row LL(1)-------------------------"<<endl;
	SYNTAXSET::printLL1Table(LL1table);
	cout<<"------------------------Fixed LL(1)-------------------------"<<endl;
	SYNTAXSET::printLL1STable(LL1Stable);
}
int main(int argc,char *argv[])
{
	//showall();
	//showLL1();
	if(argc!=3)
	{
		cout<<"parsing error!"<<endl;
		return -1;
	}
	syntaxAnalyze(argv[1],argv[2]);
	return 0;
}