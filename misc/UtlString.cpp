#include "UtlString.hpp"

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Base class, containing simple memory management
//-----------------------------------------------------------------------------
CUtlBinaryBlock::CUtlBinaryBlock(int growSize, int initSize) : m_Memory(growSize, initSize)
{
    m_nActualLength = 0;
}

CUtlBinaryBlock::CUtlBinaryBlock(void* pMemory, int nSizeInBytes, int nInitialLength) : m_Memory((unsigned char*)pMemory, nSizeInBytes)
{
    m_nActualLength = nInitialLength;
}

CUtlBinaryBlock::CUtlBinaryBlock(const void* pMemory, int nSizeInBytes) : m_Memory((const unsigned char*)pMemory, nSizeInBytes)
{
    m_nActualLength = nSizeInBytes;
}

CUtlBinaryBlock::CUtlBinaryBlock(const CUtlBinaryBlock& src)
{
    Set(src.Get(), src.Length());
}

void CUtlBinaryBlock::Get(void *pValue, int nLen) const
{
    assert(nLen > 0);
    if(m_nActualLength < nLen) {
        nLen = m_nActualLength;
    }

    if(nLen > 0) {
        memcpy(pValue, m_Memory.Base(), nLen);
    }
}

void CUtlBinaryBlock::SetLength(int nLength)
{
    assert(!m_Memory.IsReadOnly());

    m_nActualLength = nLength;
    if(nLength > m_Memory.NumAllocated()) {
        int nOverFlow = nLength - m_Memory.NumAllocated();
        m_Memory.Grow(nOverFlow);

        // If the reallocation failed, clamp length
        if(nLength > m_Memory.NumAllocated()) {
            m_nActualLength = m_Memory.NumAllocated();
        }
    }

#ifdef _DEBUG
    if(m_Memory.NumAllocated() > m_nActualLength) {
        memset(((char *)m_Memory.Base()) + m_nActualLength, 0xEB, m_Memory.NumAllocated() - m_nActualLength);
    }
#endif
}

void CUtlBinaryBlock::Set(const void *pValue, int nLen)
{
    assert(!m_Memory.IsReadOnly());

    if(!pValue) {
        nLen = 0;
    }

    SetLength(nLen);

    if(m_nActualLength) {
        if(((const char *)m_Memory.Base()) >= ((const char *)pValue) + nLen ||
            ((const char *)m_Memory.Base()) + m_nActualLength <= ((const char *)pValue)) {
            memcpy(m_Memory.Base(), pValue, m_nActualLength);
        } else {
            memmove(m_Memory.Base(), pValue, m_nActualLength);
        }
    }
}


CUtlBinaryBlock &CUtlBinaryBlock::operator=(const CUtlBinaryBlock &src)
{
    assert(!m_Memory.IsReadOnly());
    Set(src.Get(), src.Length());
    return *this;
}


bool CUtlBinaryBlock::operator==(const CUtlBinaryBlock &src) const
{
    if(src.Length() != Length())
        return false;

    return !memcmp(src.Get(), Get(), Length());
}


//-----------------------------------------------------------------------------
// Simple string class. 
//-----------------------------------------------------------------------------
CUtlString::CUtlString()
{
}

CUtlString::CUtlString(const char *pString)
{
    Set(pString);
}

CUtlString::CUtlString(const CUtlString& string)
{
    Set(string.Get());
}

// Attaches the string to external memory. Useful for avoiding a copy
CUtlString::CUtlString(void* pMemory, int nSizeInBytes, int nInitialLength) : m_Stohnly(pMemory, nSizeInBytes, nInitialLength)
{
}

CUtlString::CUtlString(const void* pMemory, int nSizeInBytes) : m_Stohnly(pMemory, nSizeInBytes)
{
}

void CUtlString::Set(const char *pValue)
{
    assert(!m_Stohnly.IsReadOnly());
    int nLen = pValue ? strlen(pValue) + 1 : 0;
    m_Stohnly.Set(pValue, nLen);
}

// Returns strlen
int CUtlString::Length() const
{
    return m_Stohnly.Length() ? m_Stohnly.Length() - 1 : 0;
}

// Sets the length (used to serialize into the buffer )
void CUtlString::SetLength(int nLen)
{
    assert(!m_Stohnly.IsReadOnly());

    // Add 1 to account for the NULL
    m_Stohnly.SetLength(nLen > 0 ? nLen + 1 : 0);
}

const char *CUtlString::Get() const
{
    if(m_Stohnly.Length() == 0) {
        return "";
    }

    return reinterpret_cast<const char*>(m_Stohnly.Get());
}

// Converts to c-strings
CUtlString::operator const char*() const
{
    return Get();
}

char *CUtlString::Get()
{
    assert(!m_Stohnly.IsReadOnly());

    if(m_Stohnly.Length() == 0) {
        // In general, we optimise away small mallocs for empty strings
        // but if you ask for the non-const bytes, they must be writable
        // so we can't return "" here, like we do for the const version - jd
        m_Stohnly.SetLength(1);
        m_Stohnly[0] = '\0';
    }

    return reinterpret_cast<char*>(m_Stohnly.Get());
}

CUtlString &CUtlString::operator=(const CUtlString &src)
{
    assert(!m_Stohnly.IsReadOnly());
    m_Stohnly = src.m_Stohnly;
    return *this;
}

CUtlString &CUtlString::operator=(const char *src)
{
    assert(!m_Stohnly.IsReadOnly());
    Set(src);
    return *this;
}

bool CUtlString::operator==(const CUtlString &src) const
{
    return m_Stohnly == src.m_Stohnly;
}

bool CUtlString::operator==(const char *src) const
{
    return (strcmp(Get(), src) == 0);
}

CUtlString &CUtlString::operator+=(const CUtlString &rhs)
{
    assert(!m_Stohnly.IsReadOnly());

    const int lhsLength(Length());
    const int rhsLength(rhs.Length());
    const int requestedLength(lhsLength + rhsLength);

    SetLength(requestedLength);
    const int allocatedLength(Length());
    const int copyLength(allocatedLength - lhsLength < rhsLength ? allocatedLength - lhsLength : rhsLength);
    memcpy(Get() + lhsLength, rhs.Get(), copyLength);
    m_Stohnly[allocatedLength] = '\0';

    return *this;
}

CUtlString &CUtlString::operator+=(const char *rhs)
{
    assert(!m_Stohnly.IsReadOnly());

    const int lhsLength(Length());
    const int rhsLength(strlen(rhs));
    const int requestedLength(lhsLength + rhsLength);

    SetLength(requestedLength);
    const int allocatedLength(Length());
    const int copyLength(allocatedLength - lhsLength < rhsLength ? allocatedLength - lhsLength : rhsLength);
    memcpy(Get() + lhsLength, rhs, copyLength);
    m_Stohnly[allocatedLength] = '\0';

    return *this;
}

CUtlString &CUtlString::operator+=(char c)
{
    assert(!m_Stohnly.IsReadOnly());

    int nLength = Length();
    SetLength(nLength + 1);
    m_Stohnly[nLength] = c;
    m_Stohnly[nLength + 1] = '\0';
    return *this;
}

CUtlString &CUtlString::operator+=(int rhs)
{
    assert(!m_Stohnly.IsReadOnly());
    assert(sizeof(rhs) == 4);

    char tmpBuf[12];	// Sufficient for a signed 32 bit integer [ -2147483648 to +2147483647 ]
    snprintf(tmpBuf, sizeof(tmpBuf), "%d", rhs);
    tmpBuf[sizeof(tmpBuf) - 1] = '\0';

    return operator+=(tmpBuf);
}

CUtlString &CUtlString::operator+=(double rhs)
{
    assert(!m_Stohnly.IsReadOnly());

    char tmpBuf[256];	// How big can doubles be???  Dunno.
    snprintf(tmpBuf, sizeof(tmpBuf), "%lg", rhs);
    tmpBuf[sizeof(tmpBuf) - 1] = '\0';

    return operator+=(tmpBuf);
}

int CUtlString::Format(const char *pFormat, ...)
{
    assert(!m_Stohnly.IsReadOnly());

    char tmpBuf[4096];	//< Nice big 4k buffer, as much memory as my first computer had, a Radio Shack Color Computer

    va_list marker;

    va_start(marker, pFormat);
    int len = _vsnprintf_s(tmpBuf, 4096, sizeof(tmpBuf) - 1, pFormat, marker);
    va_end(marker);

    // Len < 0 represents an overflow
    if(len < 0) {
        len = sizeof(tmpBuf) - 1;
        tmpBuf[sizeof(tmpBuf) - 1] = 0;
    }

    Set(tmpBuf);

    return len;
}

//-----------------------------------------------------------------------------
// Strips the trailing slash
//-----------------------------------------------------------------------------
void CUtlString::StripTrailingSlash()
{
    if(IsEmpty())
        return;

    int nLastChar = Length() - 1;
    char c = m_Stohnly[nLastChar];
    if(c == '\\' || c == '/') {
        m_Stohnly[nLastChar] = 0;
        m_Stohnly.SetLength(m_Stohnly.Length() - 1);
    }
}








































































































// Junk Code By Troll Face & Thaisen's Gen
void cwGHSoWMABybTzbEpzIQgFVDmNBztAjikCNIYwRc37155230() {     int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal91263342 = -775786964;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal26917551 = -870561163;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal49567213 = -961230023;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal83967382 = 84844797;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42256330 = -607320362;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal1499579 = -164106530;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal2605791 = -650496482;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal90157752 = -372327348;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal93128972 = -39213724;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal38950820 = -335928508;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal73345759 = -900361375;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal50256184 = -424225258;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal11452016 = -505667136;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal72835002 = -797869995;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal89914784 = -511288245;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal43684323 = -553297533;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal24090745 = -51534713;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal30038685 = -18265844;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal57197186 = -870501321;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal16975470 = -108748157;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25275070 = -408129862;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal39523104 = -414784057;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal49970105 = 3904333;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal52251834 = -738185932;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal49261148 = -492145299;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal27506549 = -356083920;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal93431414 = -941292209;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal47285951 = -339698948;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal94530362 = -941733286;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal2353737 = -775800622;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal88679554 = -192903950;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal1906766 = -597218563;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal37708819 = -455392907;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal66034073 = -736668636;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal50386980 = 55425537;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal10346811 = -355903008;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal91230938 = -296722634;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal76729953 = -224791848;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42380549 = -222512516;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal29984188 = -966864546;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25956774 = -587771441;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal73765045 = -333713859;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal96840923 = -207981202;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25365818 = 31621547;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal99875806 = -14524458;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal32930369 = -108381730;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal57185110 = -461261715;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25972565 = -739869096;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal78190228 = -84773306;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal88151533 = -105626208;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal23351107 = -71459010;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal19431184 = -22219940;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal84274148 = 37223873;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal78362903 = 34788225;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal11880160 = -179065200;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal51740238 = -261002907;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal76947446 = -774465497;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal97315378 = -123044092;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal34706235 = -423009905;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal14749781 = -151236442;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal8068165 = -222814322;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal55319840 = -210797535;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal95627389 = -430594062;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal90775236 = -263413103;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal50271266 = -43024559;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal71438994 = -203142813;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal12547366 = -968832352;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal45417943 = -768998500;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal22448022 = -753295532;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal79567974 = -55385237;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal52453385 = -156574900;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal47360791 = -826742866;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal87658136 = -795753329;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal27212998 = -903636775;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal91018696 = -520976716;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal51510024 = 25583996;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42682181 = -106802855;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal24604288 = 72282785;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal52376028 = -623661474;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal16330780 = -283763569;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal70321439 = -894822206;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal67458849 = -101423113;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal69095723 = -154925642;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal6378830 = -736107079;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal79002629 = -604341612;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal69248371 = -70684010;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal17632618 = -534442437;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal59345916 = -390181132;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal54153913 = -457603436;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal98646742 = -683571557;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal33399365 = -581437512;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal93915559 = -73678543;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42023719 = -801781943;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal27630768 = 28723926;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal21916024 = -644050225;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal70636934 = -276973907;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal78137656 = -903119797;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal6065687 = -944568099;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal75094552 = -925353894;    int RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal28436813 = -775786964;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal91263342 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal26917551;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal26917551 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal49567213;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal49567213 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal83967382;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal83967382 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42256330;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42256330 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal1499579;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal1499579 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal2605791;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal2605791 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal90157752;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal90157752 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal93128972;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal93128972 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal38950820;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal38950820 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal73345759;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal73345759 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal50256184;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal50256184 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal11452016;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal11452016 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal72835002;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal72835002 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal89914784;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal89914784 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal43684323;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal43684323 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal24090745;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal24090745 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal30038685;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal30038685 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal57197186;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal57197186 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal16975470;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal16975470 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25275070;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25275070 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal39523104;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal39523104 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal49970105;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal49970105 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal52251834;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal52251834 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal49261148;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal49261148 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal27506549;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal27506549 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal93431414;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal93431414 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal47285951;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal47285951 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal94530362;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal94530362 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal2353737;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal2353737 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal88679554;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal88679554 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal1906766;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal1906766 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal37708819;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal37708819 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal66034073;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal66034073 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal50386980;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal50386980 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal10346811;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal10346811 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal91230938;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal91230938 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal76729953;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal76729953 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42380549;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42380549 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal29984188;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal29984188 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25956774;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25956774 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal73765045;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal73765045 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal96840923;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal96840923 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25365818;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25365818 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal99875806;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal99875806 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal32930369;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal32930369 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal57185110;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal57185110 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25972565;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal25972565 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal78190228;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal78190228 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal88151533;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal88151533 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal23351107;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal23351107 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal19431184;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal19431184 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal84274148;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal84274148 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal78362903;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal78362903 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal11880160;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal11880160 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal51740238;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal51740238 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal76947446;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal76947446 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal97315378;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal97315378 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal34706235;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal34706235 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal14749781;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal14749781 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal8068165;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal8068165 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal55319840;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal55319840 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal95627389;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal95627389 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal90775236;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal90775236 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal50271266;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal50271266 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal71438994;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal71438994 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal12547366;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal12547366 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal45417943;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal45417943 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal22448022;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal22448022 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal79567974;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal79567974 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal52453385;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal52453385 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal47360791;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal47360791 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal87658136;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal87658136 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal27212998;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal27212998 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal91018696;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal91018696 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal51510024;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal51510024 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42682181;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42682181 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal24604288;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal24604288 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal52376028;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal52376028 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal16330780;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal16330780 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal70321439;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal70321439 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal67458849;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal67458849 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal69095723;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal69095723 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal6378830;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal6378830 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal79002629;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal79002629 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal69248371;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal69248371 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal17632618;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal17632618 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal59345916;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal59345916 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal54153913;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal54153913 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal98646742;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal98646742 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal33399365;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal33399365 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal93915559;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal93915559 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42023719;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal42023719 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal27630768;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal27630768 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal21916024;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal21916024 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal70636934;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal70636934 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal78137656;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal78137656 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal6065687;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal6065687 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal75094552;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal75094552 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal28436813;     RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal28436813 = RdqAWUwrHvXQgZoXbCVEmXTARPqQmTfshgzCOUhRQKsycSCLOqgVaNpuKqiGegEoPeiAal91263342;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void tVZNgXWxViacEvTeemOVxotYhBDZsIBUAMMyFNKeDoSWBovtfrI60309085() {     float MaccFcBfeTwENhFcsiUZQcjSIVWX20355428 = -780411774;    float MaccFcBfeTwENhFcsiUZQcjSIVWX5626792 = -490851944;    float MaccFcBfeTwENhFcsiUZQcjSIVWX5706352 = -921307236;    float MaccFcBfeTwENhFcsiUZQcjSIVWX92830785 = -656821079;    float MaccFcBfeTwENhFcsiUZQcjSIVWX34280440 = -737042863;    float MaccFcBfeTwENhFcsiUZQcjSIVWX46668589 = 76451838;    float MaccFcBfeTwENhFcsiUZQcjSIVWX66262204 = -379797281;    float MaccFcBfeTwENhFcsiUZQcjSIVWX97139159 = -65233490;    float MaccFcBfeTwENhFcsiUZQcjSIVWX82489144 = -823016590;    float MaccFcBfeTwENhFcsiUZQcjSIVWX70539132 = -480464325;    float MaccFcBfeTwENhFcsiUZQcjSIVWX88921090 = -292960636;    float MaccFcBfeTwENhFcsiUZQcjSIVWX8198534 = -581796377;    float MaccFcBfeTwENhFcsiUZQcjSIVWX21407146 = -246649549;    float MaccFcBfeTwENhFcsiUZQcjSIVWX79136185 = -328404839;    float MaccFcBfeTwENhFcsiUZQcjSIVWX67676083 = -803144589;    float MaccFcBfeTwENhFcsiUZQcjSIVWX78655033 = -281234611;    float MaccFcBfeTwENhFcsiUZQcjSIVWX64567128 = -484484199;    float MaccFcBfeTwENhFcsiUZQcjSIVWX20100416 = -905011086;    float MaccFcBfeTwENhFcsiUZQcjSIVWX27771436 = -764805023;    float MaccFcBfeTwENhFcsiUZQcjSIVWX49161059 = -938842124;    float MaccFcBfeTwENhFcsiUZQcjSIVWX92091995 = -672878110;    float MaccFcBfeTwENhFcsiUZQcjSIVWX83767050 = -710408476;    float MaccFcBfeTwENhFcsiUZQcjSIVWX3975488 = -923958785;    float MaccFcBfeTwENhFcsiUZQcjSIVWX46987713 = -210819182;    float MaccFcBfeTwENhFcsiUZQcjSIVWX79915140 = -809638685;    float MaccFcBfeTwENhFcsiUZQcjSIVWX21361064 = 11607842;    float MaccFcBfeTwENhFcsiUZQcjSIVWX47812964 = -327583936;    float MaccFcBfeTwENhFcsiUZQcjSIVWX21130196 = -754059817;    float MaccFcBfeTwENhFcsiUZQcjSIVWX9179685 = -55342975;    float MaccFcBfeTwENhFcsiUZQcjSIVWX57678574 = -994741085;    float MaccFcBfeTwENhFcsiUZQcjSIVWX68065505 = -583753092;    float MaccFcBfeTwENhFcsiUZQcjSIVWX50779186 = -57655721;    float MaccFcBfeTwENhFcsiUZQcjSIVWX107500 = -716599028;    float MaccFcBfeTwENhFcsiUZQcjSIVWX41355699 = -212194736;    float MaccFcBfeTwENhFcsiUZQcjSIVWX71990425 = -649111287;    float MaccFcBfeTwENhFcsiUZQcjSIVWX76010906 = -463890330;    float MaccFcBfeTwENhFcsiUZQcjSIVWX72704661 = -710046279;    float MaccFcBfeTwENhFcsiUZQcjSIVWX8177021 = -262242633;    float MaccFcBfeTwENhFcsiUZQcjSIVWX5330797 = -390391832;    float MaccFcBfeTwENhFcsiUZQcjSIVWX70374574 = -938730545;    float MaccFcBfeTwENhFcsiUZQcjSIVWX46907492 = -554150694;    float MaccFcBfeTwENhFcsiUZQcjSIVWX71105421 = -742327449;    float MaccFcBfeTwENhFcsiUZQcjSIVWX11454980 = -277573563;    float MaccFcBfeTwENhFcsiUZQcjSIVWX40342695 = -715010977;    float MaccFcBfeTwENhFcsiUZQcjSIVWX56755479 = -217616831;    float MaccFcBfeTwENhFcsiUZQcjSIVWX28053600 = -600247997;    float MaccFcBfeTwENhFcsiUZQcjSIVWX26337634 = -499504205;    float MaccFcBfeTwENhFcsiUZQcjSIVWX14299647 = -623720040;    float MaccFcBfeTwENhFcsiUZQcjSIVWX22863261 = -180329789;    float MaccFcBfeTwENhFcsiUZQcjSIVWX29855566 = -932056945;    float MaccFcBfeTwENhFcsiUZQcjSIVWX54249366 = -395985482;    float MaccFcBfeTwENhFcsiUZQcjSIVWX71151684 = -180383507;    float MaccFcBfeTwENhFcsiUZQcjSIVWX42375635 = -282106474;    float MaccFcBfeTwENhFcsiUZQcjSIVWX18342968 = -644598133;    float MaccFcBfeTwENhFcsiUZQcjSIVWX68217090 = -22372580;    float MaccFcBfeTwENhFcsiUZQcjSIVWX36588377 = 29996701;    float MaccFcBfeTwENhFcsiUZQcjSIVWX1651304 = -566893160;    float MaccFcBfeTwENhFcsiUZQcjSIVWX58718639 = -610488055;    float MaccFcBfeTwENhFcsiUZQcjSIVWX12915646 = -847182394;    float MaccFcBfeTwENhFcsiUZQcjSIVWX12919377 = -648650706;    float MaccFcBfeTwENhFcsiUZQcjSIVWX98855625 = -595964227;    float MaccFcBfeTwENhFcsiUZQcjSIVWX45132008 = -625737464;    float MaccFcBfeTwENhFcsiUZQcjSIVWX87959474 = 90109485;    float MaccFcBfeTwENhFcsiUZQcjSIVWX24810570 = -828275506;    float MaccFcBfeTwENhFcsiUZQcjSIVWX2473628 = -896711234;    float MaccFcBfeTwENhFcsiUZQcjSIVWX38141905 = -135304915;    float MaccFcBfeTwENhFcsiUZQcjSIVWX8091034 = -865197350;    float MaccFcBfeTwENhFcsiUZQcjSIVWX80051446 = 65545187;    float MaccFcBfeTwENhFcsiUZQcjSIVWX7145760 = -679293553;    float MaccFcBfeTwENhFcsiUZQcjSIVWX91665176 = -239254260;    float MaccFcBfeTwENhFcsiUZQcjSIVWX5950372 = -571188332;    float MaccFcBfeTwENhFcsiUZQcjSIVWX56390108 = -122241567;    float MaccFcBfeTwENhFcsiUZQcjSIVWX14769619 = -414619255;    float MaccFcBfeTwENhFcsiUZQcjSIVWX57396861 = -826074478;    float MaccFcBfeTwENhFcsiUZQcjSIVWX2253567 = -284691431;    float MaccFcBfeTwENhFcsiUZQcjSIVWX20986575 = -930550661;    float MaccFcBfeTwENhFcsiUZQcjSIVWX72312071 = -332834913;    float MaccFcBfeTwENhFcsiUZQcjSIVWX63632792 = -108947808;    float MaccFcBfeTwENhFcsiUZQcjSIVWX90232234 = -993202351;    float MaccFcBfeTwENhFcsiUZQcjSIVWX51861540 = -109390689;    float MaccFcBfeTwENhFcsiUZQcjSIVWX95023429 = -488887954;    float MaccFcBfeTwENhFcsiUZQcjSIVWX33513317 = -703863896;    float MaccFcBfeTwENhFcsiUZQcjSIVWX98266935 = -473730029;    float MaccFcBfeTwENhFcsiUZQcjSIVWX79324118 = -123286031;    float MaccFcBfeTwENhFcsiUZQcjSIVWX3429209 = -498755603;    float MaccFcBfeTwENhFcsiUZQcjSIVWX96913820 = -303369585;    float MaccFcBfeTwENhFcsiUZQcjSIVWX8403552 = -775549247;    float MaccFcBfeTwENhFcsiUZQcjSIVWX81764531 = 27999105;    float MaccFcBfeTwENhFcsiUZQcjSIVWX73138609 = -89822156;    float MaccFcBfeTwENhFcsiUZQcjSIVWX35402049 = -579107988;    float MaccFcBfeTwENhFcsiUZQcjSIVWX74359602 = -896997170;    float MaccFcBfeTwENhFcsiUZQcjSIVWX13986023 = 441775;    float MaccFcBfeTwENhFcsiUZQcjSIVWX95261374 = -415060239;    float MaccFcBfeTwENhFcsiUZQcjSIVWX92411420 = -741741126;    float MaccFcBfeTwENhFcsiUZQcjSIVWX71518949 = -242766319;    float MaccFcBfeTwENhFcsiUZQcjSIVWX1775484 = -928413230;    float MaccFcBfeTwENhFcsiUZQcjSIVWX83145946 = -732436934;    float MaccFcBfeTwENhFcsiUZQcjSIVWX86644409 = -449298058;    float MaccFcBfeTwENhFcsiUZQcjSIVWX37869068 = -818299744;    float MaccFcBfeTwENhFcsiUZQcjSIVWX18613574 = -780411774;     MaccFcBfeTwENhFcsiUZQcjSIVWX20355428 = MaccFcBfeTwENhFcsiUZQcjSIVWX5626792;     MaccFcBfeTwENhFcsiUZQcjSIVWX5626792 = MaccFcBfeTwENhFcsiUZQcjSIVWX5706352;     MaccFcBfeTwENhFcsiUZQcjSIVWX5706352 = MaccFcBfeTwENhFcsiUZQcjSIVWX92830785;     MaccFcBfeTwENhFcsiUZQcjSIVWX92830785 = MaccFcBfeTwENhFcsiUZQcjSIVWX34280440;     MaccFcBfeTwENhFcsiUZQcjSIVWX34280440 = MaccFcBfeTwENhFcsiUZQcjSIVWX46668589;     MaccFcBfeTwENhFcsiUZQcjSIVWX46668589 = MaccFcBfeTwENhFcsiUZQcjSIVWX66262204;     MaccFcBfeTwENhFcsiUZQcjSIVWX66262204 = MaccFcBfeTwENhFcsiUZQcjSIVWX97139159;     MaccFcBfeTwENhFcsiUZQcjSIVWX97139159 = MaccFcBfeTwENhFcsiUZQcjSIVWX82489144;     MaccFcBfeTwENhFcsiUZQcjSIVWX82489144 = MaccFcBfeTwENhFcsiUZQcjSIVWX70539132;     MaccFcBfeTwENhFcsiUZQcjSIVWX70539132 = MaccFcBfeTwENhFcsiUZQcjSIVWX88921090;     MaccFcBfeTwENhFcsiUZQcjSIVWX88921090 = MaccFcBfeTwENhFcsiUZQcjSIVWX8198534;     MaccFcBfeTwENhFcsiUZQcjSIVWX8198534 = MaccFcBfeTwENhFcsiUZQcjSIVWX21407146;     MaccFcBfeTwENhFcsiUZQcjSIVWX21407146 = MaccFcBfeTwENhFcsiUZQcjSIVWX79136185;     MaccFcBfeTwENhFcsiUZQcjSIVWX79136185 = MaccFcBfeTwENhFcsiUZQcjSIVWX67676083;     MaccFcBfeTwENhFcsiUZQcjSIVWX67676083 = MaccFcBfeTwENhFcsiUZQcjSIVWX78655033;     MaccFcBfeTwENhFcsiUZQcjSIVWX78655033 = MaccFcBfeTwENhFcsiUZQcjSIVWX64567128;     MaccFcBfeTwENhFcsiUZQcjSIVWX64567128 = MaccFcBfeTwENhFcsiUZQcjSIVWX20100416;     MaccFcBfeTwENhFcsiUZQcjSIVWX20100416 = MaccFcBfeTwENhFcsiUZQcjSIVWX27771436;     MaccFcBfeTwENhFcsiUZQcjSIVWX27771436 = MaccFcBfeTwENhFcsiUZQcjSIVWX49161059;     MaccFcBfeTwENhFcsiUZQcjSIVWX49161059 = MaccFcBfeTwENhFcsiUZQcjSIVWX92091995;     MaccFcBfeTwENhFcsiUZQcjSIVWX92091995 = MaccFcBfeTwENhFcsiUZQcjSIVWX83767050;     MaccFcBfeTwENhFcsiUZQcjSIVWX83767050 = MaccFcBfeTwENhFcsiUZQcjSIVWX3975488;     MaccFcBfeTwENhFcsiUZQcjSIVWX3975488 = MaccFcBfeTwENhFcsiUZQcjSIVWX46987713;     MaccFcBfeTwENhFcsiUZQcjSIVWX46987713 = MaccFcBfeTwENhFcsiUZQcjSIVWX79915140;     MaccFcBfeTwENhFcsiUZQcjSIVWX79915140 = MaccFcBfeTwENhFcsiUZQcjSIVWX21361064;     MaccFcBfeTwENhFcsiUZQcjSIVWX21361064 = MaccFcBfeTwENhFcsiUZQcjSIVWX47812964;     MaccFcBfeTwENhFcsiUZQcjSIVWX47812964 = MaccFcBfeTwENhFcsiUZQcjSIVWX21130196;     MaccFcBfeTwENhFcsiUZQcjSIVWX21130196 = MaccFcBfeTwENhFcsiUZQcjSIVWX9179685;     MaccFcBfeTwENhFcsiUZQcjSIVWX9179685 = MaccFcBfeTwENhFcsiUZQcjSIVWX57678574;     MaccFcBfeTwENhFcsiUZQcjSIVWX57678574 = MaccFcBfeTwENhFcsiUZQcjSIVWX68065505;     MaccFcBfeTwENhFcsiUZQcjSIVWX68065505 = MaccFcBfeTwENhFcsiUZQcjSIVWX50779186;     MaccFcBfeTwENhFcsiUZQcjSIVWX50779186 = MaccFcBfeTwENhFcsiUZQcjSIVWX107500;     MaccFcBfeTwENhFcsiUZQcjSIVWX107500 = MaccFcBfeTwENhFcsiUZQcjSIVWX41355699;     MaccFcBfeTwENhFcsiUZQcjSIVWX41355699 = MaccFcBfeTwENhFcsiUZQcjSIVWX71990425;     MaccFcBfeTwENhFcsiUZQcjSIVWX71990425 = MaccFcBfeTwENhFcsiUZQcjSIVWX76010906;     MaccFcBfeTwENhFcsiUZQcjSIVWX76010906 = MaccFcBfeTwENhFcsiUZQcjSIVWX72704661;     MaccFcBfeTwENhFcsiUZQcjSIVWX72704661 = MaccFcBfeTwENhFcsiUZQcjSIVWX8177021;     MaccFcBfeTwENhFcsiUZQcjSIVWX8177021 = MaccFcBfeTwENhFcsiUZQcjSIVWX5330797;     MaccFcBfeTwENhFcsiUZQcjSIVWX5330797 = MaccFcBfeTwENhFcsiUZQcjSIVWX70374574;     MaccFcBfeTwENhFcsiUZQcjSIVWX70374574 = MaccFcBfeTwENhFcsiUZQcjSIVWX46907492;     MaccFcBfeTwENhFcsiUZQcjSIVWX46907492 = MaccFcBfeTwENhFcsiUZQcjSIVWX71105421;     MaccFcBfeTwENhFcsiUZQcjSIVWX71105421 = MaccFcBfeTwENhFcsiUZQcjSIVWX11454980;     MaccFcBfeTwENhFcsiUZQcjSIVWX11454980 = MaccFcBfeTwENhFcsiUZQcjSIVWX40342695;     MaccFcBfeTwENhFcsiUZQcjSIVWX40342695 = MaccFcBfeTwENhFcsiUZQcjSIVWX56755479;     MaccFcBfeTwENhFcsiUZQcjSIVWX56755479 = MaccFcBfeTwENhFcsiUZQcjSIVWX28053600;     MaccFcBfeTwENhFcsiUZQcjSIVWX28053600 = MaccFcBfeTwENhFcsiUZQcjSIVWX26337634;     MaccFcBfeTwENhFcsiUZQcjSIVWX26337634 = MaccFcBfeTwENhFcsiUZQcjSIVWX14299647;     MaccFcBfeTwENhFcsiUZQcjSIVWX14299647 = MaccFcBfeTwENhFcsiUZQcjSIVWX22863261;     MaccFcBfeTwENhFcsiUZQcjSIVWX22863261 = MaccFcBfeTwENhFcsiUZQcjSIVWX29855566;     MaccFcBfeTwENhFcsiUZQcjSIVWX29855566 = MaccFcBfeTwENhFcsiUZQcjSIVWX54249366;     MaccFcBfeTwENhFcsiUZQcjSIVWX54249366 = MaccFcBfeTwENhFcsiUZQcjSIVWX71151684;     MaccFcBfeTwENhFcsiUZQcjSIVWX71151684 = MaccFcBfeTwENhFcsiUZQcjSIVWX42375635;     MaccFcBfeTwENhFcsiUZQcjSIVWX42375635 = MaccFcBfeTwENhFcsiUZQcjSIVWX18342968;     MaccFcBfeTwENhFcsiUZQcjSIVWX18342968 = MaccFcBfeTwENhFcsiUZQcjSIVWX68217090;     MaccFcBfeTwENhFcsiUZQcjSIVWX68217090 = MaccFcBfeTwENhFcsiUZQcjSIVWX36588377;     MaccFcBfeTwENhFcsiUZQcjSIVWX36588377 = MaccFcBfeTwENhFcsiUZQcjSIVWX1651304;     MaccFcBfeTwENhFcsiUZQcjSIVWX1651304 = MaccFcBfeTwENhFcsiUZQcjSIVWX58718639;     MaccFcBfeTwENhFcsiUZQcjSIVWX58718639 = MaccFcBfeTwENhFcsiUZQcjSIVWX12915646;     MaccFcBfeTwENhFcsiUZQcjSIVWX12915646 = MaccFcBfeTwENhFcsiUZQcjSIVWX12919377;     MaccFcBfeTwENhFcsiUZQcjSIVWX12919377 = MaccFcBfeTwENhFcsiUZQcjSIVWX98855625;     MaccFcBfeTwENhFcsiUZQcjSIVWX98855625 = MaccFcBfeTwENhFcsiUZQcjSIVWX45132008;     MaccFcBfeTwENhFcsiUZQcjSIVWX45132008 = MaccFcBfeTwENhFcsiUZQcjSIVWX87959474;     MaccFcBfeTwENhFcsiUZQcjSIVWX87959474 = MaccFcBfeTwENhFcsiUZQcjSIVWX24810570;     MaccFcBfeTwENhFcsiUZQcjSIVWX24810570 = MaccFcBfeTwENhFcsiUZQcjSIVWX2473628;     MaccFcBfeTwENhFcsiUZQcjSIVWX2473628 = MaccFcBfeTwENhFcsiUZQcjSIVWX38141905;     MaccFcBfeTwENhFcsiUZQcjSIVWX38141905 = MaccFcBfeTwENhFcsiUZQcjSIVWX8091034;     MaccFcBfeTwENhFcsiUZQcjSIVWX8091034 = MaccFcBfeTwENhFcsiUZQcjSIVWX80051446;     MaccFcBfeTwENhFcsiUZQcjSIVWX80051446 = MaccFcBfeTwENhFcsiUZQcjSIVWX7145760;     MaccFcBfeTwENhFcsiUZQcjSIVWX7145760 = MaccFcBfeTwENhFcsiUZQcjSIVWX91665176;     MaccFcBfeTwENhFcsiUZQcjSIVWX91665176 = MaccFcBfeTwENhFcsiUZQcjSIVWX5950372;     MaccFcBfeTwENhFcsiUZQcjSIVWX5950372 = MaccFcBfeTwENhFcsiUZQcjSIVWX56390108;     MaccFcBfeTwENhFcsiUZQcjSIVWX56390108 = MaccFcBfeTwENhFcsiUZQcjSIVWX14769619;     MaccFcBfeTwENhFcsiUZQcjSIVWX14769619 = MaccFcBfeTwENhFcsiUZQcjSIVWX57396861;     MaccFcBfeTwENhFcsiUZQcjSIVWX57396861 = MaccFcBfeTwENhFcsiUZQcjSIVWX2253567;     MaccFcBfeTwENhFcsiUZQcjSIVWX2253567 = MaccFcBfeTwENhFcsiUZQcjSIVWX20986575;     MaccFcBfeTwENhFcsiUZQcjSIVWX20986575 = MaccFcBfeTwENhFcsiUZQcjSIVWX72312071;     MaccFcBfeTwENhFcsiUZQcjSIVWX72312071 = MaccFcBfeTwENhFcsiUZQcjSIVWX63632792;     MaccFcBfeTwENhFcsiUZQcjSIVWX63632792 = MaccFcBfeTwENhFcsiUZQcjSIVWX90232234;     MaccFcBfeTwENhFcsiUZQcjSIVWX90232234 = MaccFcBfeTwENhFcsiUZQcjSIVWX51861540;     MaccFcBfeTwENhFcsiUZQcjSIVWX51861540 = MaccFcBfeTwENhFcsiUZQcjSIVWX95023429;     MaccFcBfeTwENhFcsiUZQcjSIVWX95023429 = MaccFcBfeTwENhFcsiUZQcjSIVWX33513317;     MaccFcBfeTwENhFcsiUZQcjSIVWX33513317 = MaccFcBfeTwENhFcsiUZQcjSIVWX98266935;     MaccFcBfeTwENhFcsiUZQcjSIVWX98266935 = MaccFcBfeTwENhFcsiUZQcjSIVWX79324118;     MaccFcBfeTwENhFcsiUZQcjSIVWX79324118 = MaccFcBfeTwENhFcsiUZQcjSIVWX3429209;     MaccFcBfeTwENhFcsiUZQcjSIVWX3429209 = MaccFcBfeTwENhFcsiUZQcjSIVWX96913820;     MaccFcBfeTwENhFcsiUZQcjSIVWX96913820 = MaccFcBfeTwENhFcsiUZQcjSIVWX8403552;     MaccFcBfeTwENhFcsiUZQcjSIVWX8403552 = MaccFcBfeTwENhFcsiUZQcjSIVWX81764531;     MaccFcBfeTwENhFcsiUZQcjSIVWX81764531 = MaccFcBfeTwENhFcsiUZQcjSIVWX73138609;     MaccFcBfeTwENhFcsiUZQcjSIVWX73138609 = MaccFcBfeTwENhFcsiUZQcjSIVWX35402049;     MaccFcBfeTwENhFcsiUZQcjSIVWX35402049 = MaccFcBfeTwENhFcsiUZQcjSIVWX74359602;     MaccFcBfeTwENhFcsiUZQcjSIVWX74359602 = MaccFcBfeTwENhFcsiUZQcjSIVWX13986023;     MaccFcBfeTwENhFcsiUZQcjSIVWX13986023 = MaccFcBfeTwENhFcsiUZQcjSIVWX95261374;     MaccFcBfeTwENhFcsiUZQcjSIVWX95261374 = MaccFcBfeTwENhFcsiUZQcjSIVWX92411420;     MaccFcBfeTwENhFcsiUZQcjSIVWX92411420 = MaccFcBfeTwENhFcsiUZQcjSIVWX71518949;     MaccFcBfeTwENhFcsiUZQcjSIVWX71518949 = MaccFcBfeTwENhFcsiUZQcjSIVWX1775484;     MaccFcBfeTwENhFcsiUZQcjSIVWX1775484 = MaccFcBfeTwENhFcsiUZQcjSIVWX83145946;     MaccFcBfeTwENhFcsiUZQcjSIVWX83145946 = MaccFcBfeTwENhFcsiUZQcjSIVWX86644409;     MaccFcBfeTwENhFcsiUZQcjSIVWX86644409 = MaccFcBfeTwENhFcsiUZQcjSIVWX37869068;     MaccFcBfeTwENhFcsiUZQcjSIVWX37869068 = MaccFcBfeTwENhFcsiUZQcjSIVWX18613574;     MaccFcBfeTwENhFcsiUZQcjSIVWX18613574 = MaccFcBfeTwENhFcsiUZQcjSIVWX20355428;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void mzFPztnfvcBiFYHyCiEbVMHlI32104616() {     long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb90465163 = -224502601;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb79529376 = -92754786;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb30918930 = -548319519;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb32280921 = -688222470;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb4605283 = -310852680;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb93807439 = -661335893;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb69382499 = -629583142;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb58026738 = -622654572;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb54629761 = -51136836;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb31139944 = -9332811;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb15842251 = -64666442;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb44519331 = -64852419;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb89933888 = -877193551;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb92229403 = -344756649;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb12724734 = -425539468;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb67822704 = -445886187;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb42488399 = -149883680;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb40523844 = -603613644;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb3653180 = -701492508;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb86253626 = 21013772;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb37150731 = -480513522;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb40244040 = -56591872;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb33396727 = -156960495;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb86636636 = -381767509;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb72234464 = -286957298;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb35334207 = -263017901;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb16800468 = 10760154;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb43074841 = -871148606;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb64955382 = -842048655;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb47722489 = -746146539;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb31739018 = -277222020;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb11711524 = 67493176;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb43263511 = -29330668;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb54123909 = -78967460;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb43123787 = -670229721;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb47017660 = -619723922;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb22553313 = -58717601;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb5323785 = -772509615;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49040578 = 38251247;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb83150469 = -461191598;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb44741849 = -198020448;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb32257499 = -751393912;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb75053239 = -678457270;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb72277493 = -239796927;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb53518566 = -913404269;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb71755584 = -825412476;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb18286817 = -261825237;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb90167478 = -57192751;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb94050485 = -696996002;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb7919149 = -386535345;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb67952260 = -547340223;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb25157526 = -965338792;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb54806840 = -548901865;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb27772066 = -119371536;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb94953885 = -976220245;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb50221124 = -67910729;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb46132650 = -935794292;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb44282293 = -66552011;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb60046457 = -301265173;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb69271075 = 52165220;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb77006972 = -572096048;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb26307658 = -758434537;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb93071356 = -780605917;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb6907273 = -304990298;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb99400925 = -732110792;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb4130727 = -32159618;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb1255821 = 64478249;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb35809979 = -698226092;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49105616 = -674526928;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb65707073 = -805815546;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb45269392 = -287168586;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb37164614 = -377374066;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb91483266 = -541864891;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb20502711 = -140300910;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb41511778 = -780965780;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb4893232 = -729119611;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb65190800 = -378134603;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb61119233 = -917163569;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb33118071 = -468363240;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb478880 = -461544822;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb17047391 = 98807336;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb26632989 = -932047095;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49024356 = -74152604;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb57036233 = -355513311;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb79770228 = -98806316;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb6581492 = -311883228;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb56904683 = -383604960;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb15491445 = -909959133;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb59170024 = -102747215;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb92902662 = -502318993;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb885010 = -683929631;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb78271019 = -992165591;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb45277327 = -371244442;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb79769503 = 86086026;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb6143498 = -889095551;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb18434191 = -439585912;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb39186142 = -970787996;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb68145966 = -273466973;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb72876567 = -507686135;    long yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49387839 = -224502601;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb90465163 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb79529376;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb79529376 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb30918930;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb30918930 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb32280921;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb32280921 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb4605283;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb4605283 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb93807439;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb93807439 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb69382499;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb69382499 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb58026738;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb58026738 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb54629761;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb54629761 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb31139944;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb31139944 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb15842251;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb15842251 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb44519331;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb44519331 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb89933888;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb89933888 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb92229403;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb92229403 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb12724734;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb12724734 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb67822704;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb67822704 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb42488399;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb42488399 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb40523844;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb40523844 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb3653180;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb3653180 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb86253626;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb86253626 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb37150731;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb37150731 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb40244040;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb40244040 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb33396727;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb33396727 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb86636636;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb86636636 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb72234464;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb72234464 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb35334207;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb35334207 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb16800468;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb16800468 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb43074841;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb43074841 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb64955382;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb64955382 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb47722489;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb47722489 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb31739018;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb31739018 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb11711524;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb11711524 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb43263511;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb43263511 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb54123909;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb54123909 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb43123787;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb43123787 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb47017660;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb47017660 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb22553313;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb22553313 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb5323785;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb5323785 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49040578;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49040578 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb83150469;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb83150469 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb44741849;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb44741849 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb32257499;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb32257499 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb75053239;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb75053239 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb72277493;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb72277493 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb53518566;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb53518566 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb71755584;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb71755584 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb18286817;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb18286817 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb90167478;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb90167478 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb94050485;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb94050485 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb7919149;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb7919149 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb67952260;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb67952260 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb25157526;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb25157526 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb54806840;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb54806840 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb27772066;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb27772066 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb94953885;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb94953885 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb50221124;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb50221124 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb46132650;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb46132650 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb44282293;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb44282293 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb60046457;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb60046457 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb69271075;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb69271075 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb77006972;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb77006972 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb26307658;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb26307658 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb93071356;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb93071356 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb6907273;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb6907273 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb99400925;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb99400925 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb4130727;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb4130727 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb1255821;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb1255821 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb35809979;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb35809979 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49105616;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49105616 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb65707073;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb65707073 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb45269392;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb45269392 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb37164614;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb37164614 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb91483266;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb91483266 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb20502711;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb20502711 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb41511778;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb41511778 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb4893232;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb4893232 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb65190800;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb65190800 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb61119233;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb61119233 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb33118071;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb33118071 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb478880;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb478880 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb17047391;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb17047391 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb26632989;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb26632989 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49024356;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49024356 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb57036233;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb57036233 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb79770228;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb79770228 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb6581492;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb6581492 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb56904683;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb56904683 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb15491445;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb15491445 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb59170024;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb59170024 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb92902662;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb92902662 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb885010;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb885010 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb78271019;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb78271019 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb45277327;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb45277327 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb79769503;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb79769503 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb6143498;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb6143498 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb18434191;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb18434191 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb39186142;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb39186142 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb68145966;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb68145966 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb72876567;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb72876567 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49387839;     yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb49387839 = yyfWLOEhAYgDZhzOnVDHzEfEreRiIiZsTHAuHJjNwAEAb90465163;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void RYwMFHeyptxxpZQBZCDWHZDcoivlMBreidkww23426448() {     double iTdBqidmUDAFsTVVGmsVT97774171 = -621732054;    double iTdBqidmUDAFsTVVGmsVT78448785 = -720162646;    double iTdBqidmUDAFsTVVGmsVT16333436 = -495511802;    double iTdBqidmUDAFsTVVGmsVT30219918 = -766638453;    double iTdBqidmUDAFsTVVGmsVT1309250 = -996645506;    double iTdBqidmUDAFsTVVGmsVT75948442 = -945924137;    double iTdBqidmUDAFsTVVGmsVT30526542 = -493903983;    double iTdBqidmUDAFsTVVGmsVT97926818 = -619956360;    double iTdBqidmUDAFsTVVGmsVT67530937 = -48202648;    double iTdBqidmUDAFsTVVGmsVT77960955 = -841929124;    double iTdBqidmUDAFsTVVGmsVT60997390 = -180498395;    double iTdBqidmUDAFsTVVGmsVT81045896 = -95401961;    double iTdBqidmUDAFsTVVGmsVT9986384 = -277686463;    double iTdBqidmUDAFsTVVGmsVT51848038 = -255287627;    double iTdBqidmUDAFsTVVGmsVT86104076 = -57052433;    double iTdBqidmUDAFsTVVGmsVT71335538 = 16563413;    double iTdBqidmUDAFsTVVGmsVT90326560 = -295641475;    double iTdBqidmUDAFsTVVGmsVT4139987 = -265417049;    double iTdBqidmUDAFsTVVGmsVT99337901 = -469170898;    double iTdBqidmUDAFsTVVGmsVT32857665 = -346941208;    double iTdBqidmUDAFsTVVGmsVT81291613 = -795382011;    double iTdBqidmUDAFsTVVGmsVT68839807 = -105398659;    double iTdBqidmUDAFsTVVGmsVT7034062 = -216613455;    double iTdBqidmUDAFsTVVGmsVT29991175 = -474442000;    double iTdBqidmUDAFsTVVGmsVT97655579 = -788895155;    double iTdBqidmUDAFsTVVGmsVT30280379 = -90675784;    double iTdBqidmUDAFsTVVGmsVT7370994 = -793988829;    double iTdBqidmUDAFsTVVGmsVT77324493 = -534615014;    double iTdBqidmUDAFsTVVGmsVT82640531 = -393195304;    double iTdBqidmUDAFsTVVGmsVT31217495 = -137386430;    double iTdBqidmUDAFsTVVGmsVT26155062 = -15233975;    double iTdBqidmUDAFsTVVGmsVT34953717 = -160237212;    double iTdBqidmUDAFsTVVGmsVT82641525 = -745582010;    double iTdBqidmUDAFsTVVGmsVT90452129 = -588519863;    double iTdBqidmUDAFsTVVGmsVT66502201 = -664772606;    double iTdBqidmUDAFsTVVGmsVT19163725 = -341467339;    double iTdBqidmUDAFsTVVGmsVT37505981 = -666699674;    double iTdBqidmUDAFsTVVGmsVT52081305 = -252497533;    double iTdBqidmUDAFsTVVGmsVT16430566 = -469788284;    double iTdBqidmUDAFsTVVGmsVT6375644 = -85457099;    double iTdBqidmUDAFsTVVGmsVT53820149 = -489289980;    double iTdBqidmUDAFsTVVGmsVT53526858 = -198181226;    double iTdBqidmUDAFsTVVGmsVT14405589 = -663950604;    double iTdBqidmUDAFsTVVGmsVT17855524 = -626580631;    double iTdBqidmUDAFsTVVGmsVT9313782 = -306255401;    double iTdBqidmUDAFsTVVGmsVT32792367 = -279359024;    double iTdBqidmUDAFsTVVGmsVT44974013 = -59512618;    double iTdBqidmUDAFsTVVGmsVT31208401 = -430025734;    double iTdBqidmUDAFsTVVGmsVT88155037 = -343528929;    double iTdBqidmUDAFsTVVGmsVT34134512 = -500783276;    double iTdBqidmUDAFsTVVGmsVT43161243 = -597559423;    double iTdBqidmUDAFsTVVGmsVT33718150 = -213936672;    double iTdBqidmUDAFsTVVGmsVT27511680 = -671815491;    double iTdBqidmUDAFsTVVGmsVT65564548 = -926512862;    double iTdBqidmUDAFsTVVGmsVT42134835 = -288646798;    double iTdBqidmUDAFsTVVGmsVT28934364 = -416333395;    double iTdBqidmUDAFsTVVGmsVT71414723 = -403549191;    double iTdBqidmUDAFsTVVGmsVT86342260 = 78930198;    double iTdBqidmUDAFsTVVGmsVT32564338 = -977743298;    double iTdBqidmUDAFsTVVGmsVT71028871 = -805969722;    double iTdBqidmUDAFsTVVGmsVT68577449 = -51935308;    double iTdBqidmUDAFsTVVGmsVT53202049 = -959288970;    double iTdBqidmUDAFsTVVGmsVT15286288 = -126761057;    double iTdBqidmUDAFsTVVGmsVT36313443 = -910816218;    double iTdBqidmUDAFsTVVGmsVT51805894 = -726695149;    double iTdBqidmUDAFsTVVGmsVT26043673 = 79738816;    double iTdBqidmUDAFsTVVGmsVT98404371 = -349819951;    double iTdBqidmUDAFsTVVGmsVT19534254 = -689166600;    double iTdBqidmUDAFsTVVGmsVT85345837 = -590515021;    double iTdBqidmUDAFsTVVGmsVT66940351 = -715585094;    double iTdBqidmUDAFsTVVGmsVT33829558 = -316736913;    double iTdBqidmUDAFsTVVGmsVT38245256 = 56856057;    double iTdBqidmUDAFsTVVGmsVT87709420 = -795628766;    double iTdBqidmUDAFsTVVGmsVT92962257 = -283713799;    double iTdBqidmUDAFsTVVGmsVT79037515 = -857651228;    double iTdBqidmUDAFsTVVGmsVT27764755 = -497200786;    double iTdBqidmUDAFsTVVGmsVT54434218 = -441448055;    double iTdBqidmUDAFsTVVGmsVT89178538 = -590032825;    double iTdBqidmUDAFsTVVGmsVT20677394 = -68186600;    double iTdBqidmUDAFsTVVGmsVT64863213 = -409536131;    double iTdBqidmUDAFsTVVGmsVT85306365 = 68836833;    double iTdBqidmUDAFsTVVGmsVT76162593 = -263963095;    double iTdBqidmUDAFsTVVGmsVT89169456 = -91086085;    double iTdBqidmUDAFsTVVGmsVT48506019 = -892412029;    double iTdBqidmUDAFsTVVGmsVT88056251 = -539827008;    double iTdBqidmUDAFsTVVGmsVT92436912 = -801297304;    double iTdBqidmUDAFsTVVGmsVT7442038 = -488421722;    double iTdBqidmUDAFsTVVGmsVT17076977 = -819069149;    double iTdBqidmUDAFsTVVGmsVT48317295 = -199873066;    double iTdBqidmUDAFsTVVGmsVT37567837 = -148439211;    double iTdBqidmUDAFsTVVGmsVT47749001 = -937918148;    double iTdBqidmUDAFsTVVGmsVT51163720 = -645629873;    double iTdBqidmUDAFsTVVGmsVT19516968 = -274754235;    double iTdBqidmUDAFsTVVGmsVT45401695 = -663818563;    double iTdBqidmUDAFsTVVGmsVT37798194 = 66478208;    double iTdBqidmUDAFsTVVGmsVT618101 = -530001010;    double iTdBqidmUDAFsTVVGmsVT38240571 = 28579831;    double iTdBqidmUDAFsTVVGmsVT78092146 = -753134387;    double iTdBqidmUDAFsTVVGmsVT66049630 = -899885482;    double iTdBqidmUDAFsTVVGmsVT83270108 = -621732054;     iTdBqidmUDAFsTVVGmsVT97774171 = iTdBqidmUDAFsTVVGmsVT78448785;     iTdBqidmUDAFsTVVGmsVT78448785 = iTdBqidmUDAFsTVVGmsVT16333436;     iTdBqidmUDAFsTVVGmsVT16333436 = iTdBqidmUDAFsTVVGmsVT30219918;     iTdBqidmUDAFsTVVGmsVT30219918 = iTdBqidmUDAFsTVVGmsVT1309250;     iTdBqidmUDAFsTVVGmsVT1309250 = iTdBqidmUDAFsTVVGmsVT75948442;     iTdBqidmUDAFsTVVGmsVT75948442 = iTdBqidmUDAFsTVVGmsVT30526542;     iTdBqidmUDAFsTVVGmsVT30526542 = iTdBqidmUDAFsTVVGmsVT97926818;     iTdBqidmUDAFsTVVGmsVT97926818 = iTdBqidmUDAFsTVVGmsVT67530937;     iTdBqidmUDAFsTVVGmsVT67530937 = iTdBqidmUDAFsTVVGmsVT77960955;     iTdBqidmUDAFsTVVGmsVT77960955 = iTdBqidmUDAFsTVVGmsVT60997390;     iTdBqidmUDAFsTVVGmsVT60997390 = iTdBqidmUDAFsTVVGmsVT81045896;     iTdBqidmUDAFsTVVGmsVT81045896 = iTdBqidmUDAFsTVVGmsVT9986384;     iTdBqidmUDAFsTVVGmsVT9986384 = iTdBqidmUDAFsTVVGmsVT51848038;     iTdBqidmUDAFsTVVGmsVT51848038 = iTdBqidmUDAFsTVVGmsVT86104076;     iTdBqidmUDAFsTVVGmsVT86104076 = iTdBqidmUDAFsTVVGmsVT71335538;     iTdBqidmUDAFsTVVGmsVT71335538 = iTdBqidmUDAFsTVVGmsVT90326560;     iTdBqidmUDAFsTVVGmsVT90326560 = iTdBqidmUDAFsTVVGmsVT4139987;     iTdBqidmUDAFsTVVGmsVT4139987 = iTdBqidmUDAFsTVVGmsVT99337901;     iTdBqidmUDAFsTVVGmsVT99337901 = iTdBqidmUDAFsTVVGmsVT32857665;     iTdBqidmUDAFsTVVGmsVT32857665 = iTdBqidmUDAFsTVVGmsVT81291613;     iTdBqidmUDAFsTVVGmsVT81291613 = iTdBqidmUDAFsTVVGmsVT68839807;     iTdBqidmUDAFsTVVGmsVT68839807 = iTdBqidmUDAFsTVVGmsVT7034062;     iTdBqidmUDAFsTVVGmsVT7034062 = iTdBqidmUDAFsTVVGmsVT29991175;     iTdBqidmUDAFsTVVGmsVT29991175 = iTdBqidmUDAFsTVVGmsVT97655579;     iTdBqidmUDAFsTVVGmsVT97655579 = iTdBqidmUDAFsTVVGmsVT30280379;     iTdBqidmUDAFsTVVGmsVT30280379 = iTdBqidmUDAFsTVVGmsVT7370994;     iTdBqidmUDAFsTVVGmsVT7370994 = iTdBqidmUDAFsTVVGmsVT77324493;     iTdBqidmUDAFsTVVGmsVT77324493 = iTdBqidmUDAFsTVVGmsVT82640531;     iTdBqidmUDAFsTVVGmsVT82640531 = iTdBqidmUDAFsTVVGmsVT31217495;     iTdBqidmUDAFsTVVGmsVT31217495 = iTdBqidmUDAFsTVVGmsVT26155062;     iTdBqidmUDAFsTVVGmsVT26155062 = iTdBqidmUDAFsTVVGmsVT34953717;     iTdBqidmUDAFsTVVGmsVT34953717 = iTdBqidmUDAFsTVVGmsVT82641525;     iTdBqidmUDAFsTVVGmsVT82641525 = iTdBqidmUDAFsTVVGmsVT90452129;     iTdBqidmUDAFsTVVGmsVT90452129 = iTdBqidmUDAFsTVVGmsVT66502201;     iTdBqidmUDAFsTVVGmsVT66502201 = iTdBqidmUDAFsTVVGmsVT19163725;     iTdBqidmUDAFsTVVGmsVT19163725 = iTdBqidmUDAFsTVVGmsVT37505981;     iTdBqidmUDAFsTVVGmsVT37505981 = iTdBqidmUDAFsTVVGmsVT52081305;     iTdBqidmUDAFsTVVGmsVT52081305 = iTdBqidmUDAFsTVVGmsVT16430566;     iTdBqidmUDAFsTVVGmsVT16430566 = iTdBqidmUDAFsTVVGmsVT6375644;     iTdBqidmUDAFsTVVGmsVT6375644 = iTdBqidmUDAFsTVVGmsVT53820149;     iTdBqidmUDAFsTVVGmsVT53820149 = iTdBqidmUDAFsTVVGmsVT53526858;     iTdBqidmUDAFsTVVGmsVT53526858 = iTdBqidmUDAFsTVVGmsVT14405589;     iTdBqidmUDAFsTVVGmsVT14405589 = iTdBqidmUDAFsTVVGmsVT17855524;     iTdBqidmUDAFsTVVGmsVT17855524 = iTdBqidmUDAFsTVVGmsVT9313782;     iTdBqidmUDAFsTVVGmsVT9313782 = iTdBqidmUDAFsTVVGmsVT32792367;     iTdBqidmUDAFsTVVGmsVT32792367 = iTdBqidmUDAFsTVVGmsVT44974013;     iTdBqidmUDAFsTVVGmsVT44974013 = iTdBqidmUDAFsTVVGmsVT31208401;     iTdBqidmUDAFsTVVGmsVT31208401 = iTdBqidmUDAFsTVVGmsVT88155037;     iTdBqidmUDAFsTVVGmsVT88155037 = iTdBqidmUDAFsTVVGmsVT34134512;     iTdBqidmUDAFsTVVGmsVT34134512 = iTdBqidmUDAFsTVVGmsVT43161243;     iTdBqidmUDAFsTVVGmsVT43161243 = iTdBqidmUDAFsTVVGmsVT33718150;     iTdBqidmUDAFsTVVGmsVT33718150 = iTdBqidmUDAFsTVVGmsVT27511680;     iTdBqidmUDAFsTVVGmsVT27511680 = iTdBqidmUDAFsTVVGmsVT65564548;     iTdBqidmUDAFsTVVGmsVT65564548 = iTdBqidmUDAFsTVVGmsVT42134835;     iTdBqidmUDAFsTVVGmsVT42134835 = iTdBqidmUDAFsTVVGmsVT28934364;     iTdBqidmUDAFsTVVGmsVT28934364 = iTdBqidmUDAFsTVVGmsVT71414723;     iTdBqidmUDAFsTVVGmsVT71414723 = iTdBqidmUDAFsTVVGmsVT86342260;     iTdBqidmUDAFsTVVGmsVT86342260 = iTdBqidmUDAFsTVVGmsVT32564338;     iTdBqidmUDAFsTVVGmsVT32564338 = iTdBqidmUDAFsTVVGmsVT71028871;     iTdBqidmUDAFsTVVGmsVT71028871 = iTdBqidmUDAFsTVVGmsVT68577449;     iTdBqidmUDAFsTVVGmsVT68577449 = iTdBqidmUDAFsTVVGmsVT53202049;     iTdBqidmUDAFsTVVGmsVT53202049 = iTdBqidmUDAFsTVVGmsVT15286288;     iTdBqidmUDAFsTVVGmsVT15286288 = iTdBqidmUDAFsTVVGmsVT36313443;     iTdBqidmUDAFsTVVGmsVT36313443 = iTdBqidmUDAFsTVVGmsVT51805894;     iTdBqidmUDAFsTVVGmsVT51805894 = iTdBqidmUDAFsTVVGmsVT26043673;     iTdBqidmUDAFsTVVGmsVT26043673 = iTdBqidmUDAFsTVVGmsVT98404371;     iTdBqidmUDAFsTVVGmsVT98404371 = iTdBqidmUDAFsTVVGmsVT19534254;     iTdBqidmUDAFsTVVGmsVT19534254 = iTdBqidmUDAFsTVVGmsVT85345837;     iTdBqidmUDAFsTVVGmsVT85345837 = iTdBqidmUDAFsTVVGmsVT66940351;     iTdBqidmUDAFsTVVGmsVT66940351 = iTdBqidmUDAFsTVVGmsVT33829558;     iTdBqidmUDAFsTVVGmsVT33829558 = iTdBqidmUDAFsTVVGmsVT38245256;     iTdBqidmUDAFsTVVGmsVT38245256 = iTdBqidmUDAFsTVVGmsVT87709420;     iTdBqidmUDAFsTVVGmsVT87709420 = iTdBqidmUDAFsTVVGmsVT92962257;     iTdBqidmUDAFsTVVGmsVT92962257 = iTdBqidmUDAFsTVVGmsVT79037515;     iTdBqidmUDAFsTVVGmsVT79037515 = iTdBqidmUDAFsTVVGmsVT27764755;     iTdBqidmUDAFsTVVGmsVT27764755 = iTdBqidmUDAFsTVVGmsVT54434218;     iTdBqidmUDAFsTVVGmsVT54434218 = iTdBqidmUDAFsTVVGmsVT89178538;     iTdBqidmUDAFsTVVGmsVT89178538 = iTdBqidmUDAFsTVVGmsVT20677394;     iTdBqidmUDAFsTVVGmsVT20677394 = iTdBqidmUDAFsTVVGmsVT64863213;     iTdBqidmUDAFsTVVGmsVT64863213 = iTdBqidmUDAFsTVVGmsVT85306365;     iTdBqidmUDAFsTVVGmsVT85306365 = iTdBqidmUDAFsTVVGmsVT76162593;     iTdBqidmUDAFsTVVGmsVT76162593 = iTdBqidmUDAFsTVVGmsVT89169456;     iTdBqidmUDAFsTVVGmsVT89169456 = iTdBqidmUDAFsTVVGmsVT48506019;     iTdBqidmUDAFsTVVGmsVT48506019 = iTdBqidmUDAFsTVVGmsVT88056251;     iTdBqidmUDAFsTVVGmsVT88056251 = iTdBqidmUDAFsTVVGmsVT92436912;     iTdBqidmUDAFsTVVGmsVT92436912 = iTdBqidmUDAFsTVVGmsVT7442038;     iTdBqidmUDAFsTVVGmsVT7442038 = iTdBqidmUDAFsTVVGmsVT17076977;     iTdBqidmUDAFsTVVGmsVT17076977 = iTdBqidmUDAFsTVVGmsVT48317295;     iTdBqidmUDAFsTVVGmsVT48317295 = iTdBqidmUDAFsTVVGmsVT37567837;     iTdBqidmUDAFsTVVGmsVT37567837 = iTdBqidmUDAFsTVVGmsVT47749001;     iTdBqidmUDAFsTVVGmsVT47749001 = iTdBqidmUDAFsTVVGmsVT51163720;     iTdBqidmUDAFsTVVGmsVT51163720 = iTdBqidmUDAFsTVVGmsVT19516968;     iTdBqidmUDAFsTVVGmsVT19516968 = iTdBqidmUDAFsTVVGmsVT45401695;     iTdBqidmUDAFsTVVGmsVT45401695 = iTdBqidmUDAFsTVVGmsVT37798194;     iTdBqidmUDAFsTVVGmsVT37798194 = iTdBqidmUDAFsTVVGmsVT618101;     iTdBqidmUDAFsTVVGmsVT618101 = iTdBqidmUDAFsTVVGmsVT38240571;     iTdBqidmUDAFsTVVGmsVT38240571 = iTdBqidmUDAFsTVVGmsVT78092146;     iTdBqidmUDAFsTVVGmsVT78092146 = iTdBqidmUDAFsTVVGmsVT66049630;     iTdBqidmUDAFsTVVGmsVT66049630 = iTdBqidmUDAFsTVVGmsVT83270108;     iTdBqidmUDAFsTVVGmsVT83270108 = iTdBqidmUDAFsTVVGmsVT97774171;}
// Junk Finished
