#include "NLC.hpp"

struct DivineCMOS : Module {
	enum ParamIds {
		DIV2_PARAM,
        DIV4_PARAM,
        DIV8_PARAM,
        DIV16_PARAM,
        SLEW_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		CLOCK1_INPUT,
		CLOCK2_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
        OUT2_OUTPUT,
        OUT3_OUTPUT,
        OUT4_OUTPUT,

		MAIN_OUTPUT,
		SLEW_OUTPUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
		OUT1_LIGHT,
		OUT2_LIGHT,
		OUT3_LIGHT,
		OUT4_LIGHT,
        CMOS_LIGHT,
		NUM_LIGHTS
	};

	DivineCMOS() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(DivineCMOS::DIV2_PARAM, 0.0, 5.0, 0.0, "Div 2 Gain");
        configParam(DivineCMOS::DIV4_PARAM, 0.0, 5.0, 0.0, "Div 4 Gain");
        configParam(DivineCMOS::DIV8_PARAM, 0.0, 5.0, 0.0, "Div 8 Gain");
        configParam(DivineCMOS::DIV16_PARAM, 0.0, 5.0, 0.0, "Div 16 Gain");
        configParam(DivineCMOS::SLEW_PARAM, 0.0, 1.0, 0.0, "Slew");
	}

	bool divBools1[4] = {false, false, false, false};
    bool divBools2[4] = {false, false, false, false};
    bool outBools[4] = {false, false, false, false};
    float outMix[4] = {false, false, false, false};
    int stepCount1 = 0;
    int stepCount2 = 0;

	NLCTrigger clockIn1, clockIn2;

    dsp::SlewLimiter slewLimiter;
    float lastSlew = -1.0f;

	void process(const ProcessArgs& args) override 
	{
		if(clockIn1.process(inputs[CLOCK1_INPUT].getVoltage()))
		{
            if(stepCount1 == 16) stepCount1 = 1;
            else stepCount1++;

            if (stepCount1 % 2 == 0)
            {
                divBools1[0] = !divBools1[0];

                if (stepCount1 % 4 == 0)
                {
                    divBools1[1] = ! divBools1[1];

                    if (stepCount1 % 8 == 0)
                    {
                        divBools1[2] = !divBools1[2];

                        if (stepCount1 % 16 == 0)
                        {
                            divBools1[3] = ! divBools1[3];
                        }
                    }
                }
            } 
		}

        if(clockIn2.process(inputs[CLOCK2_INPUT].getVoltage()))
		{
            if(stepCount2 == 16) stepCount2 = 1;
            else stepCount2++;

            if (stepCount2 % 2 == 0)
            {
                divBools2[0] = !divBools2[0];

                if (stepCount2 % 4 == 0)
                {
                    divBools2[1] = ! divBools2[1];

                    if (stepCount2 % 8 == 0)
                    {
                        divBools2[2] = !divBools2[2];

                        if (stepCount2 % 16 == 0)
                        {
                            divBools2[3] = ! divBools2[3];
                        }
                    }
                }
            } 
		}

        float totalMix = 0.0f;

        for (int i = 0; i < 4; i++)
        {
            outBools[i] = divBools1[i] != divBools2[i];
            outMix[i] = outBools[i] ? params[DIV2_PARAM + i].getValue() : 0.0f;
            totalMix += outMix[i];

            outputs[i].setVoltage(outBools[i] ? 5.0f : 0.0f);
            lights[i].setSmoothBrightness(outBools[i] ? 5.0f : 0.0f, args.sampleTime);
        }

        const auto slewParam = params[SLEW_PARAM].getValue();
		if (lastSlew != slewParam)
		{
			lastSlew = slewParam;
			const float scaledSlew = slewParam > 0.001f ? std::pow(slewParam, 0.25f) : 0.0f;
			const auto slewHz = (1.0f - scaledSlew) * (18000.0f) + 1.0f;
			slewLimiter.setRiseFall(slewHz, slewHz);
		}
        const auto slewOut = slewLimiter.process(args.sampleTime, totalMix);

        outputs[MAIN_OUTPUT].setVoltage(totalMix);
        outputs[SLEW_OUTPUT].setVoltage(slewOut);

        lights[CMOS_LIGHT].setSmoothBrightness(totalMix * 0.2f, args.sampleTime);
	}
};


struct DivineCMOSWidget : ModuleWidget {
	DivineCMOSWidget(DivineCMOS* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NLC - DIVINE CMOS.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


        const float leftSteps = 4.5f;
        const float rightSteps = 24.0f;
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(leftSteps, 17.5f)), module, DivineCMOS::DIV2_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(rightSteps, 25.0f)), module, DivineCMOS::DIV4_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(leftSteps, 35.0f)), module, DivineCMOS::DIV8_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(rightSteps, 42.5f)), module, DivineCMOS::DIV16_PARAM));

        addParam(createParam<Davies1900hWhiteKnob>(Vec(14, 173), module, DivineCMOS::SLEW_PARAM));

        const float inJacksX = 1.5f;
        const float inJacksY = 79.0f;
		addInput(createInput<PJ301MPort>(mm2px(Vec(inJacksX, inJacksY)), module, DivineCMOS::CLOCK1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(inJacksX + 10, inJacksY)), module, DivineCMOS::CLOCK2_INPUT));

        const float outJacksX = 21.25f;
		const float outJacksY = 86.5f;
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(outJacksX, outJacksY)), module, DivineCMOS::SLEW_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(outJacksX + 10, outJacksY)), module, DivineCMOS::MAIN_OUTPUT));

		////////LIGHTS/////////
        const float divJacksX = 1.25f;
        const float divJacksY = 102.75f;
        float lightX = 4.5f;
        float lightY = 113.0f;
        int spacing = 10.0f;

        for (int i = 0; i < 4; i++)
        {
            int xOffset = i * spacing;
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(divJacksX + xOffset, divJacksY)), module, DivineCMOS::OUT1_OUTPUT + i));
            addChild(createLight<SmallLight<BlueLight>>(mm2px(Vec(lightX + xOffset, lightY)), module, DivineCMOS::OUT1_LIGHT + i));
        }
        
        addChild(createLight<SmallLight<BlueLight>>(Vec(85.25, 187.25), module, DivineCMOS::CMOS_LIGHT));
	}
};



Model* modelDivineCMOS = createModel<DivineCMOS, DivineCMOSWidget>("DivineCMOS");