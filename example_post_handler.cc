#include "coin/CbcModel.hpp"
#include "coin/OsiClpSolverInterface.hpp"

int main (int argc, const char *argv[])
{
    // LOAD PROBLEM
    OsiClpSolverInterface *si = new OsiClpSolverInterface;

    // data goes here:

    int l1 = 100;
	int l2 = 2;
	int l3 = 1;
	int l4 = 5;
	int L2[l2] = { 3000, 3200 };
	int L3[l3] = { 10000 };
    int m1 = 100000;
	int m2 = 200000;
	int M2[l2] = { 80000, 130000,  };
	int m3 = 200000;
	int M3[l3] = { 10000 };
	int m4 = 65000000;
    int w1[l3] = { 2 };
	int w2 = 14;
	int w3[l2] = { 400, 300 };

    // define objective and variable bounds
    int x1_cols = 1; int x4_cols = 1;
    int n_cols = x1_cols + x4_cols + l2 + l3;
    double * objective = new double[n_cols]; //the objective coefficients
    double * col_lb = new double[n_cols]; //the column lower bounds
    double * col_ub = new double[n_cols]; //the column upper bounds

    // WE ALSO NEED TO KNOW x1, first x2, first x3 and x4 positions in objective row
    int x1_pos = 0;
    int x2_pos = 1;
    int x3_pos = x2_pos + l2;
    int x4_pos = x3_pos + l3;

    //minimize z: x1*l1 + (sum{serv in SERV} L2[serv]*x2[serv]) + (sum{rack in RACK} L3[rack]*x3[rack]) + x4*l4;
    objective[x1_pos] = l1;
    for (int i = 0; i<l2; i++) {
            objective[x2_pos+i] = L2[i];
    }
    for (int i = 0; i<l3; i++) {
            objective[x3_pos+i] = L3[i];
    }
    objective[x4_pos] = l4;
    
    // variable bounds -> <0;inf>
    for (int i = 0; i<n_cols; i++) {
        col_lb[i] = 0.0;
        col_ub[i] = si->getInfinity();
    }

    // constraints
    int n_rows = 6 + l2 + l3;
    double * row_lb = new double[n_rows]; //the row lower bounds
    double * row_ub = new double[n_rows]; //the row upper bounds

    CoinPackedMatrix * matrix = new CoinPackedMatrix(false,0,0);
    matrix->setDimensions(0, n_cols);

    // Base vector with all zeros
    int indices[n_rows];
    for (int i=0; i<n_rows; i++) {
        indices[i] = i;
    }
    const CoinPackedVector base_vector = new CoinPackedVector(n_rows, indices, 0.0);

    int current_row = 0;
    // constraint 1
    CoinPackedVector row1 = base_vector;
    // WYWALA SIE NA LINICJE NIŻEJ SOMEHOW
    row1.setElement(x1_pos,1.0);
    row_lb[current_row] = 0;
    row_ub[current_row] = m1;
    matrix->appendRow(row1);
    current_row++;

    // constraint 2
    CoinPackedVector row2 = base_vector;
    for (int i = 0; i<l2; i++) {
        row2.setElement(x2_pos+i,1.0);
    }
    matrix->appendRow(row2);
    row_lb[current_row] = m2;
    row_ub[current_row] = si->getInfinity();
    current_row++;

    // constraint 3
    CoinPackedVector row3 = base_vector;
    for (int i = 0; i<l3; i++) {
        row3.setElement(x3_pos+i, w2); 
    }
    for (int j = 0; j<l2; j++) {
        row3.setElement(x2_pos+j, -1.0);
    }
    row_lb[current_row] = 0;
    row_ub[current_row] = si->getInfinity();
    matrix->appendRow(row3);
    current_row++;
    
    // constraint 4
    CoinPackedVector row4;
    for (int i = 0; i<l3; i++) {
        row4.setElement(x3_pos+i, w1[i]); 
    }
    row_lb[current_row] = 0;
    row_ub[current_row] = m1;
    matrix->appendRow(row4);
    current_row++;

    // constraint 5
    CoinPackedVector row5 = base_vector;
    // set x4 as -1.0
    row5.setElement(x4_pos,-1.0);
    for (int i = 0; i<l2; i++) {
        row5.setElement(x2_pos+i, w3[i]);

    }
    row_lb[current_row] = 0;
    row_ub[current_row] = 0;
    matrix->appendRow(row5);
    current_row++;

    // constraint 6
    CoinPackedVector row6 = base_vector;
    row6.setElement(x4_pos,1.0);
    row_lb[current_row] = 0;
    row_ub[current_row] = m4;
    matrix->appendRow(row6);
    current_row++;

    // constraint 7
    for (int i = 0; i<l2; i++) {
        CoinPackedVector row7 = base_vector;
        row7.setElement(x2_pos+i,1.0);
        row_lb[current_row] = 0;
        row_ub[current_row] = M2[i];
        matrix->appendRow(row7);
        current_row++;
    }

    // constraint 8
    for (int i = 0; i<l3; i++) {
        CoinPackedVector row8 = base_vector;
        row8.setElement(x3_pos+i,1.0);
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
