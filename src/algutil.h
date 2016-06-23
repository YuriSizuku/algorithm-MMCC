#include <iostream>
#include <string>
#include <map>
#include "base.h"
/*
	the univese functions and algrithms
*/
#ifndef _ALGUTILL_H
#define _ALGUTILL_H
bool isLetter(char c);
bool isDigit(char c);
bool isChar(const std::string &str);
bool isString(const std::string &str);
int getNumType(const std::string &str);

int getTokenType(const struct TOKEN &token);
int getProdsIndex(const PRODUCTIONS &prods,int left);

void DeleteSyntaxTree(struct SYNTAXTREE *ptree);

int CreateFirstSet(const PRODUCTIONS &prods,FIRST &first);
int CreateFollowSet(const PRODUCTIONS &prods,FIRST &first,FOLLOW &follow);
int CreateLL1Table(const PRODUCTIONS &prods,FIRST &first,FOLLOW &follow,LL1TABLE &LL1table);
int CreateLL1SingleTable(const LL1TABLE &LL1table,const LL1STABLE &reserve,LL1STABLE &LL1Stable);
//to solve the item in ll1table has only one production
#endif