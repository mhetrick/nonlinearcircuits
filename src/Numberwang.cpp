#include "NLC.hpp"


struct Numberwang : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		SYNC_INPUT,
		XOOO_INPUT,
		OXOO_INPUT,
		OOXO_INPUT,
        OOOX_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {

		NUM_OUTPUTS = 16
	};
	enum LightIds {
		NUM_LIGHTS = NUM_OUTPUTS
	};

	Numberwang() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

    NLCTrigger clockIn;
    float outs[16];

    void numberwangProcess()
    {
        int activeOutput = 0;

        if(inputs[OOOX_INPUT].getVoltage() > 1.0f) activeOutput = activeOutput + 1;
        if(inputs[OOXO_INPUT].getVoltage() > 1.0f) activeOutput = activeOutput + 2;
        if(inputs[OXOO_INPUT].getVoltage() > 1.0f) activeOutput = activeOutput + 4;
        if(inputs[XOOO_INPUT].getVoltage() > 1.0f) activeOutput = activeOutput + 8;


        for (int i = 0; i < 16; i++)
        {   
            outs[i] = (i == activeOutput) ? 5.0f : 0.0f;
        }
    }

	void process(const ProcessArgs& args) override 
	{
        if (inputs[SYNC_INPUT].isConnected())
        {
            if (clockIn.process(inputs[SYNC_INPUT].getVoltage()))
            {
                numberwangProcess();
            }
        }
        else
        {
            numberwangProcess();
        }

        for (int i = 0; i < 16; i++)
        {   
            outputs[i].setVoltage(outs[i]);
            lights[i].setBrightnessSmooth(outs[i] * 0.2);
        }
	}
};


struct NumberwangWidget : ModuleWidget {
	NumberwangWidget(Numberwang* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NLC - NUMBERWANG.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		//const float panelHeight = 128.5f;
		//const float panelWidth = 5.08*12;

        const float jack1X = 13.0f;
        const float jack2X = 28.50f;
        const float jack3X = 44.0f;
        const float jack4X = 60.0f;

        const float inY = 20.75f;
        const float ledYSpace = 10.75f;

        addInput(createInput<PJ301MPort>(mm2px(Vec(60.25f, inY)), module, Numberwang::SYNC_INPUT));

        for (int i = 0; i < 4; i++)
        {
            float inX = 12.75f +(i * 10.75f);
            addInput(createInput<PJ301MPort>(mm2px(Vec(inX, inY)), module, Numberwang::XOOO_INPUT + i));
        }

        float outsY[4] = {40.0f, 61.25f, 81.25f, 102.5f};

        for (int i = 0; i < 4; i++)
        {
            float outY = outsY[i];

            addOutput(createOutput<PJ301MPort>(mm2px(Vec(jack1X, outY)), module, 4*i));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(jack2X, outY)), module, 4*i + 1));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(jack3X, outY)), module, 4*i + 2));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(jack4X, outY)), module, 4*i + 3));

            addChild(createLight<SmallLight<BlueLight>>(mm2px(Vec(jack1X + 3.2f, outY + ledYSpace)), module, 4*i));
            addChild(createLight<SmallLight<BlueLight>>(mm2px(Vec(jack2X + 3.2f, outY + ledYSpace)), module, 4*i + 1));
            addChild(createLight<SmallLight<BlueLight>>(mm2px(Vec(jack3X + 3.2f, outY + ledYSpace)), module, 4*i + 2));
            addChild(createLight<SmallLight<BlueLight>>(mm2px(Vec(jack4X + 3.2f, outY + ledYSpace)), module, 4*i + 3));
        }

        
	}
};


Model* modelNumberwang = createModel<Numberwang, NumberwangWidget>("Numberwang");