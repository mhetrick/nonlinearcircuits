/*
    Sloth.cpp  -  Don Cross <cosinekitty@gmail.com>  -  2023-09-23

    A VCV Rack version of the Sloth hardware modules by Andrew Fitch:
    https://www.nonlinearcircuits.com/modules/p/4hp-sloth-chaos
*/

#include "NLC.hpp"
#include "SlothCircuit.hpp"


namespace SlothTypes
{
    enum ParamIds
    {
        BALANCE_KNOB_PARAM,
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
        INDICATOR_LIGHT_RED,
        INDICATOR_LIGHT_GREEN,
        LIGHTS_LEN
    };
}


template <typename circuit_t>
struct SlothModule : Module
{
    circuit_t circuit;

    SlothModule()
    {
        using namespace SlothTypes;

        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configOutput(X_OUTPUT, "Small");
        configOutput(Y_OUTPUT, "Big");

        configParam(BALANCE_KNOB_PARAM, 0.0f, 1.0f, 0.5f, "Balance");

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
        using namespace SlothTypes;

        circuit.setKnobPosition(params[BALANCE_KNOB_PARAM].getValue());
        circuit.update(args.sampleRate);
        outputs[X_OUTPUT].setVoltage(circuit.xVoltage(), 0);
        outputs[Y_OUTPUT].setVoltage(circuit.yVoltage(), 0);
        float z = (circuit.zVoltage() >= 0.0) ? 0.8f : 0.0f;
        lights[INDICATOR_LIGHT_RED].setBrightness(1-z);
        lights[INDICATOR_LIGHT_GREEN].setBrightness(z);
    }
};


struct RedGreenLightWidget : GrayModuleLightWidget {
    RedGreenLightWidget()
    {
        addBaseColor(nvgRGB(0xff, 0x00, 0x00));
        addBaseColor(nvgRGB(0x00, 0xff, 0x00));
    }
};

template <typename circuit_t>
struct SlothWidget : ModuleWidget
{
    RedGreenLightWidget *indicatorLight{};

    SlothWidget(SlothModule<circuit_t>* module, const char *panelSvgFileName)
    {
        using namespace SlothTypes;

        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, panelSvgFileName)));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        const float knobHeight = 18.3f;
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(3.8f, knobHeight)), module, BALANCE_KNOB_PARAM));

        const float outputHeight = 95.0f;
        addOutput(createOutput<PJ301MPort>(mm2px(Vec(1.5f,  outputHeight)), module, Y_OUTPUT));
        addOutput(createOutput<PJ301MPort>(mm2px(Vec(10.5f, outputHeight)), module, X_OUTPUT));

        indicatorLight = createLightCentered<LargeLight<RedGreenLightWidget>>(mm2px(Vec(10.2f, 55.0f)), module, INDICATOR_LIGHT_RED);
        addChild(indicatorLight);
    }
};


using SlothTorporModule = SlothModule<Analog::TorporSlothCircuit>;

struct SlothTorporWidget : SlothWidget<Analog::TorporSlothCircuit>
{
public:
    explicit SlothTorporWidget(SlothTorporModule* module)
        : SlothWidget<Analog::TorporSlothCircuit>(module, "res/SlothTorpor.svg")
        {}
};

Model* modelSlothTorpor = createModel<SlothTorporModule, SlothTorporWidget>("SlothTorpor");
