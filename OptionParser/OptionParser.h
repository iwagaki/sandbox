#ifndef OPTION_PARSER_H_
#define OPTION_PARSER_H_

#include <cstdlib>
#include <vector>
#include <cstring>
#include "verify.h"

class OptionParser
{
public:
    OptionParser(const char* pIdentifier, const char* pDescription) :
		m_pIdentifier(pIdentifier),
		m_pDescription(pDescription)
    {
        m_optionList.push_back(this);
    }

    virtual ~OptionParser() {}

    static void helpAll()
    {
        printf("Options:\n");

        for (std::vector<OptionParser*>::iterator i = m_optionList.begin(); i != m_optionList.end(); ++i)
            (*i)->help();
    }

    static void parse(int argc, char** argv)
    {
        int index = 1;

        while (index < argc)
        {
            bool isFound = false;

            char* pParameter;
            const char* pIdentifier = parse1Param(index, pParameter, argc, argv);

            for (std::vector<OptionParser*>::iterator i = m_optionList.begin(); i != m_optionList.end(); ++i)
            {
                
                if (strcmp(pIdentifier, (*i)->m_pIdentifier) == 0)
                {
                    (*i)->setValue(pParameter);
                    isFound = true;
                    break;
                }
            }

            if (!isFound)
            {
                helpAll();
                exit(-1);
            }
        }
    }

    static const char* parse1Param(int& index, char*& pParameter, int argc, char** argv)
    {
        if (index >= argc)
            VERIFY(0);

        char* p = argv[index];

        if (*p++ != '-')
            VERIFY("invalid parameter" && 0);
 
        if (*p++ != '-')
            VERIFY("invalid parameter" && 0);

        ++index;
        pParameter = 0;

        if (index < argc)
        {
            char* p2 = argv[index];
        
            if (*p2 != '-')
            {
                pParameter = p2;
                ++index;
            }
        }
        
        return p;
    }

    virtual void help() = 0;
    virtual void setValue(char* pString) = 0;

protected:
    const char* m_pIdentifier;
    const char* m_pDescription;

    static std::vector<OptionParser*> m_optionList;
};


class OPTION_INT : public OptionParser
{
public:
    OPTION_INT(const char* pIdentifier, const char* pDescription, int defaultValue) : OptionParser(pIdentifier, pDescription), m_value(defaultValue) {}

    operator int() const { return m_value; }

    void help() { printf("    --%-30s  %s (%d)\n", m_pIdentifier, m_pDescription, m_value); }
    void setValue(char* pString) { VERIFY(pString != 0); m_value = atoi(pString); }

private:
    int m_value;
};

class OPTION_DOUBLE : public OptionParser
{
public:
    OPTION_DOUBLE(const char* pIdentifier, const char* pDescription, double defaultValue) : OptionParser(pIdentifier, pDescription), m_value(defaultValue) {}

    operator double() { return m_value; }

    void help() { printf("    --%-30s  %s (%f)\n", m_pIdentifier, m_pDescription, m_value); }
    void setValue(char* pString) { VERIFY(pString != 0); m_value = atof(pString); }

private:
    double m_value;
};

class OPTION_BOOL : public OptionParser
{
public:
    OPTION_BOOL(const char* pIdentifier, const char* pDescription) : OptionParser(pIdentifier, pDescription), m_value(false) {}

    operator bool() { return m_value; }

    void help() { printf("    --%-30s  %s\n", m_pIdentifier, m_pDescription); }
    void setValue(char* pString) { VERIFY(pString == 0); m_value = true; }

private:
    bool m_value;
};

class OPTION_STRING : public OptionParser
{
public:
    OPTION_STRING(const char* pIdentifier, const char* pDescription, const char* defaultValue) : OptionParser(pIdentifier, pDescription), m_value(defaultValue) {}

    operator const char*() { return m_value; }

    void help() { printf("    --%-30s  %s (%s)\n", m_pIdentifier, m_pDescription, m_value); }
    void setValue(char* pString) { VERIFY(pString != 0); m_value = pString; }

private:
    const char* m_value;
};

#endif // OPTION_PARSER_H_
