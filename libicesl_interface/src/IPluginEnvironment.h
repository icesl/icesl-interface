#pragma once

#include "IInterface.h"

#include "Localization.h"
#include "imgui.h"

//// {EB4BE774-A86A-43ed-8AD3-D7A3C36A5776}
//ITF_GUID(Itf_IPluginEnvironment,
//  0xeb4be774, 0xa86a, 0x43ed, 0x8a, 0xd3, 0xd7, 0xa3, 0xc3, 0x6a, 0x57, 0x76);

// ----------------------------------------------------------------

class IPluginEnvironment : public IInterface
{
public:
  IPluginEnvironment() = default;
  ~IPluginEnvironment() override = default;

  IInterface *cast(const UniqueID *id) override
  {
    return nullptr; //TODO
  //  if (IsEqualUniqueID(*id, Itf_IInterface) || IsEqualUniqueID(*id, Itf_IPluginEnvironment))
  //    return static_cast<IPluginEnvironment*>(this);
  //  else
  //    return NULL;
  }
};

// ----------------------------------------------------------------

//// {EB4BE774-A86A-43ed-8AD3-D7A3C36A5776}
//ITF_GUID(Itf_ILocalizationEnvironment,
//  0xeb4be774, 0xa86a, 0x43ed, 0x8a, 0xd3, 0xd7, 0xa3, 0xc3, 0x6a, 0x57, 0x76);

class ILocalizationEnvironment : public virtual IPluginEnvironment
{
public:
  ILocalizationEnvironment(Localization& localization)
    : m_Localization(&localization)
  {}

  ~ILocalizationEnvironment() override = default;

  IInterface *cast(const UniqueID *id) override
  {
    return nullptr; //TODO
  //  if (IsEqualUniqueID(*id, Itf_IInterface) || IsEqualUniqueID(*id, Itf_ILocalizationEnvironment))
  //    return static_cast<ILocalizationEnvironment*>(this);
  //  else
  //    return NULL;
  }

protected:
  Localization* m_Localization;
};

class IImGuiEnvironment : public virtual IPluginEnvironment
{
public:
  IImGuiEnvironment()
    : m_ImGuiContext(ImGui::GetCurrentContext())
  {}

  ~IImGuiEnvironment() override = default;

  IInterface *cast(const UniqueID *id) override
  {
    return nullptr; //TODO
  //  if (IsEqualUniqueID(*id, Itf_IInterface) || IsEqualUniqueID(*id, Itf_ILocalizationEnvironment))
  //    return static_cast<ILocalizationEnvironment*>(this);
  //  else
  //    return NULL;
  }

  ImGuiContext* getImGuiContext()
  {
    return m_ImGuiContext;
  }

protected:
  ImGuiContext* m_ImGuiContext;
};

// ----------------------------------------------------------------
