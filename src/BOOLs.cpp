#include "NLC.hpp"

bool boolsOR(bool _in1, bool _in2)
{
	return _in1 || _in2;
}

bool boolsAND(bool _in1, bool _in2)
{
	return _in1 && _in2;
}

bool boolsNOR(bool _in1, bool _in2)
{
	return !(_in1 || _in2);
}

bool boolsNAND(bool _in1, bool _in2)
{
	return !(_in1 && _in2);
}

bool boolsXOR(bool _in1, bool _in2)
{
	return _in1 != _in2;
}

bool boolsXNOR(bool _in1, bool _in2)
{
	return _in1 == _in2;
}

struct BOOLs : Module {
	enum ParamIds {
		SLEW_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		SAMPLE_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
        OUT2_OUTPUT,
        OUT3_OUTPUT,
        OUT4_OUTPUT,

		SLEW_OUTPUT,
		STEP_OUTPUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
		OUT1_LIGHT,
		OUT2_LIGHT,
		OUT3_LIGHT,
		OUT4_LIGHT,
		NUM_LIGHTS
	};

	enum LogicMode
	{
		BOOLS_OR,
		BOOLS_AND,
		BOOLS_XOR,
		BOOLS_NOR,
		BOOLS_NAND,
		BOOLS_XNOR
	};

	BOOLs() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(BOOLs::SLEW_PARAM, 0.0, 1.0, 0.0, "Slew");

        configInput(IN1_INPUT, "1");
        configInput(IN2_INPUT, "2");
        configInput(IN3_INPUT, "3");
        configInput(IN4_INPUT, "4");
        configInput(SAMPLE_INPUT, "Sample");

        configOutput(OUT1_OUTPUT, "1");
        configOutput(OUT2_OUTPUT, "2");
        configOutput(OUT3_OUTPUT, "3");
        configOutput(OUT4_OUTPUT, "4");

        configOutput(SLEW_OUTPUT, "Slew");
        configOutput(STEP_OUTPUT, "Step");
	}

	bool ins[4] = {false, false, false, false};
	bool outBools[4] = {false, false, false, false};
	float outs[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	float stepValue = 0.0f;
	float lastSlew = -1.0f;

	const float gateScale = 5.0f;
	const float gateUnscale = 1.0f/gateScale;
	const float stepScale = 3.0f;
	const float step1val = 0.0364f * stepScale;
	const float step2val = 0.0729f * stepScale;
	const float step3val = 0.1458f * stepScale;
	const float step4val = 0.2915f * stepScale;

	NLCTrigger clockIn;
	dsp::SlewLimiter slewLimiter;

	LogicMode currentMode = BOOLS_XOR;
	bool (*logicFunction)(bool, bool) = &boolsXOR;

	//TODO: Maybe add ability to set different modes for each channel...
	//bool (*logicFunction1)(bool, bool) = &boolsXOR;
	//bool (*logicFunction2)(bool, bool) = &boolsXOR;
	//bool (*logicFunction3)(bool, bool) = &boolsXOR;
	//bool (*logicFunction4)(bool, bool) = &boolsXOR;

	json_t *dataToJson() override 
    {
		json_t *rootJ = json_object();
        json_object_set_new(rootJ, "logicmode", json_integer(currentMode));
		return rootJ;
	}
    void dataFromJson(json_t *rootJ) override 
    {
		json_t *modeJ = json_object_get(rootJ, "logicmode");
		if (modeJ)
		{
			setLogicMode(json_integer_value(modeJ));
		}
	}

	LogicMode getLogicMode() const
	{
		return currentMode;
	}

	void setLogicMode(int _mode)
	{
		currentMode = (LogicMode)_mode;
		switch(_mode)
		{
			case BOOLS_OR: logicFunction = &boolsOR;
			break;

			case BOOLS_AND: logicFunction = &boolsAND;
			break;

			case BOOLS_XOR: logicFunction = &boolsXOR;
			break;

			case BOOLS_NOR: logicFunction = &boolsNOR;
			break;

			case BOOLS_NAND: logicFunction = &boolsNAND;
			break;

			case BOOLS_XNOR: logicFunction = &boolsXNOR;
			break;

			default: logicFunction = &boolsXOR;
			break;
		}
	}

	void acquireInputs()
	{
		for (int i = 0; i < 4; i++)
		{
			ins[i] = inputs[i].getVoltage() > 1.0f;
		}
	}

	void applyLogic()
	{
		outBools[0] = logicFunction(ins[0], ins[1]);
		outBools[1] = logicFunction(ins[1], ins[2]);
		outBools[2] = logicFunction(ins[2], ins[3]);
		outBools[3] = logicFunction(ins[3], ins[0]);
	}

	void setOutputValues()
	{
		for (int i = 0; i < 4; i++)
		{
			outs[i] = outBools[i] ? gateScale : 0.0f;
		}

		stepValue = 0.0f;
		stepValue += (outs[0] * step1val);
		stepValue += (outs[1] * step2val);
		stepValue += (outs[2] * step3val);
		stepValue += (outs[3] * step4val);
	}

	void fullBOOLsProcess()
	{
		acquireInputs();
		applyLogic();
		setOutputValues();
	}

	void process(const ProcessArgs& args) override 
	{
		if(inputs[SAMPLE_INPUT].isConnected())
		{
			if (clockIn.process(inputs[SAMPLE_INPUT].getVoltage()))
			{
				fullBOOLsProcess();
			}
		}
		else fullBOOLsProcess();

		const auto slewParam = params[SLEW_PARAM].getValue();
		if (lastSlew != slewParam)
		{
			lastSlew = slewParam;
			const float scaledSlew = slewParam > 0.001f ? std::pow(slewParam, 0.25f) : 0.0f;
			const auto slewHz = (1.0f - scaledSlew) * (18000.0f) + 1.0f;
			slewLimiter.setRiseFall(slewHz, slewHz);
		}

		outputs[STEP_OUTPUT].setVoltage(stepValue);
		outputs[SLEW_OUTPUT].setVoltage(slewLimiter.process(args.sampleTime, stepValue));

		for (int i = 0; i < 4; i++)
		{
			outputs[i].setVoltage(outs[i]);
			lights[i].setSmoothBrightness(outs[i] * gateUnscale, args.sampleTime);
		}
		
	}
};


struct BOOLsWidget : ModuleWidget {
	BOOLsWidget(BOOLs* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BOOLs.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		const float jackSpace =  12.68f;
		const float inJacks = 4.25f;
		const float outJacks = 17.3f;

		const float jack1 = 53.5f;
		const float jack2 = jack1 + jackSpace;
		const float jack3 = jack2 + jackSpace;
		const float jack4 = jack3 + jackSpace;

		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(9.2f, 15.0f)), module, BOOLs::SLEW_PARAM));
		addInput(createInput<PJ301MPort>(mm2px(Vec(10.7f, 37.0f)), module, BOOLs::SAMPLE_INPUT));

		addInput(createInput<PJ301MPort>(mm2px(Vec(inJacks, jack1)), module, BOOLs::IN1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(inJacks, jack2)), module, BOOLs::IN2_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(inJacks, jack3)), module, BOOLs::IN3_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(inJacks, jack4)), module, BOOLs::IN4_INPUT));

		addOutput(createOutput<PJ301MPort>(mm2px(Vec(outJacks, jack1)), module, BOOLs::OUT1_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(outJacks, jack2)), module, BOOLs::OUT2_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(outJacks, jack3)), module, BOOLs::OUT3_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(outJacks, jack4)), module, BOOLs::OUT4_OUTPUT));

		const float bottomJacksHeight = 107.0f;
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(inJacks, bottomJacksHeight)), module, BOOLs::SLEW_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(outJacks, bottomJacksHeight)), module, BOOLs::STEP_OUTPUT));

		////////LIGHTS/////////
		const float lightX = outJacks + 9.5f;
		const float lightOffset = -0.10f;
		addChild(createLight<SmallLight<GreenLight>>(mm2px(Vec(lightX, jack1 + lightOffset)), module, BOOLs::OUT1_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(mm2px(Vec(lightX, jack2 + lightOffset)), module, BOOLs::OUT2_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(mm2px(Vec(lightX, jack3 + lightOffset)), module, BOOLs::OUT3_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(mm2px(Vec(lightX, jack4 + lightOffset)), module, BOOLs::OUT4_LIGHT));
	}

	void appendContextMenu(Menu *menu) override 
	{
		BOOLs *bools = dynamic_cast<BOOLs*>(module);
		assert(bools);

		struct BOOLsLogicModeMenuItem : MenuItem 
		{
			BOOLs *bools;
			BOOLs::LogicMode mode;
			void onAction(const event::Action &e) override
			{
				bools->setLogicMode(mode);
			}
			void step() override {
				rightText = (bools->getLogicMode() == mode) ? "✔" : "";
				MenuItem::step();
			}
		};

		menu->addChild(construct<MenuLabel>());
		menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Logic Mode"));
		menu->addChild(construct<BOOLsLogicModeMenuItem>(&MenuItem::text, "OR", &BOOLsLogicModeMenuItem::bools, bools, &BOOLsLogicModeMenuItem::mode, BOOLs::LogicMode::BOOLS_OR));
		menu->addChild(construct<BOOLsLogicModeMenuItem>(&MenuItem::text, "AND", &BOOLsLogicModeMenuItem::bools, bools, &BOOLsLogicModeMenuItem::mode, BOOLs::LogicMode::BOOLS_AND));
		menu->addChild(construct<BOOLsLogicModeMenuItem>(&MenuItem::text, "XOR", &BOOLsLogicModeMenuItem::bools, bools, &BOOLsLogicModeMenuItem::mode, BOOLs::LogicMode::BOOLS_XOR));
		menu->addChild(construct<BOOLsLogicModeMenuItem>(&MenuItem::text, "NOR", &BOOLsLogicModeMenuItem::bools, bools, &BOOLsLogicModeMenuItem::mode, BOOLs::LogicMode::BOOLS_NOR));
		menu->addChild(construct<BOOLsLogicModeMenuItem>(&MenuItem::text, "NAND", &BOOLsLogicModeMenuItem::bools, bools, &BOOLsLogicModeMenuItem::mode, BOOLs::LogicMode::BOOLS_NAND));
		menu->addChild(construct<BOOLsLogicModeMenuItem>(&MenuItem::text, "XNOR", &BOOLsLogicModeMenuItem::bools, bools, &BOOLsLogicModeMenuItem::mode, BOOLs::LogicMode::BOOLS_XNOR));
	}
};



Model* modelBOOLs = createModel<BOOLs, BOOLsWidget>("BOOLs");