#include "candidate_set.h"
#include "common.h"
#include "graph.h"

#define MAX 15
/*
N-Queens Problem

Representative backtracking problem. 
*/

using namespace std;

/*
int col[MAX];
int N, total = 0;

bool check(int level){
    for(int i = 0; i < level; i++){
        if(col[i] == col[level] || abs(col[level] - col[i]) == (level - i))
            return false;
    }
    return true;
}

void nqueen(int x){
    if(x == N)
        total++;
    else{
        for(int i = 0; i < N; i ++){
            col[x] = i;
            if(check(x))
                nqueen(x+1);
        }
    }
}
*/

int N, S, sum, cnt = 0;
int arr[21];

void dfs(int i, int sum){
    if(i == N) return;
    if(sum + arr[i] == S) cnt++;

    dfs(i+1, sum);
    dfs(i+1, sum + arr[i]);
}

int main(){
    cin >> N >> S;
    for(int i = 0; i < N; i++)
        cin >> arr[i];
    
    cnt = 0;
    dfs(0, 0);

    cout << cnt;
}