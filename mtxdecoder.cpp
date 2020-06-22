#include <bits/stdc++.h>
#include <assert.h>
using namespace std;
FILE *mtx,*fnt,*outp;
unsigned char strmtx[10*1024*1024],strfnt[10*1024*1024];
unsigned int posmtx,posfnt;
unsigned int fntsize,mtxsize;
vector<unsigned int> MtxSecOffsets;
vector<vector<unsigned int>> MtxStrOffsets;
unsigned int CharMap[1024];
bool hasunrec=0;
void readfile(FILE *pFile,unsigned char *dest)
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
	//cerr<<lSize<<"!"<<endl;
}
unsigned int frdint16()
{
	unsigned int ret=((strfnt[posfnt+1])<<8)|strfnt[posfnt];
	posfnt+=2;
	//if(ret==0xffffff84)cerr<<"fuck\n";
	return ret;
}
unsigned int frdint32()
{
	unsigned int ret=frdint16();
	ret+=(frdint16())<<16;
	return ret;
}
unsigned int mrdint16()
{
	unsigned int ret=(strmtx[posmtx+1]<<8)+strmtx[posmtx];
	posmtx+=2;
	return ret;
}
unsigned int mrdint32()
{
	unsigned int ret=mrdint16();
	ret+=(mrdint16())<<16;
	return ret;
}
void outputint16(unsigned int data)
{
	fprintf(outp,"%c%c",(unsigned char)data,(unsigned char)(data>>8));
}
void dumpint16(unsigned int data)
{
	for(int i=3;i>=0;i--)
	{
		unsigned int a=(data&(0xfu<<(i*4)))>>(i*4);
		if(a>=0xau)outputint16(0x0061u+a-0xau);
		else outputint16(0x0030u+a);
//		cerr<<a<<' ';
	}
//	cerr<<endl;
}
void outputstr(string str)
{
	for(unsigned char c:str)
	{
		outputint16(0x0u+c);
	}
}
void CreateCharMap()	//deals with fnt file
{
	posfnt=0xc;
	fntsize=frdint32();
	for(unsigned int i=0;i<fntsize;i++)
	{
		CharMap[i]=frdint16();
		frdint16();
	}
//	for(unsigned int i=0;i<0xf8;i++)
//	{
//		printf("%x ",strfnt[i]);
//		if(i%16==0)cerr<<endl;
//	}
//	cerr<<endl;
//	cerr<<fntsize<<endl;
//	for(unsigned int i=0;i<fntsize;i++)
//	{
//		printf("%x ",CharMap[i]);
//	}
}
void ProcessMtx()
{
	posmtx=0;
	mtxsize=mrdint32();
	posmtx=0x8;
	MtxSecOffsets.push_back(mrdint32());
	while(posmtx<MtxSecOffsets[0])MtxSecOffsets.push_back(mrdint32());
	MtxSecOffsets.push_back(mrdint32());
	for(int i=0;i<MtxSecOffsets.size()-1;i++)
	{
		posmtx=MtxSecOffsets[i];
		MtxStrOffsets.push_back(vector<unsigned int>());
		MtxStrOffsets[i].push_back(mrdint32());
		while(posmtx<MtxSecOffsets[i+1])MtxStrOffsets[i].push_back(mrdint32());
	}
	MtxStrOffsets.push_back(vector<unsigned int>());
	MtxStrOffsets[MtxSecOffsets.size()-1].push_back(mtxsize+4);
	for(int i=0;i<MtxSecOffsets.size()-1;i++)
	{
		MtxStrOffsets[i].push_back(MtxStrOffsets[i+1][0]);
		for(unsigned int j=0;j<MtxStrOffsets[i].size()-1;j++)
		{
			posmtx=MtxStrOffsets[i][j];
		//	cerr<<j<<' '<<posmtx<<' '<<MtxStrOffsets[i][j+1]<<endl;
			char temp[20];
			//sprintf(temp,"String #%d:\n",i);
			//outputstr(string(temp));
			while(posmtx<mtxsize&&posmtx<MtxStrOffsets[i][j+1])
			{
				unsigned int ch=mrdint16();
				switch(ch)
				{
					case 0xffff:break;
					case 0xfffe://'E'
						outputint16(0x0045);
						break;
					case 0xfffd://'D [CR] [LF]'
						outputint16(0x0044);
						outputint16(0x000d);
						outputint16(0x000a);
						break;
					case 0xf813://'{arrow}'
						//outputint16(0x2193);
						outputstr("<arrow>");
						break;
					case 0xf800://'{color:x}'
						outputstr("<color:");
						dumpint16(mrdint16());
						outputstr(">");
						break;
					case 0xf801://'{/color}'
						outputstr("</color>");
						break;
					case 0xf880://'{speed:x}'
						outputstr("<speed:");
						dumpint16(mrdint16());
						outputstr(">");
						break;
					case 0xf881://'{wait:x}'
						outputstr("<wait:");
						dumpint16(mrdint16());
						outputstr(">");
						break;
					case 0xf812://'{clear}'
						outputstr("<clear>");
						break;
					default:
						if(ch<fntsize)
						{
							outputint16(CharMap[ch]);
						}
						else
						{
							outputint16(0x007b);
							dumpint16(ch);
							hasunrec=1;
							outputint16(0x007d);
							//sprintf(temp,"(%x)",ch);
							//outputstr(string(temp));
						}
				}
			}
			outputint16(0x000d);
			outputint16(0x000a);
			outputint16(0x000d);
			outputint16(0x000a);
		}
		outputint16(0x000d);
		outputint16(0x000a);
		outputint16(0x0025);
		outputint16(0x0025);
		outputint16(0x0025);
		outputint16(0x000d);
		outputint16(0x000a);
	}
	
}
int main(int argc,char *argv[])
{
	if(argc!=4)
	{
		printf("Usage: mtxdecoder <mtx> <fnt> <output>");
		return 0;
	}
	if((mtx=fopen(argv[1],"rb"))==NULL)
	{
		printf("open mtx file error.");
		return 1;
	}
	if((fnt=fopen(argv[2],"rb"))==NULL)
	{
		printf("open fnt file error.");
		return 1;
	}
	if((outp=fopen(argv[3],"wb"))==NULL)
	{
		printf("open output file error.");
		return 1;
	}
	memset(strmtx,0,sizeof(strmtx));
	memset(strfnt,0,sizeof(strmtx));
	
	readfile(mtx,strmtx);
	readfile(fnt,strfnt);
	if(strfnt[0]!='F'||strfnt[1]!='N'||strfnt[2]!='T'||strfnt[3]!=0)
	{
		printf("Invalid fnt file.");
		return 1;
	}
	CreateCharMap();
	ProcessMtx();
	fclose(mtx);
	fclose(fnt);
	fclose(outp);
	if(hasunrec)return 99;
}
