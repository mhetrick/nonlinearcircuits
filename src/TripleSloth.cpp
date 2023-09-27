/*
    TripleSloth.cpp  -  Don Cross <cosinekitty@gmail.com>  -  2023-09-23

    A VCV Rack version of the Triple Sloth hardware module by Andrew Fitch:
    https://www.nonlinearcircuits.com/modules/p/triple-sloth


*/

#include "NLC.hpp"
#include "SlothCircuit.hpp"


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

        configOutput(APATHY_X_OUTPUT, "Apathy X");
        configOutput(APATHY_Y_OUTPUT, "Apathy Y");
        configOutput(APATHY_Z_OUTPUT, "Apathy Z");
        configOutput(INERTIA_X_OUTPUT, "Inertia X");
        configOutput(INERTIA_Y_OUTPUT, "Inertia Y");
        configOutput(INERTIA_Z_OUTPUT, "Inertia Z");
        configOutput(TORPOR_X_OUTPUT, "Torpor X");
        configOutput(TORPOR_Y_OUTPUT, "Torpor Y");
        configOutput(TORPOR_Z_OUTPUT, "Torpor Z");
        configOutput(NEGATIVE_ZSUM_OUTPUT, "Z+");
        configOutput(POSITIVE_ZSUM_OUTPUT, "Z-");
    }
};


struct TripleSlothWidget : ModuleWidget
{
    explicit TripleSlothWidget(TripleSlothModule *module)
    {
        using namespace TripleSlothTypes;

        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/TripleSloth.svg")));
    }
};


Model *modelTripleSloth = createModel<TripleSlothModule, TripleSlothWidget>("TripleSloth");