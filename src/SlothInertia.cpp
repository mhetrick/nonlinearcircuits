#include "SlothCommon.hpp"

//-------------------------------------------------------------------------------------------------
// Inertia

using SlothInertiaModule = SlothModule<Analog::InertiaSlothCircuit>;

struct SlothInertiaWidget : SlothWidget<Analog::InertiaSlothCircuit>
{
public:
    explicit SlothInertiaWidget(SlothInertiaModule* module)
        : SlothWidget<Analog::InertiaSlothCircuit>(module, "res/SlothInertia.svg")
        {}
};

Model* modelSlothInertia = createModel<SlothInertiaModule, SlothInertiaWidget>("SlothInertia");

//-------------------------------------------------------------------------------------------------
