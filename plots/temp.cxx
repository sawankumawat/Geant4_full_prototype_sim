#include<iostream>
using namespace std;

void temp() {

    for (int i = 0; i < 72*10; i++)
    {
        int j = i % 72;
        // cout<<j<<endl;
        int row = j / 8;
        int col = j % 8;
        cout<<row<<" "<<col<<endl;
    }
    
}