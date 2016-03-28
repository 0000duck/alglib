void setStopCondition(double epsg, 
        double epsf, 
        double epsx, 
        int maxits);

void portfolioOptimizer(int size,
        double* covMatrix,
        double* expectedReturn,
        double* tradingCost,
        double* currentWeight,
        double* lowerBound,
        double* upperBound,
        int lcNumber,
        double* linearCond,
        int* linearCondType,
        double* targetWeight,
        double* cost);

void portfolioOptimizerCuda(int size,
        double* covMatrix,
        double* expectedReturn,
        double* tradingCost,
        double* currentWeight,
        double* lowerBound,
        double* upperBound,
        int lcNumber,
        double* linearCond,
        int* linearCondType,
        double* targetWeight,
        double* cost);

