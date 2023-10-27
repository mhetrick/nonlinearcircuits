#include "NLC.hpp"


struct Neuron : Module {
	enum ParamIds {
		NEURON1SENSE_PARAM,
		NEURON1RESPONSE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NEURONIN1_INPUT,
		NEURONIN2_INPUT,
		NEURONIN3_INPUT,

		DIFFRECT1NEG1_INPUT,
		DIFFRECT1NEG2_INPUT,
		DIFFRECT1POS1_INPUT,
		DIFFRECT1POS2_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		NEURON1_OUTPUT,

		DIFFRECT1NEG_OUTPUT,
		DIFFRECT1POS_OUTPUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Neuron() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(Neuron::NEURON1SENSE_PARAM, 0.0, 5.0, 0.0, "Sense 1");

		configParam(Neuron::NEURON1RESPONSE_PARAM, 1.0, 10.0, 1.0, "Response 1");

        configInput(NEURONIN1_INPUT, "Neuron, 1");
        configInput(NEURONIN2_INPUT, "Neuron, 2");
        configInput(NEURONIN3_INPUT, "Neuron, 3");

        configInput(DIFFRECT1NEG1_INPUT, "Diff-Rect, Negative 1");
        configInput(DIFFRECT1NEG2_INPUT, "Diff-Rect, Negative 2");
        configInput(DIFFRECT1POS1_INPUT, "Diff-Rect, Positive 1");
        configInput(DIFFRECT1POS2_INPUT, "Diff-Rect, Positive 2");


        configOutput(NEURON1_OUTPUT, "Neuron");

        configOutput(DIFFRECT1NEG_OUTPUT, "Diff-Rect, Negative");
        configOutput(DIFFRECT1POS_OUTPUT, "Diff-Rect, Positive");
    }

	NLCNeuron neuron1, neuron2;
	NLCDiffRect diffrect1, diffrect2;

	void process(const ProcessArgs& args) override 
	{
		neuron1.setInput(inputs[NEURONIN1_INPUT].getVoltage(), inputs[NEURONIN2_INPUT].getVoltage(), inputs[NEURONIN3_INPUT].getVoltage());

		neuron1.setSense(params[NEURON1SENSE_PARAM].getValue());
		neuron1.setResponse(params[NEURON1RESPONSE_PARAM].getValue());

		diffrect1.setPositiveInputs(inputs[DIFFRECT1POS1_INPUT].getVoltage(), inputs[DIFFRECT1POS2_INPUT].getVoltage());
		diffrect1.setNegativeInputs(inputs[DIFFRECT1NEG1_INPUT].getVoltage(), inputs[DIFFRECT1NEG2_INPUT].getVoltage());

		diffrect1.process();
		diffrect2.process();

		outputs[NEURON1_OUTPUT].setVoltage(neuron1.process());

		outputs[DIFFRECT1POS_OUTPUT].setVoltage(diffrect1.getPositiveOutput());
		outputs[DIFFRECT1NEG_OUTPUT].setVoltage(diffrect1.getNegativeOutput());
	}
};


struct NeuronWidget : ModuleWidget {
	NeuronWidget(Neuron* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Neuron.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		const float knobHeight = 20.0f;

		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(5.0f, knobHeight)), module, Neuron::NEURON1SENSE_PARAM));
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(24.0f, knobHeight)), module, Neuron::NEURON1RESPONSE_PARAM));

		const float jackX1 = 4.75f;
		const float jackX2 = 16.25;
		const float jackX3 = 27.75;

		const float neuronInY = 49.75f;
		const float neuronOutY = 63.75f;

		const float diffRectIn1Y = 84.75f;
		const float diffRectIn2Y = 100.75f;

		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX1, neuronInY)), module, Neuron::NEURONIN1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX2, neuronInY)), module, Neuron::NEURONIN2_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX3, neuronInY)), module, Neuron::NEURONIN3_INPUT));

		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX2, neuronOutY)), module, Neuron::NEURON1_OUTPUT));
	
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX1, diffRectIn1Y)), module, Neuron::DIFFRECT1POS1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX2, diffRectIn1Y)), module, Neuron::DIFFRECT1POS2_INPUT));
        addInput(createInput<PJ301MPort>(mm2px(Vec(jackX1, diffRectIn2Y)), module, Neuron::DIFFRECT1NEG1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX2, diffRectIn2Y)), module, Neuron::DIFFRECT1NEG2_INPUT));

		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX3, diffRectIn1Y)), module, Neuron::DIFFRECT1POS_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX3, diffRectIn2Y)), module, Neuron::DIFFRECT1NEG_OUTPUT));
	}
};


Model* modelNeuron = createModel<Neuron, NeuronWidget>("Neuron");