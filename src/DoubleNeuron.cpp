#include "NLC.hpp"


struct DoubleNeuron : Module {
	enum ParamIds {
		NEURON1SENSE_PARAM,
		NEURON1RESPONSE_PARAM,
		NEURON2SENSE_PARAM,
		NEURON2RESPONSE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NEURON1IN1_INPUT,
		NEURON1IN2_INPUT,
		NEURON1IN3_INPUT,

		NEURON2IN1_INPUT,
		NEURON2IN2_INPUT,
		NEURON2IN3_INPUT,

		DIFFRECT1NEG1_INPUT,
		DIFFRECT1NEG2_INPUT,
		DIFFRECT1POS1_INPUT,
		DIFFRECT1POS2_INPUT,

		DIFFRECT2NEG1_INPUT,
		DIFFRECT2NEG2_INPUT,
		DIFFRECT2POS1_INPUT,
		DIFFRECT2POS2_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		NEURON1_OUTPUT,
		NEURON2_OUTPUT,

		DIFFRECT1NEG_OUTPUT,
		DIFFRECT1POS_OUTPUT,
		DIFFRECT2NEG_OUTPUT,
		DIFFRECT2POS_OUTPUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	DoubleNeuron() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(DoubleNeuron::NEURON1SENSE_PARAM, 0.0, 5.0, 0.0, "Sense 1");
		configParam(DoubleNeuron::NEURON2SENSE_PARAM, 0.0, 5.0, 0.0, "Sense 2");

		configParam(DoubleNeuron::NEURON1RESPONSE_PARAM, 1.0, 10.0, 1.0, "Response 1");
		configParam(DoubleNeuron::NEURON2RESPONSE_PARAM, 1.0, 10.0, 1.0, "Response 2");

        configInput(NEURON1IN1_INPUT, "Neuron 1, 1");
        configInput(NEURON1IN2_INPUT, "Neuron 1, 2");
        configInput(NEURON1IN3_INPUT, "Neuron 1, 3");

        configInput(NEURON2IN1_INPUT, "Neuron 2, 1");
        configInput(NEURON2IN2_INPUT, "Neuron 2, 2");
        configInput(NEURON2IN3_INPUT, "Neuron 2, 3");

        configInput(DIFFRECT1NEG1_INPUT, "Diff-Rect 1, Negative 1");
        configInput(DIFFRECT1NEG2_INPUT, "Diff-Rect 1, Negative 2");
        configInput(DIFFRECT1POS1_INPUT, "Diff-Rect 1, Positive 1");
        configInput(DIFFRECT1POS2_INPUT, "Diff-Rect 1, Positive 2");

        configInput(DIFFRECT2NEG1_INPUT, "Diff-Rect 2, Negative 1");
        configInput(DIFFRECT2NEG2_INPUT, "Diff-Rect 2, Negative 2");
        configInput(DIFFRECT2POS1_INPUT, "Diff-Rect 2, Positive 1");
        configInput(DIFFRECT2POS2_INPUT, "Diff-Rect 2, Positive 2");

        configOutput(NEURON1_OUTPUT, "Neuron 1");
        configOutput(NEURON2_OUTPUT, "Neuron 2");

        configOutput(DIFFRECT1NEG_OUTPUT, "Diff-Rect 1, Negative");
        configOutput(DIFFRECT1POS_OUTPUT, "Diff-Rect 1, Positive");
        configOutput(DIFFRECT2NEG_OUTPUT, "Diff-Rect 2, Negative");
        configOutput(DIFFRECT2POS_OUTPUT, "Diff-Rect 2, Positive");
    }

	NLCNeuron neuron1, neuron2;
	NLCDiffRect diffrect1, diffrect2;

	void process(const ProcessArgs& args) override 
	{
		neuron1.setInput(inputs[NEURON1IN1_INPUT].getVoltage(), inputs[NEURON1IN2_INPUT].getVoltage(), inputs[NEURON1IN3_INPUT].getVoltage());
		neuron2.setInput(inputs[NEURON2IN1_INPUT].getVoltage(), inputs[NEURON2IN2_INPUT].getVoltage(), inputs[NEURON2IN3_INPUT].getVoltage());
	
		neuron1.setSense(params[NEURON1SENSE_PARAM].getValue());
		neuron2.setSense(params[NEURON2SENSE_PARAM].getValue());

		neuron1.setResponse(params[NEURON1RESPONSE_PARAM].getValue());
		neuron2.setResponse(params[NEURON2RESPONSE_PARAM].getValue());

		diffrect1.setPositiveInputs(inputs[DIFFRECT1POS1_INPUT].getVoltage(), inputs[DIFFRECT1POS2_INPUT].getVoltage());
		diffrect1.setNegativeInputs(inputs[DIFFRECT1NEG1_INPUT].getVoltage(), inputs[DIFFRECT1NEG2_INPUT].getVoltage());

		diffrect2.setPositiveInputs(inputs[DIFFRECT2POS1_INPUT].getVoltage(), inputs[DIFFRECT2POS2_INPUT].getVoltage());
		diffrect2.setNegativeInputs(inputs[DIFFRECT2NEG1_INPUT].getVoltage(), inputs[DIFFRECT2NEG2_INPUT].getVoltage());

		diffrect1.process();
		diffrect2.process();

		outputs[NEURON1_OUTPUT].setVoltage(neuron1.process());
		outputs[NEURON2_OUTPUT].setVoltage(neuron2.process());

		outputs[DIFFRECT1POS_OUTPUT].setVoltage(diffrect1.getPositiveOutput());
		outputs[DIFFRECT1NEG_OUTPUT].setVoltage(diffrect1.getNegativeOutput());
		outputs[DIFFRECT2POS_OUTPUT].setVoltage(diffrect2.getPositiveOutput());
		outputs[DIFFRECT2NEG_OUTPUT].setVoltage(diffrect2.getNegativeOutput());
	}
};


struct DoubleNeuronWidget : ModuleWidget {
	DoubleNeuronWidget(DoubleNeuron* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/DoubleNeuronRef.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		const float panelHeight = 128.5f;
		const float panelWidth = 5.08*12;

		const float neuron1x = 9.5f;
		const float neuron2x = panelWidth - 21.5f;
		const float senseHeight = 11.0f;
		const float responseHeight = 34.6f;

		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron1x, senseHeight)), module, DoubleNeuron::NEURON1SENSE_PARAM));
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron1x, responseHeight)), module, DoubleNeuron::NEURON1RESPONSE_PARAM));
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron2x, senseHeight)), module, DoubleNeuron::NEURON2SENSE_PARAM));
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron2x, responseHeight)), module, DoubleNeuron::NEURON2RESPONSE_PARAM));

		const float jackX1 = 4.33;
		const float jackX2 = 17.55;
		const float jackX3 = 35.0;
		const float jackX4 = 48.1;

		const float neuronRow1Height = panelHeight - 72.5f;
		const float neuronRow2Height = panelHeight - 60.5f;
		const float diffRectInNegHeight = panelHeight - 46.3f;
		const float diffRectInPosHeight = panelHeight - 33.0f;
		const float diffRectOutHeight = panelHeight - 21.5f;

		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX1, neuronRow1Height)), module, DoubleNeuron::NEURON1IN1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX2, neuronRow1Height)), module, DoubleNeuron::NEURON1IN2_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX3, neuronRow1Height)), module, DoubleNeuron::NEURON2IN2_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX4, neuronRow1Height)), module, DoubleNeuron::NEURON2IN1_INPUT));

		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX1, neuronRow2Height)), module, DoubleNeuron::NEURON1IN3_INPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX2, neuronRow2Height)), module, DoubleNeuron::NEURON1_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX3, neuronRow2Height)), module, DoubleNeuron::NEURON2_OUTPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX4, neuronRow2Height)), module, DoubleNeuron::NEURON2IN3_INPUT));
		
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX1, diffRectInNegHeight)), module, DoubleNeuron::DIFFRECT1NEG1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX2, diffRectInNegHeight)), module, DoubleNeuron::DIFFRECT1NEG2_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX3, diffRectInNegHeight)), module, DoubleNeuron::DIFFRECT2NEG1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX4, diffRectInNegHeight)), module, DoubleNeuron::DIFFRECT2NEG2_INPUT));

		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX1, diffRectInPosHeight)), module, DoubleNeuron::DIFFRECT1POS1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX2, diffRectInPosHeight)), module, DoubleNeuron::DIFFRECT1POS2_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX3, diffRectInPosHeight)), module, DoubleNeuron::DIFFRECT2POS1_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(jackX4, diffRectInPosHeight)), module, DoubleNeuron::DIFFRECT2POS2_INPUT));

		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX1, diffRectOutHeight)), module, DoubleNeuron::DIFFRECT1NEG_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX2, diffRectOutHeight)), module, DoubleNeuron::DIFFRECT1POS_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX3, diffRectOutHeight)), module, DoubleNeuron::DIFFRECT2POS_OUTPUT));
		addOutput(createOutput<PJ301MPort>(mm2px(Vec(jackX4, diffRectOutHeight)), module, DoubleNeuron::DIFFRECT2NEG_OUTPUT));
	}
};


Model* modelDoubleNeuron = createModel<DoubleNeuron, DoubleNeuronWidget>("DoubleNeuron");