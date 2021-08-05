#include "NLC.hpp"


// The pluginInstance-wide instance of the Plugin class
Plugin *pluginInstance;

void init(rack::Plugin *p) {
	pluginInstance = p;

	p->addModel(model4Seq);
	p->addModel(modelBOOLs);
	p->addModel(modelDoubleNeuron);
	p->addModel(modelGenie);
	p->addModel(modelLetsSplosh);
	p->addModel(modelNeuron);
	p->addModel(modelNumberwang);
	p->addModel(modelSegue);
	p->addModel(modelSquidAxon);
	p->addModel(modelStatues);
	// Any other pluginInstance initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables within this file or the individual module files to reduce startup times of Rack.
}
