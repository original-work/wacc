#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;
 
vector<vector<double> > buffer;
vector<vector<double> >::iterator i;
vector<double>::iterator j;
 
int main()
{
	ifstream file("Data.txt");
	double tmp_d;
	char tmp_c;

	string tmp;
	stringstream ss;

	while (file >> tmp ) {
		ss<<tmp;
		vector<double> v;
		for(int i=0;i<5;++i)
		{
			ss>>tmp_c>>tmp_d;
			v.push_back(tmp_d);
		}
		ss.get();
		buffer.push_back(v);
	}

	for(i=buffer.begin();i!=buffer.end();++i)
	{
		for(j=i->begin();j!=i->end();++j)
		{
			cout<<setfill('0')<<setw(5)<<*j<<"\t";
		}
		cout<<endl;
	}

	return 0;
}
