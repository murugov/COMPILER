#include "simplify.hpp"
#include "OpInstrSet.cpp"


// elementary functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcADD(calc_context *calc_params) { return calc_params->left_val + calc_params->right_val; }
node_t *diffADD(diff_context *diff_params) { return NULL; }

double calcSUB(calc_context *calc_params) { return calc_params->left_val - calc_params->right_val; }
node_t *diffSUB(diff_context *diff_params) { return NULL; }

double calcMUL(calc_context *calc_params) { return calc_params->left_val * calc_params->right_val; }
node_t *diffMUL(diff_context *diff_params) { return NULL; }

double calcDIV(calc_context *calc_params) { return calc_params->left_val / calc_params->right_val; } // div by zero!!!
node_t *diffDIV(diff_context *diff_params) { return NULL; }

double calcPOW(calc_context *calc_params) { return pow(calc_params->left_val, calc_params->right_val); } // zero to the power of zero!!!
node_t *diffPOW(diff_context *diff_params) { return NULL; }

double calcSQRT(calc_context *calc_params) { return sqrt(calc_params->right_val); } // div by zero!!!
node_t *diffSQRT(diff_context *diff_params) { return NULL; }

//---------------------------------------------------------------------------------------------------------------------------------------------------------



// exponential functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcE(calc_context *calc_params) { return exp(calc_params->right_val); }
node_t *diffE(diff_context *diff_params) { return NULL; }

double calcLN(calc_context *calc_params) { return log(calc_params->right_val); }
node_t *diffLN(diff_context *diff_params) { return NULL; }

double calcLOG(calc_context *calc_params) { return log(calc_params->right_val) / log(calc_params->left_val); }
node_t *diffLOG(diff_context *diff_params) { return NULL; }
//---------------------------------------------------------------------------------------------------------------------------------------------------------



// trigonometric functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcSIN(calc_context *calc_params) { return sin(calc_params->right_val); }
node_t *diffSIN(diff_context *diff_params) { return NULL; }

double calcCOS(calc_context *calc_params) { return cos(calc_params->right_val); }
node_t *diffCOS(diff_context *diff_params) { return NULL; }

double calcTAN(calc_context *calc_params) { return tan(calc_params->right_val); }
node_t *diffTAN(diff_context *diff_params) { return NULL; }

double calcCOT(calc_context *calc_params) { return 1 / tan(calc_params->right_val); }
node_t *diffCOT(diff_context *diff_params) { return NULL; }

//---------------------------------------------------------------------------------------------------------------------------------------------------------



// hyperbolic functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcSINH(calc_context *calc_params) { return (exp(calc_params->right_val) - exp(-calc_params->right_val)) / 2; }
node_t *diffSINH(diff_context *diff_params) { return NULL; }

double calcCOSH(calc_context *calc_params) { return (exp(calc_params->right_val) + exp(-calc_params->right_val)) / 2; }
node_t *diffCOSH(diff_context *diff_params) { return NULL; }

double calcTANH(calc_context *calc_params) { return calcSINH(calc_params) / calcCOSH(calc_params); }
node_t *diffTANH(diff_context *diff_params) { return NULL; }

double calcCOTH(calc_context *calc_params) { return calcCOSH(calc_params) / calcSINH(calc_params); }
node_t *diffCOTH(diff_context *diff_params) { return NULL; }

//---------------------------------------------------------------------------------------------------------------------------------------------------------



// inverse trigonometric functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcARCSIN(calc_context *calc_params) { return asin(calc_params->right_val); }
node_t *diffARCSIN(diff_context *diff_params) { return NULL; }

double calcARCCOS(calc_context *calc_params) { return acos(calc_params->right_val); }
node_t *diffARCCOS(diff_context *diff_params) { return NULL; }

double calcARCTAN(calc_context *calc_params) { return atan(calc_params->right_val); }
node_t *diffARCTAN(diff_context *diff_params) { return NULL; }

double calcARCCOT(calc_context *calc_params) { return acot(calc_params->right_val); }
node_t *diffARCCOT(diff_context *diff_params) { return NULL; }

//---------------------------------------------------------------------------------------------------------------------------------------------------------


// inverse hyperbolic functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcARCSINH(calc_context *calc_params) { return asinh(calc_params->right_val); }
node_t *diffARCSINH(diff_context *diff_params) { return NULL; }

double calcARCCOSH(calc_context *calc_params) { return acosh(calc_params->right_val); }
node_t *diffARCCOSH(diff_context *diff_params) { return NULL; }

double calcARCTANH(calc_context *calc_params) { return atanh(calc_params->right_val); }
node_t *diffARCTANH(diff_context *diff_params) { return NULL; }

double calcARCCOTH(calc_context *calc_params) { return acoth(calc_params->right_val); }
node_t *diffARCCOTH(diff_context *diff_params) { return NULL; }

//---------------------------------------------------------------------------------------------------------------------------------------------------------