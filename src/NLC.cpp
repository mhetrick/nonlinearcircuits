#include "NLC.hpp"


// The pluginInstance-wide instance of the Plugin class
Plugin *pluginInstance;

void init(rack::Plugin *p) 
{
	pluginInstance = p;

	p->addModel(model4Seq);
	p->addModel(modelCipher);
	p->addModel(modelBeatFreqs);
	p->addModel(modelBOOLs);
	p->addModel(modelDeEscalate);
	p->addModel(modelDivideConquer);
	p->addModel(modelDivineCMOS);
	p->addModel(modelDoubleNeuron);
	p->addModel(modelGenie);
	p->addModel(modelLetsSplosh);
	p->addModel(modelNeuron);
	p->addModel(modelNumberwang);
	p->addModel(modelRouter);
	p->addModel(modelSegue);
	p->addModel(modelSlothApathy);
	p->addModel(modelSlothInertia);
	p->addModel(modelSlothTorpor);
	p->addModel(modelSplish);
	p->addModel(modelSquidAxon);
	p->addModel(modelStatues);
	p->addModel(modelTripleSloth);
	// Any other pluginInstance initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables within this file or the individual module files to reduce startup times of Rack.
}
