/*
 * This is an example to show the performance of different methods with respect to the following ..
 *
 * Single period optimal portfolio allocation problem:
 *
 * C = \mathrm{argmin}_{w} ( \frac{1}{2} w^TCw + |w - \bar w|^TT - w^TR)
 *
 * subjected to:
 *
 * 0 < w_i < 1 for each i,
 * \sum_i w_i = 1
 *
 * where:
 *
 * w : the target portfolio allocation vector of different securities.
 * C : the covariance matrix of the whole securities universe.
 * T : trading cost vector.
 * R : the expected return vector.
 * \bar w : current portfolio allocation vector.
 *
 * for this problem we use single optimization method `minbleic` which are implemented in Alglib combined with
 * the following 3 ways to calculate cost function or its corresponding gradient:
 *
 * 1. `calculate_fd`
 *
 * Only cost function are provided to optimizer. The gradient are calculated by finite difference method which are handled by the optimizer
 * internally.
 *
 * 2. `calculate_analytic`
 *
 * Both cost function and its gradient are provided to optimizer. The gradient are calculated explicitly by hand-written codes.
 *
 * 3. `calculate_adept`
 *
 * Both cost function and its gradient are provided to optimizer. The gradient are calculated using automatic differentiation tool Adept,
 */

#include "ParameterReader.hpp"
#include "CostCalculator_fd.hpp"
#include "CostCalculator_analytic.hpp"
#include "CostCalculator_adept.hpp"
#include <boost/timer.hpp>
#include <optimization.h>


int main(int argc, char **argv)
{

    /*
     * Read all the parameters from the file
     *
     * varMatrix: co-variance matrix of the securities.
     * tradingCost: trading costs vector of the securities.
     * expectReturn: expect returns vector of the securities.
     * currentWeight: current portfolio allocation vector of the securities.
     */
    boost::tuple<real_2d_array, real_1d_array, real_1d_array, real_1d_array>
            parameters = parameterReader("d:/20160303_500.csv");

    real_2d_array varMatrix = parameters.get<0>();
    real_1d_array tradingCost = parameters.get<1>();
    real_1d_array expectReturn = parameters.get<2>();
    real_1d_array currentWeight = parameters.get<3>();

    int variableNumber = varMatrix.rows();

    /*
     * Portfolio construction constraints
     *
     * 1. Bounded constraint: 0 < x_i < 1.0 for each i stands for one particular security.
     * 2. Linear constraint: \sum_i x_i = 1.0, no cash out and no leverage.
     */

    // bounded constraints
    real_1d_array bndl;
    bndl.setlength(variableNumber);
    for (int i = 0; i != variableNumber; ++i)
        bndl[i] = 0.0;

    real_1d_array bndu;
    bndu.setlength(variableNumber);
    for (int i = 0; i != variableNumber; ++i)
        bndu[i] = 1.0;

    // linear constraints
    real_2d_array conMatrix;
    integer_1d_array condType = "[0]";
    conMatrix.setlength(1, variableNumber + 1);
    for (int i = 0; i != variableNumber; ++i)
        conMatrix[0][i] = 1.0;
    conMatrix[0][variableNumber] = 1.0;

    // stop condition
    double epsg = 1e-6;
    double epsf = 0;
    double epsx = 0;
    alglib::ae_int_t maxits = 0;

    // start point
    real_1d_array startWeight;
    startWeight.setlength(variableNumber);
    for (int i = 0; i != variableNumber; ++i)
        startWeight[i] = 1.0 / variableNumber;


    boost::timer timer;

    {
        CostCalculator_analytic costCalc(expectReturn, varMatrix, tradingCost, currentWeight);

        timer.restart();
        alglib::minbleicstate state_analytic;
        alglib::minbleicreport rep_analytic;

        alglib::minbleiccreate(startWeight, state_analytic);
        alglib::minbleicsetlc(state_analytic, conMatrix, condType);
        alglib::minbleicsetbc(state_analytic, bndl, bndu);
        alglib::minbleicsetcond(state_analytic, epsg, epsf, epsx, maxits);

        real_1d_array targetWeight;

        alglib::minbleicoptimize(state_analytic, calculate_analytic, NULL, &costCalc);
        alglib::minbleicresults(state_analytic, targetWeight, rep_analytic);

        std::cout << "analytic method : " << timer.elapsed()
                  << "s\tfunction value: " << state_analytic.f
                  << "\tfunction evaluations: " << rep_analytic.nfev
                  << "\ttermination: " << rep_analytic.terminationtype << std::endl;
    }

    {
        CostCalculator_adept costCalc(expectReturn, varMatrix, tradingCost, currentWeight);

        timer.restart();
        alglib::minbleicstate state_adept;
        alglib::minbleicreport rep_adept;

        alglib::minbleiccreate(startWeight, state_adept);
        alglib::minbleicsetlc(state_adept, conMatrix, condType);
        alglib::minbleicsetbc(state_adept, bndl, bndu);
        alglib::minbleicsetcond(state_adept, epsg, epsf, epsx, maxits);

        real_1d_array targetWeight;
        alglib::minbleicoptimize(state_adept, calculate_adept, NULL, &costCalc);
        alglib::minbleicresults(state_adept, targetWeight, rep_adept);

        std::cout << "automatic differentiation (adept): " << timer.elapsed()
                  << "s\tfunction value: " << state_adept.f
                  << "\tfunction evaluations: " << rep_adept.nfev
                  << "\ttermination: " << rep_adept.terminationtype << std::endl;
    }

    {
        CostCalculator_fd costCalc(expectReturn, varMatrix, tradingCost, currentWeight);

        timer.restart();
        double diffstep = 1.0e-6;
        alglib::minbleicstate state_fd;
        alglib::minbleicreport rep_fd;

        alglib::minbleiccreatef(startWeight, diffstep, state_fd);
        alglib::minbleicsetlc(state_fd, conMatrix, condType);
        alglib::minbleicsetbc(state_fd, bndl, bndu);
        alglib::minbleicsetcond(state_fd, epsg, epsf, epsx, maxits);

        alglib::minbleicoptimize(state_fd, calculate_fd, NULL, &costCalc);

        real_1d_array targetWeight;
        alglib::minbleicresults(state_fd, targetWeight, rep_fd);

        std::cout << "finite difference: " << timer.elapsed()
                  << "s\tfunction value: " << state_fd.f
                  << "\tfunction evaluations: " << rep_fd.nfev
                  << "\ttermination: " << rep_fd.terminationtype << std::endl;
    }

    return 0;
}