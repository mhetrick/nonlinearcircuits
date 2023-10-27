#include "SlothCommon.hpp"

//-------------------------------------------------------------------------------------------------
// Apathy

using SlothApathyModule = SlothModule<Analog::ApathySlothCircuit>;

struct SlothApathyWidget : SlothWidget<Analog::ApathySlothCircuit>
{
public:
    explicit SlothApathyWidget(SlothApathyModule* module)
        : SlothWidget<Analog::ApathySlothCircuit>(module, "res/SlothApathy.svg")
        {}
};

Model* modelSlothApathy = createModel<SlothApathyModule, SlothApathyWidget>("SlothApathy");
