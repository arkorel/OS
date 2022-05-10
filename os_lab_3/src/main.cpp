#include<iostream>
#include<fstream>
#include<thread>
#include<vector>
#include<malloc.h>
#include<unistd.h>
using namespace std;

void dosmth(){
	sleep(10);
}

void free_pol(int *result, int *pol, int size){
	for (int i = 0; i<size; i++){
		pol[i] = result[i];
		result[i] = 0;
	}
}

void print_pol(int *polynomial, int size){
	for (int i = size-1; i >0; i--){
	       cout<<polynomial[i]<<"x^"<<i<<"+";
       }
       cout<<polynomial[0]<<endl;

}	       

void new_polyn(int *result, int *minm, int *maxm, int n, int m, int K, int np){
      // cout<<"!np="<<np<<"! n="<<n<<" m="<<m<<" K="<<K<<" np="<<np<<endl; fflush;
	for(int i = 0; i<n; i++) {
	        //cout<<"!np="<<np<<" jbegin="<<(K-(i%K)+np)%K<<endl; fflush;
		for(int j = (K-(i%K)+np)%K ; j < m; j = j + K){  
	 	//	cout<<"!np="<<np<<"! i="<<i<<" j="<<j<<" K="<<K<<endl;fflush;
		       result [i+j] = result[i+j] + (minm[i] * maxm[j]);
	        //       cout<<"!np="<<np<<"! i="<<i<<" result="<<result[j]<<endl;fflush;
	       }
       }
}    	       

int main(){
	int N;
	int answere_degree=0;
	cout<<"please, enter the number of polynominals"<<std::endl;
	cin>>N;
	if (N<2){
		cout<<"Error!";
		return 1;
	}
	int K;

	cout<<"please, insert number of threads. Insert -1 if there are no restrictions:"<<endl;
	cin>>K;
	if (!K || (K<0 && K != -1)){
		cout << "Error"<<endl;
		return 1;
	}
	if (K == -1) {K = 2;}
	int a, max; max = -1;
	cout<<"please,enter degree of first polynominal"<<endl;
	cin>>a;
	int idea2[N];
	idea2[0]= a;
	answere_degree = a ;
	for (int j = 1;j < N; j++){
		cout<<"please, enter degree of next polynomial" << endl; 
		int degree;
		cin>>degree;
		if (degree > max) {max = degree;}
		idea2[j] = degree;
		answere_degree += (degree-1) ;
	}
//	for(int i = 0; i<N; i++){
//		cout<<"degree:"<<i<<"="<<idea2[i]<<endl;
//	}
//	cout<<"*******************"<<endl;
	
	int *pol1 = new int [max];
	//cout << "answere degree="<<answere_degree<<endl; 
	int *pol = new int[answere_degree];
	cout<<"please, enter coefficients of first polynominal, starting with the oldest"<<endl;
	for (int i = a-1; i>=0; i--){
		cin>>pol[i];
	}
        fflush;
	int *result = new int[answere_degree];
	for(int i = 0; i<answere_degree; i++){
		result[i] = 0;
	}
//	for (int i = a-1 ; i >=0; i--){
//	   result[i] = pol[i]; 
//	} 	
//        cout<<"test1"; fflush;
	int k = 1;
	
	if (N!=1){
		thread th[K];
	while (N > 1){
		int b;
		cout << k << endl;
		b = idea2[k];
	        k = k + 1;	
		cout<<"please, enter coefficients of " << k <<" polynominal, starting with the oldest"<<std::endl;
		for (int i = b-1; i>=0; i--){
			cin>>pol1[i];
		}
		cout<<"  "; print_pol(pol, a);fflush; 
		cout<<"*"<<endl;
	        cout<<"  ";print_pol(pol1, b); fflush;
		cout<<"_________________________________________________________"<<endl;
	        
		//cout << "test K="<<K<<" a="<<a<<endl; fflush;

		/*for (int i = 0 ; i<a; i++){
			cout<<"pol["<<i<<"]="<<pol[i]<<endl;
		}
		for (int j = 0; j<b; j++){
			cout<<"pol1["<<j<<"]="<<pol1[j]<<endl;
		}
		cout <<"result:";
		for (int i = 0; i<answere_degree; i++){
		      cout <<result[i]<<" ";
		}
		cout<<endl;
                cout <<"answere_degree="<<answere_degree<<endl;*/
		for (int i = 0; i<K; i++){
			if (a>b){
			     th[i] = thread(new_polyn, result, pol, pol1, a, b, K, i);
			     //cout<<"i="<<i<<"=>"; 
			}	
			else{
			      th[i] = thread(new_polyn, result, pol1, pol, b, a, K, i);
			      //cout<<"2i="<<i<<"=>";
			}
		}
		for(int i = 0; i<K; i++){
			th[i].join();
		}
	
	        //cout<<"after join"<<endl; fflush;	
	        for (int i = b-1; i>=0; i--){
		    pol1[i] = 0;
		}	    
		a = a + b - 1;
		N = N - 1;
		print_pol(result, answere_degree);
		free_pol(result, pol, answere_degree);
		      	
             }
	  }
        delete[] result;
        delete[] pol;
        delete[] pol1;
}
	
