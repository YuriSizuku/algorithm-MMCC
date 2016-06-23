#include <iostream>
#include <vector>
#include "tinyxml.h"
#include "base.h"
/*
	input and output utils
*/
#ifndef _IOUTIL_H
#define _IOUTIL_H
int writeTokenXml(const char *outpath,const char *name,const std::vector<TOKEN> &tokens);
int readTokenXml(const char *inpath,std::vector<TOKEN> &tokens);
int writeSyntaxTreeXml(char *outpath,const char *name,const struct SYNTAXTREE &syntaxtree);
int readSyntaxTreeXml(const char *inpath,struct SYNTAXTREE &syntaxtree,const std::vector<TOKEN> &tokens);
int writeIcCodeXml(const char *outpath,const char *name,const std::vector<struct FUNCIC> &funcics);
#endif