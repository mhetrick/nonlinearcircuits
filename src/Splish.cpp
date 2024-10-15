#include "NLC.hpp"


struct Splish : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		SPLISH1_INPUT,
		SPLISH2_INPUT,
		SPLISH3_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		ONE_POS_OUT,
        ONE_NEG_OUT,

        TWO_POS_OUT,
        TWO_NEG_OUT,

        THREE_POS_OUT,
        THREE_NEG_OUT,

        MIX_POS_OUT,
        MIX_NEG_OUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS = NUM_INPUTS
	};

	Splish()
    {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configInput(SPLISH1_INPUT, "Splish 1");
        configInput(SPLISH2_INPUT, "Splish 2");
        configInput(SPLISH3_INPUT, "Splish 3");
	}

    NLCDiffRect diffRects[4];

	void process(const ProcessArgs& args) override 
	{
        const float in1 = inputs[SPLISH1_INPUT].getVoltage();
        const float in2 = inputs[SPLISH2_INPUT].getVoltage();
        const float in3 = inputs[SPLISH3_INPUT].getVoltage();

        //1/23, 2/13, 3/12, 123;
        diffRects[0].setPositiveInputs(in1, 0.0f);
        diffRects[0].setNegativeInputs(in2, in3);

        diffRects[1].setPositiveInputs(in2, 0.0f);
        diffRects[1].setNegativeInputs(in1, in3);

        diffRects[2].setPositiveInputs(in3, 0.0f);
        diffRects[2].setNegativeInputs(in1, in2);

        diffRects[3].setPositiveInputs(in1, in2 + in3);
        diffRects[3].setNegativeInputs(0.0f, 0.0f);

        for (int i = 0; i < 4; i++)
        {
            diffRects[i].process();
            outputs[i*2].setVoltage(diffRects[i].getPositiveOutput());
            outputs[i*2 + 1].setVoltage(diffRects[i].getNegativeOutput());
        }

        lights[0].setBrightness(outputs[ONE_POS_OUT].getVoltage() * 0.2f);
        lights[1].setBrightness(outputs[TWO_POS_OUT].getVoltage() * 0.2f);
        lights[2].setBrightness(outputs[THREE_POS_OUT].getVoltage() * 0.2f);
	}
};

template <typename TBase = GrayModuleLightWidget>
struct TPurpleLight : TBase {
	TPurpleLight() {
		this->addBaseColor(SCHEME_PURPLE);
	}
};
using PurpleLight = TPurpleLight<>;

struct SplishWidget : ModuleWidget {
	SplishWidget(Splish* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Splish.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		//const float panelHeight = 128.5f;
		//const float panelWidth = 5.08*12;

        const float column1X = 0.75f;
        const float column2X = column1X + 10.0f;

        const float inStartY = 16.0f;
        const float inYSpacing = 17.0f;

        for (int i = 0; i < Splish::NUM_INPUTS; i++)
        {
            addInput(createInput<PJ301MPort>(mm2px(Vec(column1X, inStartY + inYSpacing*i)), module, i));
        }

        addOutput(createOutput<PJ301MPort>(mm2px(Vec(column1X, inStartY + inYSpacing*4)), module, Splish::ONE_POS_OUT));
        addOutput(createOutput<PJ301MPort>(mm2px(Vec(column1X, inStartY + inYSpacing*5)), module, Splish::ONE_NEG_OUT));

        for (int i = Splish::TWO_POS_OUT; i < Splish::NUM_OUTPUTS; i++)
        {
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(column2X, inStartY + inYSpacing*(i-2))), module, i));
        }

        const float lightX = 3.33f;
        const float lightY = inStartY + inYSpacing*3 + 2.6f;
        const float lightSpacing = 5.05f;

        addChild(createLight<MediumLight<RedLight>>(mm2px(Vec(lightX, lightY - lightSpacing)), module, 0));
        addChild(createLight<MediumLight<PurpleLight>>(mm2px(Vec(lightX, lightY)), module, 1));
        addChild(createLight<MediumLight<GreenLight>>(mm2px(Vec(lightX, lightY + lightSpacing)), module, 2));
	}
};

Model* modelSplish = createModel<Splish, SplishWidget>("Splish");