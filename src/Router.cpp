#include "NLC.hpp"

struct Router : Module {
	enum ParamIds {
		ROUTE1_PARAM,
        ROUTE2_PARAM,
        ROUTE3_PARAM,
        ROUTE4_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
        IN2_INPUT,
        IN3_INPUT,
        IN4_INPUT,
		
        CV12_INPUT,
        CV34_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
        OUT2_OUTPUT,
        OUT3_OUTPUT,
        OUT4_OUTPUT,

        MIX_OUTPUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
		OUT1_POS_LIGHT, OUT1_NEG_LIGHT,
		OUT2_POS_LIGHT, OUT2_NEG_LIGHT,
        OUT3_POS_LIGHT, OUT3_NEG_LIGHT,
        OUT4_POS_LIGHT, OUT4_NEG_LIGHT,
		NUM_LIGHTS
	};

    float outs[4]{};

	Router() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(ROUTE1_PARAM, -10.0, 10.0, 0.0, "Comparator 1", "v");
        configParam(ROUTE2_PARAM, -10.0, 10.0, 0.0, "Comparator 2", "v");
        configParam(ROUTE3_PARAM, -10.0, 10.0, 0.0, "Comparator 3", "v");
        configParam(ROUTE4_PARAM, -10.0, 10.0, 0.0, "Comparator 4", "v");

        configInput(IN1_INPUT, "1");
        configInput(IN2_INPUT, "2");
        configInput(IN3_INPUT, "3");
        configInput(IN4_INPUT, "4");

        configInput(CV12_INPUT, "CV 1&2");
        configInput(CV34_INPUT, "CV 3&4");

        configOutput(OUT1_OUTPUT, "1");
        configOutput(OUT2_OUTPUT, "2");
        configOutput(OUT3_OUTPUT, "3");
        configOutput(OUT4_OUTPUT, "4");

        configOutput(MIX_OUTPUT, "Mix");
    }


	void process(const ProcessArgs& args) override 
	{

        const float cv12 = inputs[CV12_INPUT].getVoltage();
        const float cv34 = inputs[CV34_INPUT].getNormalVoltage(cv12);

        float in1 = inputs[IN1_INPUT].getVoltage();
        float in2 = inputs[IN2_INPUT].getNormalVoltage(in1);
        float in3 = inputs[IN3_INPUT].getNormalVoltage(in2);
        float in4 = inputs[IN4_INPUT].getNormalVoltage(in3);

        const float compare1 = params[ROUTE1_PARAM].getValue();
        const float compare2 = params[ROUTE2_PARAM].getValue();
        const float compare3 = params[ROUTE3_PARAM].getValue();
        const float compare4 = params[ROUTE4_PARAM].getValue();

        outs[0] = cv12 <= compare1 ? in1 : 0.0f;
        outs[1] = cv12 <= compare2 ? in2 : 0.0f;
        outs[2] = cv34 <= compare3 ? in3 : 0.0f;
        outs[3] = cv34 <= compare4 ? in4 : 0.0f;

        float mix = 0.0f;
        for (int i = 0; i < 4; i++)
        {
            outputs[OUT1_OUTPUT + i].setVoltage(outs[i]);
            mix += outs[i];
            lights[i*2].setSmoothBrightness(outs[i] * 0.2f, args.sampleTime);
            lights[1 + i*2].setSmoothBrightness(outs[i] * -0.2f, args.sampleTime);
        }
        
        outputs[MIX_OUTPUT].setVoltage(mix * 0.25f);
	}
};


struct RouterWidget : ModuleWidget {
	RouterWidget(Router* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NLC - ROUTER.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


        const float leftSteps = 4.25f;
        const float rightSteps = 24.5f;
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(leftSteps, 17.5f)), module, Router::ROUTE1_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(rightSteps, 25.0f)), module, Router::ROUTE2_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(leftSteps, 35.0f)), module, Router::ROUTE3_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(rightSteps, 42.5f)), module, Router::ROUTE4_PARAM));

		////////LIGHTS/////////
        const float jacksX = 2.4f;
        const float cvJacksY = 69.5f;
        const float inJacksY = 84.75f;
        const float outJacksY = 100.0f;
        float lightX = 5.25f;
        float lightY = 112.0f;
        float spacing = 9.5f;

        addInput(createInput<PJ301MPort>(mm2px(Vec(jacksX, cvJacksY)), module, Router::CV12_INPUT));
        addInput(createInput<PJ301MPort>(mm2px(Vec(jacksX + (spacing*2), cvJacksY)), module, Router::CV34_INPUT));
        addOutput(createOutput<PJ301MPort>(mm2px(Vec(jacksX + (spacing*3), cvJacksY)), module, Router::MIX_OUTPUT));

        for (int i = 0; i < 4; i++)
        {
            float xOffset = i * spacing;
            addInput(createInput<PJ301MPort>(mm2px(Vec(jacksX + xOffset, inJacksY)), module, Router::IN1_INPUT + i));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(jacksX + xOffset, outJacksY)), module, Router::OUT1_OUTPUT + i));
            addChild(createLight<SmallLight<GreenRedLight>>(mm2px(Vec(lightX + xOffset, lightY)), module, Router::OUT1_POS_LIGHT + i*2));
        }
        
	}
};



Model* modelRouter = createModel<Router, RouterWidget>("Router");