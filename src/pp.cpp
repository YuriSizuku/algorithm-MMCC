#include<iostream>
#include<string>
#include<fstream>
using namespace std;
/*
	preprocess
	remove comments (the physical line number is the same)
*/
int preprocess(char *inpath,char *outpath)
{
	ifstream fin(inpath);
	ofstream fout(outpath);
	string line;
	string tmp;
	size_t start,end;
	int flag=0;//flag=1 /* comment
	while(getline(fin,line))
	{
		tmp="";
		start=-2;
		//while(1)
		//{
		//	start=line.find("\r\n",start+2);
		//	if(start!=string::npos)
		//		line=line.replace(start,2,"");
		//	else break;
		//}
		if(flag==1)
		{
			start=line.find("*/");
			if(start!=string::npos)
				flag=0;
		}
		else
		{
			start=line.find("//");
			if(start!=string::npos)
				line=line.substr(0,start);
			start=-2;
		}
		while(1)
		{
			start+=2;
			end=line.find("/*",start);
			if(flag==0)
				tmp+=line.substr(start,end-start);
			if(end!=string::npos)
				flag=1;
			else break;
			start=line.find("*/",end+2);
			if(start!=string::npos)
				flag=0;
			else break;
		}
		fout<<tmp<<endl;
	}
	fin.close();
	fout.close();
	return 0;
}
int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		cout<<"pp error!"<<endl;
		return -1;
	}
	preprocess(argv[1],argv[2]);
	return 0;
}
