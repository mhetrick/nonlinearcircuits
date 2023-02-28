#include "NLC.hpp"


struct Statues : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		IN_INPUT,
		XOO_INPUT,
		OXO_INPUT,
		OOX_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {

		NUM_OUTPUTS = 8
	};
	enum LightIds {
		NUM_LIGHTS = NUM_OUTPUTS*2
	};

    float outs[8]{};

	Statues() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        for (int i = 0; i < 8; i++)
        {
            outs[i] = 0.0f;
        }
	}

    

	void process(const ProcessArgs& args) override 
	{
        float input = inputs[IN_INPUT].getVoltage();

        int activeInput = 0;

        if(inputs[OOX_INPUT].getVoltage() > 1.0f) activeInput = activeInput + 1;
        if(inputs[OXO_INPUT].getVoltage() > 1.0f) activeInput = activeInput + 2;
        if(inputs[XOO_INPUT].getVoltage() > 1.0f) activeInput = activeInput + 4;

        outs[activeInput] = input;

        for (int i = 0; i < 8; i++)
        {
            outputs[i].setVoltage(outs[i]);
            lights[i*2].setSmoothBrightness(outs[i] * 0.2f, args.sampleTime);
            lights[1 + i*2].setSmoothBrightness(outs[i] * -0.2f, args.sampleTime);
        }
	}
};


struct StatuesWidget : ModuleWidget {
	StatuesWidget(Statues* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NLC - STATUES.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		//const float panelHeight = 128.5f;
		//const float panelWidth = 5.08*12;

        const float inX = 16.25f;
        const float inY = 26.0f;

        const float binY = 45.75f;

        const float outStartX = 2;
        const float out1Y = 70.25f;
        const float out2Y = 91.5f;

        const float ledYSpace = 12.25f;
        const float led1Y = out1Y + ledYSpace;
        const float led2Y = out2Y + ledYSpace;

        addInput(createInput<PJ301MPort>(mm2px(Vec(inX, inY)), module, Statues::IN_INPUT));

        addInput(createInput<PJ301MPort>(mm2px(Vec(4.75f, binY)), module, Statues::XOO_INPUT));
        addInput(createInput<PJ301MPort>(mm2px(Vec(inX, binY)), module, Statues::OXO_INPUT));
        addInput(createInput<PJ301MPort>(mm2px(Vec(27.75f, binY)), module, Statues::OOX_INPUT));

        for (int i = 0; i < 4; i++)
        {
            float jackX = outStartX + i*(9.5f);
            float ledX = jackX + 3.2f;

            addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX, out1Y)), module, i));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX, out2Y)), module, i + 4));

            addChild(createLight<SmallLight<GreenRedLight>>(mm2px(Vec(ledX, led1Y)), module, i*2));
            addChild(createLight<SmallLight<GreenRedLight>>(mm2px(Vec(ledX, led2Y)), module, 8 + i*2));
        }

        
	}
};


Model* modelStatues = createModel<Statues, StatuesWidget>("Statues");