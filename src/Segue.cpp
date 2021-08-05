#include "NLC.hpp"

//HetrickCV's FlipPan was essentially an emulation of this module, 
//so this is a copy-paste of that, minus the linear/equal-pan switch

struct Segue : Module
{
	enum ParamIds
	{
		AMOUNT_PARAM,
        SCALE_PARAM,
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
		configParam(Segue::AMOUNT_PARAM, 0.0, 5.0, 2.5, "");
		configParam(Segue::SCALE_PARAM, -1.0, 1.0, 1.0, "");
	}

    void process(const ProcessArgs &args) override;


	// For more advanced Module features, read Rack's engine.hpp header file
	// - dataToJson, dataFromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - reset, randomize: implements special behavior when user clicks these from the context menu
};


void Segue::process(const ProcessArgs &args)
{
	float inL = inputs[LEFT_INPUT].getVoltage();
	float inR = inputs[RIGHT_INPUT].getVoltage();

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
	addParam(createParam<Davies1900hBlackKnob>(Vec(43, 116), module, Segue::AMOUNT_PARAM));
    addParam(createParam<Trimpot>(Vec(48, 177), module, Segue::SCALE_PARAM));

	//////INPUTS//////
    addInput(createInput<PJ301MPort>(Vec(14, 238), module, Segue::LEFT_INPUT));
    addInput(createInput<PJ301MPort>(Vec(78, 238), module, Segue::RIGHT_INPUT));
    addInput(createInput<PJ301MPort>(Vec(46, 212), module, Segue::AMOUNT_INPUT));

	//////OUTPUTS//////
    addOutput(createOutput<PJ301MPort>(Vec(29, 291), module, Segue::LEFT_OUTPUT));
    addOutput(createOutput<PJ301MPort>(Vec(63, 291), module, Segue::RIGHT_OUTPUT));
}

Model *modelSegue = createModel<Segue, SegueWidget>("Segue");
