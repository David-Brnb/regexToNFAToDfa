#include <iostream> 
#include <bits/stdc++.h>

using namespace std;
typedef pair<char, int> pci;

#ifndef GRAPH
#define GRAPH

/*
    Esta clase se encarga de la construcción de los autómatas, tanto determinístico como no determinístico. 
    Para ello, se declara un grafo para el NFA (Automata Finito No Determinista) y otro para el DFA (Automata Finito Determinista).
*/
class Graph {       
private:
    vector<vector<pci> > adj; // Lista de adyacencia para el NFA
    vector<vector<pci> > adjDFA; // Lista de adyacencia para el DFA
    map<int, set<int> > closureDP; // Map para almacenar closures calculados
    map<int, bool > closureDPB; // Map para indicar si un closure ya fue calculado
    set<char> abc; // Alfabeto del autómata
    set<int> accepted; // Estados de aceptación del DFA
    int N = 1e6; // Límite superior para los nodos
    int nfaBegin; // Nodo inicial del NFA
    int nfaEnd;  // Nodo final del NFA
    int last_used; // Último nodo utilizado en la construcción del DFA
    string polak; // Expresión regular en notación posfija

    /*
        Este método construye el NFA (Automata Finito No Determinista) de manera algorítmica.
        Aprovecha la expresión regular en notación posfija y aplica los operadores de expresiones regulares 
        de forma similar a los operadores aritméticos. 
        Utiliza una lista de adyacencia para representar el grafo y manejar la conexión de nodos.

        Complejidad temporal: O(n) amortizada, donde n es el tamaño de la expresión. 
        Se recorre la expresión de manera lineal y las operaciones son constantes.
    */
    bool buildNFA(){
        int last_used_node = 0;
        int last_used_at = 0;
        vector<int> bgg(N, -1); // Nodos iniciales de sub-autómatas
        vector<int> endg(N, -1); // Nodos finales de sub-autómatas

        stack<int> sti; // Pila para manejar los estados

        for(int i=0; i<polak.size(); i++){
            if(abc.count(polak[i])>0){
                // Crear un nuevo autómata para el símbolo
                int newBg = last_used_node, newEnd = last_used_node+1;
                int currAt = last_used_at;
                last_used_node += 2;
                last_used_at += 1;

                if(newBg>=N || newEnd >=N || currAt>=N)
                    return false; // Validación de límites

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

    /*
        Método auxiliar para calcular los estados alcanzables desde un conjunto de nodos 
        bajo una transición con un carácter específico del alfabeto.

        Complejidad temporal: O(V), donde V es la cantidad de nodos en el NFA. 
        Esto se debe a que, por construcción, cada nodo tiene un máximo de 3 aristas salientes.
    */
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

    /*
        Método auxiliar para calcular el cierre epsilon (epsilon-closure) de un conjunto de nodos.
        Realiza un BFS para incluir todos los nodos alcanzables a través de transiciones epsilon (#).

        Complejidad temporal: O(V * (V + E)), donde V son los nodos y E las aristas del NFA. 
        Usa programación dinámica para evitar cálculos redundantes, mejorando el rendimiento.
    */
    set<int> closure(set<int> state){
        set<int> result;

        for(auto e: state){
            // Realizar BFS desde el nodo
            set<int> cre;
            map<int, bool> visited;

            if(closureDPB[e]){
                // Usar closure previamente calculado
                cre = closureDP[e];
                for(auto node: cre){
                    result.insert(node);
                }
                continue;
            } 

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

            closureDPB[e]=true;
            closureDP[e]=cre;

            for(auto node: cre){
                result.insert(node);
            }
        }
        
        return result;
    }


    /*
        Este método construye el DFA (Automata Finito Determinista) a partir del NFA.
        Utiliza los métodos auxiliares `move` y `closure`, así como el grafo del NFA.

        Complejidad temporal: O(2^V * (V * (V + E))), donde V son los nodos y E las aristas del NFA. 
        Esto se debe a que el número máximo de estados posibles en el DFA es 2^V (conjuntos de nodos del NFA).
    */
    bool buildDFA(){
        map<set<int>, int> mapi; // Mapeo de conjuntos de nodos a índices
        map<int, set<int>> ipam; // Mapeo inverso de índices a conjuntos de nodos
        accepted.clear();
        closureDP.clear();
        closureDPB.clear();
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

                // Usar cierre calculado previamente si está disponible
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

        // Registrar los estados finales
        for(auto e: ipam){
            if(e.second.count(nfaEnd)>0)
                accepted.insert(e.first);
        }

        return true;
    }

public: 
    // Constructor que inicializa las variables de la clase
    Graph(set<char> abec, string exp) {
        this->adj = vector<vector<pci>>(N);
        this->adjDFA = vector<vector<pci>>(N);
        this->nfaBegin = -1;
        this->nfaEnd = -1;
        this->last_used = 1;
        this->abc = abec;
        this->polak = exp;
    }

    // Construcción de los autómatas NFA y DFA. Informa si ocurre algún error.
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

    // Imprime el NFA realizando un recorrido BFS
    // Complejidad: O(V + E), donde V son los nodos y E las aristas.
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

    // Imprime el DFA
    // Complejidad: O(V*(V-1)), donde V son los nodos del DFA.
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