#include "SlothCommon.hpp"

//-------------------------------------------------------------------------------------------------
// Torpor

using SlothTorporModule = SlothModule<Analog::TorporSlothCircuit>;

struct SlothTorporWidget : SlothWidget<Analog::TorporSlothCircuit>
{
public:
    explicit SlothTorporWidget(SlothTorporModule* module)
        : SlothWidget<Analog::TorporSlothCircuit>(module, "res/SlothTorpor.svg")
        {}
};

Model* modelSlothTorpor = createModel<SlothTorporModule, SlothTorporWidget>("SlothTorpor");

