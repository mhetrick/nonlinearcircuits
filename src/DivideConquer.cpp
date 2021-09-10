#include "NLC.hpp"

struct DivideConquer : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		CLOCK1_INPUT,
		CLOCK3_INPUT,
        CLOCK5_INPUT,
		CLOCK7_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		DIV2_OUTPUT,
        DIV4_OUTPUT,
        DIV8_OUTPUT,
        DIV16_OUTPUT,
        DIV32_OUTPUT,
        DIV64_OUTPUT,
        DIV128_OUTPUT,
        DIV256_OUTPUT,

        DIV3_OUTPUT,
        DIV3DIV2_OUTPUT,

        DIV5_OUTPUT,
        DIV5DIV2_OUTPUT,

        DIV7_OUTPUT,
        DIV7DIV2_OUTPUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
		DIV2_LIGHT,
        DIV4_LIGHT,
        DIV8_LIGHT,
        DIV16_LIGHT,
        DIV32_LIGHT,
        DIV64_LIGHT,
        DIV128_LIGHT,
        DIV256_LIGHT,

        DIV3_LIGHT,
        DIV3DIV2_LIGHT,

        DIV5_LIGHT,
        DIV5DIV2_LIGHT,

        DIV7_LIGHT,
        DIV7DIV2_LIGHT,
		NUM_LIGHTS
	};

	DivideConquer() 
    {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        clockIn3.doubleTrig = true;
        clockIn5.doubleTrig = true;
        clockIn7.doubleTrig = true;
	}

	NLCTrigger clockIn1, clockIn3, clockIn5, clockIn7;
    int stepCount1 = 0, stepCount3 = 0, stepCount5 = 0, stepCount7 = 0;
    double gateOutValue = 5.0;
    bool div2 = false, div4 = false, div8 = false, div16 = false;
    bool div32 = false, div64 = false, div128 = false, div256 = false;
    bool div3 = false, div3div2 = false;
    bool div5 = false, div5div2 = false;
    bool div7 = false, div7div2 = false;

	void process(const ProcessArgs& args) override 
	{
        double mainClock = inputs[CLOCK1_INPUT].getVoltage();
        double clock3 = inputs[CLOCK3_INPUT].isConnected() ? inputs[CLOCK3_INPUT].getVoltage() : mainClock;
        double clock5 = inputs[CLOCK5_INPUT].isConnected() ? inputs[CLOCK5_INPUT].getVoltage() : mainClock;
        double clock7 = inputs[CLOCK7_INPUT].isConnected() ? inputs[CLOCK7_INPUT].getVoltage() : mainClock;

        if(clockIn1.process(mainClock))
        {

            if(stepCount1 == 256) stepCount1 = 1;
            else stepCount1++;

            if (stepCount1 % 2 == 0)
            {
                div2 = !div2;

                if (stepCount1 % 4 == 0)
                {
                    div4 = ! div4;

                    if (stepCount1 % 8 == 0)
                    {
                        div8 = !div8;

                        if (stepCount1 % 16 == 0)
                        {
                            div16 = ! div16;
                            if (stepCount1 % 32 == 0)
                            {
                                div32 = ! div32;
                                if (stepCount1 % 64 == 0)
                                {
                                    div64 = ! div64;
                                    if (stepCount1 % 128 == 0)
                                    {
                                        div128 = ! div128;
                                        if (stepCount1 % 256 == 0)
                                        {
                                            div256 = ! div256;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        if(clockIn3.process(clock3))
        {
            if(stepCount3 == 6) stepCount3 = 1;
            else stepCount3++;
            
            if (stepCount3 % 3 == 0)
            {
                div3div2 = !div3div2;
                if (stepCount3 % 6 == 0)
                {
                    div3 = !div3;
                }
            }
        }
        
        if(clockIn5.process(clock5))
        {
            if(stepCount5 == 10) stepCount5 = 1;
            else stepCount5++;
            
            if (stepCount5 % 5 == 0)
            {
                div5div2 = !div5div2;
                if (stepCount5 % 10 == 0)
                {
                    div5 = !div5;
                }
            }
        }
        
        if(clockIn7.process(clock7))
        {
            if(stepCount7 == 14) stepCount7 = 1;
            else stepCount7++;
            
            if (stepCount7 % 7 == 0)
            {
                div7div2 = !div7div2;
                if (stepCount7 % 14 == 0)
                {
                    div7 = !div7;
                }
            }
        }

        outputs[DIV2_OUTPUT].setVoltage(div2 ? gateOutValue : 0.0);
        outputs[DIV4_OUTPUT].setVoltage(div4 ? gateOutValue : 0.0);
        outputs[DIV8_OUTPUT].setVoltage(div8 ? gateOutValue : 0.0);
        outputs[DIV16_OUTPUT].setVoltage(div16 ? gateOutValue : 0.0);
        outputs[DIV32_OUTPUT].setVoltage(div32 ? gateOutValue : 0.0);
        outputs[DIV64_OUTPUT].setVoltage(div64 ? gateOutValue : 0.0);
        outputs[DIV128_OUTPUT].setVoltage(div128 ? gateOutValue : 0.0);
        outputs[DIV256_OUTPUT].setVoltage(div256 ? gateOutValue : 0.0);

        outputs[DIV3_OUTPUT].setVoltage(div3 ? gateOutValue : 0.0);
        outputs[DIV3DIV2_OUTPUT].setVoltage(div3div2 ? gateOutValue : 0.0);

        outputs[DIV5_OUTPUT].setVoltage(div5 ? gateOutValue : 0.0);
        outputs[DIV5DIV2_OUTPUT].setVoltage(div5div2 ? gateOutValue : 0.0);

        outputs[DIV7_OUTPUT].setVoltage(div7 ? gateOutValue : 0.0);
        outputs[DIV7DIV2_OUTPUT].setVoltage(div7div2 ? gateOutValue : 0.0);

        lights[DIV2_LIGHT].setBrightness(div2 ? 1.0 : 0.0);
        lights[DIV4_LIGHT].setBrightness(div4 ? 1.0 : 0.0);
        lights[DIV8_LIGHT].setBrightness(div8 ? 1.0 : 0.0);
        lights[DIV16_LIGHT].setBrightness(div16 ? 1.0 : 0.0);
        lights[DIV32_LIGHT].setBrightness(div32 ? 1.0 : 0.0);
        lights[DIV64_LIGHT].setBrightness(div64 ? 1.0 : 0.0);
        lights[DIV128_LIGHT].setBrightness(div128 ? 1.0 : 0.0);
        lights[DIV256_LIGHT].setBrightness(div256 ? 1.0 : 0.0);

        lights[DIV3_LIGHT].setBrightness(div3 ? 1.0 : 0.0);
        lights[DIV3DIV2_LIGHT].setBrightness(div3div2 ? 1.0 : 0.0);

        lights[DIV5_LIGHT].setBrightness(div5 ? 1.0 : 0.0);
        lights[DIV5DIV2_LIGHT].setBrightness(div5div2 ? 1.0 : 0.0);

        lights[DIV7_LIGHT].setBrightness(div7 ? 1.0 : 0.0);
        lights[DIV7DIV2_LIGHT].setBrightness(div7div2 ? 1.0 : 0.0);
	}
};


struct DivideConquerWidget : ModuleWidget {
	DivideConquerWidget(DivideConquer* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NLC - DIVIDE & CONQUER.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        float col1X = 14.25;
        float col2X = 48;
        float col3X = 81.5;

        float row1Y = 69.25;
        float row2Y = 117.25;
        float row3Y = 165.25;
        float row4Y = 227.25;
        float row5Y = 272;
        float row6Y = 315.25;

		addInput(createInput<PJ301MPort>(Vec(col1X, row4Y), module, DivideConquer::CLOCK1_INPUT));
		addInput(createInput<PJ301MPort>(Vec(col1X, row1Y), module, DivideConquer::CLOCK3_INPUT));
        addInput(createInput<PJ301MPort>(Vec(col1X, row2Y), module, DivideConquer::CLOCK5_INPUT));
        addInput(createInput<PJ301MPort>(Vec(col1X, row3Y), module, DivideConquer::CLOCK7_INPUT));

		addOutput(createOutput<PJ301MPort>(Vec(col2X, row1Y), module, DivideConquer::DIV3DIV2_OUTPUT));
		addOutput(createOutput<PJ301MPort>(Vec(col3X, row1Y), module, DivideConquer::DIV3_OUTPUT));

        addOutput(createOutput<PJ301MPort>(Vec(col2X, row2Y), module, DivideConquer::DIV5DIV2_OUTPUT));
		addOutput(createOutput<PJ301MPort>(Vec(col3X, row2Y), module, DivideConquer::DIV5_OUTPUT));

        addOutput(createOutput<PJ301MPort>(Vec(col2X, row3Y), module, DivideConquer::DIV7DIV2_OUTPUT));
		addOutput(createOutput<PJ301MPort>(Vec(col3X, row3Y), module, DivideConquer::DIV7_OUTPUT));

        addOutput(createOutput<PJ301MPort>(Vec(col2X, row4Y), module, DivideConquer::DIV2_OUTPUT));
		addOutput(createOutput<PJ301MPort>(Vec(col3X, row4Y), module, DivideConquer::DIV4_OUTPUT));

        addOutput(createOutput<PJ301MPort>(Vec(col1X, row5Y), module, DivideConquer::DIV8_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(col2X, row5Y), module, DivideConquer::DIV16_OUTPUT));
		addOutput(createOutput<PJ301MPort>(Vec(col3X, row5Y), module, DivideConquer::DIV32_OUTPUT));

        addOutput(createOutput<PJ301MPort>(Vec(col1X, row6Y), module, DivideConquer::DIV64_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(col2X, row6Y), module, DivideConquer::DIV128_OUTPUT));
		addOutput(createOutput<PJ301MPort>(Vec(col3X, row6Y), module, DivideConquer::DIV256_OUTPUT));

		////////LIGHTS/////////
        int light1x = 40;
        int light2x = 75;
        int downOffset1 = 20;
        int downOffset2 = 21;
        int upOffset = -1.75;

        addChild(createLight<SmallLight<BlueLight>>(Vec(light1x, row5Y + downOffset1), module, DivideConquer::DIV8_LIGHT));
        addChild(createLight<SmallLight<BlueLight>>(Vec(light1x, row6Y + downOffset1), module, DivideConquer::DIV64_LIGHT));

        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row1Y + upOffset), module, DivideConquer::DIV3DIV2_LIGHT));
        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row1Y + downOffset2), module, DivideConquer::DIV3_LIGHT));
        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row2Y + upOffset), module, DivideConquer::DIV5DIV2_LIGHT));
        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row2Y + downOffset2), module, DivideConquer::DIV5_LIGHT));
        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row3Y + upOffset), module, DivideConquer::DIV7DIV2_LIGHT));
        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row3Y + downOffset2), module, DivideConquer::DIV7_LIGHT));

        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row4Y + upOffset), module, DivideConquer::DIV2_LIGHT));
        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row4Y + downOffset2), module, DivideConquer::DIV4_LIGHT));
        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row5Y + upOffset - 1), module, DivideConquer::DIV16_LIGHT));
        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row5Y + downOffset2 - 1), module, DivideConquer::DIV32_LIGHT));
        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row6Y + upOffset), module, DivideConquer::DIV128_LIGHT));
        addChild(createLight<SmallLight<BlueLight>>(Vec(light2x, row6Y + downOffset2), module, DivideConquer::DIV256_LIGHT));
	}
};



Model* modelDivideConquer = createModel<DivideConquer, DivideConquerWidget>("DivideConquer");