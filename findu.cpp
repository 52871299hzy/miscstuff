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
int main()
{
	auto ss=get_all_files("..\\wiitxt\\",".txt");
	auto sss=get_all_files("..\\ndstextdump\\",".txt");
	for(auto s:ss)
	{
		string ans="nomatch";
		for(auto t:sss)
		{
			char cmd[100];
			sprintf(cmd,"fc /b ..\\wiitxtn\\%s ..\\ndstextdump\\%s >nul",s.c_str(),t.c_str());
			if(!system(cmd))
			{
				ans=t;
				break;
			}
		}
		cout<<s<<"->"<<ans<<endl;
	}
}
