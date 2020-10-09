#include "SampleMachineCodePlugin.h"
#include "Path.h"

#include <LibSL/System/System.h>

// -----------------------------------------------

// --- Shared library interface ---
IceSLInterface::IPlugin* createPlugin()
{
  return new SampleMachineCodePlugin();
}

void destroyPlugin(IceSLInterface::IPlugin* plugin)
{
  delete plugin;
}

// -----------------------------------------------

bool SampleMachineCode::startWriting(const std::string& filename)
{
  try {
    m_Output = std::ofstream(filename);


    m_NbVertices = 0;
    m_NbSegments = 0;
    for (int nLayer=0; nLayer<m_LayerSet->getNumLayers(); ++nLayer)
    {
      const IceSLInterface::ILayer& layer = m_LayerSet->getLayer(nLayer);

      for (int nPath=0; nPath<layer.getNumPaths(); ++nPath)
      {
        const IceSLInterface::IPath& path = layer.getPath(nPath);
        m_NbVertices += path.getNumVertices();
        m_NbSegments += path.getNumVertices() - (path.isCyclic() ? 0 : 1);
      }
    }

    m_Output << m_NbVertices << "\n";

    for (int nLayer=0; nLayer<m_LayerSet->getNumLayers(); ++nLayer)
    {
      const IceSLInterface::ILayer& layer = m_LayerSet->getLayer(nLayer);

      for (int nPath=0; nPath<layer.getNumPaths(); ++nPath)
      {
        const IceSLInterface::IPath& path = layer.getPath(nPath);

        for (int nVertex=0; nVertex<path.getNumVertices(); ++nVertex)
        {
          v2f xy = m_LayerSet->xyWorldSpace(path.getVertexPos(nVertex));
          float z = static_cast<float>(layer.zTopWorldSpace()) - 0.5f * static_cast<float>(layer.thickness());
          m_Output << xy[0] << " " << xy[1] << " " << z << " " << 0.2 << "\n";
        }
      }
    }

    m_Output << m_NbSegments << "\n";

    m_Offset = 0;

    // TAKE INTO ACCOUNT LAYER RANGE IF NOT DONE INTERNALY in ILayerSet !
    m_WritingLayer = 0;
    m_LastLayer = m_LayerSet->getNumLayers();
  } catch (Fatal& f) {
    std::cout << f.message() << std::endl;
    abortWriting();
    return false;
  }
  return true;
}

bool SampleMachineCode::stepWriting()
{
  try {

    if (m_WritingLayer < (int)m_LastLayer) {

        const IceSLInterface::ILayer& layer = m_LayerSet->getLayer(m_WritingLayer);

        for (int nPath=0; nPath<layer.getNumPaths(); ++nPath)
        {
          const IceSLInterface::IPath& path = layer.getPath(nPath);

          for (int nVertex=0; nVertex<path.getNumVertices()-1; ++nVertex)
          {
            m_Output << nVertex+m_Offset << " " << nVertex+m_Offset+1  << "\n";
          }
          if (path.isCyclic()) {
            m_Output << path.getNumVertices()-1+m_Offset << " " << m_Offset  << "\n";
          }
          m_Offset += path.getNumVertices();
        }

      m_WritingLayer++;
      return true; // keep going

    } else {

      // done
      m_WritingLayer = -1;
      m_Output.close();
      return false;

    }


  } catch (Fatal& f) {
    std::cerr << f.message() << std::endl;
    abortWriting();
    return false;
  }
  return true;
}

void SampleMachineCode::abortWriting()
{
  m_Output.close();
}

float SampleMachineCode::writingProgress() const
{
  return m_WritingLayer / ((float)m_LayerSet->getNumLayers()-1.0f);
}

// -----------------------------------------------
