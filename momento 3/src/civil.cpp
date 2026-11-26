#include "civil.h"

void Civil::rescatar()
{
    siguiendo_ = true;
}

void Civil::llegarASalvo()
{
    siguiendo_ = false;
    aSalvo_ = true;
}
