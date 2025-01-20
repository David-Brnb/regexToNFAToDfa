#include <iostream> 
#include <bits/stdc++.h>

using namespace std;
typedef pair<char, int> pci;

#pragma once
#ifndef GRAPH
#define GRAPH

class Graph {       
private:         
    int N = 1e6;
    vector<vector<pci> > adj;
    int nfaBegin;
    int nfaEnd; 
    set<char> abc;

public: 
    Graph(set<char> abec) {
        this->adj = vector<vector<pci>>(N);
        this->nfaBegin = -1;
        this->nfaEnd = -1;
        this->abc = abec;
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



};


#endif