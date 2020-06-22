#include <bits/stdc++.h>
#include <dirent.h>
#include <iostream>
#include <string>
using namespace std;
std::vector<std::string> get_all_files(std::string path, std::string suffix)
{
    std::vector<std::string> files;
    files.clear();
    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(path.c_str())) == NULL)
    {
        cout << "Can not open " << path << endl;
        return files;
    }
    while((dirp = readdir(dp)) != NULL)
    {
        if(strstr(dirp->d_name,suffix.c_str())!=NULL)
        {
        	files.push_back(string(dirp->d_name));
		}
    }
    closedir(dp);
    return files;
}
int main()
{
	auto ss=get_all_files(".\\dat\\",".fnt");
	for(auto s:ss)
	{
		
		int num;
		sscanf(s.c_str(),"%d",&num);
		char cmd[100];
		sprintf(cmd,"mtxdecoder dat\\%04d dat\\%04d.fnt %04d.txt",num+1,num,num+1);
		int el=system(cmd);
		cout<<s<<' '<<el<<endl;
	}
}
