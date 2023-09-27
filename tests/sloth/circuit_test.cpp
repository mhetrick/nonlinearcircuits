#include <cstdio>
#include "SlothCircuit.hpp"
#include "TimeInSeconds.hpp"


inline bool CheckVoltage(double v, const char *module, const char *name, int sample)
{
    if (!std::isfinite(v))
    {
        printf("FAILURE - Non-finite value for %s %s in sample %d\n", module, name, sample);
        return false;
    }

    if (v < Analog::QNEG || v > Analog::QPOS)
    {
        printf("FAILURE - %s %s = %lg V is out of bounds in sample %d\n", module, name, v, sample);
        return false;
    }

    return true;
}


bool CheckVoltages(const char *module, const Analog::SlothCircuit& circuit, int sample)
{
    return (
        CheckVoltage(circuit.xVoltage(), module, "x", sample) &&
        CheckVoltage(circuit.yVoltage(), module, "y", sample) &&
        CheckVoltage(circuit.zVoltage(), module, "z", sample)
    );
}


template <typename circuit_t>
bool PerformanceAndStability(const char *name)
{
    circuit_t circuit;
    circuit.setControlVoltage(+0.1);
    circuit.setKnobPosition(0.5);

    const int SAMPLE_RATE = 44100;
    const int SIMULATION_SECONDS = 3600;
    const int SIMULATION_SAMPLES = SIMULATION_SECONDS * SAMPLE_RATE;

    printf("PerformanceAndStability(%s): Simulating %d seconds of sloth...\n", name, SIMULATION_SECONDS);
    long iterSum = 0;
    int maxIter = 0;
    double startTime = TimeInSeconds();
    for (int sample = 0; sample < SIMULATION_SAMPLES; ++sample)
    {
        int iter = circuit.update(SAMPLE_RATE);
        if (iter < 1 || iter >= circuit.iterationLimit)
        {
            printf("PerformanceAndStability(%s): SIMULATION FAILURE at sample %d: unexpected iteration count %d\n", name, sample, iter);
            return false;
        }

        if (!CheckVoltages(name, circuit, sample)) return false;
        iterSum += iter;
        maxIter = std::max(maxIter, iter);
    }
    double elapsedSeconds = TimeInSeconds() - startTime;
    printf("PerformanceAndStability(%s): Elapsed time = %0.3lf seconds, ratio = %0.6lg, CPU overhead = %lg percent.\n",
        name,
        elapsedSeconds,
        SIMULATION_SECONDS / elapsedSeconds,
        100.0 * (elapsedSeconds / SIMULATION_SECONDS)
    );

    double meanIter = static_cast<double>(iterSum) / SIMULATION_SAMPLES;
    printf("PerformanceAndStability(%s): Mean iter = %0.16lf, max iter = %d.\n", name, meanIter, maxIter);

    printf("PerformanceAndStability(%s): PASS\n", name);
    return true;
}


template <typename circuit_t>
bool ButterflyEffect(const char *name)
{
    // The hallmark of a chaotic system is that small changes in its
    // inputs will initially have little effect on its output, but
    // eventually will cause wildly diverging outputs.
    // We verify that here by running 3 simultaneous simulations:
    // 1. a default circuit
    // 2. a circuit with a slight change in CV input
    // 3. a circuit with a slight change in knob setting

    printf("ButterflyEffect(%s): starting\n", name);

    Analog::TorporSlothCircuit circuit1;
    circuit1.setControlVoltage(+0.1000);
    circuit1.setKnobPosition(0.500);

    Analog::TorporSlothCircuit circuit2;
    circuit2.setControlVoltage(+0.1001);
    circuit2.setKnobPosition(0.500);

    Analog::TorporSlothCircuit circuit3;
    circuit3.setControlVoltage(+0.1000);
    circuit3.setKnobPosition(0.4999);

    // Run all three circuits until we detect divergence.
    // Measure divergence as the pythagorean distance between
    // the points (x1, y1), (x2, y2), and (x3, y3).

    const int SAMPLE_RATE = 44100;
    const int SIMULATION_SECONDS = 100;
    const int SIMULATION_SAMPLES = SIMULATION_SECONDS * SAMPLE_RATE;

    const double divergenceThreshold = 0.1;     // arbitrary voltage difference to trigger a divergence ruling

    bool diverge2 = false;      // has circuit2 diverged from circuit1 yet?
    bool diverge3 = false;      // has circuit3 diverged from circuit1 yet?
    double time2 = -1.0;        // how many seconds did it take for circuit2 to diverge?
    double time3 = -1.0;        // how many seconds did it take for circuit3 to diverge?

    for (int sample = 0; sample < SIMULATION_SAMPLES && !(diverge2 && diverge3); ++sample)
    {
        circuit1.update(SAMPLE_RATE);
        circuit2.update(SAMPLE_RATE);
        circuit3.update(SAMPLE_RATE);

        double x1 = circuit1.xVoltage();
        if (!CheckVoltage(x1, "circuit1", "x", sample)) return false;
        double y1 = circuit1.yVoltage();
        if (!CheckVoltage(y1, "circuit1", "y", sample)) return false;

        double x2 = circuit2.xVoltage();
        if (!CheckVoltage(x2, "circuit2", "x", sample)) return false;
        double y2 = circuit2.yVoltage();
        if (!CheckVoltage(y2, "circuit2", "y", sample)) return false;

        double x3 = circuit3.xVoltage();
        if (!CheckVoltage(x3, "circuit3", "x", sample)) return false;
        double y3 = circuit3.yVoltage();
        if (!CheckVoltage(y3, "circuit3", "y", sample)) return false;

        double dist2 = std::hypot(x2-x1, y2-y1);
        double dist3 = std::hypot(x3-x1, y3-y1);

        if (!diverge2 && dist2 > divergenceThreshold)
        {
            diverge2 = true;
            time2 = static_cast<double>(sample) / SAMPLE_RATE;
            printf("ButterflyEffect(%s): Circuit 2 diverged at sample %d, time2 = %0.3lf seconds\n", name, sample, time2);
        }

        if (!diverge3 && dist3 > divergenceThreshold)
        {
            diverge3 = true;
            time3 = static_cast<double>(sample) / SAMPLE_RATE;
            printf("ButterflyEffect(%s): Circuit 3 diverged at sample %d, time3 = %0.3lf seconds\n", name, sample, time3);
        }
    }

    if (time2 < 96.7 || time2 > 96.8)
    {
        printf("ButterflyEffect(%s): UNEXPECTED DIVERGENCE for time3 = %0.3lf seconds\n", name, time3);
        return false;
    }

    if (time3 < 82.2 || time3 > 82.3)
    {
        printf("ButterflyEffect(%s): UNEXPECTED DIVERGENCE for time2 = %0.3lf seconds\n", name, time2);
        return false;
    }

    printf("ButterflyEffect(%s): PASS\n", name);
    return true;
}


void PrintVoltages(const char *name, const Analog::SlothCircuit& circuit)
{
    printf("%s x = %9.6lf, y = %9.6lf, z = %9.6lf\n",
        name, circuit.xVoltage(), circuit.yVoltage(), circuit.zVoltage());
}


bool TripleSloth()
{
    const int SAMPLE_RATE = 44100;
    const int SIMULATION_SECONDS = 10;
    const int SIMULATION_SAMPLES = SIMULATION_SECONDS * SAMPLE_RATE;

    printf("TripleSloth: starting\n");

    Analog::TripleSlothCircuit circuit;

    circuit.initialize();
    for (int sample = 0; sample < SIMULATION_SAMPLES; ++sample)
    {
        circuit.update(SAMPLE_RATE);
        if (!CheckVoltages("Torpor",  circuit.torpor,  sample)) return false;
        if (!CheckVoltages("Apathy",  circuit.apathy,  sample)) return false;
        if (!CheckVoltages("Inertia", circuit.inertia, sample)) return false;
    }

    PrintVoltages("Torpor ", circuit.torpor);
    PrintVoltages("Apathy ", circuit.apathy);
    PrintVoltages("Inertia", circuit.inertia);

    printf("TripleSloth: PASS\n");
    return true;
}


int main()
{
    using namespace Analog;

    return (
        PerformanceAndStability<TorporSlothCircuit>("Torpor") &&
        PerformanceAndStability<ApathySlothCircuit>("Apathy") &&
        PerformanceAndStability<InertiaSlothCircuit>("Inertia") &&
        ButterflyEffect<TorporSlothCircuit>("Torpor") &&
        TripleSloth()
    ) ? 0 : 1;
}
