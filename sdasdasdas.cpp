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
    	string s=dirp->d_name;
        if(s!="."&&s!=".."&&strstr(dirp->d_name,suffix.c_str())!=NULL)
        {
        	files.push_back(string(dirp->d_name));
		}
    }
    closedir(dp);
    return files;
}
string sn;
int main()
{
	auto ss=get_all_files("..\\ndstextdump\\",".fnt");
	for(auto s:ss)
	{
		sn=s;
		sn.resize(s.length()-4);
		char cmd[1000];
		sprintf(cmd,"mtxdecoder ..\\ndstextdump\\%s.mtx ..\\ndstextdump\\%s.fnt ..\\ndstextdump\\%s.mtx.txt --noctrlchr",sn.c_str(),sn.c_str(),sn.c_str());
		int el=system(cmd);
		cout<<s<<' '<<el<<endl;
	}
}
