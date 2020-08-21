#include <algorithm>
#include <iostream>
#include <fstream>
#include <new>
#include <cmath>
#include <iomanip>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>

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
double dot(double **Mat1,double **Mat2,const int rows,const int nc1,const int nc2, float *greatestProduct = 0){
  double res=0.f;

  for(int i=0;i<rows;i++){
    res+=Mat1[i][nc1]*Mat2[i][nc2];
  }
  
  if (isgreaterequal(abs(res*1E8), abs(*greatestProduct)))
    *greatestProduct = abs(res*1E8);
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
 float temp;
 init(M2,nl,nc);
 init(M3,nl,nc);
 for(int j=0;j<nc;j++){
   vecassign(Mat,M2,nl,j,j);
   for(int i=0;i<j;i++){
     if(met=="CGS") rij=dot(M3,Mat,nl,i,j, &temp);
     else rij=dot(M3,M2,nl,i,j, &temp);
     ortog(M2,M3,rij,nl,j,i);
    }
   rjj=sqrt(dot(M2,M2,nl,j,j, &temp));
   scalar(M3,M2,1.f/rjj,nl,j);
 }
 return M2;
}

int main(int argc, char *argv[])
{
  int rank, nprocs;
  double elapsed, start = 0.0, end = 0.0;
  float greatestProduct = 0.0;
  char filename[128];
  ifstream arq;
  int lin=100,col=100; // deve-se entrar com as dimensões da matriz a ser lida
  string method="CGS";  //CGS é o Gram-Schmidt clássico e MGS o modificado.

  double** M1 = matrixAllocate(lin, col, M1);

  if(argc < 2) {
    cout << "argumento faltando: nome do arquivo" << endl;
    exit(-1);
  }
  strcpy(filename, argv[1]);
  MPI_Init(NULL, NULL);
  start = MPI_Wtime();

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  double** res = matrixAllocate(lin, col, res);

  arq.open (filename);
  for(int i=0;i<lin;i++){
     for(int j=0;j<col;j++){
     arq >> M1[i][j];
     }
  }
  arq.close();
  res=GSt(M1,lin,col,method);
  MPI_Barrier(MPI_COMM_WORLD);

  std::cout << std::setprecision(8) << std::fixed;
  for(int i=0;i<col;i++){
    for(int j=i+1;j<col;j++){
      dot(res,res,lin,i,j, &greatestProduct);
    }
  }

  end = MPI_Wtime();
  elapsed = (end-start)*1E3;
    
  printf("Gram-Schmidt process from process %d of %d\n", rank, nprocs);
  printf("Matrix %d x %d\n", lin, col);
  printf("Total time = %lfms\n", elapsed);
  printf("Ek = %.8f\n", greatestProduct);

  delete [] M1;
  delete [] res;
  MPI_Finalize();
}
