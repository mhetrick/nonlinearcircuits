/*
    SlothTorpor.cpp  -  Don Cross <cosinekitty@gmail.com>  -  2023-09-23

    A VCV Rack version of the Sloth Chaos "Torpor" hardware module by Andrew Fitch:
    https://www.nonlinearcircuits.com/modules/p/4hp-sloth-chaos
*/

#include "NLC.hpp"
#include "SlothCircuit.hpp"


struct SlothTorporModule : Module
{
    Analog::TorporSlothCircuit circuit;

    enum ParamIds
    {
        CHAOS_KNOB_PARAM,
        PARAMS_LEN
    };

    enum InputIds
    {
        INPUTS_LEN
    };

    enum OutputIds
    {
        X_OUTPUT,
        Y_OUTPUT,
        OUTPUTS_LEN
    };

    enum LightIds
    {
        INDICATOR_LIGHT,
        LIGHTS_LEN
    };

    SlothTorporModule()
    {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configOutput(X_OUTPUT, "Small");
        configOutput(Y_OUTPUT, "Big");

        configParam(CHAOS_KNOB_PARAM, 0.0f, 1.0f, 0.5f, "Chaos");

        outputs[X_OUTPUT].setChannels(1);
        outputs[Y_OUTPUT].setChannels(1);

        initialize();
    }

    void initialize()
    {
        circuit.initialize();
        circuit.setControlVoltage(+0.1);
    }

    void onReset(const ResetEvent& e) override
    {
        Module::onReset(e);
        initialize();
    }

    void process(const ProcessArgs& args) override
    {
        circuit.setKnobPosition(params[CHAOS_KNOB_PARAM].getValue());
        circuit.update(args.sampleRate);
        outputs[X_OUTPUT].setVoltage(circuit.xVoltage(), 0);
        outputs[Y_OUTPUT].setVoltage(circuit.yVoltage(), 0);
    }
};


struct SlothTorporWidget : ModuleWidget
{
    explicit SlothTorporWidget(SlothTorporModule* module)
    {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/SlothTorpor.svg")));
    }
};


Model* modelSlothTorpor = createModel<SlothTorporModule, SlothTorporWidget>("SlothTorpor");
