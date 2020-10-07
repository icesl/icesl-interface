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

    // count number of segments...

    // TAKE INTO ACCOUNT LAYER RANGE IF NOT DONE INTERNALY in ILayerSet !
    m_WritingLayer = 0;
    m_LastLayer = m_LayerSet->getNumLayer();
  } catch (Fatal& f) {
    abortWriting();
    return false;
  }
  return true;
}

bool SampleMachineCode::stepWriting()
{
  try {

    if (m_WritingLayer < (int)m_LastLayer) {

      // TODO output ....
      m_WritingLayer++;
      return true; // keep going

    } else {

      // done
      m_WritingLayer = -1;
      m_Output.close();
      return false;

    }


  } catch (Fatal& f) {
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
  return m_WritingLayer / ((float)m_LayerSet->getNumLayer()-1.0f);
}

// -----------------------------------------------
