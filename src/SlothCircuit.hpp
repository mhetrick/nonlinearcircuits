/*
    SlothCircuit.hpp  -  Don Cross <cosinekitty@gmail.com>  -  2023-09-23

    A software emulation of the "Sloth Chaos" hardware module by Andrew Fitch:
    https://www.nonlinearcircuits.com/modules/p/4hp-sloth-chaos

    The following document describes the circuit schematic, analysis,
    and the mathematical derivation of this emulation algorithm.
    https://github.com/cosinekitty/sloth/blob/main/README.md
*/
#pragma once

#include <algorithm>
#include <cmath>

namespace Analog
{
    // Op-amp saturation voltages.
    // I measured these from my breadboard build of Sloth on the comparator U1.
    const double QNEG = -10.64;
    const double QPOS = +11.38;

    class SlothCircuit
    {
    private:
        const double timeDilation;      // The time dilation factor creates variants that run at different speeds.
        const double w0;                // Initial charge voltage of C3, which affects initial trajectory.

        // Inputs
        double K{};     // the total resistance R3 (fixed) + R9 (variable)
        double U{};     // control voltage fed into the circuit via R8

        // Node voltages
        double x1{};    // voltage at the output of op-amp U3
        double w1{};    // voltage at the top of capacitor C3
        double y1{};    // voltage at the output of op-amp U4
        double z1{};    // voltage at the output of op-amp U2

        // Node voltage increments. We remember these across samples
        // to form an initial guess about how the voltage will change
        // in the next sample. This improves efficiency by converging faster.
        double dx{};
        double dw{};
        double dy{};

        // Capacitor values in farads.
        const double C1 = 2.0e-6;
        const double C2 = 1.42e-6;    // schematic says 1uF, but this value acts more "slothy"
        const double C3 = 50.0e-6;

        // Resistor values in ohms.
        // R3 + R9 = K, so R3 and R9 are not listed here.
        const double R1 = 1.0e+6;
        const double R2 = 4.7e+6;
        const double R4 = 100.0e+3;
        const double R5 = 100.0e+3;
        const double R6 = 100.0e+3;
        const double R7 = 100.0e+3;
        const double R8 = 470.0e+3;

        // Power supply rail voltages.
        const double VNEG = -12.0;
        const double VPOS = +12.0;

        double Q(double z)
        {
            // The comparator U1 output responds immediately to the voltage z.
            // It is an inverting amplifier whose output is saturated.
            return (z < 0.0) ? QPOS : QNEG;
        }

    protected:
        SlothCircuit(double _timeDilation, double _w0)
            : timeDilation(_timeDilation)
            , w0(_w0)
        {
            initialize();
            setKnobPosition(0.0);
            setControlVoltage(0.0);
        }

    public:
        // The iteration safety limit for the convergence solver.
        const int iterationLimit = 5;

        void initialize()
        {
            w1 = w0;
            x1 = y1 = z1 = 0;
            dx = dw = dy = 0;
        }

        void setKnobPosition(double fraction)
        {
            // Clamp the fraction to the range [0, 1].
            double clamped = std::max(0.0, std::min(1.0, fraction));

            // The maximum value of the variable resistor is 10K.
            // This is in series with a fixed resistance of 100K.
            K = 100.0e+3 + (clamped * 10.0e+3);
        }

        void setControlVoltage(double cv)
        {
            // Clamp the control voltage to the circuit's supply rails.
            U = std::max(VNEG, std::min(VPOS, cv));
        }

        double xVoltage() const
        {
            return x1;
        }

        double yVoltage() const
        {
            return y1;
        }

        double zVoltage() const
        {
            return z1;
        }

        int update(float sampleRateHz)      // returns the number of iterations needed for convergence [1..iterationLimit]
        {
            double dt = timeDilation / sampleRateHz;

            // Form an initial guess about the mean voltage during the time interval `dt`.
            // Use linear extrapolation to guess that the voltages will keep changing
            // at the same rate they did in the previous sample.
            double xm = x1 + dx/2;
            double wm = w1 + dw/2;
            double ym = y1 + dy/2;
            double zm = -R4*(ym/R5 + U/R8);
            double Qm = Q(zm);

            // Iterate until convergence.
            const double tolerance = 1.0e-12;        // one picovolt
            const double toleranceSquared = tolerance * tolerance;

            double ex, ew, ey;
            for (int iter = 1; true; ++iter)
            {
                // Remember the previous delta voltages, so we can tell whether we have converged next time.
                ex = dx;
                ew = dw;
                ey = dy;

                // Update the finite changes of the voltage variables after the time interval.
                dx = -dt/C1 * (zm/R1 + Qm/R2 + wm/K);
                dw = dt/C3*(xm/R6-(1/R6 + 1/K + 1/R7)*wm);
                dy = (-dt/(R7*C2)) * wm;

                double x2 = x1 + dx;
                double w2 = w1 + dw;
                double y2 = y1 + dy;

                // Assume z changes instantaneously because there is no capacitor the U2 feedback loop.
                double z2 = -R4*(y2/R5 + U/R8);

                // Has the solver converged?
                // Calculate how much the deltas have changed since last time.
                double ddx = dx - ex;
                double ddw = dw - ew;
                double ddy = dy - ey;
                double variance = ddx*ddx + ddw*ddw + ddy*ddy;
                if (variance < toleranceSquared || iter >= iterationLimit)
                {
                    // The solution has converged, or we have hit the iteration safety limit.
                    // Update the circuit state voltages and return.
                    x1 = x2;
                    w1 = w2;
                    y1 = y2;
                    z1 = z2;
                    return iter;
                }

                // We approximate the mean value over the time interval as the average
                // of the starting value with the estimated next value.
                xm = x1 + dx/2;
                wm = w1 + dw/2;
                zm = (z1 + z2)/2;

                // Usually Q remains constant, but it toggles when z changes polarity.
                if (z1 * z2 >= 0)
                {
                    Qm = Q(zm);
                }
                else
                {
                    // alpha = the fraction into the time step at which z(t) = 0.
                    double alpha = z1 / (z1 - z2);
                    Qm = alpha*Q(z1) + (1-alpha)*Q(z2);
                }
            }
        }
    };


    class TorporSlothCircuit : public SlothCircuit
    {
    public:
        TorporSlothCircuit()
            : SlothCircuit(1.0, 0.0)
            {}
    };


    class ApathySlothCircuit : public SlothCircuit
    {
    public:
        ApathySlothCircuit()
            : SlothCircuit(0.27391343022607395, +0.017)
            {}
    };


    class InertiaSlothCircuit : public SlothCircuit
    {
    public:
        InertiaSlothCircuit()
            : SlothCircuit(0.009697118406631193, -0.023)
            {}
    };
}
