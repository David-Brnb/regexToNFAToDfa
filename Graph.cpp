#include <iostream> 
#include <bits/stdc++.h>

using namespace std;
typedef pair<char, int> pci;

#ifndef GRAPH
#define GRAPH

class Graph {       
private:         
    int N = 1e6;
    vector<vector<pci> > adj;
    vector<vector<pci> > adjDFA;
    int nfaBegin;
    int nfaEnd; 
    int last_used;
    set<char> abc;
    set<int> accepted;
    string polak;

    bool buildNFA(){
        int last_used_node = 0;
        int last_used_at = 0;
        vector<int> bgg(N, -1);
        vector<int> endg(N, -1);

        stack<int> sti;

        for(int i=0; i<polak.size(); i++){
            if(abc.count(polak[i])>0){
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
        }

        if(sti.empty())
            return false;
        int at = sti.top();
        sti.pop();

        nfaBegin = bgg[at];
        nfaEnd = endg[at];

        return true;
    }

    set<int> move(set<int> state, char letter){
        set<int> mve;
        for(auto e: state){
            for(auto n: adj[e]){
                if(n.first == letter){
                    mve.insert(n.second);
                }
            }
        }

        return mve;
    }

    set<int> closure(set<int> state){
        set<int> cre;
        map<int, bool> visited;

        for(auto e: state){
            //bfs
            queue<int> q;

            visited[e]=true;
            q.push(e);


            while(!q.empty()){
                int node = q.front();
                q.pop();

                // process node
                cre.insert(node);

                for(auto e: adj[node]){
                    int nd = e.second;
                    if(!visited[nd] && e.first == '#'){
                        visited[nd]=true;
                        q.push(nd);
                    }
                }
            }

        }

        return cre;
    }



    bool buildDFA(){
        map<set<int>, int> mapi;
        map<int, set<int>> ipam;
        accepted.clear();
        set<int> curr;
        queue<int> q;

        curr.insert(nfaBegin);
        curr = closure(curr);

        mapi[curr]=last_used;
        ipam[last_used] = curr;
        q.push(last_used);
        last_used++;

        while(!q.empty()){
            int n = q.front();
            q.pop();
            curr = ipam[n];
            set<int> st;
            for(auto let: abc){
                st = move(curr, let);
                // aqui podemos omitir el closure si guardamos los sets que entran y el resultado;
                st = closure(st);

                if(st.empty())
                    continue;

                if(mapi[st]!=0){
                    adjDFA[n].push_back(pci(let, mapi[st]));

                } else {
                    mapi[st]=last_used;
                    ipam[last_used]=st;
                    adjDFA[n].push_back(pci(let, last_used));
                    q.push(last_used);
                    last_used++;
                }

            }
        }

        // registramos todos los estados finales
        for(auto e: ipam){
            if(e.second.count(nfaEnd)>0)
                accepted.insert(e.first);
        }

        return true;
    }

public: 
    Graph(set<char> abec, string exp) {
        this->adj = vector<vector<pci>>(N);
        this->adjDFA = vector<vector<pci>>(N);
        this->nfaBegin = -1;
        this->nfaEnd = -1;
        this->last_used = 1;
        this->abc = abec;
        this->polak = exp;
    }

    bool buildGraphs(){
        if(!buildNFA()){
            cout << "NFA building failed" << endl;
            return false;
        }

        if(!buildDFA()){
            cout << "DFA building failed" << endl;
            return false;
        }

        return true;
    }

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
            if(nfaEnd != node) cout << node << " => [";

            int j=0;
            for(auto e: adj[node]){
                if(j!=0) printf(", ");
                cout << "(" << e.second << ", '"<< e.first <<"')";
                int nd = e.second;
                if(!visited[nd]){
                    visited[nd]=true;
                    q.push(nd);
                }
                j++;
            }

            if(nfaEnd != node) cout << "]"<< endl;
        }

        cout << "Accepting state: " << nfaEnd << endl;

    }

    void printDFA(){
        cout << endl;
        for(int i=1; i<last_used; i++){
            cout << char(i+64) << " => [";
            int j=0; 
            for(auto e: adjDFA[i]){
                if(j!=0) printf(", ");
                printf("('%c', '%c')", char(e.second+64), e.first);
                j++;
            }
            cout << "]" << endl;
        }

        cout <<"Accepting states: [";
        int j=0;
        for(auto e: accepted){
            if(j!=0) printf(", ");
            printf("'%c'", char(e+64));
            j++;
        }
        cout << "]" << endl;


    }



};


#endif