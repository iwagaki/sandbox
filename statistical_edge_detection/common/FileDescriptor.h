#ifndef FILE_DESCRIPTOR_H_
#define FILE_DESCRIPTOR_H_

#include <sys/stat.h>
#include <cstdio>
#include "verify.h"


class FileDescriptor
{
public:
    FileDescriptor(const char* filename, const char* attr = "wb") : m_fp(0)
    {
        VERIFY(filename != 0);

        m_fp = fopen(filename, attr);
        VERIFY(m_fp != 0);
    }

    virtual ~FileDescriptor()
    {
        if (m_fp)
        {
            int rc = fclose(m_fp);
            assert(rc == 0);
            m_fp = 0;
        }
    }

    operator FILE*() const
    {
        return fp();
    }

    FILE* fp() const
    {
        VERIFY(m_fp != 0);
        return m_fp;
    }

private:
    FILE* m_fp;
};


class FileReader : public FileDescriptor
{
public:
    FileReader(const char* filename) : FileDescriptor(filename, "rb") {}
};


class FileWriter : public FileDescriptor
{
public:
    FileWriter(const char* filename) : FileDescriptor(filename, "wb") {}
};


class FileLoader
{
public:
    FileLoader(const char* filename) : m_pBuffer(0)
    {
        FileReader loadFile(filename);

        size_t fileSize = getSize(filename);
        size_t loadSize = 0;

        m_pBuffer = new uint8_t[fileSize];

        loadSize = fread(m_pBuffer, 1, fileSize, loadFile);
        VERIFY(fileSize == loadSize);
    }

    ~FileLoader()
    {
        if (m_pBuffer)
            delete[] m_pBuffer;
    }

    static size_t getSize(const char* filename)
    {
        struct stat st;

        VERIFY(filename != 0);
        VERIFY(stat(filename, &st) == 0);

        return st.st_size;
    }

    operator uint8_t*() const
    {
        VERIFY(m_pBuffer != 0);

        return m_pBuffer;
    }

private:
    uint8_t* m_pBuffer;
};

#endif // FILE_DESCRIPTOR_H_
