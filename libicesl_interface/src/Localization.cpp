#include "Localization.h"

#include <CppHelpers/CppHelpers.h>

#include <iostream>
#include <sstream>
#include <regex>
#include <cassert>

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;
using namespace IceSLInterface;

// ----------------------------------------------------------------------------

Localization* StaticLocalization::s_Localization{};

// ----------------------------------------------------------------------------

Localization::Localization()
{
  // Supported languages
  // u8 literals for UTF-8 strings are not supported in VS2013 https://msdn.microsoft.com/en-us/library/69ze775t(v=vs.120).aspx
  m_SupportedLanguages.push_back("en"); m_SupportedLanguagesNames.push_back(L"English") ; m_LanguageFonts.push_back("Cousine-Regular.ttf"); // see ImGuiBindings.cpp for possible fonts
  m_SupportedLanguages.push_back("es"); m_SupportedLanguagesNames.push_back(L"Español") ; m_LanguageFonts.push_back("Cousine-Regular.ttf");
  m_SupportedLanguages.push_back("fr"); m_SupportedLanguagesNames.push_back(L"Français"); m_LanguageFonts.push_back("Cousine-Regular.ttf");
#ifndef EMSCRIPTEN
  m_SupportedLanguages.push_back("ch"); m_SupportedLanguagesNames.push_back(L"中文")    ; m_LanguageFonts.push_back("NotoSansCJKtc-Medium.ttf");
#endif
}

// ----------------------------------------------------------------------------

void Localization::init(const std::string& lang)
{
  // Language
  std::string language = lang;
  if (language.empty()) { // Not found on settings, from locale
    language = getLocaleLanguage();
  }
  if (language.empty()) { // Default language (all else fails)
    language = getDefaultLanguage();
  }
  setLanguage(language);
  cerr << "[Localization] language used \"" << language << "\"\n";

  m_Initialized = true;
}


// ----------------------------------------------------------------------------

const std::string& Localization::getLanguage() const
{
  assert(m_Initialized);
  return m_SupportedLanguages[m_Language];
}

// ----------------------------------------------------------------------------

void Localization::setLanguage(std::string lang)
{
  auto it = std::find(m_SupportedLanguages.begin(), m_SupportedLanguages.end(), lang);
  if (it != m_SupportedLanguages.end())
  {
    m_Language = (int)std::distance(m_SupportedLanguages.begin(), it); // Language supported!
  }
}

// ----------------------------------------------------------------------------

const std::string Localization::getLanguageName(const std::string lang) const
{
  if (lang == "") {
    return wstring2string(m_SupportedLanguagesNames[m_Language]);
  } else {
    auto it = std::find(m_SupportedLanguages.begin(), m_SupportedLanguages.end(), lang);
    if (it != m_SupportedLanguages.end())
    {
      return wstring2string(m_SupportedLanguagesNames[std::distance(m_SupportedLanguages.begin(), it)]); // Language supported!
    } else {
      return std::string();
    }
  }
}

// ----------------------------------------------------------------------------

std::string Localization::getLanguageFont(const std::string lang) const
{
  if (lang == "") {
    return m_LanguageFonts[m_Language];
  } else {
    auto it = std::find(m_SupportedLanguages.begin(), m_SupportedLanguages.end(), lang);
    if (it != m_SupportedLanguages.end())
    {
      return m_LanguageFonts[std::distance(m_SupportedLanguages.begin(), it)]; // Language supported!
    } else {
      return "";
    }
  }
}

// ----------------------------------------------------------------------------

const std::vector<std::string>& Localization::getSupportedLanguages() const
{
  return m_SupportedLanguages;
}

// ----------------------------------------------------------------------------

std::vector<std::string> Localization::getSupportedLanguagesNames() const
{
  std::vector<std::string> langNames;
  for (std::wstring l : m_SupportedLanguagesNames)
  {
    langNames.push_back(wstring2string(l));
  }
  return langNames;
}

// ----------------------------------------------------------------------------

const std::string Localization::getLocaleLanguage() const
{
  std::string language;
#ifdef WIN32
  // https://msdn.microsoft.com/en-us/library/dd318137(VS.85).aspx
  // https://msdn.microsoft.com/en-us/library/dd373908(v=vs.85).aspx
  // https://msdn.microsoft.com/en-us/library/dd318693(v=vs.85).aspx
  LANGID lang = GetUserDefaultUILanguage();

  switch (MAKELANGID(LANG_INVARIANT, SUBLANG_NEUTRAL) & lang)
  {
  case MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL):
    language = "en";
    break;
  case MAKELANGID(LANG_SPANISH, SUBLANG_NEUTRAL):
    language = "es";
    break;
  case MAKELANGID(LANG_CHINESE, SUBLANG_NEUTRAL):
    language = "ch";
    break;
  case MAKELANGID(LANG_FRENCH, SUBLANG_NEUTRAL):
    language = "fr";
    break;
  }
#else
  // https://stackoverflow.com/questions/12170488/how-to-get-current-locale-of-my-environment
  string current = setlocale(LC_ALL, "");
  string locale = setlocale(LC_ALL, NULL);
  setlocale(LC_ALL, current.c_str());

  regex lang("^([a-z][a-z])_");
  if (regex_search(locale, lang))
    language = regex_replace(locale, lang, "$1", regex_constants::format_first_only | regex_constants::format_no_copy);
#endif
  return language;
}

// ----------------------------------------------------------------------------

// https://stackoverflow.com/questions/4775437/read-unicode-utf-8-file-into-wstring
// https://stackoverflow.com/questions/26121782/read-unicode-file-with-special-characters-using-stdwifstream
std::wstring Localization::readFileW(const std::string& filename)
{
  std::wifstream wif(filename);
  wif.imbue(std::locale(wif.getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::consume_header>));
  std::wstringstream wss;
  wss << wif.rdbuf();
  return wss.str();
}

// ----------------------------------------------------------------------------

void Localization::addTranslationsFromFile(const std::string& lang, const std::string& filename)
{
  if (!LibSL::System::File::exists(filename.c_str())) {
#ifdef _DEBUG
    cerr << LibSL::CppHelpers::Console::blue << "[Localization] (" << lang << ") translation file \"" << filename << "\" not found!" << LibSL::CppHelpers::Console::gray << endl;
#endif
    return;
  }
  std::wstring translationsText = Localization::readFileW(filename);
  while (!translationsText.empty()) {
    // entry
    size_t pos1 = translationsText.find_first_of('\n');
    std::wstring entry = translationsText.substr(0, pos1);
    // commentary (i.e., starts with "##")
    if (entry.find(L"##") != wstring::npos) {
      translationsText = translationsText.substr(pos1 + 1, wstring::npos);
      continue;
    }
    // translation
    size_t pos2 = translationsText.substr(pos1 + 1, std::wstring::npos).find_first_of('\n');
    std::wstring translation = translationsText.substr(pos1 + 1, pos2);
    // add translation   
    addWTranslation(lang, entry, translation);
    // continue onto the next two lines
    translationsText = translationsText.substr(pos1 + 1 + pos2 + 1, std::wstring::npos);
  }
}

// ----------------------------------------------------------------------------

void Localization::addWTranslation(const std::string& lang, const std::wstring& str,const std::wstring& trans)
{
	if (m_LocalizationLanguages.find(lang) == m_LocalizationLanguages.end()) { // language doesn't exist
		std::map<std::wstring, std::wstring> langMap;
		m_LocalizationMaps.push_back(langMap);
		m_LocalizationLanguages[lang] = (int)m_LocalizationMaps.size() - 1;
	}
	// Add translation
#ifdef _DEBUG
  if(m_LocalizationMaps[m_LocalizationLanguages[lang]].count(str)) {
    cerr << LibSL::CppHelpers::Console::blue << "[Localization] (" << lang << ") translation for \"" << wstring2string(str) << "\" already exists!" << LibSL::CppHelpers::Console::gray << endl;
  }
#endif
	m_LocalizationMaps[m_LocalizationLanguages[lang]][str] = trans;
}

// ----------------------------------------------------------------------------

void Localization::addTranslation(const std::string& lang, const std::string& str, const std::string& trans)
{
	addWTranslation(lang, string2wstring(str), string2wstring(trans));
}

// ----------------------------------------------------------------------------

std::wstring Localization::getWTranslation(const std::string& lang, const std::wstring& str)
{
	if (m_LocalizationLanguages.find(lang) == m_LocalizationLanguages.end() ||
		m_LocalizationMaps[m_LocalizationLanguages[lang]].find(str) == m_LocalizationMaps[m_LocalizationLanguages[lang]].end()) {
		if (m_UnlocalizedStrings.find(str + L"_" + string2wstring(lang)) == m_UnlocalizedStrings.end()) {
#ifdef _DEBUG
			//wcerr << "[Localization] no (" << string2wstring(lang) << ") translation found for \"" << str << "\"" << endl;
			cerr << LibSL::CppHelpers::Console::blue << "[Localization] no (" << lang << ") translation found for \"" << wstring2string(str) << "\"" << LibSL::CppHelpers::Console::gray << endl;
#endif
			m_UnlocalizedStrings.insert(str + L"_" + string2wstring(lang));
		}
		return str;
	}
	return m_LocalizationMaps[m_LocalizationLanguages[lang]][str];
}

// ----------------------------------------------------------------------------

std::string Localization::getTranslation(const std::string& lang, const std::string& str)
{
  assert(m_Initialized);
	return wstring2string(getWTranslation(lang, string2wstring(str)));
}

// ----------------------------------------------------------------------------

static std::map<wchar_t, wchar_t> to_ascii_map = { 
  { L'Á', 'A' },{ L'É', 'E' },{ L'Í', 'I' },{ L'Ó', 'O' },{ L'Ú', 'u' },
  { L'À', 'A' },{ L'È', 'E' },{ L'Ì', 'I' },{ L'Ò', 'O' },{ L'Ù', 'u' },
  { L'Â', 'A' },{ L'Ê', 'E' },{ L'Î', 'I' },{ L'Ô', 'O' },{ L'Û', 'u' },
  { L'Ä', 'A' },{ L'Ë', 'E' },{ L'Ï', 'I' },{ L'Ö', 'O' },{ L'Ü', 'u' },
  { L'á', 'a' },{ L'é', 'e' },{ L'í', 'i' },{ L'ó', 'o' },{ L'ú', 'u' },
  { L'à', 'a' },{ L'è', 'e' },{ L'ì', 'i' },{ L'ò', 'o' },{ L'ù', 'u' },
  { L'â', 'a' },{ L'ê', 'e' },{ L'î', 'i' },{ L'ô', 'o' },{ L'û', 'u' },
  { L'ä', 'a' },{ L'ë', 'e' },{ L'ï', 'i' },{ L'ö', 'o' },{ L'ü', 'u' },
  { L'Ñ', 'N' },{ L'ñ', 'n' },{ L'Ç', 'C' },{ L'ç', 'c' },
  { L'æ', 'e' },{ L'œ', 'o' },
};

std::wstring Localization::wstring2ascii(const std::wstring& input)
{
  std::wstring wascii = input;
  for (auto& c : wascii) {
    if (to_ascii_map.find(c) != to_ascii_map.end()) {
      c = to_ascii_map[c];
    }
  }
  return wascii;
}

// ----------------------------------------------------------------------------
