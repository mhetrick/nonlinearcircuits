#include "NLC.hpp"

struct FourSeq : Module {
	enum ParamIds {
		STEP1_PARAM,
        STEP2_PARAM,
        STEP3_PARAM,
        STEP4_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		CLOCK_INPUT,
		DIR_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
        OUT2_OUTPUT,
        OUT3_OUTPUT,
        OUT4_OUTPUT,

		X_OUTPUT,
		Y_OUTPUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
		OUT1_LIGHT,
		OUT2_LIGHT,
		OUT3_LIGHT,
		OUT4_LIGHT,
		NUM_LIGHTS
	};

	FourSeq() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(FourSeq::STEP1_PARAM, 0.0, 5.0, 0.0, "Step x0/y3");
        configParam(FourSeq::STEP2_PARAM, 0.0, 5.0, 0.0, "Step x1/y2");
        configParam(FourSeq::STEP3_PARAM, 0.0, 5.0, 0.0, "Step x2/y1");
        configParam(FourSeq::STEP4_PARAM, 0.0, 5.0, 0.0, "Step x3/y0");
	}

	bool divBools[4] = {false, false, false, false};
	float steps[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    int xStep = 0;
    int stepCount = 0;

	NLCTrigger clockIn;

	void process(const ProcessArgs& args) override 
	{
		if(clockIn.process(inputs[CLOCK_INPUT].getVoltage()))
		{
            bool backwards = inputs[DIR_INPUT].getVoltage() > 1.0f;

            if(backwards) xStep = xStep + 1;
            else xStep = xStep - 1;

            if(xStep > 3) xStep = 0;
            else if (xStep < 0) xStep = 3;

            if(stepCount == 16) stepCount = 1;
            else stepCount++;

            if (stepCount % 2 == 0)
            {
                divBools[0] = !divBools[0];

                if (stepCount % 4 == 0)
                {
                    divBools[1] = ! divBools[1];

                    if (stepCount % 8 == 0)
                    {
                        divBools[2] = !divBools[2];

                        if (stepCount % 16 == 0)
                        {
                            divBools[3] = ! divBools[3];
                        }
                    }
                }
            }
            
		}

        for (int i = 0; i < 4; i++)
        {
            steps[i] = params[i].getValue();
            outputs[i].setVoltage(divBools[i] ? 5.0f : 0.0f);
            lights[i].setBrightnessSmooth(divBools[i] ? 5.0f : 0.0f);
        }

        outputs[X_OUTPUT].setVoltage(steps[xStep]);
        outputs[Y_OUTPUT].setVoltage(steps[3 - xStep]);
	}
};


struct FourSeqWidget : ModuleWidget {
	FourSeqWidget(FourSeq* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NLC - 4seq.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


        const float leftSteps = 4.5f;
        const float rightSteps = 24.0f;
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(leftSteps, 17.5f)), module, FourSeq::STEP1_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(rightSteps, 25.0f)), module, FourSeq::STEP2_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(leftSteps, 35.0f)), module, FourSeq::STEP3_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(rightSteps, 42.5f)), module, FourSeq::STEP4_PARAM));

        const float inJacksX = 1.5f;
        const float inJacksY = 72.0f;
		addInput(createInput<PJ301MPort>(mm2px(Vec(inJacksX, inJacksY)), module, FourSeq::CLOCK_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(inJacksX + 10, inJacksY)), module, FourSeq::DIR_INPUT));

        const float outJacksX = 21.5f;
		const float outJacksY = 79.5f;
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(outJacksX, outJacksY)), module, FourSeq::X_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(outJacksX + 10, outJacksY)), module, FourSeq::Y_OUTPUT));

		////////LIGHTS/////////
        const float divJacksX = 1.25f;
        const float divJacksY = 96.0f;
        float lightX = 4.5f;
        float lightY = 108.0f;
        int spacing = 10.0f;

        for (int i = 0; i < 4; i++)
        {
            int xOffset = i * spacing;
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(divJacksX + xOffset, divJacksY)), module, FourSeq::OUT1_OUTPUT + i));
            addChild(createLight<SmallLight<BlueLight>>(mm2px(Vec(lightX + xOffset, lightY)), module, FourSeq::OUT1_LIGHT + i));
        }
        
	}
};



Model* model4Seq = createModel<FourSeq, FourSeqWidget>("4Seq");