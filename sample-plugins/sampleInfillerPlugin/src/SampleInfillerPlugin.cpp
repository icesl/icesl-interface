#include "SampleInfillerPlugin.h"

// -----------------------------------------------

IPlugin* createPlugin()
{
  return new SampleInfillerPlugin();
}

// -----------------------------------------------

IInterface *SampleInfillerPlugin::cast(const UniqueID *id)
{
  return nullptr; 
}

// -----------------------------------------------

void SampleInfillerPlugin::setNumSlices(int num)
{

}

// -----------------------------------------------

void SampleInfillerPlugin::setWorldSpace(v3f world_corner_mm, v3f extent_mm)
{

}

// -----------------------------------------------

void SampleInfillerPlugin::prepareInfill(int brush)
{

}

// -----------------------------------------------

void SampleInfillerPlugin::terminateInfill(int brush)
{

}

// -----------------------------------------------

void SampleInfillerPlugin::prepareInfillForSlice(int id, const AAB<2, int>& xy_slice_box, float height, int brush)
{

}

// -----------------------------------------------

bool SampleInfillerPlugin::generateInfill(int id, float height_mm, int brush, const ClipperLib::Paths& surface, std::vector<ClipperLib::Paths>& fills)
{

}

// -----------------------------------------------
