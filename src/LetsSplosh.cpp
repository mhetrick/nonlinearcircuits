#include "NLC.hpp"


struct LetsSplosh : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		SPLOSH1_INPUT,
		SPLOSH2_INPUT,
		SPLOSH3_INPUT,
		SPLOSH4_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		CT_NB_POS_OUT,
        CT_NB_NEG_OUT,

        CN_TB_POS_OUT,
        CN_TB_NEG_OUT,

        CB_TN_POS_OUT,
        CB_TN_NEG_OUT,

        CTN_B_POS_OUT,
        CTN_B_NEG_OUT,

        CTB_N_POS_OUT,
        CTB_N_NEG_OUT,

        CNB_T_POS_OUT,
        CNB_T_NEG_OUT,

        TNB_C_POS_OUT,
        TNB_C_NEG_OUT,

        CTNB_POS_OUT,
        CTNB_NEG_OUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS = NUM_OUTPUTS
	};

	LetsSplosh()
    {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configInput(SPLOSH1_INPUT, "Splosh 1 - Custard");
        configInput(SPLOSH2_INPUT, "Splosh 2 - Treacle");
        configInput(SPLOSH3_INPUT, "Splosh 3 - Natto");
        configInput(SPLOSH4_INPUT, "Splosh 4 - Batter");
	}

	//NLCDiffRect CT_NB, CN_TB, CB_TN, CTN_B, CTB_N, CNB_T, TNB_C, CTNB;

    NLCDiffRect diffRects[8];

	void process(const ProcessArgs& args) override 
	{
        const float custard = inputs[SPLOSH1_INPUT].getVoltage();
        const float treacle = inputs[SPLOSH2_INPUT].getVoltage();
        const float natto   = inputs[SPLOSH3_INPUT].getVoltage();
        const float batter  = inputs[SPLOSH4_INPUT].getVoltage();

        //CT_NB, CN_TB, CB_TN, CTN_B, CTB_N, CNB_T, TNB_C, CTNB;
        diffRects[0].setPositiveInputs(custard, treacle);
        diffRects[0].setNegativeInputs(natto, batter);

        diffRects[1].setPositiveInputs(custard, natto);
        diffRects[1].setNegativeInputs(treacle, batter);

        diffRects[2].setPositiveInputs(custard, batter);
        diffRects[2].setNegativeInputs(treacle, natto);

        diffRects[3].setPositiveInputs(custard, treacle + natto);
        diffRects[3].setNegativeInputs(0.0f, batter);

        diffRects[4].setPositiveInputs(custard, treacle + batter);
        diffRects[4].setNegativeInputs(0.0f, natto);

        diffRects[5].setPositiveInputs(custard, natto + batter);
        diffRects[5].setNegativeInputs(0.0f, treacle);

        diffRects[6].setPositiveInputs(natto, treacle + batter);
        diffRects[6].setNegativeInputs(0.0f, custard);

        diffRects[7].setPositiveInputs(custard + natto, treacle + batter);
        diffRects[7].setNegativeInputs(0.0f, 0.0f);

        for (int i = 0; i < 8; i++)
        {
            diffRects[i].process();
            outputs[i*2].setVoltage(diffRects[i].getPositiveOutput());
            outputs[i*2 + 1].setVoltage(diffRects[i].getNegativeOutput());
            lights[i*2].setSmoothBrightness(diffRects[i].getPositiveOutput() * 0.2f, args.sampleTime);
            lights[i*2 + 1].setSmoothBrightness(diffRects[i].getNegativeOutput() * -0.2f, args.sampleTime);
        }
	}
};


struct LetsSploshWidget : ModuleWidget {
	LetsSploshWidget(LetsSplosh* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LetsSplosh.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		//const float panelHeight = 128.5f;
		//const float panelWidth = 5.08*12;

        const float inX = 21.0f;
        const float inStartY = 18.5f;
        const float inYSpacing = 27.75f;

        const float outPosX = 6.25f;
        const float outNegX = 36.0f;

        const float out1Y = 13.0f;
        const float out2Y = 24.0f;
        const float light2Y = out2Y + 6.5f;

        for (int i = 0; i < LetsSplosh::NUM_INPUTS; i++)
        {
            addInput(createInput<PJ301MPort>(mm2px(Vec(inX, inStartY + inYSpacing*i)), module, i));
        }

        for (int i = 0; i < 4; i++)
        {
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(outPosX, out1Y + inYSpacing*i)), module, 4*i));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(outNegX, out1Y + inYSpacing*i)), module, 4*i + 1));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(outPosX, out2Y + inYSpacing*i)), module, 4*i + 2));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(outNegX, out2Y + inYSpacing*i)), module, 4*i + 3));

            addChild(createLight<SmallLight<GreenLight>>(mm2px(Vec(outPosX - 4.0f, out1Y + inYSpacing*i)), module, 4*i));
            addChild(createLight<SmallLight<RedLight>>(mm2px(Vec(outNegX + 10.0f, out1Y + inYSpacing*i)), module, 4*i + 1));
            addChild(createLight<SmallLight<GreenLight>>(mm2px(Vec(outPosX - 4.0f, light2Y + inYSpacing*i)), module, 4*i + 2));
            addChild(createLight<SmallLight<RedLight>>(mm2px(Vec(outNegX + 10.0f, light2Y + inYSpacing*i)), module, 4*i + 3));
        }
        

	}
};


Model* modelLetsSplosh = createModel<LetsSplosh, LetsSploshWidget>("LetsSplosh");