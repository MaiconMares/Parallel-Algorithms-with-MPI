#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

using namespace std;

void print(double **matrix, int lines, int cols) {
    for (int l = 0; l < lines; l++)
        for (int c = 0; c < cols; c++)
            cout << matrix[l][c] << (c+1 == cols ? '\n' : ' ');
    cout << endl;
}

void invertMatrix(double **originalMatrix, double **newMatrix, int origLines, int origCols) {
    for (int i = 0; i < origCols; i++)
        for (int j = 0; j < origLines; j++) {
            newMatrix[i][j] = originalMatrix[j][i];
        }
}

double ** allocMatrix(double **matrix, int lines, int cols) {
    matrix = (double **) malloc(lines*sizeof(double));

    if (matrix == NULL)
        cout << "Memory allocation error" << endl;

    for (int i = 0; i < lines; i++) {
        matrix[i] = (double *) malloc(cols*sizeof(double));
        if (matrix[i] == NULL)
            cout << "Memory allocation error" << endl;       
    }

    return matrix;
}

double computeModule(double *v, int lines) {
    double module = 0;
    
    for (int i = 0; i < lines; i++)
            module += v[i]*v[i];

    return sqrt(module);
}

void desallocMatrix(double **matrix, int lines) {
    for (int i = 0; i < lines; i++)
        free(matrix[i]);
    free(matrix);
}

double ** gramSchmidtProcedure(double **matrix, int lines, int cols) {
    double **invertedMatrix, *vProj, **resultMatrix;

    vProj = (double *) malloc(lines*sizeof(double));

    //The quantity of lines and cols, especifically in this case, are exchanged
    invertedMatrix = allocMatrix(invertedMatrix, cols, lines);

    //Operation to swap cols by lines
    //It isn't the inverse matrix, just an operation to simplify algorithm work
    invertMatrix(matrix, invertedMatrix, lines, cols);

    auto computeProj = [lines](double *u, double *z) { 
        double *proj = z, a = 0, b = 0;

        proj = (double *) malloc(sizeof(z));

        for (int i = 0; i < lines; i++) {
            a += u[i]*z[i];
            b += z[i]*z[i];
        }
        for (int j = 0; j < lines; j++)
            proj[j] = (a/b)*z[j];
        return proj;
    };

    for (int v = 1; v < cols; v++)
        for (int k = 0; k < v; k++) {
            vProj = computeProj(invertedMatrix[v], invertedMatrix[k]);
            for (int x = 0; x < lines; x++)
                invertedMatrix[v][x] -= vProj[x];
        }

    resultMatrix = allocMatrix(resultMatrix, lines, cols);

    //Store computed matrix after Gram-Schmidt procedure
    for (int i = 0; i < lines; i++)
        for (int j = 0; j < cols; j++)
            resultMatrix[i][j] = invertedMatrix[j][i];
    return resultMatrix;
}

double ** computeMatrixQ(double **ortoMatrix, int lines, int cols) {
    double invertedMatrix[cols][lines], vModule, **resultMatrix;

    for (int i = 0; i < cols; i++)
        for (int j = 0; j < lines; j++)
            invertedMatrix[i][j] = ortoMatrix[j][i];

    for (int v = 0; v < cols; v++) {
        vModule = computeModule(invertedMatrix[v], lines);
        for (int k = 0; k < lines; k++)
            invertedMatrix[v][k] /= vModule;
    }

    resultMatrix = allocMatrix(resultMatrix, lines, cols);
    
    for (int k = 0; k < lines; k++)
        for (int r = 0; r < cols; r++)
            resultMatrix[k][r] = invertedMatrix[r][k];

    return resultMatrix;
}

double ** computeMatrixR(double **q, double **orthoMatrix, double **a, 
    int matrixAlines, int matrixAcols) {
    double **invertedQ, **invertedOrtho, **invertedA, vModule = 0, 
            **resultMatrix;
    
    resultMatrix = allocMatrix(resultMatrix, matrixAcols, matrixAcols);
    
    invertedQ = allocMatrix(invertedQ, matrixAcols, matrixAlines);
    invertedA = allocMatrix(invertedA, matrixAcols, matrixAlines);
    invertedOrtho = allocMatrix(invertedOrtho, matrixAcols, matrixAlines);

    invertMatrix(q, invertedQ, matrixAlines, matrixAcols);
    invertMatrix(a, invertedA, matrixAlines, matrixAcols);
    invertMatrix(orthoMatrix, invertedOrtho, matrixAlines, matrixAcols);

    auto prodQA = [matrixAlines](double *q, double *a) {
        double matrixElem = 0;

        for (int i = 0; i < matrixAlines; i++)
            matrixElem += q[i]*a[i];

        return matrixElem;
    };

    for (int i = 0; i < matrixAcols; i++) {
        for (int j = 0; j < matrixAcols; j++) {
            if (i == j) {
                vModule = computeModule(invertedOrtho[i], matrixAlines);
                resultMatrix[i][j] = vModule;
            } else {
                resultMatrix[i][j] = prodQA(invertedQ[i], invertedA[j]);
            }
        }
    }

    return resultMatrix;
}

int main(int argc, char *argv[])
{
    int lines, cols, t = 0;

    cout << "Enter with dimension of matrix, lines and columns, respectively" << endl;
    cout << "Remember: the matrix must be formed by independent linear columns" << endl;
    cin >> lines >> cols;

    //Remember to verify if matrix has a det != 0
    double **a, **q, **r, 
        **orthoMatrix;

    a = allocMatrix(a, lines, cols);
    q = allocMatrix(q, lines, cols);
    orthoMatrix = allocMatrix(orthoMatrix, lines, cols);
    r = allocMatrix(r, cols, cols);


    for (int i = 0; i < lines; i++) {
        cout << "Enter with the line " << i << endl;
        for (int j = 0; j < cols; j++) {
            cin >> t;
            a[i][j] = t;
        }
    }

    orthoMatrix = gramSchmidtProcedure(a, lines, cols);
    q = computeMatrixQ(orthoMatrix, lines, cols);
    r = computeMatrixR(q, orthoMatrix, a, lines, cols);

    cout << "MATRIX A" << endl;
    print(a, lines, cols);

    cout << "MATRIX Q" << endl;
    print(q, lines, cols);

    cout << "MATRIX R" << endl;
    print(r, cols, cols);

    desallocMatrix(orthoMatrix, lines);
    desallocMatrix(q, lines);
    desallocMatrix(a, lines);
    desallocMatrix(r, cols);

    return 0;
}