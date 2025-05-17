#include "NLC.hpp"

struct BeatFreqs : Module 
{
	enum ParamIds 
    {
		NUM_PARAMS
	};
	enum InputIds 
    {
		LEFT_INPUT,
		RIGHT_INPUT,

		NUM_INPUTS
	};
	enum OutputIds 
    {
		ENUMS(GROUP1_OUT, 4),
        ENUMS(GROUP2_OUT, 4),
        ENUMS(GROUP3_OUT, 4),
        ENUMS(GROUP4_OUT, 4),
		
		NUM_OUTPUTS
	};
	enum LightIds 
    {
		ENUMS(GROUP1_LIGHT, 4),
        ENUMS(GROUP2_LIGHT, 4),
        ENUMS(GROUP3_LIGHT, 4),
        ENUMS(GROUP4_LIGHT, 4),
		NUM_LIGHTS
	};

	BeatFreqs() 
    {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configInput(LEFT_INPUT, "In 1");
        configInput(RIGHT_INPUT, "In 2");

        for (int i = 0; i < 4; i++)
        {
            configOutput(GROUP1_OUT + i, "Group 1:" + std::to_string(i + 1));
            configOutput(GROUP2_OUT + i, "Group 2:" + std::to_string(i + 1));
            configOutput(GROUP3_OUT + i, "Group 3:" + std::to_string(i + 1));
            configOutput(GROUP4_OUT + i, "Group 4:" + std::to_string(i + 1));
        }
        
        
        clockInLeft.doubleTrigger = true;
        clockInRight.doubleTrigger = true;
    }

	bool divBools[4] = {false, false, false, false};
	float steps[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    int counter = 0;

	NLCTrigger clockInLeft, clockInRight;
    
    const float G_HI = 5.0f;
    const float G_LO = 0.0f;
    
    std::vector<std::vector<float>> group1 = 
    {
        {G_HI, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO},
        {G_LO, G_LO, G_HI, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_LO, G_LO, G_LO, G_LO, G_LO},
        {G_LO, G_LO, G_LO, G_LO, G_HI, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_LO, G_LO, G_LO},
        {G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_LO}
    };
    std::vector<std::vector<float>> group2 = 
    {
        {G_HI, G_HI, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO},
        {G_LO, G_LO, G_LO, G_LO, G_HI, G_HI, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO},
        {G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_HI, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO},
        {G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_HI, G_LO, G_LO}
    };
    std::vector<std::vector<float>> group3 = 
    {
        {G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_LO, G_HI, G_LO, G_LO, G_LO, G_LO, G_LO},
        {G_HI, G_LO, G_HI, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO},
        {G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_LO, G_HI, G_LO, G_LO, G_LO, G_LO},
        {G_LO, G_HI, G_LO, G_HI, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO}
    };
    std::vector<std::vector<float>> group4 = 
    {
        {G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_LO, G_LO, G_LO, G_HI, G_LO, G_LO, G_LO},
        {G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_LO, G_LO, G_LO, G_HI, G_LO, G_LO},
        {G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_LO, G_LO, G_LO, G_HI, G_LO},
        {G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_LO, G_HI, G_LO, G_LO, G_LO, G_HI}
    };
    

	void process(const ProcessArgs& args) override 
	{
        if(clockInLeft.process(inputs[LEFT_INPUT].getVoltage()))
        {
            counter++;
            if (counter > 15)
                counter = 0;
        }
        if(clockInRight.process(inputs[RIGHT_INPUT].getVoltage()))
        {
            counter--;
            if (counter < 0)
                counter = 15;
        }
        
        // Update the outputs based on the current counter value
        for (int i = 0; i < 4; i++)
        {
            outputs[GROUP1_OUT + i].setVoltage(group1[i][counter]);
            outputs[GROUP2_OUT + i].setVoltage(group2[i][counter]);
            outputs[GROUP3_OUT + i].setVoltage(group3[i][counter]);
            outputs[GROUP4_OUT + i].setVoltage(group4[i][counter]);
            
            lights[GROUP1_LIGHT + i].setBrightness(group1[i][counter]);
            lights[GROUP2_LIGHT + i].setBrightness(group2[i][counter]);
            lights[GROUP3_LIGHT + i].setBrightness(group3[i][counter]);
            lights[GROUP4_LIGHT + i].setBrightness(group4[i][counter]);
        }
        
	}
};


struct BeatFreqsWidget : ModuleWidget 
{
	BeatFreqsWidget(BeatFreqs* module) 
    {
		setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/BeatFreqsLight.svg"), asset::plugin(pluginInstance, "res/BeatFreqs.svg")));


		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        //INPUTS
        const float inJacksX = 7.75f;
        const float inJacksY = 101.0f;
		addInput(createInput<PJ301MPort>(mm2px(Vec(inJacksX, inJacksY)), module, BeatFreqs::LEFT_INPUT));
		addInput(createInput<PJ301MPort>(mm2px(Vec(inJacksX + 17, inJacksY)), module, BeatFreqs::RIGHT_INPUT));

        //OUTPUTS AND LIGHTS
        const float outXLeft = 2.5f;
        const float outXRight = 30.25f;

        const float lightXLeft = 14.25f;
        const float lightXRight = 24.25f;
        
        const float groupYSpacing = 20.0f;
        
        for (int i = 0; i < 4; i++)
        {
            int groupIndex = BeatFreqs::GROUP1_OUT + i * 4;
            
            const float outYTop = 16.0f + i * groupYSpacing;
            const float outYBottom = 26.0f + i * groupYSpacing;
            const float lightYTop = 19.0f + i * groupYSpacing;
            const float lightYBottom = 29.0f + i * groupYSpacing;
            
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(outXRight, outYBottom)), module, groupIndex));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(outXRight, outYTop)), module, groupIndex + 1));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(outXLeft, outYTop)), module, groupIndex + 2));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(outXLeft, outYBottom)), module, groupIndex + 3));
            
            addChild(createLight<SmallLight<WhiteLight>>(mm2px(Vec(lightXRight, lightYBottom)), module, groupIndex));
            addChild(createLight<SmallLight<WhiteLight>>(mm2px(Vec(lightXRight, lightYTop)), module, groupIndex + 1));
            addChild(createLight<SmallLight<WhiteLight>>(mm2px(Vec(lightXLeft, lightYTop)), module, groupIndex + 2));
            addChild(createLight<SmallLight<WhiteLight>>(mm2px(Vec(lightXLeft, lightYBottom)), module, groupIndex + 3));
        }
        
	}
};



Model* modelBeatFreqs = createModel<BeatFreqs, BeatFreqsWidget>("BeatFreqs");