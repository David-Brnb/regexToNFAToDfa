#include <iostream> 
#include <bits/stdc++.h>

using namespace std; 

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
        if(mapAbcNum[str[i]]==1){
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

int main(){
    string abc, arg; 
    cin >> abc; 
    if(!mapeo(abc)){
        cout << "Existe un error en el abecedario, no puedes agregar '(, ), +, *, ., |, #' ";
        return 0;
    }

    // revisar los parentesis ?...

    cin >> arg;
    arg = refactoriza(arg);
    cout << arg << endl;

    arg = polish(arg);
    cout << arg << endl;

    if(!buildNFA(arg)){
        cout << "wtf" << endl;
        return 0;
    }
    cout << "NFA:" << endl;
    bfs();
}