#include "SampleInfillerPlugin.h"
#include "Path.h"

// -----------------------------------------------

IceSLInterface::IPlugin* createPlugin()
{
  return new SampleInfillerPlugin();
}

// -----------------------------------------------

void SampleInfiller::setNumSlices(int num)
{

}

// -----------------------------------------------

void SampleInfiller::setWorldSpace(v3f world_corner_mm, v3f extent_mm)
{

}

// -----------------------------------------------

void SampleInfiller::prepareInfill(int brush)
{

}

// -----------------------------------------------

void SampleInfiller::terminateInfill(int brush)
{

}

// -----------------------------------------------

void SampleInfiller::prepareInfillForSlice(int id, const AAB<2, int>& xy_slice_box, float height, int brush)
{

}

// ----------------------------------------------- 

bool SampleInfiller::generateInfill(int id, float height_mm, int brush, const ClipperLib::Paths& surface, std::vector<std::unique_ptr<IceSLInterface::IPath> >& fills)
{
  /// This sample generates a simple concentric fill.
  //
  // => Note that this is a naive approach resulting in overfill,
  //    just used here for illustration.
  //
  /// retrieve nozzle diameter
  float nozzle_diameter = 0.0f;
  {
    auto s = m_EnumerableSettings->getSettingByName("nozzle_diameter_mm_0");
    if (s == nullptr) {
      throw Fatal("This plugin is only for FDM");
    }
    s->getValue(nozzle_diameter);
  }
  /// retrieve processing scale factor
  float xy_mm_per_int = 0.0f;
  {
    auto s = m_EnumerableSettings->getSettingByName("xy_mm_per_int");
    if (s == nullptr) {
      throw Fatal("Missing setting");
    }
    s->getValue(xy_mm_per_int);
  }
  /// generate paths
  float radius = nozzle_diameter / 2.0f;
  ClipperLib::Paths current = surface;
  while (1) {
    ClipperLib::ClipperOffset offseter;
    offseter.AddPaths(current, ClipperLib::jtMiter, ClipperLib::EndType::etClosedPolygon);
    ClipperLib::Paths contours;
    offseter.Execute(contours, - radius / xy_mm_per_int);
    // next uses nozzle diameter as spacing
    radius = nozzle_diameter;
    // if result of offset is empty stop
    if (contours.empty()) {
      break;
    }
    /// Add paths to output vector
    ClipperLib::CleanPolygons(contours);
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
 
auto SampleInfillerPlugin::getInfillerBuilder() -> std::pair<std::string,InfillerBuilderFunction>
{
  return std::make_pair(name(), SampleInfillerPlugin::infillerBuilder);
}

// -----------------------------------------------
