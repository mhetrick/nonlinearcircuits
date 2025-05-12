#include "NLC.hpp"

struct DeEscalate : Module {
	enum ParamIds {
        GAIN1_PARAM,
        GAIN2_PARAM,
        GAIN3_PARAM,
        GAIN4_PARAM,
        GAIN5_PARAM,
        GAIN6_PARAM,
        GAIN7_PARAM,

		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
        IN2_INPUT,
        IN3_INPUT,
        IN4_INPUT,
        IN5_INPUT,
        IN6_INPUT,
        IN7_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
        OUT2_OUTPUT,
        OUT3_OUTPUT,
        OUT4_OUTPUT,
        OUT5_OUTPUT,
        OUT6_OUTPUT,
        OUT7_OUTPUT,

		NUM_OUTPUTS
	};
	enum LightIds {

		NUM_LIGHTS
	};

	DeEscalate() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        for (int i = 0; i < 7; i++)
        {
            const auto channelString = std::to_string(i + 1);

            configInput(IN1_INPUT + i, "In " + channelString);
            configParam(GAIN1_PARAM + i, 0.0, 1.0, 0.0, "Attenuate " + channelString);
            configOutput(OUT1_OUTPUT+ i, "Out " + channelString);
        }
    }

	void process(const ProcessArgs& args) override 
	{
        for (int i = 0; i < 7; i++)
        {
            const float in = inputs[IN1_INPUT + i].getVoltage();
            const float gain = params[GAIN1_PARAM + i].getValue();
            outputs[OUT1_OUTPUT + i].setVoltage(in * gain);
        }
    }
};


struct DeEscalateWidget : ModuleWidget {
	DeEscalateWidget(DeEscalate* module) {
		setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/DeEscalateLight.svg"), asset::plugin(pluginInstance, "res/DeEscalate.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        const float inJacksX = 2.0f;
        const float paramX = 17.0f;
        const float outJacksX = 30.5f;

        const float startY = 14.75f;
        float ySpacing = 15.0f;

        for (int i = 0; i < 7; i++)
        {
            addInput(createInput<PJ301MPort>(mm2px(Vec(inJacksX, startY + ySpacing*i)), module, DeEscalate::IN1_INPUT + i));
            addParam(createParam<RoundSmallBlackKnob>(mm2px(Vec(paramX, startY + ySpacing*i)), module, DeEscalate::GAIN1_PARAM + i));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(outJacksX, startY + ySpacing*i)), module, DeEscalate::OUT1_OUTPUT + i));
        }
	}
};



Model* modelDeEscalate = createModel<DeEscalate, DeEscalateWidget>("DeEscalate");