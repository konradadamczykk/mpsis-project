#include "coin/CbcModel.hpp"
#include "coin/OsiClpSolverInterface.hpp"

int main (int argc, const char *argv[])
{
    // LOAD PROBLEM
    OsiClpSolverInterface *si = new OsiClpSolverInterface;

    int n_cols = 4;
    double * objective = new double[n_cols];//the objective coefficients
    double * col_lb = new double[n_cols];//the column lower bounds
    double * col_ub = new double[n_cols];//the column upper bounds

    //minimize z: x1*l1 + (sum{serv in SERV} L2[serv]*x2[serv]) + (sum{rack in RACK} L3[rack]*x3[rack]) + x4*l4;
    objective[0] = l1;
    objective[1] = /* sum_serv(l2[i]*x2[i]); */
    objective[3] = /* sum_rack(l3[i]*x3[i]); */
    objective[1] = l4;

    // variable bounds -> <0;inf>
    col_lb[0] = 0.0;
    col_lb[1] = 0.0;
    col_lb[2] = 0.0;
    col_lb[3] = 0.0;

    col_ub[0] = si->getInfinity();
    col_ub[1] = si->getInfinity();
    col_ub[2] = si->getInfinity();
    col_ub[3] = si->getInfinity();

    int n_rows = 4;
    double * row_lb = new double[n_rows]; //the row lower bounds
    double * row_ub = new double[n_rows]; //the row upper bounds

    // constraints
    CoinPackedMatrix * matrix = new CoinPackedMatrix(false,0,0);
    matrix->setDimensions(0, n_cols);

    // constraint 1
    CoinPackedVector row1;
    row1.insert(0,1);
    row1.insert(1,0);
    row1.insert(2,0);
    row1.insert(3,0);
    row_lb[0] = 0;
    row_ub[0] = m1;

    // constraint 2
    CoinPackedVector row2;
    row2.insert(0,0);
    row2.insert(1,/* sum_serv(x2[i]) */);
    row2.insert(2,0);
    row2.insert(3,0);
    row_lb[1] = m2;
    row_ub[1] = si->getInfinity();

    // constraint 3
    CoinPackedVector row3;
    row3.insert(0,0);
    row3.insert(1,/* -sum_serv[x2[i]] */);
    row3.insert(2,/* sum_rack(w2*x3[i]) */);
    row3.insert(3,0);
    row_lb[2] = 0;
    row_ub[2] = si->getInfinity();

    // constraint 4
    CoinPackedVector row4;
    row4.insert(0,0);
    row4.insert(1,0);
    row4.insert(2,/* sum_rack(w1[i]*x3[i]) */);
    row4.insert(3,0);
    row_lb[3] = 0;
    row_ub[3] = m1;

    // constraint 5
    CoinPackedVector row5;
    row5.insert(0,0);
    row5.insert(1,/* sum_rack(w3[i]*x2[i]) */);
    row5.insert(2,0);
    row5.insert(3,-1.0);
    row_lb[4] = 0;
    row_ub[4] = 0;

    // constraint 6
    CoinPackedVector row6;
    row6.insert(0,0);
    row6.insert(1,0);
    row6.insert(2,0);
    row6.insert(3,1.0);
    row_lb[5] = 0;
    row_ub[5] = m4;

    // constraint 7
    // to jest problematyczne, kazdy element wektora ma swoj upper bound
    CoinPackedVector row7;
    row7.insert(0,0);
    row7.insert(1,serv[i]);
    row7.insert(2,0);
    row7.insert(3,1.0);
    row_lb[5] = 0;
    row_ub[5] = M2[i];

    // constraint 8
    // to też jest problematyczne
    CoinPackedVector row8;
    row8.insert(0,0);
    row8.insert(1,0);
    row8.insert(2,rack[i]);
    row8.insert(3,1.0);
    row_lb[5] = 0;
    row_ub[5] = M2[i];

    // constraint 9
    // i to również, ograniczenie jest na danych, nie na zmiennych?
    CoinPackedVector row9;
    row9.insert(0,0);
    row9.insert(1,0);
    row9.insert(2,0);
    row9.insert(3,0);
    row_lb[5] = 0;
    row_ub[5] = 0;

    // constraint 10
    // i to również, ograniczenie jest na danych, nie na zmiennych?
    CoinPackedVector row10;
    row10.insert(0,0);
    row10.insert(1,0);
    row10.insert(2,0);
    row10.insert(3,0);
    row_lb[5] = 0;
    row_ub[5] = 0;

    matrix->appendRow(row1);
    matrix->appendRow(row2);
    matrix->appendRow(row3);
    matrix->appendRow(row4);
    matrix->appendRow(row5);
    matrix->appendRow(row6);
    matrix->appendRow(row7);
    matrix->appendRow(row8);
    matrix->appendRow(row9);
    matrix->appendRow(row10);


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
