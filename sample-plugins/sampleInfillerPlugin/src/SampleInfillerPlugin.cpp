#include "SampleInfillerPlugin.h"
#include "Path.h"

// -----------------------------------------------

IceSLInterface::IPlugin* createPlugin()
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

bool SampleInfillerPlugin::generateInfill(int id, float height_mm, int brush, const ClipperLib::Paths& surface, std::vector<std::unique_ptr<IceSLInterface::IPath> >& fills)
{
  // this sample generates a simple concentric fill
  float nozzle_diameter = 0.0f;
  {
    auto s = m_EnumerableSettings->getSettingByName("nozzle_diameter_mm_0");
    if (s == nullptr) {
      throw Fatal("This plugin is only for FDM");
    }
    s->getValue(nozzle_diameter);
  }
  float xy_mm_per_int = 0.0f;
  {
    auto s = m_EnumerableSettings->getSettingByName("xy_mm_per_pixels");
    if (s == nullptr) {
      throw Fatal("Missing setting");
    }
    s->getValue(xy_mm_per_int);
  }
  // now generate paths
  float radius = nozzle_diameter / 2.0f;
  ClipperLib::Paths current = surface;
  while (1) {
    ClipperLib::ClipperOffset offseter;
    offseter.AddPaths(current, ClipperLib::jtMiter, ClipperLib::EndType::etClosedPolygon);
    ClipperLib::Paths contours;
    offseter.Execute(contours, - 10.0f * radius / xy_mm_per_int);
    // next uses nozzle diameter as spacing
    radius = nozzle_diameter;
    // if result of offset is empty stop
    if (contours.empty()) {
      break;
    }
    // add paths
    for (const auto &cnt : contours) {
      fills.push_back(std::unique_ptr<IceSLInterface::IPath>(new IceSLInterface::Path()));
      fills.back()->createPath(cnt, true);
    }
    // next
    current = contours;
  }
  return true;
}

// -----------------------------------------------
 
