#include <iostream> 
#include <bits/stdc++.h>

using namespace std; 

typedef pair<char, int> pci;
map<char, int> mapAbcNum;
set<char> abc;
int N = 1e6;

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

vector<vector<pci> > adj(N);
int nfaBegin=-1, nfaEnd=-1;

void bfs(){
    vector<bool> visited(nfaEnd+10, false);
    cout << endl;
    queue<int> q;
    if(nfaBegin==-1){
        nfaBegin = 0;
    } 

    visited[nfaBegin]=true;
    q.push(nfaBegin);


    while(!q.empty()){
        int node = q.front();
        q.pop();

        // process node
        cout << node << ": ";

        for(auto e: adj[node]){
            cout << "{ " << e.first << " , " << e.second << " } ";
            int nd = e.second;
            if(!visited[nd]){
                visited[nd]=true;
                q.push(nd);
            }
        }
        cout << endl;
    }

}

bool buildNFA(string polak){
    int last_used_node = 0;
    int last_used_at = 0;
    vector<int> bgg(N, -1);
    vector<int> endg(N, -1);

    stack<int> sti;

    for(int i=0; i<polak.size(); i++){
        if(mapAbcNum[polak[i]] == 1){
            int newBg = last_used_node, newEnd = last_used_node+1;
            int currAt = last_used_at;
            last_used_node += 2;
            last_used_at += 1;

            if(newBg>=N || newEnd >=N || currAt>=N)
                return false;

            adj[newBg].push_back(pci(polak[i], newEnd));

            bgg[currAt] = newBg;
            endg[currAt] = newEnd;

            sti.push(currAt);


        } else {
            if(polak[i] == '*' || polak[i] == '+'){
                if(sti.empty())
                    return false;
                
                int token = sti.top(); 
                sti.pop();

                int newBg = last_used_node, newEnd = last_used_node+1;
                int currbg=bgg[token], currEnd=endg[token];
                last_used_node += 2;

                bgg[token] = newBg;
                endg[token] = newEnd;

                adj[newBg].push_back(pci('#', currbg));
                adj[currEnd].push_back(pci('#', newEnd));
                adj[currEnd].push_back(pci('#', currbg));

                if(polak[i] == '*')
                    adj[newBg].push_back(pci('#', newEnd));
                
                sti.push(token);

            } else if(polak[i] == '|'){
                int newBg = last_used_node, newEnd = last_used_node+1;
                last_used_node+=2;

                if(sti.empty())
                    return false;
                int token2 = sti.top(); 
                sti.pop();

                if(sti.empty())
                    return false;
                int token1 = sti.top(); 
                sti.pop();
                
                adj[newBg].push_back(pci('#', bgg[token1]));
                adj[newBg].push_back(pci('#', bgg[token2]));
                adj[endg[token1]].push_back(pci('#', newEnd));
                adj[endg[token2]].push_back(pci('#', newEnd));

                bgg[token1] = newBg;
                endg[token1] = newEnd;

                bgg[token2] = -1;
                endg[token2] = -1; 

                sti.push(token1);            

            } else if(polak[i]=='.'){
                if(sti.empty())
                    return false;
                int token2 = sti.top(); 
                sti.pop();

                if(sti.empty())
                    return false;
                int token1 = sti.top(); 
                sti.pop();
                
                adj[endg[token1]].push_back(pci('#', bgg[token2]));
                endg[token1] = endg[token2];

                bgg[token2] = -1;
                endg[token2] = -1; 

                sti.push(token1);  
            }

        }
        cout << polak[i] << " ";
    }

    if(sti.empty())
        return false;
    int at = sti.top();
    sti.pop();

    nfaBegin = bgg[at];
    nfaEnd = endg[at];

    return true;
}

vector<vector<pci> > adjDFA(N);

//mejor le paso el set
set<int> closure(int node){
    set<int> ab;
    //hacer bfs

    return ab;

}

set<int> move(char let, set<int> cj){
    set<int> ab;
    //hacer solo un recorrido a los adyacentes

    return ab;
}

bool buildDFA(int begin, int end){
    map<set<int>, int> mapi;
    map<int, set<int>> ipam;
    queue<int> q;
    int last_used_node=1;

    //mejor hago el ciclo por fuera y solo lo que tiene que hacer move;
    set<int> curr = closure(begin);
    mapi[curr] = last_used_node;
    ipam[last_used_node] = curr;
    q.push(last_used_node);
    last_used_node++;

    while(!q.empty()){
        int node = q.front();
        q.pop();

        //esto se tiene que repensar, no me funciona asi.
        for(auto e: ipam[node]){
            set<int> moves;
            for(auto l: abc){
                curr = move(l, e);
            }

        }

    }





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