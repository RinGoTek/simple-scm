#include <iostream>
using namespace std;


int main(int count, char* parameters[]) {
    cout<<"count: "<<count<<endl;
    for(int i=0;i<count;++i)
        cout<<"parameter "<<i<<' '<<parameters[i]<<endl;
}
