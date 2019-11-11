#include "NLC.hpp"


struct SquidAxon : Module {
	enum ParamIds {
		NLFEEDBACK_PARAM,
		IN3FEEDBACK_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,

		CLOCK_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
        OUT2_OUTPUT,
        OUT3_OUTPUT,
        OUT4_OUTPUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	SquidAxon() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(SquidAxon::NLFEEDBACK_PARAM, 0.0, 4.0, 0.0, "Nonlinear Feedback");
		configParam(SquidAxon::IN3FEEDBACK_PARAM, 0.0, 1.0, 0.0, "In 3 + Feedback");
	}

	int stage = 0;
	float outs[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	float linearFeedback = 0.0f;
	float nonlinearFeedback = 0.0f;
	dsp::SchmittTrigger clockIn;

	float squidDiode(const float _input)
	{
		static float diodeScalar = 0.0432477f * 28.0f * 10.0f;
		float sign = _input > 0.0f ? 1.0f : -1.0f;

		const float diodeIn = std::abs(_input * 0.1f) - 0.667;
		const float diodeStage2 = diodeIn + std::abs(diodeIn);
		const float diodeStage3 = diodeStage2 * diodeStage2 * sign;

		return diodeStage3 * diodeScalar;
	}

	void process(const ProcessArgs& args) override 
	{
		if (clockIn.process(inputs[CLOCK_INPUT].getVoltage()))
		{
			bool stage3Connected = inputs[IN3_INPUT].isConnected();
			if (stage == 0)
			{
				float mixIn = inputs[IN1_INPUT].getVoltage() + inputs[IN2_INPUT].getVoltage();
				if (stage3Connected)
				{
					mixIn += inputs[IN3_INPUT].getVoltage() * params[IN3FEEDBACK_PARAM].getValue();
				}
				else
				{
					mixIn += outs[3] * params[IN3FEEDBACK_PARAM].getValue();
				}

				//add nonlinear feedback
				nonlinearFeedback = outs[3] * params[NLFEEDBACK_PARAM].getValue();
				nonlinearFeedback = squidDiode(nonlinearFeedback);
				nonlinearFeedback = rack::math::clamp(nonlinearFeedback, -9.0f, 9.0f);
				nonlinearFeedback *= -0.7f;

				mixIn += nonlinearFeedback;

				outs[0] = rack::math::clamp(mixIn, -10.0f, 10.0f);
			}
			else
			{
				outs[stage] = outs[stage - 1];
			}

			stage = (stage + 1) % 4;
		}

		for (int i = 0; i < 4; i++)
		{
			outputs[i].setVoltage(outs[i]);
		}
		
	}
};


struct SquidAxonWidget : ModuleWidget {
	SquidAxonWidget(SquidAxon* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/AxonClassic.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		//const float panelHeight = 128.5f;
		//const float panelWidth = 5.08*8;

		const float knobHeight = 15.0f;

		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(5.0f, knobHeight)), module, SquidAxon::NLFEEDBACK_PARAM));
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(20.0f, knobHeight)), module, SquidAxon::IN3FEEDBACK_PARAM));

		const float jackSpace =  9.0f;
		const float jackX1 = 3.0f;
		const float jackX2 = jackX1 + jackSpace;
		const float jackX3 = jackX2 + jackSpace;
		const float jackX4 = jackX3 + jackSpace;

		const float inputHeight = 70.0f;
		const float outputHeight = inputHeight + 15;

		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX1, inputHeight)), module, SquidAxon::CLOCK_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX2, inputHeight)), module, SquidAxon::IN1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX3, inputHeight)), module, SquidAxon::IN2_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX4, inputHeight)), module, SquidAxon::IN3_INPUT));

		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX1, outputHeight)), module, SquidAxon::OUT1_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX2, outputHeight)), module, SquidAxon::OUT2_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX3, outputHeight)), module, SquidAxon::OUT3_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX4, outputHeight)), module, SquidAxon::OUT4_OUTPUT));
	}
};


Model* modelSquidAxon = createModel<SquidAxon, SquidAxonWidget>("SquidAxon");