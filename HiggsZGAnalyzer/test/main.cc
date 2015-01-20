#include <iostream>
#include <fstream>
using namespace std;

int main() {
  std::cout<<"this is it"<<std::endl;
  ofstream myfile;
  myfile.open ("example.txt");
  myfile << "Writing this to a file.\n";
  myfile.close();
  return 0;
}
