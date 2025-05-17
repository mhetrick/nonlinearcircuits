#pragma once

#include "rack.hpp"


float LERP(const float _amountOfA, const float _inA, const float _inB);


//modified SchmittTrigger with no hysteresis.
struct NLCTrigger 
{
	bool state = true;
    bool doubleTrigger = false;

	void reset() {
		state = true;
	}

	bool process(float in) {
		if (state) {
			// HIGH to LOW
			if (in < 1.f) 
            {
				state = false;
                if(doubleTrigger)
                    return true;
			}
		}
		else 
        {
			// LOW to HIGH
			if (in >= 1.f) 
            {
				state = true;
				return true;
			}
		}
		return false;
	}

	bool isHigh() const {
		return state;
	}
};


class NLCNeuron
{
public:
    void setSense(float _sense){sense = _sense;}
    void setResponse(float _response){response = _response;}
    void setInput(float _in1, float _in2, float _in3)
    {
        lastInput = _in1 + _in2 + _in3;
    }
    void setInput(float _in)
    {
        lastInput = _in;
    }
    
    float process() const
    {
        float rectifiedInput = rack::math::clamp(lastInput + sense, 0.0f, 10.0f);

        float comparatorOutput = rectifiedInput > 0.0f ? response : response * -1.0f;

        return (rectifiedInput - comparatorOutput);
    }

private:
    float sense = 0.0f;
    float response = 1.0f;
    float lastInput = 0.0f;
};

class NLCDiffRect
{
public:
    void setPositiveInputs(float _in1, float _in2)
    {
        lastPositiveInput = _in1 + _in2;
    }

    void setNegativeInputs(float _in1, float _in2)
    {
        lastNegativeInput = _in1 + _in2;
    }

    void setPositiveInput(float _in)
    {
        lastPositiveInput = _in;
    }
    void setNegativeInput(float _in)
    {
        lastNegativeInput = _in;
    }

    void process()
    {
        float diff = lastPositiveInput - lastNegativeInput;
        diff = rack::math::clamp(diff, -10.0f, 10.0f);
        positiveOutput = diff > 0.0f ? diff : 0.0f;
        negativeOutput = diff < 0.0f ? diff : 0.0f;
    }

    float getPositiveOutput() const {return positiveOutput;}
    float getNegativeOutput() const {return negativeOutput;}

private:
    float lastPositiveInput = 0.0f;
    float lastNegativeInput = 0.0f;
    float positiveOutput = 0.0f;
    float negativeOutput = 0.0f;
};


struct SlothRedGreenLightWidget : rack::GrayModuleLightWidget
{
    SlothRedGreenLightWidget()
    {
        addBaseColor(nvgRGB(0xff, 0x00, 0x00));
        addBaseColor(nvgRGB(0x00, 0xff, 0x00));
    }
};


inline float SlothLedBrightness(float v)
{
    // Turn to full brightness at 2.0 V.
    // Act like a true diode by lighting up only on positive voltages.
    return std::max(0.0f, std::min(1.0f, v / 2.0f));
}
