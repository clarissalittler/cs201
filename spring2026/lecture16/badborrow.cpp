#include <vector>
#include <iostream>
using namespace std;


int main(){

  vector<int> v{1, 2, 3};
  int& first = v[0];
  cout << first << endl;
  v.push_back(4);             // realloc moves the buffer; `first` now dangles
  cout << first << endl;      // this compiles but is naaaasty
}
