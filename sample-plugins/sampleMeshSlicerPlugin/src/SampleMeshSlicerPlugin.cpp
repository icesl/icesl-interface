#include "SampleMeshSlicerPlugin.h"
#include <LibSL/LibSL.h>
LIBSL_WIN32_FIX;

#include <iomanip>

// -----------------------------------------------

// --- Shared library interface ---
IceSLInterface::IPlugin* createPlugin()
{
  return new SampleMeshSlicerPlugin();
}

void destroyPlugin(IceSLInterface::IPlugin* plugin)
{
  delete plugin;
}
// ----

// -----------------------------------------------

std::string genFilename(const std::string folder, const std::string name, const std::string ext, unsigned int layer_id, unsigned int max_layers)
{
  std::stringstream filename;
  filename << folder << "/" << name << "_" << std::setw(std::to_string(max_layers).size()) << std::setfill('0') << layer_id << "." << ext;
  return filename.str();
}

// -----------------------------------------------

void SampleMeshSlicer::startSlicing(const IceSLInterface::ISlicingPlan* slicing_plan, TriangleMesh_Ptr& mesh)
{
  sl_assert(slicing_plan != nullptr && mesh.raw() != nullptr);

  // initialize all members
  m_SlicingPlan           = slicing_plan;
  m_Mesh                  = mesh;
  m_CurrentLayerProcessed = 0;
  m_NumOfLayers           = slicing_plan->getNumOfSlices();
  m_CurrentStep           = SampleMeshSlicer::e_Step::e_Rendering; // first step

  // prepare rasterization assets
  // -> render taget
  AAB<3> bbox      = m_Mesh->bbox();
  bbox.minCorner() = bbox.minCorner() / m_MMperIntPlugin;
  bbox.maxCorner() = bbox.maxCorner() / m_MMperIntPlugin;
  bbox = bbox.enlarge(1.0f / m_MMperIntPlugin); // add a 1mm space to the borders of the render target to leave some blank space
  m_RenderTarget = std::unique_ptr<SampleMeshSlicer::RTFormat>(new SampleMeshSlicer::RTFormat(static_cast<uint>(bbox.extent()[0]), static_cast<uint>(bbox.extent()[1])));
  // -> render shader
  m_Shader = std::unique_ptr<AutoBindShader::sh_simplerender>(new AutoBindShader::sh_simplerender()); // simple rendering shader
  m_Shader->init();
  // -> mesh renderer
  m_MeshRenderer = std::unique_ptr<MeshRenderer<SampleMeshSlicer::t_VertexFormat>>(new MeshRenderer<SampleMeshSlicer::t_VertexFormat>(mesh.raw())); // init mesh renderer
  // -> vertex array of simple quad
  m_VAO = std::unique_ptr<t_BufferFormat>(new t_BufferFormat());
  m_VAO->begin(GPUMESH_TRIANGLESTRIP);
  m_VAO->vertex_3(0.0f, 1.0f, 0.0f);
  m_VAO->vertex_3(0.0f, 0.0f, 0.0f);
  m_VAO->vertex_3(1.0f, 1.0f, 0.0f);
  m_VAO->vertex_3(1.0f, 0.0f, 0.0f);
  m_VAO->end();
}

// -----------------------------------------------

void SampleMeshSlicer::rasterizedLayer(unsigned int layer_id)
{
  // NOTE: Layers start at the bottom of the mesh and continue all the way to the upper edge,
  //       a layer's bottom thus closer to the bottom of the mesh and vice-versa
  
  // -> calculate the cutting planes distances corresponding to the layer's bottom and top
  // 
  // Z+
  // ^
  // |---------------------------> Z = 0
  // |   |    |  | (zNear) |
  // |   |mesh|  |         | (zFar)
  // |   |    |  v         |
  // | ----------------------- layer's top
  // |   \    /            |
  // |    |  |             v
  // | ----------------------- layer's bottom
  // |    |  |
  // |   |    |
  // |   ------
  // v
  // Z-
  // 
  // distance from layer's top to upper (w.r.t. Z) edge of mesh
  float z_near = m_Mesh->bbox().extent()[2] - 
    (static_cast<float>(m_SlicingPlan->getHeightRangeOfSlice(layer_id).second) - m_Mesh->bbox().minCorner()[2]);
  // distance from layer's bottom to upper (w.r.t. Z) edge of mesh
  float z_far  = m_Mesh->bbox().extent()[2] -
    (static_cast<float>(m_SlicingPlan->getHeightRangeOfSlice(layer_id).first) - m_Mesh->bbox().minCorner()[2]);

  // projection matrix (orthogonal)
  AAB<3> bbox = m_Mesh->bbox().enlarge(1); // add 1mm to the borders of the orthogonal view
  m4x4f proj = orthoMatrixGL(
    bbox.minCorner()[0], bbox.maxCorner()[0],  // mesh occupies the whole X area
    bbox.minCorner()[1], bbox.maxCorner()[1],  // mesh occupies the while Y area
    z_near, bbox.extent()[2]                  // clip on zNear to the bottom the mesh
                                                  // Note: it cannot clip on ZFar because it would leave
                                                  // the mesh open thus making the stencil technique fail.
  );
  // transformation matrix (identity)
  m4x4f transf = LibSL::Math::m4x4f::identity();

  // NOTE: The following is a common technique to slice shapes through cutting planes using the stencil buffer
  //       http://glbook.gamedev.net/GLBOOK/glbook.gamedev.net/moglgp/advclip.html
  // -> we perform the layer rasterization in GPU thus we use a render target
  m_RenderTarget->bind();
  // -> viewport corresponds to the whole rendering area
  glViewport(0, 0, m_RenderTarget->w(), m_RenderTarget->h());
  clearScreen(LIBSL_COLOR_BUFFER | LIBSL_STENCIL_BUFFER);
  // -> we use a simple shader that renders a geometry with a particular color w.r.t. a modelview and projection matrix
  m_Shader->begin();
  m_Shader->p_matrix.set(proj);
  m_Shader->u_color.set(v4f(1));

  // -> create the shape of the sliced section in the stencil buffer
  glEnable(GL_STENCIL_TEST); // enable stencil
  glDisable(GL_CULL_FACE);   // neither back nor front faces will be discared
  glDisable(GL_DEPTH_TEST);  // no need for depth testing
  glStencilMask(0xFF); // default mask (i.e., write to stencil without any filter)
  glStencilFunc(GL_ALWAYS, 0x00, 0xFF); // stencil never fails (i.e., vertices pass entirely)
  // -> turn on (i.e., invert - from 0x00 to 0xFF) stencil value on back faces to create the bottom of the layer
  glStencilOpSeparate(GL_BACK, GL_INVERT, GL_INVERT, GL_INVERT); 
  // -> turn off (i.e., invert - from 0xFF to 0x00) stencil with front faces to create the top of the layer
  glStencilOpSeparate(GL_FRONT, GL_INVERT, GL_INVERT, GL_INVERT);
  // -> the above is based on the fact that back faces create solid areas while front faces are hollow
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // don't render. Only use stencil
  // -> place mesh in -Z space (i.e., mesh top on Z=0). This is due to default "camera" looking in the direction of -Z
  transf = LibSL::Math::translationMatrix(v3f(0, 0, -m_Mesh->bbox().maxCorner()[2]));
  m_Shader->t_matrix.set(transf);
  m_MeshRenderer->render();

  // -> render stencil buffer into the framebuffer
  glStencilFunc(GL_NOTEQUAL, 0x00, 0xFF);                // draw only where stencil is turned on
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);              // keep stencil value when stencil fails or passes
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // activate framebuffer
  transf = // -> translate square to appropiate position
    // -> place the square in the position of the mesh place it in the middle of the layer (z height)
    LibSL::Math::translationMatrix(v3f(bbox.minCorner()[0], bbox.minCorner()[1], -z_near - (z_far - z_near) / 2.0f)) * 
    // -> scale the square to the size of the mesh
    LibSL::Math::scaleMatrix(v3f(bbox.extent()[0], bbox.extent()[1], 1)); // scale to viewport size
  m_Shader->t_matrix.set(transf);
  m_VAO->render(); // render square
  glDisable(GL_STENCIL_TEST);
  m_Shader->end();
  m_RenderTarget->unbind();

  // -> allocate memory for rasterized layer
  m_Images.emplace_back(new SampleMeshSlicer::IMGFormat(m_RenderTarget->w(), m_RenderTarget->h()));
  // -> tarnsfer rasteization from GPU to CPU memory
  m_RenderTarget->readBack(m_Images.back()->pixels()); // read back rasterized layer from render target

#if 0 // DEBUG: dump layer rasterization
  saveImage(genFilename("dump", "layer", "png", layer_id, m_NumOfLayers).c_str(), m_Images.back().get());
#endif
}

// -----------------------------------------------

void SampleMeshSlicer::vectorizeLayer(unsigned int layer_id)
{
  // Note: The idea is to detect the contours in the layer by means of contrast (i.e., from black to white)
  //       then to vectorize the contours by following their outline and discretize each line with a tolerance angle.
  //       Contour pixels will be marked along the way to avoid repeating them

  auto& layer_img = m_Images.at(layer_id);

  // -> mark contours in layer
  const unsigned char empty = 0;
  const unsigned char solid = 255;
  const unsigned char contour = 127;
  ForImage(layer_img, i, j) {
    if (layer_img->pixel(i, j) == solid) {
      // assert that solid pixel isn't on edge of image. It should not as the render target was enlarged by 1mm
      sl_assert((i - 1) >= 0 && (i - 1) < layer_img->w() && (j - 1) >= 0 && (j - 1) < layer_img->h());
      if (layer_img->pixel(i - 1, j) == empty) {        // left
        layer_img->pixel(i, j) = contour;
      } else if (layer_img->pixel(i, j - 1) == empty) { // up
        layer_img->pixel(i, j) = contour;
      } else if (layer_img->pixel(i + 1, j) == empty) { // right
        layer_img->pixel(i, j) = contour;
      } else if (layer_img->pixel(i, j + 1) == empty) { // down
        layer_img->pixel(i, j) = contour;
      }
    }
  }

#if 0 // DEBUG: dump layer_contouring
  std::shared_ptr<ImageL8> layer_denoised = std::shared_ptr<ImageL8>(new ImageL8(*layer_img));
  ForImage(layer_denoised, i, j) {
    if (layer_denoised->pixel(i, j) == solid) { // empty solid areas
      layer_denoised->pixel(i, j) = empty;
    } else if (layer_denoised->pixel(i, j) == contour) { // solidify contours
      layer_denoised->pixel(i, j) = solid;
    }
  }
  saveImage(genFilename("dump", "layer_contours", "png", layer_id, m_NumOfLayers).c_str(), layer_denoised.get());
#endif

  // -> vectorize contours
  m_Slices.emplace_back();
  auto& layer_vect = m_Slices.back();
  const unsigned char start = 100;
  const unsigned char visited = 101;

  const float max_angle_deviation_tolerance = 1.0f * (static_cast<float>(M_PI) / 180.0f); // maximum contour line deviation before starting new line
  const unsigned int initial_angle_line_steps = 4; // initial distance (in contour steps) to estimate first angle

  ForImage(layer_img, i, j) {
    // -> for all contours in image
    if (layer_img->pixel(i, j) == contour) { // start of countour

      IceSLInterface::Contour cnt;
      cnt.outline().push_back(v2i(i, j)); // contour starting position
      layer_img->pixel(i, j) = start; // start of contour
      unsigned int i0 = i, j0 = j; // start position of contour
      unsigned int ic = i, jc = j; // start position of line
      unsigned int in = i, jn = j; // current position of line
      unsigned int ip = i, jp = j; // next position of line
      unsigned int il = i, jl = j; // last position of line
      float angle = 0;
      unsigned int line_steps = 0;
      while (true) {
        bool continue_contour = false;

        // -> choose contour next position
        /*  */ if (layer_img->pixel(in - 1, jn + 0) == contour) { // left
          ip = in - 1; jp = jn + 0;
        } else if (layer_img->pixel(in - 1, jn + 1) == contour) { // bottom left
          ip = in - 1; jp = jn + 1;
        } else if (layer_img->pixel(in + 0, jn + 1) == contour) { // bottom
          ip = in + 0; jp = jn + 1;
        } else if (layer_img->pixel(in + 1, jn + 1) == contour) { // bottom right
          ip = in + 1; jp = jn + 1;
        } else if (layer_img->pixel(in + 1, jn + 0) == contour) { // right
          ip = in + 1; jp = jn + 0;
        } else if (layer_img->pixel(in + 1, jn - 1) == contour) { // top right
          ip = in + 1; jp = jn - 1;
        } else if (layer_img->pixel(in + 0, jn - 1) == contour) { // top
          ip = in + 0; jp = jn - 1;
        } else if (layer_img->pixel(in - 1, jn - 1) == contour) { // top left
          ip = in - 1; jp = jn - 1;
        } else { // contour finished?
          // -> try to follow contour from last position (fallback!)
          for (unsigned int u = il - 1; u <= il + 1; ++u) {
            for (unsigned int v = jl - 1; v <= jl + 1; ++v) {
              if (!(u == in && v == jn)) { // ignore current position
                if (layer_img->pixel(u, v) == contour) {
                  continue_contour = true;
                  ip = u; jp = v;
                }
              }
            }
          }
          // -> contour really finished
          if (!continue_contour) {
            // -> detect if near start of contour
            bool closed_contour = false;
            const int search_radius = 2;
            for (int u = static_cast<int>(in) - search_radius; u <= static_cast<int>(in) + search_radius; ++u) {
              for (int v = static_cast<int>(jn) - search_radius; v <= static_cast<int>(jn) + search_radius; ++v) {
                if (u >= 0 && u < static_cast<int>(layer_img->w()) && v >= 0 && v < static_cast<int>(layer_img->h())) {
                  closed_contour |= (layer_img->pixel(u, v) == start);
                }
              }
            }
            if (closed_contour) {
              cnt.outline().push_back(v2i(in, jn)); // save closing point in contour
              // -> detect if contour is counter-clockwise
              long int sum_orientation = 0;
              for (unsigned int i = 0; i < cnt.outline().size(); ++i) {
                sum_orientation +=
                  (cnt.outline().at(i)[0] - cnt.outline().at(i == 0 ? cnt.outline().size() - 1 : i - 1)[0]) *
                  (cnt.outline().at(i)[1] + cnt.outline().at(i == 0 ? cnt.outline().size() - 1 : i - 1)[1]);
              }
              // -> reverse if counterclockwise
              if (sum_orientation >= 0) {
                std::reverse(cnt.outline().begin(), cnt.outline().end());
              }
              layer_vect.push_back(cnt); // insert contour in layer
            }
            // -> mark start and end of contour visited
            layer_img->pixel(i0, j0) = visited;
            layer_img->pixel(in, jn) = visited;
            break;
          }
        }

        // -> mark current position as visited (unless it's contour's start)
        if (layer_img->pixel(in, jn) != start) {
          layer_img->pixel(in, jn) = visited;
        }

        // -> detect if line breaks
        if (line_steps++ <= initial_angle_line_steps) { // line has just started
          if (line_steps > initial_angle_line_steps) { // time to estimate line angle
            angle = std::acos((static_cast<float>(ip) - static_cast<float>(in)) / length(v2f(static_cast<float>(ip) - static_cast<float>(in), static_cast<float>(jp) - static_cast<float>(jn))));
          }
        } else {
          // angle of current traced line
          float current_angle = std::acos((static_cast<float>(ip) - static_cast<float>(ic)) / length(v2f(static_cast<float>(ip) - static_cast<float>(ic), static_cast<float>(jp) - static_cast<float>(jc))));
          if (std::abs(angle - current_angle) > max_angle_deviation_tolerance) { // line breaks
            cnt.outline().push_back(v2i(in, jn)); // save point in contour
            ic = in; jc = jn; // start of new line
            line_steps = 0;
          }
        }

        // -> save last position
        if (!continue_contour) { // unless we fell back!
          il = in; jl = jn;
        }
        // -> update current position
        in = ip; jn = jp;

      }
    }
  }

#if 0 // DEBUG: dump layer vectorization
  LibSL::SvgHelpers::Svg dump(genFilename("dump", "layer_vectorization", "svg", layer_id, m_NumOfLayers).c_str());
  dump.setProperties("#000000", "none", 10.0f);
  for (const auto& contour : m_Slices.back()) {
    dump.startPath();
    for (const auto& point : contour.outline()) {
      dump.addPoint(static_cast<float>(point[0]), static_cast<float>(point[1]));
    }
    dump.endPath();
  }
#endif

  // -> reposition layer:
  //    a. offset enlargement made to avoid edges of image
  //    b. offset to center of view
  // -> rescale layer:
  //    a. rescale to the mm_per_int value passed by IceSL to the plugin
  float factor = m_MMperIntPlugin / m_MMperInt;
  for (auto& contour : layer_vect) {
    for (auto& point : contour.outline()) {
      point -= v2i(static_cast<int>(1.0f / m_MMperIntPlugin));
      point += v2i(static_cast<int>(m_Mesh->bbox().minCorner()[0] / m_MMperIntPlugin), static_cast<int>(m_Mesh->bbox().minCorner()[1] / m_MMperIntPlugin));
      point  = point * v2i(static_cast<int>(factor));
    }
  }
}

// -----------------------------------------------

bool SampleMeshSlicer::processSlicing()
{
  if (m_CurrentStep == SampleMeshSlicer::e_Step::e_Rendering) {

    if (m_CurrentLayerProcessed == m_NumOfLayers) { // check if next step is due
      m_CurrentStep = SampleMeshSlicer::e_Step::e_Vectorizing;
      m_CurrentLayerProcessed = 0;
    } else {
      // rasterize layer and continue
      rasterizedLayer(m_CurrentLayerProcessed++);
    }

  } else if (m_CurrentStep == SampleMeshSlicer::e_Step::e_Vectorizing) {

    if (m_CurrentLayerProcessed == m_NumOfLayers) { // check if done
      m_Images.clear(); // free up memory
      m_Shader->terminate();
      m_RenderTarget->clear();
      return true;
    } else {
      // vectorize layer
      vectorizeLayer(m_CurrentLayerProcessed++);
    }
  }
  return false;
}

// -----------------------------------------------

void SampleMeshSlicer::slice(const IceSLInterface::ISlicingPlan* slicing_plan, TriangleMesh_Ptr& mesh)
{
  SampleMeshSlicer::startSlicing(slicing_plan, mesh);
  while (!processSlicing()) { continue; }
}

// -----------------------------------------------

unsigned int SampleMeshSlicer::getNumOfProcessingSteps() const
{
  return static_cast<unsigned int>(SampleMeshSlicer::e_Step::size);
}

// -----------------------------------------------

unsigned int SampleMeshSlicer::getCurrentProcessingStep() const
{
  return static_cast<unsigned int>(m_CurrentStep);
}

// -----------------------------------------------

float SampleMeshSlicer::getCurrentProcessingStepProgress() const
{
  if (m_CurrentStep == SampleMeshSlicer::e_Step::e_Rendering) {
    return
      (static_cast<float>(SampleMeshSlicer::e_Step::e_Rendering) / static_cast<float>(SampleMeshSlicer::e_Step::size)) + // overall progress
      ((1.0f / static_cast<float>(SampleMeshSlicer::e_Step::size)) *                        // step progress
        (static_cast<float>(m_CurrentLayerProcessed) / static_cast<float>(m_NumOfLayers))); // inter-step progress
  } else if (m_CurrentStep == SampleMeshSlicer::e_Step::e_Vectorizing) {
    return 
      (static_cast<float>(SampleMeshSlicer::e_Step::e_Vectorizing) / static_cast<float>(SampleMeshSlicer::e_Step::size)) + // overall progress
      ((1.0f / static_cast<float>(SampleMeshSlicer::e_Step::size)) *                        // step progress
        (static_cast<float>(m_CurrentLayerProcessed) / static_cast<float>(m_NumOfLayers))); // inter-step progress
  } else {
    sl_assert(false);
    return 0;
  }
}

// -----------------------------------------------

std::string SampleMeshSlicer::getCurrentProcessingStepDescription() const
{
  if (m_CurrentStep == SampleMeshSlicer::e_Step::e_Rendering) {
    return "Rendering";
  } else if (m_CurrentStep == SampleMeshSlicer::e_Step::e_Rendering) {
    return "Vectorizing";
  } else {
    sl_assert(false);
    return "";
  }
}

// -----------------------------------------------

bool SampleMeshSlicerPlugin::addPluginSettings(IceSLInterface::EnumerableSettingsInterface& enumerable_settings)
{
  return true;
}

// -----------------------------------------------