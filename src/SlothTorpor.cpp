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

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        const float knobHeight = 18.3f;
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(3.8f, knobHeight)), module, SlothTorporModule::CHAOS_KNOB_PARAM));

        const float outputHeight = 95.0f;
        addOutput(createOutput<PJ301MPort>(mm2px(Vec(1.5f,  outputHeight)), module, SlothTorporModule::Y_OUTPUT));
        addOutput(createOutput<PJ301MPort>(mm2px(Vec(10.5f, outputHeight)), module, SlothTorporModule::X_OUTPUT));
    }
};


Model* modelSlothTorpor = createModel<SlothTorporModule, SlothTorporWidget>("SlothTorpor");
