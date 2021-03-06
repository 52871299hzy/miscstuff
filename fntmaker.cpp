#include <bits/stdc++.h>
#include <string>
using namespace std;
typedef unsigned int ui;
FILE *txt,*outp,*tmpout,*mtx;
int txtsize;
int mtxsize=0;
unsigned char stxt[1000000];
string vu8txt;
string fntname;
set<string> setu8chr;
map<unsigned int,unsigned int> Charmap;
set<unsigned char> ignorechar={'\"','\n','[',']','\t','\r',','};
vector<unsigned int> MtxSecOffsets;
vector<vector<unsigned int>> MtxStrOffsets;
vector<unsigned int> MtxSecSizes;
vector<vector<unsigned int>> MtxStrSizes;
vector<unsigned short> Data;
int MtxAllStrNo=0;
ui GetNxtU8toU16(int &pos)
{
	unsigned int ans=0;
	bool is_ok = true;
	uint32_t ch = stxt[pos];	
	if ((ch & 0x80) == 0) {
		ans=ch;
		return ans;
	}
	switch (ch & 0xF0)
	{
		case 0xE0:
		{
			uint32_t c2 = stxt[++pos];
			uint32_t c3 = stxt[++pos];
			uint32_t codePoint = ((ch & 0x0FU) << 12) | ((c2 & 0x3FU) << 6) | (c3 & 0x3FU);
			ans=((char16_t)codePoint);
		}
		break;
		case 0xD0:
		case 0xC0:
		{
			uint32_t c2 = stxt[++pos];
			uint32_t codePoint = ((ch & 0x1FU) << 12) | ((c2 & 0x3FU) << 6);
			ans=((char16_t)codePoint);
		}
		break;
		default:
			is_ok = false;
			break;
	}
	return ans;
}
ui getNum(int &pos)
{
	ui ans=0;
	for(int i=0;i<4;i++)
	{
		ans=(ans<<4);
		if(isdigit(stxt[pos]))ans+=stxt[pos]-'0';
		else ans+=stxt[pos]-'a'+10;
		pos++;
	}
	pos--;
	return ans;
}
ui utf8_to_utf16le(string& str)
{
	unsigned int ans=0;
	bool is_ok = true;
	int pos=0;
	uint32_t ch = str[pos];	
	if ((ch & 0x80) == 0) {
		ans=ch;
		return ans;
	}
	switch (ch & 0xF0)
	{
		case 0xE0:
		{
			uint32_t c2 = str[++pos];
			uint32_t c3 = str[++pos];
			uint32_t codePoint = ((ch & 0x0FU) << 12) | ((c2 & 0x3FU) << 6) | (c3 & 0x3FU);
			ans=((char16_t)codePoint);
		}
		break;
		case 0xD0:
		case 0xC0:
		{
			uint32_t c2 = str[++pos];
			uint32_t codePoint = ((ch & 0x1FU) << 12) | ((c2 & 0x3FU) << 6);
			ans=((char16_t)codePoint);
		}
		break;
		default:
			is_ok = false;
			break;
	}
	return ans;
}
void outputint16(unsigned int data)
{
	fprintf(outp,"%c%c",(unsigned char)data,(unsigned char)(data>>8));
}
void moutputint16(unsigned int data)
{
	fprintf(mtx,"%c%c",(unsigned char)data,(unsigned char)(data>>8));
}
void moutputint32(unsigned int data)
{
	moutputint16(data&0xffffu);
	moutputint16(data>>16);
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
//	for(auto c:vu8txt)cerr<<c;
//	cerr<<endl;
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
	#ifndef _WIN32
	system("mono VrConvert.exe -e tmp.png gvr rgb5a3 rgb5a3 -gcix");
	char cmd[1000]="";
	sprintf(cmd,"cat %s tmp.gvr > tmp.fnt",fntname.c_str());
	system(cmd);
	sprintf(cmd,"mv tmp.fnt %s",fntname.c_str());
	system(cmd);
	#else
	system("python a.py tmp.txt tmp.png");
	system("VrConvert.exe -e tmp.png gvr rgb5a3 rgb5a3 -gcix");
	char cmd[1000]="";
	sprintf(cmd,"copy %s /b + tmp.gvr /b tmp.fnt /b",fntname.c_str());
	system(cmd);
	sprintf(cmd,"copy tmp.fnt %s",fntname.c_str());
	system(cmd);
	#endif
}
void makemtx()
{
	int pos=0;
	bool fileend=0;
	if(stxt[0]==0xef&&stxt[1]==0xbb&&stxt[2]==0xbf)pos=3;
	unsigned char ch=stxt[pos];
	while(ch!='[')ch=stxt[++pos];
	pos++;
	while(!fileend)
	{
		while(ch!='[')ch=stxt[++pos];
		int sectionno=0;
		bool sectionend=0;
		MtxStrSizes.push_back(vector<unsigned int>());
		int strno=0;
		while(!sectionend)
		{
			while(ch!='\"')ch=stxt[++pos];
			ch=stxt[++pos];
			int lenstr=0;
			while(ch!='\"')
			{
				switch(ch)
				{
					case '\\':Data.push_back(0xFFFDu);pos++;lenstr++;
						break;
					case '<':
						switch(stxt[pos+1])
						{
							case 'a'://<arrow>
								pos+=6;
								Data.push_back(0xf813u);
								lenstr++;
								break;
							case 'c':
								if(stxt[pos+2]=='l')//<clear>
								{
									pos+=6;
									Data.push_back(0xf812u);
									lenstr++;
								}
								else //<color:0000>
								{
									pos+=7;
									Data.push_back(0xf800u);
									Data.push_back(getNum(pos));
									lenstr+=2;
									pos--;
								}
								break;
							case '/'://</color>
								pos+=7;
								Data.push_back(0xf801u);
								lenstr++;
								break;
							case 's'://<speed:0000>
								pos+=7;
								Data.push_back(0xf880u);
								Data.push_back(getNum(pos));
								pos--;
								lenstr+=2;
								break;
							case 'w'://<wait:0000>
								pos+=6;
								Data.push_back(0xf881u);
								Data.push_back(getNum(pos));
								pos--;
								lenstr+=2;
								break;
						}
						break;
					default:
						ui c=GetNxtU8toU16(pos);
						Data.push_back(Charmap[c]);
						lenstr+=1;
						break;
				}
				ch=stxt[++pos];
			}
			Data.push_back(0xffffu);
			lenstr++;
			strno++;
			MtxAllStrNo++;
			MtxStrSizes[sectionno].push_back(lenstr);
			if((ch=stxt[++pos])!=',')sectionend=1;
		}
		MtxSecSizes.push_back(strno);
		sectionno++;
		while(ch!='['&&pos+1<txtsize)ch=stxt[++pos];
		if(pos==txtsize-1)fileend=1;
	}
	mtxsize+=8;
	mtxsize+=MtxSecSizes.size()*4;
	mtxsize+=MtxAllStrNo*4;
	mtxsize+=Data.size()*2;
	moutputint32(mtxsize);
	moutputint32(0x08u);
	int stroffst=8+MtxSecSizes.size()*4;
	int stroffpos=stroffst;
	for(int i=0;i<MtxSecSizes.size();i++)
	{
		moutputint32(stroffpos);
		stroffpos+=MtxSecSizes[i]*4;
	}
	for(int i=0;i<MtxSecSizes.size();i++)
	{
		for(int j=0;j<MtxStrSizes[i].size();j++)
		{
			moutputint32(stroffpos);
			stroffpos+=MtxStrSizes[i][j]*2;
		}
	}
	for(auto i:Data)moutputint16(i);
}
int main(int argc,char *argv[])
{
	if(argc<4)
	{
		cout<<"Usage: fntmaker <txt> <outputfnt> <outputmtx>"<<endl;
		return 0;
	}
	if((txt=fopen(argv[1],"rb"))==NULL)
	{
		cout<<"Open txt error."<<endl;
		return 1;
	}
	if((outp=fopen(argv[2],"wb"))==NULL)
	{
		cout<<"Open outputfnt error."<<endl;
		return 2;
	}
	if((mtx=fopen(argv[3],"wb"))==NULL)
	{
		cout<<"Open outputmtx error."<<endl;
		return 3;
	}
	fntname=argv[2];
	initmap();
	makemtx();
	for(auto i:Data)
	{
		fprintf(stderr,"%04x ",i);
	}
}
