#include "LIS.h"
#include "Matrix.h"
#include "Pack.h"
#include <bits/stdc++.h>
using namespace std;
int main() {

    int type;
    
    LIS lis;
    Matrix maxtrix;
    Pack pack;

    int type;

    scanf("%d", &type);
    
    if(type == 1) {
        lis.start();
    }

    else if(type == 2) {
       matrix.start(); 
    }

    else if(type == 3) {
       pack.start();
    }

    return 0;
}
