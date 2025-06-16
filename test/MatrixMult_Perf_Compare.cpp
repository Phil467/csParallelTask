#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <string.h>
#include "csPARALLEL.h"
#include "csPERF_MEASUREMENT.h"

// Structure to store a matrix
struct Matrix
{
    std::vector<double> data;
    size_t rows;
    size_t cols;

    typedef struct
    {
        int x, y, cx, cy;
    }Section;

    Matrix(size_t r, size_t c) : rows(r), cols(c), data(r * c, 0.0) {}

    double& at(size_t i, size_t j)
    {
        return data[i * cols + j];
    }

    double at(size_t i, size_t j) const
    {
        return data[i * cols + j];
    }

    void randomize(double min, double max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min, max);

        for (size_t i = 0; i < data.size(); i++)
        {
            data[i] = dis(gen);
        }
    }

};

void printMulOp(Matrix A, Matrix B, Matrix X, Matrix::Section ASec, Matrix::Section BSec, int precision)
{
    if(A.cols >= ASec.cx+ASec.x && A.rows >= ASec.cy+ASec.y && B.cols >= BSec.cx+BSec.x && B.rows >= BSec.cy+BSec.y)
    {
        int xfirstPart = ASec.cx + 1;
        int yfirstPart = ASec.cy + 1;

        int width = xfirstPart + BSec.cx;
        int height = yfirstPart + BSec.cy;

        if(ASec.y > 0)
        {
            if(BSec.x > 0)
            {
                cout<<"  ";
            }
            for(int i=0; i<width; i++)
            {
                if(i > BSec.cx)
                {
                    cout<<setw(precision+2+2)<<":";
                }
                else
                {
                    cout<<setw(precision+2+2)<<" ";
                }
                if(i != BSec.cx)
                    cout<<" ";
            }
            cout<<"\n";
        }

        for(int j=0; j<height; j++)
        {
            if(BSec.x > 0)
            {
                if(j >= yfirstPart) cout<<"..";
                else cout<<"  ";
            }

            for(int i=0; i<width; i++)
            {
                if(i < BSec.cx && j < yfirstPart)
                {
                    cout<<setw(precision+2+2+1)<<" ";
                }
                else if(i == BSec.cx)
                {
                    if(j >= yfirstPart && BSec.x+BSec.cx < B.cols)
                        cout<<setw(precision+2+2)<<"....";
                    else
                        cout<<setw(precision+2+2)<<" ";
                }
                else if(j == ASec.cy)
                {
                    cout<<setw(precision+2+2)<<":";
                    cout<<" ";
                }
                else if(i < BSec.cx && j >= yfirstPart)
                {
                    cout<<setw(precision+2+2)<<fixed<<setprecision(precision)<<B.at(i+BSec.x, j-yfirstPart+BSec.y)<<" ";
                }
                else if(i >= xfirstPart && j < ASec.cy)
                {
                    cout<<setw(precision+2+2)<<fixed<<setprecision(precision)<<A.at(i-xfirstPart+ASec.x, j+ASec.y)<<" ";
                }
                else if(i >= xfirstPart && j >= yfirstPart)
                {
                    cout<<setw(precision+2+2)<<fixed<<setprecision(precision)<<X.at(i-xfirstPart+BSec.x, j-yfirstPart+ASec.y)<<" ";
                }
            }
            cout<<"\n";
            if(j == ASec.cy)
            {
                if(BSec.x > 0)
                {
                    if(j >= yfirstPart) cout<<"..";
                    else cout<<"  ";
                }
                for(int i=0; i<width; i++)
                {

                    if(ASec.x+ASec.cx <= A.rows && i >= xfirstPart)
                    {
                        cout<<setw(precision+2+2)<<":";
                        cout<<" ";
                    }
                    else
                    {
                        if(i == BSec.cx)
                            cout<<setw(precision+2+2)<<" ";
                        else
                        {
                            cout<<setw(precision+2+2)<<" ";
                            cout<<" ";
                        }
                    }
                }
                cout<<"\n";
            }
        }
    }
    else
    {
        cout << "Section out of range !";
    }
}

// Sequential matrix multiplication
void multiplyMatricesSequential(const Matrix& A, const Matrix& B, Matrix& C)
{
    size_t Arows = A.rows;
    size_t Bcols = B.cols, Acols = A.cols;

    for (size_t i = 0; i < Arows; i++)
    {
        for (size_t j = 0; j < Bcols; j++)
        {
            double sum = 0.0;
            for (size_t k = 0; k < Acols; k++)
            {
                sum += A.at(i, k) * B.at(k, j);
            }
            C.at(i, j) = sum;
        }
    }
}

// Parallel processing function (used by each thread)
// This function processes one or more rows of the resulting matrix
void multiplyMatricesParallel(csPARGS args)
{
    // Retrieve arguments
    Matrix* A = args.getArgPtr<Matrix>(0);
    Matrix* B = args.getArgPtr<Matrix>(1);
    Matrix* C = args.getArgPtr<Matrix>(2);

    // Get the work block bounds (which rows this thread will process)
    csPARGS::BOUNDS bounds = args.getBounds();
    size_t i1 = bounds.first, i2 = bounds.last;
    size_t Bcols = B->cols, Acols = A->cols;

    // Process all assigned rows
    for (size_t i = i1; i < i2; i++)
    {
        // Process all columns for the current row
        for (size_t j = 0; j < Bcols; j++)
        {
            double sum = 0.0;
            for (size_t k = 0; k < Acols; k++)
            {
                sum += A->at(i, k) * B->at(k, j);
            }
            C->at(i, j) = sum;
        }
    }
}

int main()
{
    // Matrix dimensions
    const size_t N = 100;

    // Create matrices
    Matrix A(N, N);
    Matrix B(N, N);
    Matrix C_seq(N, N);
    Matrix C_par(N, N);

    // Initialize with random values
    A.randomize(0.0, 1.0);
    B.randomize(0.0, 1.0);

    // Prepare parallel processing
    size_t workSize = A.rows;  // Total number of rows to process
    size_t nThreads = csParallelTask::getMaxThreadNumber();

    // Register the parallel function using registerFunctionRegularEx
    // Note: This version doesn't require specifying the number of arguments
    size_t funcId = csParallelTask::registerFunctionRegularEx(
        nThreads,
        workSize,
        (char*)"multiplyMatricesParallel",
        multiplyMatricesParallel,
        &A, &B, &C_par
    );


    // Measure time for parallel multiplication
    csPERF_MEASUREMENT parTime(CSTIME_UNIT_MILLISECOND);
    parTime.start();
    csParallelTask::execute(funcId);
    parTime.stop();
    parTime.printReport("Parallel Matrix Multiplication");

    // Measure time for sequential multiplication
    csPERF_MEASUREMENT seqTime(CSTIME_UNIT_MILLISECOND);
    seqTime.start();
    multiplyMatricesSequential(A, B, C_seq);
    seqTime.stop();
    seqTime.printReport("Sequential Matrix Multiplication");

    // Verify results
    double maxDiff = 0.0;
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            double diff = std::abs(C_seq.at(i, j) - C_par.at(i, j));
            if (diff > maxDiff)
            {
                maxDiff = diff;
            }
        }
    }

    std::cout << "Maximum difference between results: " << maxDiff << std::endl;

    // Calculate speedup
    size_t seqElapsed = seqTime.getEllapsedTime();
    size_t parElapsed = parTime.getEllapsedTime();
    double speedup = static_cast<double>(seqElapsed) / parElapsed;

    std::cout << "Speedup: " << speedup << "x" << std::endl;
    std::cout << "Efficiency: " << (speedup / nThreads) * 100 << "%" << std::endl;

    int cx = 10;
    printMulOp(A, B, C_par, {0,10,cx,cx}, {10,0,cx,cx}, 3);

    return 0;
}