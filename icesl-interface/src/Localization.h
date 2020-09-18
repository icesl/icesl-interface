/*************************************/
/* SP 2017-07-11: Localization class */
/*                                   */
/*  String translation system        */
/*                                   */
/*************************************/

#pragma once

#include <string>
#include <set>
#include <locale>
#include <codecvt>
#include <map>
#include <vector>

namespace IceSLInterface
{

class Localization
{
public:

  Localization();
  ~Localization() = default;

  void             init(const std::string& lang); // should be called before using!

  void             addTranslationsFromFile(const std::string& lang, const std::string& filename);
  void             addWTranslation        (const std::string& lang, const std::wstring& str, const std::wstring& trans);
  void             addTranslation         (const std::string& lang, const std::string&  str, const std::string&  trans);
  std::wstring     getWTranslation        (const std::string& lang, const std::wstring& str);
  std::string      getTranslation         (const std::string& lang, const std::string&  str);


  const std::string&                  getLanguage() const;
  void                                setLanguage(std::string lang);
  static const std::string            getDefaultLanguage() { return std::string("en"); }
  const std::string                   getLanguageName(const std::string lang = "") const; // "" means current language
  std::string                         getLanguageFont(const std::string lang = "") const; // "" means current language, "" return value means language not found
  const std::vector<std::string>&     getSupportedLanguages() const;
  std::vector<std::string>            getSupportedLanguagesNames() const;

  const std::string         getLocaleLanguage() const;

private:

  bool m_Initialized = false;

  std::map<std::string, int>                          m_LocalizationLanguages;
  std::vector<std::map<std::wstring, std::wstring>>   m_LocalizationMaps;
  std::set<std::wstring>                              m_UnlocalizedStrings;

  unsigned int              m_Language;
  unsigned int              m_SelectedLanguage;
  std::vector<std::string>  m_SupportedLanguages;
  std::vector<std::wstring> m_SupportedLanguagesNames;
  std::vector<std::string>  m_LanguageFonts;

  // for std::wstring_convert
  template <typename Facet>
  struct deletable_facet : Facet
  {
    using Facet::Facet;
  };

public:
  static std::wstring readFileW     (const std::string & filename); // read UTF8 files
  static std::wstring string2wstring(const std::string & input) { return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(input); }
  static std::string  wstring2string(const std::wstring& input) { return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes  (input); }
  static std::wstring wstring2ascii (const std::wstring& input);
};

// -----------------------------------------------

/**
 * @brief The StaticLocalization class is used to provide static access
 * to an instance of localization system which is managed by the application.
 * In order to be used, the method setStaticLocalization should first be called
 * in the application (AND any dynamically loaded library used).
 */
class StaticLocalization
{
public:
  static void setStaticLocalization(Localization& localization)
  {
    s_Localization = &localization;
  }

  static Localization& getLocalization()
  {
    //    assert(s_Localization!=nullptr);
    return *s_Localization;
  }
  static std::string getTranslation(const std::string& str)
  {
    //    assert(s_Localization!=nullptr);
    return s_Localization->getTranslation(s_Localization->getLanguage(), str);
  }
  static std::wstring getWTranslation(std::wstring str)
  {
    //    assert(s_Localization!=nullptr);
    return s_Localization->getWTranslation(s_Localization->getLanguage(), str);
  }

private:
  static Localization* s_Localization;
};

//#undef TR
#define TR(str) IceSLInterface::StaticLocalization::getTranslation(str)

}
