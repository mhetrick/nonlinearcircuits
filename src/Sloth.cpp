/*
    Sloth.cpp  -  Don Cross <cosinekitty@gmail.com>  -  2023-09-23

    A VCV Rack version of the Sloth hardware modules by Andrew Fitch:
    https://www.nonlinearcircuits.com/modules/p/4hp-sloth-chaos
*/

#include "NLC.hpp"
#include "SlothCircuit.hpp"
#include "SlothPanelCoords.hpp"


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
        lights[INDICATOR_LIGHT_RED].setBrightness(SlothLedBrightness(-circuit.yVoltage()));
        lights[INDICATOR_LIGHT_GREEN].setBrightness(SlothLedBrightness(+circuit.yVoltage()));
    }
};


template <typename circuit_t>
struct SlothWidget : ModuleWidget
{
    SlothWidget(SlothModule<circuit_t>* module, const char *panelSvgFileName)
    {
        using namespace SlothTypes;
        using namespace SlothPanel;

        const float xc = 10.16f;

        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, panelSvgFileName)));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(xc, Y_KNOB)), module, BALANCE_KNOB_PARAM));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(xc - 4.5f, Y_OUT2)), module, Y_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(xc + 4.5f, Y_OUT2)), module, X_OUTPUT));

        addChild(createLightCentered<LargeLight<SlothRedGreenLightWidget>>(mm2px(Vec(xc, Y_LED)), module, INDICATOR_LIGHT_RED));
    }
};


//-------------------------------------------------------------------------------------------------
// Torpor

using SlothTorporModule = SlothModule<Analog::TorporSlothCircuit>;

struct SlothTorporWidget : SlothWidget<Analog::TorporSlothCircuit>
{
public:
    explicit SlothTorporWidget(SlothTorporModule* module)
        : SlothWidget<Analog::TorporSlothCircuit>(module, "res/SlothTorpor.svg")
        {}
};

Model* modelSlothTorpor = createModel<SlothTorporModule, SlothTorporWidget>("SlothTorpor");


//-------------------------------------------------------------------------------------------------
// Apathy

using SlothApathyModule = SlothModule<Analog::ApathySlothCircuit>;

struct SlothApathyWidget : SlothWidget<Analog::ApathySlothCircuit>
{
public:
    explicit SlothApathyWidget(SlothApathyModule* module)
        : SlothWidget<Analog::ApathySlothCircuit>(module, "res/SlothApathy.svg")
        {}
};

Model* modelSlothApathy = createModel<SlothApathyModule, SlothApathyWidget>("SlothApathy");


//-------------------------------------------------------------------------------------------------
// Inertia

using SlothInertiaModule = SlothModule<Analog::InertiaSlothCircuit>;

struct SlothInertiaWidget : SlothWidget<Analog::InertiaSlothCircuit>
{
public:
    explicit SlothInertiaWidget(SlothInertiaModule* module)
        : SlothWidget<Analog::InertiaSlothCircuit>(module, "res/SlothInertia.svg")
        {}
};

Model* modelSlothInertia = createModel<SlothInertiaModule, SlothInertiaWidget>("SlothInertia");

//-------------------------------------------------------------------------------------------------
