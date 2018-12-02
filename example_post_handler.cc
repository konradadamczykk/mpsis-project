#include "coin/CbcModel.hpp"
#include "coin/OsiClpSolverInterface.hpp"

int main (int argc, const char *argv[])
{
    // LOAD PROBLEM
    OsiClpSolverInterface *si = new OsiClpSolverInterface;

    // data goes here:
    int RACK = 1; int SERV = 1;
    int l1, L2[RACK], L3[SERV], l4;
    int m1, m2, M2[SERV], m3, M3[RACK], m4;
    int w1[RACK], w2, w3[SERV];

    // define objective and variable bounds
    int x1_cols = 1; int x4_cols = 1;
    int n_cols = x1_cols + x4_cols + SERV + RACK;
    double * objective = new double[n_cols]; //the objective coefficients
    double * col_lb = new double[n_cols]; //the column lower bounds
    double * col_ub = new double[n_cols]; //the column upper bounds

    // WE ALSO NEED TO KNOW x1, first x2, first x3 and x4 positions in objective row
    int x1_pos = 0;
    int x2_pos = 1;
    int x3_pos = x2_pos + SERV;
    int x4_pos = x3_pos + SERV;

    //minimize z: x1*l1 + (sum{serv in SERV} L2[serv]*x2[serv]) + (sum{rack in RACK} L3[rack]*x3[rack]) + x4*l4;
    objective[x1_pos] = l1;
    for (int i = 0; i<SERV; i++) {
            objective[x2_pos+i] = L2[i];
    }
    for (int i = 0; i<RACK; i++) {
            objective[x3_pos+i] = L3[i];
    }
    objective[x4_pos] = l4;

    // variable bounds -> <0;inf>
    for (int i = 0; i<n_cols; i++) {
        col_lb[i] = 0.0;
        col_ub[i] = si->getInfinity();
    }

    // constraints
    int n_rows = x1_cols + x4_cols + SERV*3 + RACK*2 + SERV*RACK; // using sum/iterating 3 times on both RACK AND SERV
    double * row_lb = new double[n_rows]; //the row lower bounds
    double * row_ub = new double[n_rows]; //the row upper bounds

    CoinPackedMatrix * matrix = new CoinPackedMatrix(false,0,0);
    matrix->setDimensions(0, n_cols);

    // Base vector with all zeros
    int indices[n_rows];
    const CoinPackedVector base_vector = new CoinPackedVector(n_rows, indices, 0.0);
    int current_row = 0;

    // constraint 1
    CoinPackedVector row1 = base_vector;
    row1.setElement(x1_pos,1.0);
    row_lb[current_row] = 0;
    row_ub[current_row] = m1;
    matrix->appendRow(row1);
    current_row++;

    // constraint 2
    for (int i = 0; i<SERV; i++) {
        CoinPackedVector row2 = base_vector;
        // ADD ANOTHER FOR LOOP HERE:
        row2.setElement(x2_pos+i,1.0);
        for (int j = 1; j<=SERV; j++) {
            row2.setElement(x2_pos+((i+j)%SERV),-1.0);
        }
        row_lb[current_row] = m2;
        row_ub[current_row] = si->getInfinity();
        matrix->appendRow(row2);
        current_row++;
    }

    // constraint 3
    for (int i = 0; i<RACK; i++) {
        for (int j = 0; j<SERV; j++) {
            CoinPackedVector row5 = base_vector;
            row5.setElement(x3_pos+i, w2); 
            for (int k = 1; k<=RACK; k++) {
                row5.setElement(x3_pos+((i+k)%RACK),-w2);
            }
            row5.setElement(x2_pos+j, -1.0);
            for (int k = 1; k<=SERV; k++) {
                row5.setElement(x3_pos+((j+k)%SERV),1.0);
            }
            row_lb[current_row] = 0;
            row_ub[current_row] = si->getInfinity();
            matrix->appendRow(row5);
            current_row++;
    }


    // constraint 4
    for (int i = 0; i<RACK; i++) {
        CoinPackedVector row4;
        row4.setElement(x3_pos+i, w1[i]); 
        for (int j = 1; j<=RACK; j++) {
            row4.setElement(x3_pos+((i+j)%RACK),-w1[(i+j)%RACK]);
        }
        row_lb[current_row] = 0;
        row_ub[current_row] = m1;
        matrix->appendRow(row4);
        current_row++;
    }

    // constraint 5
    for (int i = 0; i<SERV; i++) {
        CoinPackedVector row5 = base_vector;
        // set x4 as -1.0
        row5.setElement(x4_pos,-1.0);
        row5.setElement(x3_pos+i, w3[i]);
        for (int j = 1; j<=SERV; j++) {
            row5.setElement(x3_pos+((i+j)%SERV),-w3[(i+j)%SERV]);
        }
        row_lb[current_row] = 0;
        row_ub[current_row] = 0;
        matrix->appendRow(row5);
        current_row++;
    }

    // constraint 6
    CoinPackedVector row6 = base_vector;
    row6.setElement(x4_pos,1.0);
    row_lb[current_row] = 0;
    row_ub[current_row] = m4;
    matrix->appendRow(row6);
    current_row++;

    // constraint 7
    for (int i = 0; i<SERV; i++) {
        CoinPackedVector row7 = base_vector;
        row7.setElement(current_row+i,1.0);
        row_lb[current_row] = 0;
        row_ub[current_row] = M2[i];
        matrix->appendRow(row7);
        current_row++;
    }

    // constraint 8
    for (int i = 0; i<RACK; i++) {
        CoinPackedVector row8 = base_vector;
        row8.setElement(current_row+i,1.0);
        row_lb[current_row] = 0;
        row_ub[current_row] = M3[i];
        matrix->appendRow(row8);
        current_row++;
    }

    //load the problem to OSI
    si->loadProblem(*matrix, col_lb, col_ub, objective, row_lb, row_ub);

    // Pass the solver with the problem to be solved to CbcModel 
    CbcModel model(*si);

    // Do complete search
    model.branchAndBound();

    /* Print the solution.  CbcModel clones the solver so we
    need to get current copy from the CbcModel */
    int numberColumns = model.solver()->getNumCols();

    const double * solution = model.bestSolution();

    for (int iColumn=0;iColumn<numberColumns;iColumn++) {
        double value=solution[iColumn];
        if (fabs(value)>1.0e-7&&model.solver()->isInteger(iColumn)) 
        printf("%d has value %g\n",iColumn,value);
    }
}
