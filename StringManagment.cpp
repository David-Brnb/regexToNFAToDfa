#include <iostream> 
#include <bits/stdc++.h>

using namespace std;

#ifndef STRINGMANAGMENT
#define STRINGMANAGMENT

class StringManagment {
private: 
    map<char, int> mapAbcNum;
    set<char> abc;

    string refactoriza(string &str){
        string news="";
        for(int i=0; i<str.size(); i++){
            if(i!=0 && (str[i-1]==')' || str[i-1]=='*' || str[i-1]=='+' || mapAbcNum[str[i-1]] == 1) && (str[i]=='(' || mapAbcNum[str[i]] == 1)){
                news+=".";
            }
            news+=str[i];
        }

        return news;
    }

public: 
    StringManagment(){};

    bool mapeo(string str){
        mapAbcNum.clear();
        abc.clear();

        for(auto e: str){
            if(e=='(' || e=='*' || e=='+' || e=='.' || e=='|' || e==')' || e=='#')
                return false;
        }

        for(auto e: str){
            mapAbcNum[e]=1;
            abc.insert(e);
        }

        return true;
    }

    string polish(string str){
        str = refactoriza(str);
        map<char, int> mapi;
        mapi['('] = 0;
        mapi['|'] = 1;
        mapi['.'] = 2;
        mapi['+'] = 3;
        mapi['*'] = 3;
        mapi[')'] = -1;

        stack<char> sti;
        queue<char> q; 

        for(int i=0; i<str.size(); i++){
            if(abc.count(str[i])>0){
                q.push(str[i]);

            } else if(str[i] == ')'){
                while(!sti.empty() && sti.top() !='('){
                    q.push(sti.top());
                    sti.pop();
                }

                if(!sti.empty())
                    sti.pop();

            } else {
                while(!sti.empty() && mapi[sti.top()]>=mapi[str[i]] && str[i]!='('){
                    if(sti.top()!='(') q.push(sti.top());
                    sti.pop();
                }
                sti.push(str[i]);
            }
        }

        while(!sti.empty()){
            if(sti.top()!='(') q.push(sti.top());
            sti.pop();
        }
        
        string news="";
        while(!q.empty()){
            news+=q.front();
            q.pop();
        }

        return news;
    }

    set<char> getAlph(){
        return this->abc;
    }

};




#endif