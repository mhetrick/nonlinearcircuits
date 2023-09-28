#include "NLC.hpp"

//HetrickCV's FlipPan was essentially an emulation of this module, 
//so this is a copy-paste of that, minus the linear/equal-pan switch

struct Segue : Module
{
	enum ParamIds
	{
		AMOUNT_PARAM,
        SCALE_PARAM,
		GAIN1_PARAM,
		GAIN2_PARAM,
		NUM_PARAMS
	};
	enum InputIds
	{
        LEFT_INPUT,
        RIGHT_INPUT,
        AMOUNT_INPUT,
		NUM_INPUTS
	};
	enum OutputIds
	{
        LEFT_OUTPUT,
        RIGHT_OUTPUT,
		NUM_OUTPUTS
	};

	Segue()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(Segue::AMOUNT_PARAM, 0.0, 5.0, 2.5, "Initial Pan Position");
		configParam(Segue::SCALE_PARAM, -1.0, 1.0, 1.0, "Pan CV Depth");
		configParam(Segue::GAIN1_PARAM, 0.0, 1.0, 1.0, "Gain X");
		configParam(Segue::GAIN2_PARAM, 0.0, 1.0, 1.0, "Gain Y");

        configInput(LEFT_INPUT, "X");
        configInput(RIGHT_INPUT, "Y");
        configInput(AMOUNT_INPUT, "Pan CV");

        configOutput(LEFT_OUTPUT, "1");
        configOutput(RIGHT_OUTPUT, "2");
	}

    void process(const ProcessArgs &args) override;


	// For more advanced Module features, read Rack's engine.hpp header file
	// - dataToJson, dataFromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - reset, randomize: implements special behavior when user clicks these from the context menu
};


void Segue::process(const ProcessArgs &args)
{
	float inL = inputs[LEFT_INPUT].getVoltage() * params[GAIN1_PARAM].getValue();
	float inR = inputs[RIGHT_INPUT].getVoltage() * params[GAIN2_PARAM].getValue();

    float pan = params[AMOUNT_PARAM].getValue() + (inputs[AMOUNT_INPUT].getVoltage() * params[SCALE_PARAM].getValue());
    pan = clamp(pan, 0.0f, 5.0f) * 0.2f;

    outputs[LEFT_OUTPUT].setVoltage(LERP(pan, inR, inL));
    outputs[RIGHT_OUTPUT].setVoltage(LERP(pan, inL, inR));
}


struct SegueWidget : ModuleWidget { SegueWidget(Segue *module); };

SegueWidget::SegueWidget(Segue *module)
{
	setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		auto *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NLC - SEGUE.svg")));
		addChild(panel);
	}

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

	//////PARAMS//////
	addParam(createParam<Davies1900hWhiteKnob>(Vec(41, 116), module, Segue::AMOUNT_PARAM));
    addParam(createParam<Trimpot>(Vec(52, 177), module, Segue::SCALE_PARAM));
	addParam(createParam<Davies1900hWhiteKnob>(Vec(9, 53), module, Segue::GAIN1_PARAM));
	addParam(createParam<Davies1900hWhiteKnob>(Vec(74, 53), module, Segue::GAIN2_PARAM));

	//////INPUTS//////
    addInput(createInput<PJ301MPort>(Vec(14, 252), module, Segue::LEFT_INPUT));
    addInput(createInput<PJ301MPort>(Vec(81, 252), module, Segue::RIGHT_INPUT));
    addInput(createInput<PJ301MPort>(Vec(48, 222), module, Segue::AMOUNT_INPUT));

	//////OUTPUTS//////
    addOutput(createOutput<PJ301MPort>(Vec(31, 307), module, Segue::LEFT_OUTPUT));
    addOutput(createOutput<PJ301MPort>(Vec(66, 307), module, Segue::RIGHT_OUTPUT));
}

Model *modelSegue = createModel<Segue, SegueWidget>("Segue");
