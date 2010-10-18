#ifndef VRPTW_PROBLEMINSTANCE_HPP_
#define VRPTW_PROBLEMINSTANCE_HPP_

#include <iostream>

#include "../../Util/Scanner++/Scanner.h"

using namespace std;

class VRPTWProblemInstance
{
public:
	int n,v,c;
	vector<int> x,y,d,ti,tf,ts;
    double ** dist;
	VRPTWProblemInstance(Scanner& scan)
	{
	// Put here your code
        // You can read the input data from the 'scanner' object
		//Scanner scan(new File(arq));
		string str;
		for (int i = 0; i < 4; i++) str = scan.next();
		v = scan.nextInt(); c = scan.nextInt();
		for (int i = 0; i < 12; i++) str = scan.next();
//		int i = 0;
		while (scan.hasNext()) {
			scan.nextInt();
			x.push_back(scan.nextInt()); // x coord
			y.push_back(scan.nextInt()); // y coord
			d.push_back(scan.nextInt()); // demandas
			ti.push_back(scan.nextInt());// tempo de inicio
			tf.push_back(scan.nextInt());// tempo de fim
			ts.push_back(scan.nextInt());// tempo de serviço
//			cout << i++ << "\t" << x.back() << "\t" << y.back() << "\t" << d.back() << "\t" << ti.back() << "\t" << tf.back() << "\t" << ts.back() << "\n";
		}
		n = x.size();
		dist = new double*[n+1];
        for (int i = 0 ; i <= n ; i++) {
	        dist[i] = new double[n+1];
            for (int j = 0 ; j <= n ; j++) {
               dist[i][j] = sqrt( pow( x[j] - x[i] ,2) + pow( y[j] - y[i] ,2) );
               //cout << dist[i][j] << " ";
            }
            //cout << "\n";
        }
//		cout << n << "\t" << v << "\t" << c << "\n";
//		cout << "Leitura feita! " << n << " cidades lidas."<<endl;
	};
	virtual ~VRPTWProblemInstance(){ /*Implementar*/ };
private:
   // Your private vars
};


#endif /*VRPTW_PROBLEMINSTANCE_HPP_*/
