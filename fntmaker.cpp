#include <bits/stdc++.h>
using namespace std;
FILE *txt,*outp,*tmpout,*mtx;
int txtsize;
unsigned char stxt[1000000];
string vu8txt;
string fntname;
set<string> setu8chr;
map<unsigned int,unsigned int> Charmap;
set<unsigned char> ignorechar={'\"','\n','[',']','\t','\r',','};
unsigned int utf8_to_utf16le(const string& u8str)
{
	unsigned int ans=0;
	int i=0;
	bool is_ok = true;
	// 开始转换
	uint32_t ch = u8str[i];	// 取出UTF8序列首字节
	if ((ch & 0x80) == 0) {
		// 最高位为0，只有1字节表示UNICODE代码点
		ans=ch;
		return ans;
	}
	switch (ch & 0xF0)
	{
		case 0xE0: // 3 字节字符, 0x800 到 0xFFFF
		{
			uint32_t c2 = u8str[++i];
			uint32_t c3 = u8str[++i];
			// 计算UNICODE代码点值(第一个字节取低4bit，其余取6bit)
			uint32_t codePoint = ((ch & 0x0FU) << 12) | ((c2 & 0x3FU) << 6) | (c3 & 0x3FU);
			ans=((char16_t)codePoint);
		}
		break;
		case 0xD0: // 2 字节字符, 0x80 到 0x7FF
		case 0xC0:
		{
			uint32_t c2 = u8str[++i];
			// 计算UNICODE代码点值(第一个字节取低5bit，其余取6bit)
			uint32_t codePoint = ((ch & 0x1FU) << 12) | ((c2 & 0x3FU) << 6);
			ans=((char16_t)codePoint);
		}
		break;
		default:	// 单字节部分(前面已经处理，所以不应该进来)
			is_ok = false;
			break;
	}
	return ans;
}
void outputint16(unsigned int data)
{
	fprintf(outp,"%c%c",(unsigned char)data,(unsigned char)(data>>8));
}
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
	fprintf(outp,"FNT");
	fputc(0,outp);
	outputint16(0x14);
	outputint16(0);
	outputint16(0x15);
	outputint16(0);
	outputint16(setu8chr.size());
	outputint16(0);
	i=0;
	for(auto s:setu8chr)
	{
		unsigned int ch=utf8_to_utf16le(s);
		outputint16(ch);
		Charmap[ch]=i;
		i++;
		outputint16(0x15);
	}
	fflush(outp);
	fclose(outp);
	#ifndef WIN32
	system("mono VrConvert.exe -e tmp.png gvr rgb5a3 rgb5a3 -gcix");
	char cmd[1000]="";
	sprintf(cmd,"cat %s tmp.gvr > tmp.fnt",fntname.c_str());
	system(cmd);
	sprintf(cmd,"mv tmp.fnt %s",fntname.c_str());
	system(cmd);
	#endif
}
void makemtx()
{
	int pos=0;
	if(stxt[0]==0xef&&stxt[1]==0xbb&&stxt[2]==0xbf)pos=3;
	unsigned char ch=stxt[pos];
	while(ch!='[')
	{
		ch=stxt[++pos];
	}
}
int main(int argc,char *argv[])
{
	if(argc<3)
	{
		cout<<"Usage: fntmaker <txt> <outputfnt>"<<endl;
		return 0;
	}
	if((txt=fopen(argv[1],"rb"))==NULL)
	{
		cout<<"Open txt error."<<endl;
		return 1;
	}
	if((outp=fopen(argv[2],"wb"))==NULL)
	{
		cout<<"Open output error."<<endl;
		return 2;
	}
	initmap();
	makemtx();
}