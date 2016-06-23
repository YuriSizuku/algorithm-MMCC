#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <Windows.h>
using namespace std;
//#define _XDEBUG
#ifdef _XDEBUG
char *pp_exec="..\\..\\debug\\pp.exe";
char *scanning_exec="..\\..\\debug\\scanning.exe";
char *parsing_exec="..\\..\\debug\\parsing.exe";
char *semantic_exec="..\\..\\debug\\semantic.exe";
#else
char *pp_exec="pp.exe";
char *scanning_exec="scanning.exe";
char *parsing_exec="parsing.exe";
char *semantic_exec="semantic.exe";
#endif

char *pp_ext=".pp.c";
char *scanning_ext=".token.xml";
char *parsing_ext=".tree.xml";
char *semantic_ext=".ic.xml";

void BITMiniCC(int argc,char *argv[])
{
	char cmd[1000];
	sprintf(cmd,"run.bat %s",argv[1]);
	system(cmd);
	system("pause");
}
int mmcc(int argc,char *argv[])
{
	if(argc!=2)
	{
		cout<<"usesage: mmcc xxx.c"<<endl;
		return -1;
	}
	string name=argv[1];
	char cmd[1000];
	int pos;
	bool ret;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	ZeroMemory(&si,sizeof(si));
	ZeroMemory(&pi,sizeof(pi));
	pos=name.rfind(".");
	name=name.substr(0,pos);

	//pp
	sprintf(cmd,"%s %s %s%s",pp_exec,argv[1],name.c_str(),pp_ext);
	ret=CreateProcess(pp_exec,cmd,NULL,NULL,true,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&si,&pi);
	if(ret==false)
	{
		cout<<cmd<<" fail!"<<endl;
		return -1;
	}
	WaitForSingleObject(pi.hProcess,INFINITE);
	cout<<cmd<<" done!"<<endl;
	//scanning
	sprintf(cmd,"%s %s%s %s%s",scanning_exec,name.c_str(),pp_ext,name.c_str(),scanning_ext);
	ret=CreateProcess(scanning_exec,cmd,NULL,NULL,true,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&si,&pi);
	if(ret==false)
	{
		cout<<cmd<<" fail!"<<endl;
		return -1;
	}
	WaitForSingleObject(pi.hProcess,INFINITE);
	cout<<scanning_exec<<" "<<cmd<<" done!"<<endl;
	//parsing
	sprintf(cmd,"%s %s%s %s%s",parsing_exec,name.c_str(),scanning_ext,name.c_str(),parsing_ext);
	ret=CreateProcess(parsing_exec,cmd,NULL,NULL,true,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&si,&pi);
	if(ret==false)
	{
		cout<<cmd<<" fail!"<<endl;
		return -1;
	}
	WaitForSingleObject(pi.hProcess,INFINITE);
	cout<<cmd<<" done!"<<endl;
	//semantic
	sprintf(cmd,"%s %s%s %s%s",semantic_exec,name.c_str(),parsing_ext,name.c_str(),semantic_ext);
	ret=CreateProcess(semantic_exec,cmd,NULL,NULL,true,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&si,&pi);
	if(ret==false)
	{
		cout<<" "<<cmd<<" fail!"<<endl;
		return -1;
	}
	WaitForSingleObject(pi.hProcess,INFINITE);
	cout<<cmd<<" done!"<<endl;
	cout<<"mmcc compiler done!"<<endl;
	return 0;
}
int main(int argc,char *argv[])
{
	//BITMiniCC(argc,argv);
	int ret;
	ret=mmcc(argc,argv);
	system("pause");
	return ret;
}