/*
    TripleSloth.cpp  -  Don Cross <cosinekitty@gmail.com>  -  2023-09-23

    A VCV Rack version of the Triple Sloth hardware module by Andrew Fitch:
    https://www.nonlinearcircuits.com/modules/p/triple-sloth


*/
#include "NLC.hpp"
#include "SlothCircuit.hpp"
#include "SlothPanelCoords.hpp"


namespace TripleSlothTypes
{
    enum ParamIds
    {
        APATHY_KNOB_PARAM,
        TORPOR_KNOB_PARAM,
        PARAMS_LEN
    };

    enum InputIds
    {
        APATHY_CV_INPUT,
        TORPOR_CV_INPUT,
        INPUTS_LEN
    };

    enum OutputIds
    {
        APATHY_X_OUTPUT,
        APATHY_Y_OUTPUT,
        APATHY_Z_OUTPUT,
        INERTIA_X_OUTPUT,
        INERTIA_Y_OUTPUT,
        INERTIA_Z_OUTPUT,
        TORPOR_X_OUTPUT,
        TORPOR_Y_OUTPUT,
        TORPOR_Z_OUTPUT,
        NEGATIVE_ZSUM_OUTPUT,
        POSITIVE_ZSUM_OUTPUT,
        OUTPUTS_LEN
    };

    enum LightIds
    {
        APATHY_LIGHT_RED,
        APATHY_LIGHT_GREEN,
        INERTIA_LIGHT_RED,
        INERTIA_LIGHT_GREEN,
        TORPOR_LIGHT_RED,
        TORPOR_LIGHT_GREEN,
        LIGHTS_LEN
    };
}


struct TripleSlothModule : Module
{
    Analog::TripleSlothCircuit circuit;

    TripleSlothModule()
    {
        using namespace TripleSlothTypes;

        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configParam(APATHY_KNOB_PARAM, 0.0f, 1.0f, 0.5f, "Apathy balance");
        configParam(TORPOR_KNOB_PARAM, 0.0f, 1.0f, 0.5f, "Torpor balance");

        configInput(APATHY_CV_INPUT, "Apathy CV");
        configInput(TORPOR_CV_INPUT, "Torpor CV");

        configOutput(APATHY_X_OUTPUT, "Apathy X");
        configOutput(APATHY_Y_OUTPUT, "Apathy Y");
        configOutput(APATHY_Z_OUTPUT, "Apathy Z");
        configOutput(INERTIA_X_OUTPUT, "Inertia X");
        configOutput(INERTIA_Y_OUTPUT, "Inertia Y");
        configOutput(INERTIA_Z_OUTPUT, "Inertia Z");
        configOutput(TORPOR_X_OUTPUT, "Torpor X");
        configOutput(TORPOR_Y_OUTPUT, "Torpor Y");
        configOutput(TORPOR_Z_OUTPUT, "Torpor Z");
        configOutput(NEGATIVE_ZSUM_OUTPUT, "Z−");
        configOutput(POSITIVE_ZSUM_OUTPUT, "Z+");

        for (int oid = APATHY_X_OUTPUT; oid < OUTPUTS_LEN; ++oid)
            outputs[oid].setChannels(1);
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
        using namespace TripleSlothTypes;

        circuit.apathy.setKnobPosition(params[APATHY_KNOB_PARAM].getValue());
        circuit.torpor.setKnobPosition(params[TORPOR_KNOB_PARAM].getValue());
        circuit.apathy.setControlVoltage(inputs[APATHY_CV_INPUT].getVoltageSum());
        circuit.torpor.setControlVoltage(inputs[TORPOR_CV_INPUT].getVoltageSum());

        circuit.update(args.sampleRate);

        outputs[APATHY_X_OUTPUT].setVoltage(circuit.apathy.xVoltage(), 0);
        outputs[APATHY_Y_OUTPUT].setVoltage(circuit.apathy.yVoltage(), 0);
        outputs[APATHY_Z_OUTPUT].setVoltage(circuit.apathy.zVoltage(), 0);

        outputs[INERTIA_X_OUTPUT].setVoltage(circuit.inertia.xVoltage(), 0);
        outputs[INERTIA_Y_OUTPUT].setVoltage(circuit.inertia.yVoltage(), 0);
        outputs[INERTIA_Z_OUTPUT].setVoltage(circuit.inertia.zVoltage(), 0);

        outputs[TORPOR_X_OUTPUT].setVoltage(circuit.torpor.xVoltage(), 0);
        outputs[TORPOR_Y_OUTPUT].setVoltage(circuit.torpor.yVoltage(), 0);
        outputs[TORPOR_Z_OUTPUT].setVoltage(circuit.torpor.zVoltage(), 0);

        float zsum = circuit.zsum();
        outputs[NEGATIVE_ZSUM_OUTPUT].setVoltage(std::min(0.0f, zsum));
        outputs[POSITIVE_ZSUM_OUTPUT].setVoltage(std::max(0.0f, zsum));

        lights[APATHY_LIGHT_RED].setBrightness(SlothLedBrightness(-circuit.apathy.yVoltage()));
        lights[APATHY_LIGHT_GREEN].setBrightness(SlothLedBrightness(+circuit.apathy.yVoltage()));

        lights[INERTIA_LIGHT_RED].setBrightness(SlothLedBrightness(-circuit.inertia.yVoltage()));
        lights[INERTIA_LIGHT_GREEN].setBrightness(SlothLedBrightness(+circuit.inertia.yVoltage()));

        lights[TORPOR_LIGHT_RED].setBrightness(SlothLedBrightness(-circuit.torpor.yVoltage()));
        lights[TORPOR_LIGHT_GREEN].setBrightness(SlothLedBrightness(+circuit.torpor.yVoltage()));
    }
};


struct TripleSlothWidget : ModuleWidget
{
    explicit TripleSlothWidget(TripleSlothModule *module)
    {
        using namespace TripleSlothTypes;
        using namespace SlothPanel;

        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/TripleSloth.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(X1, Y_KNOB)), module, APATHY_KNOB_PARAM));
        addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(X3, Y_KNOB)), module, TORPOR_KNOB_PARAM));

        addChild(createLightCentered<LargeLight<SlothRedGreenLightWidget>>(mm2px(Vec(X1, Y_LED)), module, APATHY_LIGHT_RED));
        addChild(createLightCentered<LargeLight<SlothRedGreenLightWidget>>(mm2px(Vec(X2, Y_LED)), module, INERTIA_LIGHT_RED));
        addChild(createLightCentered<LargeLight<SlothRedGreenLightWidget>>(mm2px(Vec(X3, Y_LED)), module, TORPOR_LIGHT_RED));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(X1, Y_CVIN)), module, APATHY_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(X3, Y_CVIN)), module, TORPOR_CV_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(X1, Y_OUT0)), module, APATHY_X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(X1, Y_OUT1)), module, APATHY_Y_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(X1, Y_OUT2)), module, APATHY_Z_OUTPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(X2, Y_OUT0)), module, INERTIA_X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(X2, Y_OUT1)), module, INERTIA_Y_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(X2, Y_OUT2)), module, INERTIA_Z_OUTPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(X3, Y_OUT0)), module, TORPOR_X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(X3, Y_OUT1)), module, TORPOR_Y_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(X3, Y_OUT2)), module, TORPOR_Z_OUTPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(XN, Y_OUT3)), module, NEGATIVE_ZSUM_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(XP, Y_OUT3)), module, POSITIVE_ZSUM_OUTPUT));
    }
};


Model *modelTripleSloth = createModel<TripleSlothModule, TripleSlothWidget>("TripleSloth");