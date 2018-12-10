#include <sys/types.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <kcgi.h>
#include <kcgihtml.h>
#include <stdio.h>
#include <string.h>
#include "jsmn/jsmn.h"
#include <limits.h>
#include <stdlib.h>
#include "coin/CbcModel.hpp"
#include "coin/OsiClpSolverInterface.hpp"

enum key {
  KEY__MAX
};

enum page {
  PAGE_INDEX,
  PAGE__MAX
};

const char *const pages[PAGE__MAX] = {
  "index",
};

static const struct kvalid key = { kvalid_stringne, ""};

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if(tok->type == JSMN_STRING 
     && (int) strlen(s) == tok->end - tok->start 
     && strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

static void process(struct kreq *r) {
  struct kpair *p;
  if (p = r->fieldmap[0]) {
    jsmn_parser parser;
    jsmntok_t tokens[128];
    jsmn_init(&parser);
    int j = jsmn_parse(&parser, p->parsed.s, strlen(p->parsed.s), tokens, 128);
    if (j < 0) {
      printf("FAILED: %d\n", j);
    }
  
    int l1,l2,l3,l4,m1,m2,m3,m4,w2;

//first scan of json to get values for initialization another ones
//TODO: change this to swtich..case model 
    for (int i = 1; i<j; i++) {
       if(jsoneq(p->parsed.s, &tokens[i], "m2_cost") == 0) {
         l1 = atoi(strndup(p->parsed.s + tokens[i+1].start, tokens[i+1].end-tokens[i+1].start));
       }
       if(jsoneq(p->parsed.s, &tokens[i], "num_server_vendors") == 0) {
         l2 = atoi(strndup(p->parsed.s + tokens[i+1].start, tokens[i+1].end-tokens[i+1].start));
       }
       if(jsoneq(p->parsed.s, &tokens[i], "num_rack_vendors") == 0) {
         l3 = atoi(strndup(p->parsed.s + tokens[i+1].start, tokens[i+1].end-tokens[i+1].start));
       }
       if(jsoneq(p->parsed.s, &tokens[i], "kWh_cost") == 0) {
         l4 = atoi(strndup(p->parsed.s + tokens[i+1].start, tokens[i+1].end-tokens[i+1].start));
       }
       if(jsoneq(p->parsed.s, &tokens[i], "max_m2") == 0) {
         m1 = atoi(strndup(p->parsed.s + tokens[i+1].start, tokens[i+1].end-tokens[i+1].start));
       }
       if(jsoneq(p->parsed.s, &tokens[i], "min_all_servers") == 0) {
         m2 = atoi(strndup(p->parsed.s + tokens[i+1].start, tokens[i+1].end-tokens[i+1].start));
       }
       if(jsoneq(p->parsed.s, &tokens[i], "min_num_racks") == 0) {
         m3 = atoi(strndup(p->parsed.s + tokens[i+1].start, tokens[i+1].end-tokens[i+1].start));
       }
       if(jsoneq(p->parsed.s, &tokens[i], "max_kwh_usage") == 0) {
         m4 = atoi(strndup(p->parsed.s + tokens[i+1].start, tokens[i+1].end-tokens[i+1].start));
       }
       if(jsoneq(p->parsed.s, &tokens[i], "num_serv_in_rack_vendor") == 0) {
         w2 = atoi(strndup(p->parsed.s + tokens[i+1].start, tokens[i+1].end-tokens[i+1].start));
       }
     }

//and second scan for 'dynamic' arrays

     int L2[l2],L3[l3],M2[l2],M3[l3],w1[l3],w3[l2];
     
     for (int i = 1; i<j; i++) {
       if(jsoneq(p->parsed.s, &tokens[i], "cost_server") == 0) {
         if(tokens[i+1].type != JSMN_ARRAY) {
           continue;
         }
         for(int k=0; k<l2; k++) {
           jsmntok_t *g = &tokens[i+k+2];
           L2[k] = atoi(strndup(p->parsed.s + g->start, g->end-g->start));
         }
         i += tokens[i+1].size+1;
       }
       if(jsoneq(p->parsed.s, &tokens[i], "rack_cost_vendor") == 0) {
         if(tokens[i+1].type != JSMN_ARRAY) {
           continue;
         }
         for(int k=0; k<l3; k++) {
           jsmntok_t *g = &tokens[i+k+2];
           L3[k] = atoi(strndup(p->parsed.s + g->start, g->end-g->start));
         }
         i += tokens[i+1].size+1;
       }
       if(jsoneq(p->parsed.s, &tokens[i], "max_num_serv_vendor") == 0) {
         if(tokens[i+1].type != JSMN_ARRAY) {
           continue;
         }
         for(int k=0; k<l2; k++) {
           jsmntok_t *g = &tokens[i+k+2];
           M2[k] = atoi(strndup(p->parsed.s + g->start, g->end-g->start));
         }
         i += tokens[i+1].size+1;
       }
       if(jsoneq(p->parsed.s, &tokens[i], "rack_max_num_vendor") == 0) {
         if(tokens[i+1].type != JSMN_ARRAY) {
           continue;
         }
         for(int k=0; k<l3; k++) {
           jsmntok_t *g = &tokens[i+k+2];
           M3[k] = atoi(strndup(p->parsed.s + g->start, g->end-g->start));
         }
         i += tokens[i+1].size+1;
       }
       if(jsoneq(p->parsed.s, &tokens[i], "rack_surface_vendor") == 0) {
         if(tokens[i+1].type != JSMN_ARRAY) {
           continue;
         }
         for(int k=0; k<l3; k++) {
           jsmntok_t *g = &tokens[i+k+2];
           w1[k] = atoi(strndup(p->parsed.s + g->start, g->end-g->start));
         }
         i += tokens[i+1].size+1;
       }
       if(jsoneq(p->parsed.s, &tokens[i], "monthly_elec_server_vendor") == 0) {
         if(tokens[i+1].type != JSMN_ARRAY) {
           continue;
         }
         for(int k=0; k<l2; k++) {
           jsmntok_t *g = &tokens[i+k+2];
           w3[k] = atoi(strndup(p->parsed.s + g->start, g->end-g->start));
         }
         i += tokens[i+1].size+1;
       }
	 }
//all params ready!

    OsiClpSolverInterface *si = new OsiClpSolverInterface;
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
    const CoinPackedVector base_vector = CoinPackedVector(n_rows, indices, 0.0);

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
    CoinPackedVector row4 = base_vector;
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

    //ClpSimplex * clpPointer;
    //clpPointer = (dynamic_cast<OsiClpSolverInterface *>(si))->getModelPtr();
    //clpPointer->setLogLevel(0);
    // clpPointer->setMaximumIterations(10);
    // Could tell Clp many other things

    // Solve the (relaxation of the) problem
    si->initialSolve();
    // Check the solution
    if ( si->isProvenOptimal() )
    {
        khttp_puts(r,"Found optimal solution!");

        //khttp_puts(r,"Objective value is " << si->getObjValue());
        int n = si->getNumCols();
        const double *solution;
        solution = si->getColSolution();
        // We could then print the solution or examine it.
    }
    else
    {
        khttp_puts(r, "Didn’t find optimal solution.");
        // Could then check other status functions.
    }
   } else if (r->fieldnmap[0]) khttp_puts(r, "Bad data format");
   else khttp_puts(r, "Data not provided.");
}

static enum khttp sanitise(const struct kreq *r) {
  if (PAGE_INDEX != r->page)
    return KHTTP_404;
  else if ('\0' != *r->path)
    return KHTTP_404;
  else if (KMIME_TEXT_HTML != r->mime)
    return KHTTP_404;
  else if (KMETHOD_POST != r->method)
    return KHTTP_405;
  return KHTTP_200;
};

int main(void) {
  struct kreq r;
  enum khttp er;

  if (KCGI_OK != khttp_parse(&r, &key, 1, pages, PAGE__MAX, PAGE_INDEX))
    return 0;
  if (KHTTP_200 != (er = sanitise(&r))) {
    khttp_head(&r, kresps[KRESP_STATUS], "%s", khttps[er]);
    khttp_head(&r, kresps[KRESP_CONTENT_TYPE], "%s", kmimetypes[KMIME_TEXT_PLAIN]);
    khttp_body(&r);
    if(KMIME_TEXT_HTML == r.mime)
      khttp_puts(&r, "Could not");
  } else {
    khttp_head(&r, kresps[KRESP_STATUS], "%s", khttps[KHTTP_200]);
    khttp_head(&r, kresps[KRESP_CONTENT_TYPE], "%s", kmimetypes[r.mime]);
    khttp_body(&r);
    process(&r);
  }
  khttp_free(&r);
  return 0;
};
