#pragma once

#include "rack.hpp"
#include "engine/Engine.hpp"

//modified SchmittTrigger with no hysteresis.
struct NLCTrigger 
{
	bool state = true;

	void reset() {
		state = true;
	}

	bool process(float in) {
		if (state) {
			// HIGH to LOW
			if (in < 1.f) {
				state = false;
			}
		}
		else {
			// LOW to HIGH
			if (in >= 1.f) {
				state = true;
				return true;
			}
		}
		return false;
	}

	bool isHigh() {
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
    
    float process()
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
        lastPositiveInput = _in;
    }

    void process()
    {
        float diff = lastPositiveInput - lastNegativeInput;
        diff = rack::math::clamp(diff, -10.0f, 10.0f);
        positiveOutput = diff > 0.0f ? diff : 0.0f;
        negativeOutput = diff < 0.0f ? diff : 0.0f;
    }

    float getPositiveOutput(){return positiveOutput;}
    float getNegativeOutput(){return negativeOutput;}

private:
    float lastPositiveInput = 0.0f;
    float lastNegativeInput = 0.0f;
    float positiveOutput = 0.0f;
    float negativeOutput = 0.0f;
};