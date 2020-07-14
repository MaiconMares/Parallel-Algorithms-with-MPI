#include <algorithm>
#include <iostream>
#include <fstream>
#include <new>
#include <cmath>
#include <string>
#include <iomanip>
using namespace std;

template<typename T> T** matrixAllocate(int rows, int cols, T **M)
{
    M = new T*[rows];
    for (int i = 0; i < rows; i++){
        M[i] = new T[cols];
    }
    return M;
}

//volta o produto escalar entre duas colunas de duas matrizes
double dot(double **Mat1,double **Mat2,const int rows,const int nc1,const int nc2){
 double res=0.f;
 for(int i=0;i<rows;i++){
  res+=Mat1[i][nc1]*Mat2[i][nc2];
 }
 return res;
}

//transfere a coluna de Mat1  para uma coluna de Mat2
void vecassign(double **Mat1,double **Mat2,const int nl,const int nc1,const int nc2){
 for(int i=0;i<nl;i++){
  Mat2[i][nc2]=Mat1[i][nc1];
 }
}

//Armazena o produto de uma coluna de Mat2 por um escalar na coluna de Mat1
void scalar(double **Mat1,double **Mat2,const double scl,const int nl,const int nc){
 for(int i=0;i<nl;i++){
  Mat1[i][nc]=scl*Mat2[i][nc];
 }
}

//Subtrai uma coluna de Mat2 multiplicada por um escalar de uma coluna de Mat1
void ortog(double **Mat1,double **Mat2,const double scl,const int nl,const int nc1,const int nc2){
 for(int i=0;i<nl;i++){
  Mat1[i][nc1]-=scl*Mat2[i][nc2];
 }
}

//inicia com zero as entradas de uma matriz 
void init(double **Mat,const int nl,const int nc){
 for(int i=0;i<nl;i++){
  for(int j=0;j<nc;j++){
   Mat[i][j]=0.f;
  }
 }
}

//tem como saida a matriz cujas colunas foram ortogonalizadas por Gram-Schmidt
double** GSt(double **Mat,const int nl, const int nc, const string met){
  double** M2= matrixAllocate(nl, nc, M2);
  double** M3 = matrixAllocate(nl, nc, M3);
  double rij,rjj;
  init(M2,nl,nc);
  init(M3,nl,nc);
  for(int j=0;j<nc;j++){
    vecassign(Mat,M2,nl,j,j);
    for(int i=0;i<j;i++){
      if(met=="CGS") rij=dot(M3,Mat,nl,i,j);
      else rij=dot(M3,M2,nl,i,j);
      ortog(M2,M3,rij,nl,j,i);
    }
    rjj=sqrt(dot(M2,M2,nl,j,j));
    scalar(M3,M2,1.f/rjj,nl,j);
  }
  return M2;
}

int main()
{
  ifstream arq;
  arq.open ("matrizP5x5.dat");     //modificar para cada caso em estudo "matriz.dat", "matriz20x20.dat" ou "matriz50x50.dat"
  int lin,col;
  string method;
  cout << "Dimensões da matriz: ";  // deve-se entrar com as dimensões da matriz a ser lida separadas por espaço
  cin >> lin >> col;
  cout << "digite CGS ou MGS: ";    //CGS é o Gram-Schmidt clássico e MGS o modificado.
  cin >> method;
  double** M1 = matrixAllocate(lin, col, M1);
  double** res;
  for(int i=0;i<lin;i++){
     for(int j=0;j<col;j++){
     arq >> M1[i][j];
     }
  }
  res=GSt(M1,lin,col,method);
  std::cout << std::setprecision(8) << std::fixed;
  for(int i=0;i<lin;i++){
    for(int j=0;j<col;j++){
      cout << res[i][j] << "\t" << " \n"[j == col-1];
    }
  }
  for(int i=0;i<col;i++){
    for(int j=i+1;j<col;j++){
      cout << "produto das colunas " << i+1 << " " << j+1 << ":\t" << dot(res,res,lin,i,j) << endl;
    }
  }
  arq.close();
  delete [] M1;
  return 0;
}
