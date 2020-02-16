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
	return !(_in1 != _in2);
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
		NUM_LIGHTS
	};

	BOOLs() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(BOOLs::SLEW_PARAM, 0.0, 1.0, 0.0, "Slew");
	}

	bool ins[4] = {false, false, false, false};
	bool outBools[4] = {false, false, false, false};
	float outs[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	float stepValue = 0.0f;

	const float gateScale = 5.0f;
	const float step1val = 0.0364f;
	const float step2val = 0.0729f;
	const float step3val = 0.1458f;
	const float step4val = 0.2915f;

	NLCTrigger clockIn;
	dsp::SlewLimiter slewLimiter;

	bool (*logicFunction1)(bool, bool) = &boolsOR;
	bool (*logicFunction2)(bool, bool) = &boolsOR;
	bool (*logicFunction3)(bool, bool) = &boolsOR;
	bool (*logicFunction4)(bool, bool) = &boolsOR;

	void acquireInputs()
	{
		for (int i = 0; i < 4; i++)
		{
			ins[i] = inputs[i].getVoltage() > 1.0f;
		}
	}

	void applyLogic()
	{
		outBools[0] = logicFunction1(ins[0], ins[1]);
		outBools[1] = logicFunction2(ins[1], ins[2]);
		outBools[2] = logicFunction3(ins[2], ins[3]);
		outBools[3] = logicFunction4(ins[3], ins[0]);
	}

	void setOutputValues()
	{
		for (int i = 0; i < 4; i++)
		{
			outs[i] = outBools[i] ? gateScale : 0.0f;
		}

		stepValue = 0.0f;
		stepValue += outs[0] * step1val;
		stepValue += outs[1] * step2val;
		stepValue += outs[2] * step3val;
		stepValue += outs[3] * step4val;
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

		const auto slewParam = params[SLEW_PARAM].getValue() * params[SLEW_PARAM].getValue();
		slewLimiter.setRiseFall(slewParam, slewParam);

		outputs[STEP_OUTPUT].setVoltage(stepValue);
		outputs[SLEW_OUTPUT].setVoltage(slewLimiter.process(1.0f, stepValue));

		for (int i = 0; i < 4; i++)
		{
			outputs[i].setVoltage(outs[i]);
		}
		
	}
};


struct BOOLsWidget : ModuleWidget {
	BOOLsWidget(BOOLs* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BOOLs.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		

		const float jackSpace =  12.65f;
		const float inJacks = 4.25f;
		const float outJacks = 17.3f;

		const float jack1 = 53.5f;
		const float jack2 = jack1 + jackSpace;
		const float jack3 = jack2 + jackSpace;
		const float jack4 = jack3 + jackSpace;

		addParam(createParam<Davies1900hBlackKnob>(mm2px(Vec(9.2f, 15.0f)), module, BOOLs::SLEW_PARAM));
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
	}
};


Model* modelBOOLs = createModel<BOOLs, BOOLsWidget>("BOOLs");