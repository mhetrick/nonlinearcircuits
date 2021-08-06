#include "NLC.hpp"

struct Cipher : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		CLOCK_INPUT,
		DATA1_INPUT,
        DATA2_INPUT,
        STROBE_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
        OOO_OUTPUT,
        OOX_OUTPUT,
        OXO_OUTPUT,
        OXX_OUTPUT,
        XOO_OUTPUT,
        XOX_OUTPUT,
        XXO_OUTPUT,
        XXX_OUTPUT,

        SERIAL_OUTPUT,

        CV1_OUTPUT,
        CV2_OUTPUT,
        CV3_OUTPUT,
        CV4_OUTPUT,
		
		NUM_OUTPUTS
	};
	enum LightIds {
		OOO_LIGHT,
		OOX_LIGHT,
		OXO_LIGHT,
		OXX_LIGHT,
        XOO_LIGHT,
		XOX_LIGHT,
		XXO_LIGHT,
		XXX_LIGHT,

        SERIAL_LIGHT,
		NUM_LIGHTS
	};

	Cipher() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	bool bits[8] = {true, false, false, false, false, false, false, false};
    bool bitsOut[8] = {true, false, false, false, false, false, false, false};
    bool serialBit = false;
    float cv1Value, cv2Value, cv3Value, cv4Value;

	NLCTrigger clockIn;

	void process(const ProcessArgs& args) override 
	{
		if(clockIn.process(inputs[CLOCK_INPUT].getVoltage()))
		{
            bits[7] = bits[6];
            bits[6] = bits[5];
            bits[5] = bits[4];
            bits[4] = bits[3];
            bits[3] = bits[2];
            bits[2] = bits[1];
            bits[1] = bits[0];

		    serialBit = bits[7];

            bool data1Bool = (inputs[DATA1_INPUT].getVoltage() > 1.0);

            if(inputs[DATA2_INPUT].isConnected())
            {
                bits[0] = data1Bool || (inputs[DATA2_INPUT].getVoltage() > 1.0);
            }
            else
            {
                bool feedback = bits[7] != bits[0];
                bits[0] = data1Bool || feedback;
            }
            
            if(inputs[STROBE_INPUT].getVoltage() < 1.0)
            {
                for(int i = 0; i < 8; i++)
                {
                    bitsOut[i] = bits[i];
                }
            }
            
            cv1Value = 0.0;
            cv2Value = 0.0;
            cv3Value = 0.0;
            cv4Value = 0.0;
            
            if(bitsOut[0])
            {
                cv1Value += 0.243f;
                cv2Value += 0.0522f;
            }
            if(bitsOut[1])
            {
                cv2Value += 0.5279f;
                cv3Value += 0.1132f;
            }
            if(bitsOut[2])
            {
                cv3Value += 0.5293f;
                cv4Value += 0.1129f;
            }
            if(bitsOut[3])
            {
                cv1Value += 0.0522f;
                cv4Value += 0.2452f;
            }
            if(bitsOut[4])
            {
                cv1Value += 0.5268f;
                cv2Value += 0.1129f;
            }
            if(bitsOut[5])
            {
                cv2Value += 0.2465f;
                cv3Value += 0.0528f;
            }
            if(bitsOut[6])
            {
                cv3Value += 0.2475f;
                cv4Value += 0.0527f;
            }
            if(bitsOut[7])
            {
                cv1Value += 0.1103f;
                cv4Value += 0.5129f;
            }
            
            cv1Value *= 5.0;
            cv2Value *= 5.0;
            cv3Value *= 5.0;
            cv4Value *= 5.0;

		}

        for(int i = OOO_OUTPUT; i < 8; i++)
        {
            outputs[i].setVoltage(bitsOut[i] ? 5.0 : 0.0);
            lights[OOO_LIGHT].setSmoothBrightness(bitsOut[i] ? 1.0f : 0.0f, args.sampleTime);
        }

        outputs[SERIAL_OUTPUT].setVoltage(serialBit ? 5.0 : 0.0);
        lights[SERIAL_LIGHT].setSmoothBrightness(serialBit ? 1.0f : 0.0f, args.sampleTime);
        
        outputs[CV1_OUTPUT].setVoltage(cv1Value);
        outputs[CV2_OUTPUT].setVoltage(cv2Value);
        outputs[CV3_OUTPUT].setVoltage(cv3Value);
        outputs[CV4_OUTPUT].setVoltage(cv4Value);
	}
};


struct CipherWidget : ModuleWidget {
	CipherWidget(Cipher* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/NLC - 8 BIT CIPHER.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        const float row1Y = 50.0f;
        const float row2Y = 80.5f;
        const float row3Y = 153.0f;
        const float row4Y = 225.0f;
        const float row5Y = 265.0f;

        const float col1X = 31.25f;
        const float col2X = 61.0f;
        const float col3X = 134.0f;
        const float col4X = 216.0f;
        const float col5X = 245.0f;

        addOutput(createOutput<PJ301MPort>(Vec(col3X, row1Y), module, Cipher::OOO_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(col4X, row2Y), module, Cipher::OOX_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(col5X, row3Y), module, Cipher::OXO_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(col4X, row4Y), module, Cipher::OXX_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(col3X, row5Y), module, Cipher::XOO_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(col2X, row4Y), module, Cipher::XOX_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(col1X, row3Y), module, Cipher::XXO_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(col2X, row2Y), module, Cipher::XXX_OUTPUT));

        addOutput(createOutput<PJ301MPort>(Vec(col3X, row3Y), module, Cipher::SERIAL_OUTPUT));


		////////LIGHTS/////////
        const float inJacksX = 7.0f;
        const float outJacksX = 56.75f;
        const float bottomJacksY = 106.0f;
        int spacing = 10.0f;

        for (int i = 0; i < 4; i++)
        {
            int xOffset = i * spacing;
            addInput(createInput<PJ301MPort>(mm2px(Vec(inJacksX + xOffset, bottomJacksY)), module, Cipher::CLOCK_INPUT + i));
            addOutput(createOutput<PJ301MPort>(mm2px(Vec(outJacksX + xOffset, bottomJacksY)), module, Cipher::CV1_OUTPUT + i));
        }
        
        addChild(createLight<SmallLight<BlueLight>>(Vec(85.25, 187.25), module, Cipher::SERIAL_LIGHT));
	}
};



Model* modelCipher = createModel<Cipher, CipherWidget>("8BitCipher");