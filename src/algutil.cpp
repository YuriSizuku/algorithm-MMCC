#include <iostream>
#include <string>
#include <stack>
#include<set>
#include<algorithm>
#include "base.h"
using namespace std;
bool isLetter(char c)
{
	if(c>='a'&&c<='z' || c>='A'&&c<='Z')
		return true;
	else return false;
}
bool isDigit(char c) 
{
	if(c>='0' && c<='9')
		return true;
	else return false;
}
int getNumType(const std::string &str)
{
	/*the number may be like 
		1123,0,3324
		0334,00
		0x3324,0X332,0xabcde,0xAADDF,
		442.445 (.324 forbiden) */
	if(!isDigit(str[0]) || str=="" || str.length()==0)
		return -1;//error
	
	int flag=0;//0 dex,1 oct,2 hex 3 char
	int isFloat=0;
	unsigned int i;
	if(str.length()==1)
		return WORDSET::CONST_INT;
	else if(str.length()==2)
	{
		if(str[0]=='0') flag=1;
		else flag=0;
	}
	else
	{
		if(str.substr(0,2)=="0x" || str.substr(0,2)=="0X")
			flag=2;
		else if(str[0]=='0' && str[1]!='.')
			flag=1;
		else flag=0;
	}
	for(i=0;i<str.length();i++)
	{	
		if(flag==0)//dex
		{
			if(isFloat)
			{
				if(!isDigit(str[i]))
					break;
			}
			else
			{
				if(str[i]=='.')
					isFloat=1;
				else if(!isDigit(str[i]))
					break;
			}
		}
		else if(flag==1)//oct
		{
			if(str[i]>='0' && str[i]<='7')
				continue;
			else break;
		}
		else if(flag==2)//dex
		{
			if(i>=2)
			{
				if(isDigit(str[i]) || 
					str[i]>='a'&&str[i]<='f' ||
					str[i]>='A'&&str[i]<='F')
					continue;
				else break;
			}
		}
	}
	if(i<str.length())
		return WORDSET::UNDEFINED;
	if(isFloat==1) return WORDSET::CONST_FLOAT;
	else return WORDSET::CONST_INT;
}
bool isChar(const std::string &str)
{
	if(str.length()<=2)
		return false;
	if(str[0]!='\'' || str[str.length()-1]!='\'')
		return false;
	if(str[1]!='\\' && str.length()!=3)
		return false;
	if(str[1]=='\\' && str.length()!=4)
		return false;
	return true;
}
bool isString(const std::string &str)
{
	if(str.length()<=2)
		return false;
	if(str[0]!='"' || str[str.length()-1]!='"')
		return false;
	if(str.length()==3 && str[1]=='\\')
		return false;
	if(str[str.length()-2]=='\\' && str[str.length()-3]!='\\')
		return false;
	return true;
}
int getTokenType(const struct TOKEN &token)
{
	int wordtype=WORDSET::typeID(token.type);
	int ret;
	switch(wordtype)
	{
	case WORDSET::IDENTIFIER:
		ret=SYNTAXSET::VT.TKN_ID;
		break;
	case WORDSET::KEYWORD:
	case WORDSET::SEPERATOR:
	case WORDSET::OPERATOR:
		ret=SYNTAXSET::symId(token.value);
		break;
	case WORDSET::CONST_INT:
		ret=SYNTAXSET::VT.TKN_CONSTI;
		break;
	case WORDSET::CONST_FLOAT:
		ret=SYNTAXSET::VT.TKN_CONSTF;
		break;
	case WORDSET::CONST_CHAR:
	case WORDSET::CONST_STRING:
	case WORDSET::UNDEFINED:
	default:
		ret=SYNTAXNODE::TYPE_INVALID;
	}
	if(token.valid==false)
		return SYNTAXNODE::TYPE_INVALID;
	return ret;
}
int getProdsIndex(const PRODUCTIONS &prods,int left)
{
	unsigned int i;
	for(i=0;i<prods.size();i++)
	{
		if(prods[i].left==left)
			break;
	}
	return i;
}
void DeleteSyntaxTree(struct SYNTAXTREE *ptree)
{
	for(unsigned int i=0;i<ptree->children.size();i++)
		DeleteSyntaxTree(ptree->children[i]);
	ptree->children.clear();
	if(ptree->children.size()==0)
		delete ptree;//also delete the root
}
int CreateFirstSet(const PRODUCTIONS &prods,FIRST &first)
//suppose no left recursion and only 1 root
{
	struct PATHNODE
	{
		int cur;
		int pre;//pre=cur mains no pre node
	};
	struct PATHNODE pnode,ptnode;
	stack<struct PATHNODE> s;
	set<int> emptys;//to log the start non-terminators having empty item
	map<int,set<int>> unknown;//the left nterms needs several nterms in right which has unknown first set(if size=0,known)
	map<int,bool> vndone;//record if the first set has been done
	unsigned int i,j,k,i0;
	bool isBack,isEmpty;
	//to make it convinient
	const int TERM=SYNTAXNODE::TYPE_TERM;
	const int NTERM=SYNTAXNODE::TYPE_NTERM;
	const int END=SYNTAXNODE::TYPE_END;
	const int EMPTY=SYNTAXNODE::TYPE_EMPTY;

	for(i=0;i<prods.size();i++)//initialize the first,unknow set
	{
		vndone[prods[i].left]=false;
		for(j=0;j<prods[i].right.size();j++)
		{
			if(prods[i].right[j][0].symType==EMPTY)
				emptys.insert(prods[i].left);
		}
	}
	first.clear();

	for(i0=0;i0<prods.size();i0++)
	{
		pnode.cur=pnode.pre=prods[i0].left;
		if(vndone[pnode.cur]==true)
			continue;
		s.push(pnode);
		isBack=false;
		//cout<<i0<<endl;
		while(!s.empty())
		{
			pnode=s.top();
			isEmpty=false;
			for(i=0;i<prods.size();i++) //find the production's left
			{
				int left=prods[i].left;
				if(pnode.cur!=left)
					continue;
				for(j=0;j<prods[i].right.size();j++)
				{
					const SYNTAXNODE *synode=&(prods[i].right[j][0]);
					if(synode->symType==TERM) //terminator
					{
						first[left].insert(synode->symbol);
					}
					else if(synode->symType==NTERM) //non-terminator
					{
						unsigned int n;
						for(k=0;k<prods[i].right[j].size()&&
							emptys.find(prods[i].right[j][k].symbol)!=emptys.end();k++)
						{
							
						}//non-terminator has no $
						n=k;
						for(k=0;k<=n && k<prods[i].right[j].size();k++)
						{
							if(synode[k].symType==TERM)
							{
								first[left].insert(synode[k].symbol);
							}
							else
							{
								if(!vndone[synode[k].symbol])
								{
									if(pnode.cur!=synode[k].symbol) //to invoid self recursion
									{
										unknown[left].insert(synode[k].symbol);
										ptnode.pre=left;
										ptnode.cur=synode[k].symbol;
										s.push(ptnode);
										isBack=false;
									}
								}
								else //combine known FIRST set
								{
									set_union(first[synode[k].symbol].begin(),first[synode[k].symbol].end(),
										first[left].begin(),first[left].end(),
										insert_iterator<set<int>>(first[left],first[left].begin()));
									first[left].erase(EMPTY);
									//attetions to $ only if when no terms and all nterms can be empty occurs
								}
							}
						}
					}
					else if(synode->symType==EMPTY) //$
					{
						first[left].insert(EMPTY);
						isEmpty=true;
					}
				}
			}
			if(unknown[pnode.cur].size()==0) //all first set included knowned
			{
				vndone[pnode.cur]=true;//if no unknow non-terminators, the first set has been done
				unknown[pnode.pre].erase(pnode.cur);//to notify
				if(isEmpty==false)//check $
				{
					for(i=0;i<prods.size();i++)
					{
						if(prods[i].left!=pnode.cur)
								continue;
						for(j=0;j<prods[i].right.size();j++)
						{
							for(k=0;k<prods[i].right[j].size();k++)
							{
								if(prods[i].right[j][k].symType!=NTERM)
									break;
								if(first[prods[i].right[j][k].symbol].find(EMPTY)==first[prods[i].right[j][k].symbol].end())
									break;
							}
							if(k>=prods[i].right[j].size())
								isEmpty=true;
						}
					}
					if(isEmpty==false)
						first[pnode.cur].erase(EMPTY);
					else //ATTENTIONS! ETLIST3       -> OPTP3¡¤ETLIST4¡¤ETLIST3 | ETLIST4 
						first[pnode.cur].insert(EMPTY);
				}
				s.pop();
				isBack=true;
			}
			if(pnode.cur==pnode.pre && isBack==true)
			{
				if(s.size()>1)
					return 1;//left recursion
				else
					vndone[pnode.cur]=true;
			}
		}
	}
	return 0;
}
int CreateFollowSet(const PRODUCTIONS &prods,FIRST &first,FOLLOW &follow)
{
	struct FLINKNODE
	{
		set<int> follows;
		set<int> firsts;
		set<int> terms;
	};//left need that set
	struct COEDGE
	{
		int from;
		int to;
		bool operator <(const struct COEDGE &co) const
		{
			if(from <co.from)
				return true;
			else return false;
		}
	};//no self loop
	const struct SYNTAXNODE *synode;
	map<int,struct FLINKNODE> flink;
	map<int,bool> vndone;//record if the first set has been done
	set<struct COEDGE> backedge;//record the loop edge
	vector<int> s;//stack
	unsigned int left,pos,rnum,flag;
	unsigned int i,j,k,l;
	//to make it convinient
	const int TERM=SYNTAXNODE::TYPE_TERM;
	const int NTERM=SYNTAXNODE::TYPE_NTERM;
	const int END=SYNTAXNODE::TYPE_END;
	const int EMPTY=SYNTAXNODE::TYPE_EMPTY;

	for(i=0;i<prods.size();i++)//initialize
		vndone[prods[i].left]=false;
	follow.clear();

	flink[prods[0].left].terms.insert(END);//add # to start
	for(i=0;i<prods.size();i++)//trace the produces to create the FOLLOW FIRST reference graphy 
	{
		left=prods[i].left;
		for(j=0;j<prods[i].right.size();j++)
		{
			synode=&(prods[i].right[j][0]);
			rnum=prods[i].right[j].size();
			if(rnum==1)
			{
				if(synode[0].symType==NTERM)
					flink[synode[0].symbol].follows.insert(left);
				//else if(synode[0].symType==END)
				//	flink[left].terms.insert(END);
			}
			for(k=1;k<rnum;)//add first set link
			{
				for(l=k;l<rnum;l++)
				{
					if(synode[l].symType!=NTERM)
						break;
					else if(first[synode[l].symbol].find(EMPTY)==first[synode[l].symbol].end())//not find empty
						break;
				}
				if(l>=prods[i].right[j].size())//mains the series have no terminators
				{
					while(k<rnum)
					{
						if(synode[k-1].symType==NTERM)
						{
							flink[synode[k-1].symbol].firsts.insert(synode[k].symbol);  // B->aAb b!=$  FIRST(b):<FOLLOW(A)
							flink[synode[k-1].symbol].follows.insert(left);//B->aA | aAb  $:<b  FOLLOW(B):<FOLLOW(A)
						}
						k++;
					}
					flink[synode[k-1].symbol].follows.insert(left);//B->aA | aAb  $:<b  FOLLOW(B):<FOLLOW(A)
				}
				else
				{
					while(k<=l)
					{
						if(synode[k-1].symType==NTERM)
						{
							if(synode[k].symType==NTERM)
								flink[synode[k-1].symbol].firsts.insert(synode[k].symbol);  // B->aAb b!=$  FIRST(b):<FOLLOW(A)
							else
								flink[synode[k-1].symbol].terms.insert(synode[k].symbol);
						}
						k++;
					}
				}
			}
			if(synode[rnum-1].symType==NTERM)
			{
				flink[synode[rnum-1].symbol].follows.insert(left);
			}
		}
	}

	////debug
	//cout<<"------------------------FOLLOW_INTER-------------------------"<<endl;
	//for(i=0;i<prods.size();i++)
	//{
	//	set<int>::iterator iter;
	//	left=prods[i].left;
	//	cout<<left<<endl;
	//	cout<<"first={ ";
	//	for(iter=flink[left].firsts.begin();iter!=flink[left].firsts.end();iter++)
	//	{
	//		cout<<*iter<<" ";
	//	}
	//	cout<<"}"<<endl;
	//	cout<<"follow={ ";
	//	for(iter=flink[left].follows.begin();iter!=flink[left].follows.end();iter++)
	//	{
	//		cout<<*iter<<" ";
	//	}
	//	cout<<"}"<<endl;
	//	cout<<"terms={ ";
	//	for(iter=flink[left].terms.begin();iter!=flink[left].terms.end();iter++)
	//	{
	//		cout<<*iter<<" ";
	//	}
	//	cout<<"}"<<endl<<endl;
	//}

	for(i=0;i<prods.size();i++)//merge all sets without loop
	{
		set<int>::iterator iter;
		vector<int>::iterator iter2;
		left=prods[i].left;
		s.push_back(left);
		while(s.size())
		{
			pos=s[s.size()-1];
			flag=0;
			if(vndone[pos]==false)
			{
				if(flink[pos].follows.size()>0)
				{
					for(iter=flink[pos].follows.begin();iter!=flink[pos].follows.end();iter++)
					{
						if(vndone[*iter]==false)
						{
							iter2=find(s.begin(),s.end(),*iter);
							if(iter2!=s.end()) //if have loop,delete it and merge after
							{
								struct COEDGE coedge;
								coedge.from=pos;//from :< to
								coedge.to=*iter;
								if(coedge.from!=coedge.to)
									backedge.insert(coedge);
							}
							else//no loop
							{
								s.push_back(*iter);
								flag=1;
							}
						}
					}
				}
				if(flag==0) //nothing unknow, merge all
				{
					set_union(flink[pos].terms.begin(),flink[pos].terms.end(),
								follow[pos].begin(),follow[pos].end(),
								insert_iterator<set<int>>(follow[pos],follow[pos].begin()));
					for(iter=flink[pos].firsts.begin();iter!=flink[pos].firsts.end();iter++)
					{
						set_union(first[*iter].begin(),first[*iter].end(),
									follow[pos].begin(),follow[pos].end(),
									insert_iterator<set<int>>(follow[pos],follow[pos].begin()));
						follow[pos].erase(EMPTY);
					}
					for(iter=flink[pos].follows.begin();iter!=flink[pos].follows.end();iter++)
					{
						if(find(s.begin(),s.end(),*iter)!=s.end())
							continue;
						set_union(follow[*iter].begin(),follow[*iter].end(),
										  follow[pos].begin(),follow[pos].end(),
										  insert_iterator<set<int>>(follow[pos],follow[pos].begin()));
					}
					vndone[pos]=true;
					s.pop_back();
				}
			}
			else
			{
				s.pop_back();
			}
		}
	}

	//deal with loop edge
	set<struct COEDGE>::iterator iter;
	for(iter=backedge.begin();iter!=backedge.end();iter++)
	{
		set_union(follow[iter->to].begin(),follow[iter->to].end(),
			follow[iter->from].begin(),follow[iter->from].end(),
			insert_iterator<set<int>>(follow[iter->from],follow[iter->from].begin()));
	}
	return 0;
}
int CreateLL1Table(const PRODUCTIONS &prods,FIRST &first,FOLLOW &follow,LL1TABLE &LL1table)
{
	unsigned int i,j,k;
	unsigned int left,rnum;
	const struct SYNTAXNODE *synode;
	set<int>::iterator iter;

	const int TERM=SYNTAXNODE::TYPE_TERM;
	const int NTERM=SYNTAXNODE::TYPE_NTERM;
	const int END=SYNTAXNODE::TYPE_END;
	const int EMPTY=SYNTAXNODE::TYPE_EMPTY;

	for(i=0;i<prods.size();i++)
	{
		left=prods[i].left;
		for(j=0;j<prods[i].right.size();j++)
		{
			synode=&(prods[i].right[j][0]);	 
			rnum=prods[i].right[j].size();
			for(k=0;k<rnum;k++)
			{
				if(synode[k].symType==NTERM)
				{
					for(iter=first[synode[k].symbol].begin();iter!=first[synode[k].symbol].end();iter++)
					{
						if(left!=synode[k].symbol)
							LL1table[pair<int,int>(left,*iter)].insert(j);
					}
					if(first[synode[k].symbol].find(EMPTY)==first[synode[k].symbol].end())
					{
						break;
					}
				}
				else if(synode[0].symType==TERM)
				{
					LL1table[pair<int,int>(left,synode[k].symbol)].insert(j);
					break;
				}
			}
			if(k>=rnum)
			{
				for(iter=follow[left].begin();iter!=follow[left].end();iter++)
				{
					LL1table[pair<int,int>(left,*iter)].insert(j);
				}
			}
		}
	}
	return 0;
}
int CreateLL1SingleTable(const LL1TABLE &LL1table,const LL1STABLE &reserve,LL1STABLE &LL1Stable)
{
	LL1TABLE::const_iterator iter;
	int num;

	LL1Stable.clear();
	for(iter=LL1table.cbegin();iter!=LL1table.cend();iter++)
	{
		if(iter->second.size()<=0)
			continue;
		if(iter->second.size()<=1)
		{
			num=*(iter->second.cbegin());
		}
		else
		{
			if(reserve.find(pair<int,int>(iter->first.first,iter->first.second))!=reserve.cend())
				num=reserve.at(pair<int,int>(iter->first.first,iter->first.second));
			else
				return -1;
		}
		LL1Stable[pair<int,int>(iter->first.first,iter->first.second)]=num;
	}
	return 0;
}