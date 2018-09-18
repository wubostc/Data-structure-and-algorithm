
//T 为输入的行数

//输入
//2
//aaaabbbbbyyyyiuuuaaa
//dddd####aaadddc111

//输出
//i
//c


#include <iostream>
#include <map>
using namespace std;

int main()
{
    int T;
    cin >> T;

    string str;
    int len;


    map<char, int> m;
    map<char, int>::iterator it;

    for(int i= 0; i< T;++i)
    {
        cin >> str;
        len = str.length();

        for(int j = 0; j <len; ++j)
        {
            if((it = m.find(str[j])) != m.end())
            {
                it->second = 1;
            }
            else
            {
                m.insert(make_pair(str[j],0));
            }
        }

        for(auto &x:m)
        {
            if(x.second == 0)
            {
                cout << x.first;
            }
        }

        m.clear();
        str.clear();
    }

}

