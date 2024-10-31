/******************************************************************************
* Copyright 2019-present, Joseph Garnier
* All rights reserved.
*
* This source code is licensed under the license found in the
* LICENSE file in the root directory of this source tree.
******************************************************************************/

#include "project-name/include1.h"
#include "project-name/include2.h"
#include "source1.h"
#include "sub1/sub1.h"
#include "sub2/sub2.h"
#include "project-name/project-name_pch.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>

double integrand(double x) {
    return sin(x) + cos(x);
}

double trapezoidalIntegral(int startIndex, int strips, int totalStrips) {
    double a = -1.0;
    double b = 1.0;
    double h = (b - a) / totalStrips;
    double integral = 0.0;

    for (int i = startIndex; i < startIndex + strips; ++i) {
        double x = a + i * h;
        if (i == startIndex || i == startIndex + strips - 1) {
            integral += integrand(x) / 2.0;
        } else {
            integral += integrand(x);
        }
    }

    integral *= h;
    return integral;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    const char* stripsEnv = std::getenv("TRAPEZOID_STRIPS");
    if (stripsEnv == nullptr) {
        std::cerr << "Error: TRAPEZOID_STRIPS environment variable is not set." << std::endl;
        MPI_Finalize();
        return 1;
    }

    int totalStrips = std::stoi(stripsEnv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int stripsPerProcess = totalStrips / size;
    int remainderStrips = totalStrips % size;
    int startIndex = rank * stripsPerProcess;
    if (rank == size - 1) {
        stripsPerProcess += remainderStrips;
    }

    double startTime = MPI_Wtime();
    double localIntegral = trapezoidalIntegral(startIndex, stripsPerProcess, totalStrips);
    double endTime = MPI_Wtime();
    double globalIntegral = 0.0;
    MPI_Reduce(&localIntegral, &globalIntegral, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        std::cout << "Approximate integral: " << globalIntegral << " ... ";
        std::cout << "time taken: " << (endTime - startTime) << " seconds." << std::endl;
    }

    MPI_Finalize();
    return 0;
}
