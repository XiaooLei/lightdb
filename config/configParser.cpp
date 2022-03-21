//
// Created by 9 on 2022/3/19.
//
#include <iostream>
#include <fstream>
#include <assert.h>

#include "configParser.h"
#include "../util/str.h"

namespace lightdb{

        CConfigParser::CConfigParser()
        {

        }

        CConfigParser::~CConfigParser()
        {
            auto it = m_mpConfigData.begin();
            for (; it != m_mpConfigData.end(); it++) {
                map <string, string> *pSection = it->second;
                if (pSection) {
                    delete pSection;
                    it->second = NULL;
                }
            }
            m_mpConfigData.clear();
        }


        bool CConfigParser::Parser(const string & cFilePath)
        {
            ifstream input(cFilePath);
            if (!input) {
                cout << "cannot open input file" << endl;
                return false;
            }

            try {
                string cCurSection;
                for (string line; getline(input, line);) {
                    if (line.find_first_of("#") != string::npos) {
                        line.erase(line.find_first_of("#"));
                    }
                    if (line.empty()) continue;

                    if (line[0] == '[') {
                        if (line[line.size() - 1] != ']') {
                            throw runtime_error("section format error");
                        }
                        map <string, string> *pSection = new map<string, string>();
                        if (pSection == NULL) {
                            throw runtime_error("run out of memory");
                        }
                        string sectionName = string(line, 1, line.size() - 2);
                        m_mpConfigData.insert(make_pair(sectionName, pSection));
                        cCurSection = sectionName;
                    } else {
                        vector <string> kv = Split(line, "=");
                        if (kv.size() != 2) {
                            throw runtime_error("ini format error");
                        }
                        string k = Strim(kv[0], " ");
                        string v = Strim(kv[1], " ");
                        if (cCurSection.empty()) {
                            throw runtime_error("lack of section");
                        }
                        auto it = m_mpConfigData.find(cCurSection);
                        assert(it != m_mpConfigData.end());
                        it->second->insert(make_pair(k, v));
                    }
                }
                input.close();
                return true;
            }
            catch (runtime_error e) {
                cout << e.what() << endl;
                auto it = m_mpConfigData.begin();
                for (; it != m_mpConfigData.end(); it++) {
                    map <string, string> *pSection = it->second;
                    if (pSection) {
                        delete pSection;
                        it->second = NULL;
                    }
                }
                m_mpConfigData.clear();
                input.close();
                return false;
            }

        }

        bool CConfigParser::HasSection(const string & cSection)
        {
            return m_mpConfigData.find(cSection) != m_mpConfigData.end() ? true : false;
        }

        int CConfigParser::GetSections(vector<string> & vecSections)
        {
            for (auto it = m_mpConfigData.begin(); it != m_mpConfigData.end(); it++) {
                vecSections.push_back(it->first);
            }
            return 0;
        }

        int CConfigParser::GetKeys(const string & cSection, vector<string> & vecKeys)
        {
            auto it = m_mpConfigData.find(cSection);
            if (it != m_mpConfigData.end()) {
                map <string, string> *pSection = it->second;
                for (auto it2 = pSection->begin(); it2 != pSection->end(); it2++) {
                    vecKeys.push_back(it2->first);
                }
            }
            return 0;
        }

        const map<string, string> *CConfigParser::GetSectionConfig(const string & cSection)
        {
            auto it = m_mpConfigData.find(cSection);
            if (it != m_mpConfigData.end()) {
                return it->second;
            }
            return NULL;
        }

        string CConfigParser::GetConfig(const string & cSection, const string & cKey)
        {
            const map <string, string> *pSection = GetSectionConfig(cSection);
            if (pSection) {
                auto it = pSection->find(cKey);
                if (it != pSection->end()) {
                    return it->second;
                }
            }
            return "";
        }

        string CConfigParser::GetDefConfig(const string & cSection, const string & cKey, const string & cDef)
        {
            const map <string, string> *pSection = GetSectionConfig(cSection);
            if (pSection) {
                auto it = pSection->find(cKey);
                if (it != pSection->end()) {
                    return it->second;
                }
            }
            return cDef;
        }
}
