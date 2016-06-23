#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "tinyxml.h"
#include "base.h"
#include "algutil.h"
#include "ioutil.h"
using namespace std;
/*
	lexical analysis
	(not based on restricted DFA,this will look for more chars)
*/
int lineAnalyse(string str,int num_line,vector<TOKEN> &tokens)
{
	int pos=0,begin=0,end=0;
	int len=str.length();
	int type;
	string word;
	struct TOKEN token;
	while(pos<len)
	{
		char c=str[pos];
		if(isLetter(c) || c=='_') //indentifier or keyword(azAZ_)
		{
			begin=pos;
			pos++;
			while(pos<len && 
				!WORDSET::isOperator(str.substr(pos,1)) &&
				!WORDSET::isSeperator(str.substr(pos,1)) &&
				str[pos]!=' ' && 
				str[pos]!='\t' && 
				str[pos]!='\r' &&
				str[pos]!='\n')
			{pos++;}
			end=pos;
			word=str.substr(begin,end-begin);
			token.line=num_line;
			token.num=tokens.size()+1;
			token.value=word;
			if(WORDSET::isKeyword(word))
				token.type=WORDSET::typeStr(WORDSET::KEYWORD);
			else
				token.type=WORDSET::typeStr(WORDSET::IDENTIFIER);
			token.valid=true;
			tokens.push_back(token);
		}
		else if(isDigit(c)) //int,float
		{
			begin=pos;
			pos++;
			while(pos<len && 
				!WORDSET::isOperator(str.substr(pos,1)) &&
				!WORDSET::isSeperator(str.substr(pos,1)) &&
				str[pos]!=' ' && 
				str[pos]!='\t' && 
				str[pos]!='\r' &&
				str[pos]!='\n')
			{pos++;}
			end=pos;
			word=str.substr(begin,end-begin);
			type=getNumType(word);
			token.line=num_line;
			token.num=tokens.size()+1;
			token.value=word;
			token.type=WORDSET::typeStr(type);
			if(type==-1 || type==WORDSET::UNDEFINED)
				token.valid=false;
			else token.valid=true;
			tokens.push_back(token);
		}
		else if(c=='\'') //char
		{
			type=0 ;//1 mains under '\'
			begin=pos;
			while(pos<len)
			{
				pos++;
				if(str[pos]=='\\')
					type=-type+1;
				else if(str[pos]=='\'' && !type)
					break;
				else type=0;
			}
			end=++pos;
			word=str.substr(begin,end-begin);
			token.line=num_line;
			token.num=tokens.size()+1;
			token.value=word;
			if(isChar(word))
			{
				token.type=WORDSET::typeStr(WORDSET::CONST_CHAR);
				token.valid=true;
			}
			else
			{
				token.type=WORDSET::typeStr(WORDSET::UNDEFINED);
				token.valid=false;
			}
			tokens.push_back(token);
		}
		else if(c=='"') //string
		{
			type=0 ;//1 mains under '\'
			begin=pos;
			while(pos<len)
			{
				pos++;
				if(str[pos]=='\\')
					type=-type+1;
				else if(str[pos]=='"' && !type)
					break;
				else type=0;
			}
			end=++pos;
			word=str.substr(begin,end-begin);
			token.line=num_line;
			token.num=tokens.size()+1;
			token.value=word;
			if(isString(word))
			{
				token.type=WORDSET::typeStr(WORDSET::CONST_STRING);
				token.valid=true;
			}
			else
			{
				token.type=WORDSET::typeStr(WORDSET::UNDEFINED);
				token.valid=false;
			}
			tokens.push_back(token);
		}
		else if(c==' ' || c=='\r' || c=='\n' || c=='\t') //space
		{
			pos++;
		}
		else //operators and seperaters
		{
			token.line=num_line;
			token.num=tokens.size()+1;
			switch(c)
			{
				case '+':
				case '-':
				case '=':
				case '&':
				case '|':
					if(pos<str.length()-1 && str[pos+1]==c)
					{
						word=c;word+=c;
						pos++;
					}
					else word=c;
					break;
				case '%':
				case '>':
				case '<':
				case '!':
					if(pos<str.length()-1 && str[pos+1]=='=')
					{
						word=c;word+='=';
						pos++;
					}
					else word=c;
					break;
				case '*':
				case '/':	
				case '~':
				case '(':
				case ')':
				case '{':
				case '}':
				case '[':
				case ']':
				default:
					word=c;
					break;
			}
			token.value=word;
			if(WORDSET::isOperator(word))
			{
				token.type=WORDSET::typeStr(WORDSET::OPERATOR);
				token.valid=true;
			}
			else if(WORDSET::isSeperator(word))
			{
				token.type=WORDSET::typeStr(WORDSET::SEPERATOR);
				token.valid=true;
			}
			else
			{
				token.type=WORDSET::typeStr(WORDSET::UNDEFINED);
				token.valid=false;
			}
			pos++;
			tokens.push_back(token);
		}
	}
	return 0;
}
int lexAnalyse(char *inpath,char *outpath)
{
	ifstream fin(inpath);
	string line,tmp;
	vector<TOKEN> tokens;
	int num_line=1;
	while(getline(fin,line))
	{
		if(line!="")
			lineAnalyse(line,num_line,tokens);
		num_line++;
	}
	writeTokenXml(outpath,inpath,tokens);
	fin.close();
	return 0;
}
int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		cout<<"scanning error!"<<endl;
		return -1;
	}
	lexAnalyse(argv[1],argv[2]);
	return 0;
}