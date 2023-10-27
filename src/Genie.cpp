#include "NLC.hpp"


struct Genie : Module {
	enum ParamIds {
		NEURON1SENSE_PARAM,
		NEURON1RESPONSE_PARAM,
        NEURON1IN_PARAM,
		NEURON2SENSE_PARAM,
		NEURON2RESPONSE_PARAM,
        NEURON2IN_PARAM,
        NEURON3SENSE_PARAM,
        NEURON3RESPONSE_PARAM,
        NEURON3IN_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NEURON1IN1_INPUT,
		NEURON12IN1_INPUT,
		NEURON2IN1_INPUT,
		NEURON23IN1_INPUT,
		NEURON3IN1_INPUT,

        NEURON1IN2_INPUT,
		NEURON12IN2_INPUT,
		NEURON2IN2_INPUT,
		NEURON23IN2_INPUT,
		NEURON3IN2_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		NEURON1_OUTPUT,
        DIFFRECT_NEG_OUTPUT,
		NEURON2_OUTPUT,
        DIFFRECT_POS_OUTPUT,
        NEURON3_OUTPUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
        NEURON1_LIGHT,
        NEURON2_LIGHT,
        NEURON3_LIGHT,
		NUM_LIGHTS
	};

	Genie() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(Genie::NEURON1SENSE_PARAM, 0.0, 5.0, 0.0, "Sense 1");
		configParam(Genie::NEURON2SENSE_PARAM, 0.0, 5.0, 0.0, "Sense 2");
        configParam(Genie::NEURON3SENSE_PARAM, 0.0, 5.0, 0.0, "Sense 3");

		configParam(Genie::NEURON1RESPONSE_PARAM, 1.0, 10.0, 1.0, "Response 1");
		configParam(Genie::NEURON2RESPONSE_PARAM, 1.0, 10.0, 1.0, "Response 2");
        configParam(Genie::NEURON3RESPONSE_PARAM, 1.0, 10.0, 1.0, "Response 3");

        configParam(Genie::NEURON1IN_PARAM, 0.0, 1.0, 0.0, "In Gain 1");
        configParam(Genie::NEURON2IN_PARAM, 0.0, 1.0, 0.0, "In Gain 2");
        configParam(Genie::NEURON3IN_PARAM, 0.0, 1.0, 0.0, "In Gain 3");

        configInput(NEURON1IN1_INPUT, "Neuron 1, 1");
        configInput(NEURON1IN2_INPUT, "Neuron 1, 2");

        configInput(NEURON2IN1_INPUT, "Neuron 2, 1");
        configInput(NEURON2IN2_INPUT, "Neuron 2, 2");

        configInput(NEURON3IN1_INPUT, "Neuron 3, 1");
        configInput(NEURON3IN2_INPUT, "Neuron 3, 2");

        configInput(NEURON12IN1_INPUT, "Neurons 1 & 2, 1");
        configInput(NEURON12IN2_INPUT, "Neurons 1 & 2, 2");

        configInput(NEURON23IN1_INPUT, "Neurons 3 & 4, 1");
        configInput(NEURON23IN2_INPUT, "Neurons 3 & 4, 2");

        configOutput(NEURON1_OUTPUT, "Neuron 1");
        configOutput(NEURON2_OUTPUT, "Neuron 2");
        configOutput(NEURON3_OUTPUT, "Neuron 3");

        configOutput(DIFFRECT_NEG_OUTPUT, "Diff-Rect, Negative");
        configOutput(DIFFRECT_POS_OUTPUT, "Diff-Rect, Positive");
    }

	NLCNeuron neuron1, neuron2, neuron3;
	NLCDiffRect diffrect;

    float neuron1Out = 0.0f, neuron2Out = 0.0f, neuron3Out = 0.0f;

	void process(const ProcessArgs& args) override 
	{
        const float inGain1 = params[NEURON1IN_PARAM].getValue();
        const float inGain2 = params[NEURON2IN_PARAM].getValue();
        const float inGain3 = params[NEURON3IN_PARAM].getValue();

        float neuron1Input = 0.0f;
        if (inputs[NEURON1IN1_INPUT].isConnected())
        {
            neuron1Input = inputs[NEURON1IN1_INPUT].getVoltage() * inGain1;
        }
        else
        {
            neuron1Input = neuron3Out * inGain1;
        }

        float neuron2Input = 0.0f;
        if (inputs[NEURON2IN1_INPUT].isConnected())
        {
            neuron2Input = inputs[NEURON2IN1_INPUT].getVoltage() * inGain2;
        }
        else
        {
            neuron2Input = neuron1Out * inGain2;
        }

        float neuron3Input = 0.0f;
        if (inputs[NEURON3IN1_INPUT].isConnected())
        {
            neuron3Input = inputs[NEURON3IN1_INPUT].getVoltage() * inGain3;
        }
        else
        {
            neuron3Input = neuron2Out * inGain3;
        }

        const float neuron1Inputs = neuron1Input + inputs[NEURON1IN2_INPUT].getVoltage();
        const float neuron2Inputs = neuron2Input + inputs[NEURON2IN2_INPUT].getVoltage();
        const float neuron3Inputs = neuron3Input + inputs[NEURON3IN2_INPUT].getVoltage();

        const float neuron12Inputs = inputs[NEURON12IN1_INPUT].getVoltage() + inputs[NEURON12IN2_INPUT].getVoltage();
        const float neuron23Inputs = inputs[NEURON23IN1_INPUT].getVoltage() + inputs[NEURON23IN2_INPUT].getVoltage();

		neuron1.setInput(neuron1Inputs + neuron12Inputs);
		neuron2.setInput(neuron2Inputs + neuron12Inputs + neuron23Inputs);
        neuron3.setInput(neuron3Inputs + neuron23Inputs);
	
		neuron1.setSense(params[NEURON1SENSE_PARAM].getValue());
		neuron2.setSense(params[NEURON2SENSE_PARAM].getValue());
        neuron3.setSense(params[NEURON3SENSE_PARAM].getValue());

		neuron1.setResponse(params[NEURON1RESPONSE_PARAM].getValue());
		neuron2.setResponse(params[NEURON2RESPONSE_PARAM].getValue());
        neuron3.setResponse(params[NEURON3RESPONSE_PARAM].getValue());

        neuron1Out = neuron1.process();
        neuron2Out = neuron2.process();
        neuron3Out = neuron3.process();

		diffrect.setPositiveInputs(neuron1Out, neuron3Out);
		diffrect.setNegativeInput(neuron2Out);

		diffrect.process();

		outputs[NEURON1_OUTPUT].setVoltage(neuron1Out);
		outputs[NEURON2_OUTPUT].setVoltage(neuron2Out);
        outputs[NEURON3_OUTPUT].setVoltage(neuron3Out);

		outputs[DIFFRECT_POS_OUTPUT].setVoltage(diffrect.getPositiveOutput());
		outputs[DIFFRECT_NEG_OUTPUT].setVoltage(diffrect.getNegativeOutput());

        lights[NEURON1_LIGHT].setSmoothBrightness(neuron1Out, args.sampleTime);
        lights[NEURON2_LIGHT].setSmoothBrightness(neuron2Out, args.sampleTime);
        lights[NEURON3_LIGHT].setSmoothBrightness(neuron3Out, args.sampleTime);
	}
};


struct GenieWidget : ModuleWidget {
	GenieWidget(Genie* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/GENiE.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        const float senseY = 26.0f;
        const float reactY = 42.0f;
        const float inParamY = 58.0f;
        const float neuron1X = 24.0f;
        const float neuron2X = 40.0f;
        const float neuron3X = 56.0f;

        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron1X, senseY)), module, Genie::NEURON1SENSE_PARAM));
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron1X, reactY)), module, Genie::NEURON1RESPONSE_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron1X, inParamY)), module, Genie::NEURON1IN_PARAM));
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron2X, senseY)), module, Genie::NEURON2SENSE_PARAM));
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron2X, reactY)), module, Genie::NEURON2RESPONSE_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron2X, inParamY)), module, Genie::NEURON2IN_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron3X, senseY)), module, Genie::NEURON3SENSE_PARAM));
		addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron3X, reactY)), module, Genie::NEURON3RESPONSE_PARAM));
        addParam(createParam<Davies1900hWhiteKnob>(mm2px(Vec(neuron3X, inParamY)), module, Genie::NEURON3IN_PARAM));


        const float jackX1 = 10.25f;
		const float jackSpacing = 15.4f;

        const float inRow1Y = 77.0f;
        const float inRow2Y = 94.5f;
        const float outRowY = 109.5f;

        for (int i = 0; i < 5; i++)
        {
            const float thisX = jackX1 + (i * jackSpacing);
            addInput(createInput<PJ301MPort>(mm2px(Vec(thisX, inRow1Y)), module, i));
            addInput(createInput<PJ301MPort>(mm2px(Vec(thisX, inRow2Y)), module, i + 5));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(thisX, outRowY)), module, i));
        }

        const float lightX = 28.5f;
        const float lightY = 18.0f;

        for (int i = 0; i < 3; i++)
        {
            addChild(createLight<SmallLight<BlueLight>>(mm2px(Vec(lightX + 16.0f*i, lightY)), module, i));
        }
        
	}
};


Model* modelGenie = createModel<Genie, GenieWidget>("GENiE");