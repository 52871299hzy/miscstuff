#include <bits/stdc++.h>
using namespace std;
FILE *txt,*outp,*tmpout;
int txtsize;
unsigned char stxt[1000000];
string vu8txt;
set<string> setu8chr;
set<unsigned char> ignorechar={'\"','\n','[',']','\t','\r',','};
long readfile(FILE *pFile,unsigned char *dest)
{
	long lSize;
	fseek(pFile,0,SEEK_END);
	lSize=ftell(pFile);
	rewind(pFile);
	long result=fread(dest,1,lSize,pFile);
	if(result!=lSize)
	{
		fputs("Reading error",stderr);
		assert(0);
	}
	return lSize;
}
void utf8_to_charset()
{
	auto input=vu8txt;
	string ch;
	for (size_t i=0,len=0;i!=input.length();i+=len)
	{
		unsigned char byte=(unsigned)input[i];
		if (byte >= 0xFC)
			len=6;
		else if (byte >= 0xF8)
			len=5;
		else if (byte >= 0xF0)
			len=4;
		else if (byte >= 0xE0)
			len=3;
		else if (byte >= 0xC0)
			len=2;
		else
			len=1;
		ch=input.substr(i,len);
		setu8chr.insert(ch);
	}
}
void initmap()
{
	txtsize=readfile(txt,stxt);
	int i=0;
	if(stxt[0]==0xef&&stxt[1]==0xbb&&stxt[2]==0xbf)i=3;
	for(;i<txtsize;i++)
	{
		unsigned char c=stxt[i];
		if(ignorechar.find(c)!=ignorechar.end())continue;
		if(c=='\\'||c=='n')continue;
		if(c=='<')
		{
			while(c!='>'&&i<txtsize)
			{
				i++;
				c=stxt[i];
			}
			continue;
		}
		vu8txt.push_back(c);
	}
	for(auto c:vu8txt)cerr<<c;
	cerr<<endl;
	utf8_to_charset();
	tmpout=fopen("tmp.txt","wb");
	for(auto s:setu8chr)
	{
		fprintf(tmpout,"%s",s.c_str());
	}
	fflush(tmpout);
	fclose(tmpout);
	system("python3 a.py tmp.txt tmp.png");
}
int main(int argc,char *argv[])
{
	if(argc<2)
	{
		cout<<"Usage: fntmaker <txt> <outputfnt>"<<endl;
		return 0;
	}
	if((txt=fopen(argv[1],"rb"))==NULL)
	{
		cout<<"Open txt error."<<endl;
		return 1;
	}
	//if((outp=fopen(argv[2],"wb"))==NULL)
	//{
	//	cout<<"Open output error."<<endl;
	//	return 2;
	//}
	initmap();
}