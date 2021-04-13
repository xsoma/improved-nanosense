#include "Convar.hpp"

#include "../sdk.hpp"

#include "characterset.hpp"
#include "UtlBuffer.hpp"

#define ALIGN_VALUE( val, alignment ) ( ( val + alignment - 1 ) & ~( alignment - 1 ) ) 
#define stackalloc( _size )		_alloca( ALIGN_VALUE( _size, 16 ) )

ConCommandBase *ConCommandBase::s_pConCommandBases = NULL;
ConCommandBase *ConCommandBase::s_pRegisteredCommands = NULL;
IConCommandBaseAccessor	*ConCommandBase::s_pAccessor = NULL;
static int s_nDLLIdentifier = -1;
static int s_nCVarFlag = 0;
static bool s_bRegistered = false;

class CDefaultAccessor : public IConCommandBaseAccessor
{
public:
    virtual bool RegisterConCommandBase(ConCommandBase *pVar)
    {
        // Link to engine's list instead
        g_CVar->RegisterConCommand(pVar);
        return true;
    }
};

static CDefaultAccessor s_DefaultAccessor;

//-----------------------------------------------------------------------------
// Called by the framework to register ConCommandBases with the ICVar
//-----------------------------------------------------------------------------
void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor *pAccessor)
{
    if(!g_CVar || s_bRegistered)
        return;

    assert(s_nDLLIdentifier < 0);
    s_bRegistered = true;
    s_nCVarFlag = nCVarFlag;
    s_nDLLIdentifier = g_CVar->AllocateDLLIdentifier();

    ConCommandBase *pCur, *pNext;

    ConCommandBase::s_pAccessor = pAccessor ? pAccessor : &s_DefaultAccessor;
    pCur = ConCommandBase::s_pConCommandBases;

    while(pCur) {
        pNext = pCur->m_pNext;
        pCur->AddFlags(s_nCVarFlag);
        pCur->Init();

        ConCommandBase::s_pRegisteredCommands = pCur;

        pCur = pNext;
    }

    ConCommandBase::s_pConCommandBases = NULL;
}

void ConVar_Unregister()
{
    if(!g_CVar || !s_bRegistered)
        return;

    assert(s_nDLLIdentifier >= 0);
    g_CVar->UnregisterConCommands(s_nDLLIdentifier);
    s_nDLLIdentifier = -1;
    s_bRegistered = false;
}

ConCommandBase::ConCommandBase(void)
{
    m_bRegistered = false;
    m_pszName = NULL;
    m_pszHelpString = NULL;

    m_nFlags = 0;
    m_pNext = NULL;
}

ConCommandBase::ConCommandBase(const char *pName, const char *pHelpString /*=0*/, int flags /*= 0*/)
{
    Create(pName, pHelpString, flags);
}

ConCommandBase::~ConCommandBase(void)
{
}

bool ConCommandBase::IsCommand(void) const
{
    //	assert( 0 ); This can't assert. . causes a recursive assert in Sys_Printf, etc.
    return true;
}

CVarDLLIdentifier_t ConCommandBase::GetDLLIdentifier() const
{
    return s_nDLLIdentifier;
}

void ConCommandBase::Create(const char *pName, const char *pHelpString /*= 0*/, int flags /*= 0*/)
{
    static const char *empty_string = "";

    m_bRegistered = false;

    // Name should be static data
    m_pszName = pName;
    m_pszHelpString = pHelpString ? pHelpString : empty_string;

    m_nFlags = flags;

    if(!(m_nFlags & FCVAR_UNREGISTERED)) {
        m_pNext = s_pConCommandBases;
        s_pConCommandBases = this;
    } else {
        m_pNext = NULL;
    }
}

void ConCommandBase::Init()
{
    if(s_pAccessor) {
        s_pAccessor->RegisterConCommandBase(this);
    }
}

void ConCommandBase::Shutdown()
{
    if(g_CVar) {
        g_CVar->UnregisterConCommand(this);
    }
}

const char *ConCommandBase::GetName(void) const
{
    return m_pszName;
}

bool ConCommandBase::IsFlagSet(int flag) const
{
    return (flag & m_nFlags) ? true : false;
}

void ConCommandBase::AddFlags(int flags)
{
    m_nFlags |= flags;
}

void ConCommandBase::RemoveFlags(int flags)
{
    m_nFlags &= ~flags;
}

int ConCommandBase::GetFlags(void) const
{
    return m_nFlags;
}

const ConCommandBase *ConCommandBase::GetNext(void) const
{
    return m_pNext;
}

ConCommandBase *ConCommandBase::GetNext(void)
{
    return m_pNext;
}

char *ConCommandBase::CopyString(const char *from)
{
    int		len;
    char	*to;

    len = strlen(from);
    if(len <= 0) {
        to = new char[1];
        to[0] = 0;
    } else {
        to = new char[len + 1];
        strncpy_s(to, len + 1, from, len + 1);
    }
    return to;
}

const char *ConCommandBase::GetHelpText(void) const
{
    return m_pszHelpString;
}

bool ConCommandBase::IsRegistered(void) const
{
    return m_bRegistered;
}

static characterset_t s_BreakSet;
static bool s_bBuiltBreakSet = false;

CCommand::CCommand()
{
    if(!s_bBuiltBreakSet) {
        s_bBuiltBreakSet = true;
        CharacterSetBuild(&s_BreakSet, "{}()':");
    }

    Reset();
}

CCommand::CCommand(int nArgC, const char **ppArgV)
{
    assert(nArgC > 0);

    if(!s_bBuiltBreakSet) {
        s_bBuiltBreakSet = true;
        CharacterSetBuild(&s_BreakSet, "{}()':");
    }

    Reset();

    char *pBuf = m_pArgvBuffer;
    char *pSBuf = m_pArgSBuffer;
    m_nArgc = nArgC;
    for(int i = 0; i < nArgC; ++i) {
        m_ppArgv[i] = pBuf;
        int nLen = strlen(ppArgV[i]);
        memcpy(pBuf, ppArgV[i], nLen + 1);
        if(i == 0) {
            m_nArgv0Size = nLen;
        }
        pBuf += nLen + 1;

        bool bContainsSpace = strchr(ppArgV[i], ' ') != NULL;
        if(bContainsSpace) {
            *pSBuf++ = '\"';
        }
        memcpy(pSBuf, ppArgV[i], nLen);
        pSBuf += nLen;
        if(bContainsSpace) {
            *pSBuf++ = '\"';
        }

        if(i != nArgC - 1) {
            *pSBuf++ = ' ';
        }
    }
}

void CCommand::Reset()
{
    m_nArgc = 0;
    m_nArgv0Size = 0;
    m_pArgSBuffer[0] = 0;
}

characterset_t* CCommand::DefaultBreakSet()
{
    return &s_BreakSet;
}

bool CCommand::Tokenize(const char *pCommand, characterset_t *pBreakSet)
{
    Reset();
    if(!pCommand)
        return false;

    // Use default break Set
    if(!pBreakSet) {
        pBreakSet = &s_BreakSet;
    }

    // Copy the current command into a temp buffer
    // NOTE: This is here to avoid the pointers returned by DequeueNextCommand
    // to become invalid by calling AddText. Is there a way we can avoid the memcpy?
    int nLen = strlen(pCommand);
    if(nLen >= COMMAND_MAX_LENGTH - 1) {
        //Warning("CCommand::Tokenize: Encountered command which overflows the tokenizer buffer.. Skipping!\n");
        return false;
    }

    memcpy(m_pArgSBuffer, pCommand, nLen + 1);

    // Parse the current command into the current command buffer
    CUtlBuffer bufParse(m_pArgSBuffer, nLen, CUtlBuffer::TEXT_BUFFER | CUtlBuffer::READ_ONLY);
    int nArgvBufferSize = 0;
    while(bufParse.IsValid() && (m_nArgc < COMMAND_MAX_ARGC)) {
        char *pArgvBuf = &m_pArgvBuffer[nArgvBufferSize];
        int nMaxLen = COMMAND_MAX_LENGTH - nArgvBufferSize;
        int nStartGet = bufParse.TellGet();
        int	nSize = bufParse.ParseToken(pBreakSet, pArgvBuf, nMaxLen);
        if(nSize < 0)
            break;

        // Check for overflow condition
        if(nMaxLen == nSize) {
            Reset();
            return false;
        }

        if(m_nArgc == 1) {
            // Deal with the case where the arguments were quoted
            m_nArgv0Size = bufParse.TellGet();
            bool bFoundEndQuote = m_pArgSBuffer[m_nArgv0Size - 1] == '\"';
            if(bFoundEndQuote) {
                --m_nArgv0Size;
            }
            m_nArgv0Size -= nSize;
            assert(m_nArgv0Size != 0);

            // The StartGet check is to handle this case: "foo"bar
            // which will parse into 2 different args. ArgS should point to bar.
            bool bFoundStartQuote = (m_nArgv0Size > nStartGet) && (m_pArgSBuffer[m_nArgv0Size - 1] == '\"');
            assert(bFoundEndQuote == bFoundStartQuote);
            if(bFoundStartQuote) {
                --m_nArgv0Size;
            }
        }

        m_ppArgv[m_nArgc++] = pArgvBuf;
        if(m_nArgc >= COMMAND_MAX_ARGC) {
            //Warning("CCommand::Tokenize: Encountered command which overflows the argument buffer.. Clamped!\n");
        }

        nArgvBufferSize += nSize + 1;
        assert(nArgvBufferSize <= COMMAND_MAX_LENGTH);
    }

    return true;
}

const char* CCommand::FindArg(const char *pName) const
{
    int nArgC = ArgC();
    for(int i = 1; i < nArgC; i++) {
        if(!_stricmp(Arg(i), pName))
            return (i + 1) < nArgC ? Arg(i + 1) : "";
    }
    return 0;
}

int CCommand::FindArgInt(const char *pName, int nDefaultVal) const
{
    const char *pVal = FindArg(pName);
    if(pVal)
        return atoi(pVal);
    else
        return nDefaultVal;
}

int DefaultCompletionFunc(const char *partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH])
{
    return 0;
}

ConCommand::ConCommand(const char *pName, FnCommandCallbackV1_t callback, const char *pHelpString /*= 0*/, int flags /*= 0*/, FnCommandCompletionCallback completionFunc /*= 0*/)
{
    // Set the callback
    m_fnCommandCallbackV1 = callback;
    m_bUsingNewCommandCallback = false;
    m_bUsingCommandCallbackInterface = false;
    m_fnCompletionCallback = completionFunc ? completionFunc : DefaultCompletionFunc;
    m_bHasCompletionCallback = completionFunc != 0 ? true : false;

    // Setup the rest
    BaseClass::Create(pName, pHelpString, flags);
}

ConCommand::ConCommand(const char *pName, FnCommandCallback_t callback, const char *pHelpString /*= 0*/, int flags /*= 0*/, FnCommandCompletionCallback completionFunc /*= 0*/)
{
    // Set the callback
    m_fnCommandCallback = callback;
    m_bUsingNewCommandCallback = true;
    m_fnCompletionCallback = completionFunc ? completionFunc : DefaultCompletionFunc;
    m_bHasCompletionCallback = completionFunc != 0 ? true : false;
    m_bUsingCommandCallbackInterface = false;

    // Setup the rest
    BaseClass::Create(pName, pHelpString, flags);
}

ConCommand::ConCommand(const char *pName, ICommandCallback *pCallback, const char *pHelpString /*= 0*/, int flags /*= 0*/, ICommandCompletionCallback *pCompletionCallback /*= 0*/)
{
    // Set the callback
    m_pCommandCallback = pCallback;
    m_bUsingNewCommandCallback = false;
    m_pCommandCompletionCallback = pCompletionCallback;
    m_bHasCompletionCallback = (pCompletionCallback != 0);
    m_bUsingCommandCallbackInterface = true;

    // Setup the rest
    BaseClass::Create(pName, pHelpString, flags);
}

ConCommand::~ConCommand(void)
{
}

bool ConCommand::IsCommand(void) const
{
    return true;
}

void ConCommand::Dispatch(const CCommand &command)
{
    if(m_bUsingNewCommandCallback) {
        if(m_fnCommandCallback) {
            (*m_fnCommandCallback)(command);
            return;
        }
    } else if(m_bUsingCommandCallbackInterface) {
        if(m_pCommandCallback) {
            m_pCommandCallback->CommandCallback(command);
            return;
        }
    } else {
        if(m_fnCommandCallbackV1) {
            (*m_fnCommandCallbackV1)();
            return;
        }
    }

    // Command without callback!!!
    //AssertMsg(0, ("Encountered ConCommand without a callback!\n"));
}

int	ConCommand::AutoCompleteSuggest(const char *partial, CUtlVector< CUtlString > &commands)
{
    if(m_bUsingCommandCallbackInterface) {
        if(!m_pCommandCompletionCallback)
            return 0;
        return m_pCommandCompletionCallback->CommandCompletionCallback(partial, commands);
    }

    if(!m_fnCompletionCallback)
        return 0;

    char rgpchCommands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH];
    int iret = (m_fnCompletionCallback)(partial, rgpchCommands);
    for(int i = 0; i < iret; ++i) {
        CUtlString str = rgpchCommands[i];
        commands.AddToTail(str);
    }
    return iret;
}

bool ConCommand::CanAutoComplete(void)
{
    return m_bHasCompletionCallback;
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags /* = 0 */)
{
    Create(pName, pDefaultValue, flags);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString)
{
    Create(pName, pDefaultValue, flags, pHelpString);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax)
{
    Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, FnChangeCallback_t callback)
{
    Create(pName, pDefaultValue, flags, pHelpString, false, 0.0, false, 0.0, callback);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback)
{
    Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax, callback);
}

ConVar::~ConVar(void)
{
    //if(IsRegistered())
    //    convar->UnregisterConCommand(this);
    if(m_Value.m_pszString) {
        delete[] m_Value.m_pszString;
        m_Value.m_pszString = NULL;
    }
}

void ConVar::InstallChangeCallback(FnChangeCallback_t callback, bool bInvoke)
{
    if(callback) {
        if(m_fnChangeCallbacks.GetOffset(callback) != -1) {
            m_fnChangeCallbacks.AddToTail(callback);
            if(bInvoke)
                callback(this, m_Value.m_pszString, m_Value.m_fValue);
        } else {
            //Warning("InstallChangeCallback ignoring duplicate change callback!!!\n");
        }
    } else {
        //Warning("InstallChangeCallback called with NULL callback, ignoring!!!\n");
    }
}

bool ConVar::IsFlagSet(int flag) const
{
    return (flag & m_pParent->m_nFlags) ? true : false;
}

const char *ConVar::GetHelpText(void) const
{
    return m_pParent->m_pszHelpString;
}

void ConVar::AddFlags(int flags)
{
    m_pParent->m_nFlags |= flags;

#ifdef ALLOW_DEVELOPMENT_CVARS
    m_pParent->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
#endif
}

int ConVar::GetFlags(void) const
{
    return m_pParent->m_nFlags;
}

bool ConVar::IsRegistered(void) const
{
    return m_pParent->m_bRegistered;
}

const char *ConVar::GetName(void) const
{
    return m_pParent->m_pszName;
}

bool ConVar::IsCommand(void) const
{
    return false;
}

void ConVar::Init()
{
    BaseClass::Init();
}

const char *ConVar::GetBaseName(void) const
{
    return m_pParent->m_pszName;
}

int ConVar::GetSplitScreenPlayerSlot(void) const
{
    return 0;
}

void ConVar::InternalSetValue(const char *value)
{
    float fNewValue;
    char  tempVal[32];
    char  *val;

    auto temp = *(uint32_t*)&m_Value.m_fValue ^ (uint32_t)this;
    float flOldValue = *(float*)(&temp);

    val = (char *)value;
    fNewValue = (float)atof(value);

    if(ClampValue(fNewValue)) {
        snprintf(tempVal, sizeof(tempVal), "%f", fNewValue);
        val = tempVal;
    }

    // Redetermine value
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fNewValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = (uint32_t)fNewValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        ChangeStringValue(val, flOldValue);
    }
}

void ConVar::ChangeStringValue(const char *tempVal, float flOldValue)
{
    char* pszOldValue = (char*)stackalloc(m_Value.m_StringLength);
    memcpy(pszOldValue, m_Value.m_pszString, m_Value.m_StringLength);

    int len = strlen(tempVal) + 1;

    if(len > m_Value.m_StringLength) {
        if(m_Value.m_pszString) {
            delete[] m_Value.m_pszString;
        }

        m_Value.m_pszString = new char[len];
        m_Value.m_StringLength = len;
    }

    memcpy(m_Value.m_pszString, tempVal, len);

    // Invoke any necessary callback function
    for(int i = 0; i < m_fnChangeCallbacks.Count(); i++) {
        m_fnChangeCallbacks[i](this, pszOldValue, flOldValue);
    }

    if(g_CVar)
        g_CVar->CallGlobalChangeCallbacks(this, pszOldValue, flOldValue);
}

bool ConVar::ClampValue(float& value)
{
    if(m_bHasMin && (value < m_fMinVal)) {
        value = m_fMinVal;
        return true;
    }

    if(m_bHasMax && (value > m_fMaxVal)) {
        value = m_fMaxVal;
        return true;
    }

    return false;
}

void ConVar::InternalSetFloatValue(float fNewValue)
{
    if(fNewValue == m_Value.m_fValue)
        return;

    ClampValue(fNewValue);

    // Redetermine value
    float flOldValue = m_Value.m_fValue;
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fNewValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = (uint32_t)fNewValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        char tempVal[32];
        snprintf(tempVal, sizeof(tempVal), "%f", m_Value.m_fValue);
        ChangeStringValue(tempVal, flOldValue);
    } else {
        //assert(m_fnChangeCallbacks.Count() == 0);
    }
}

void ConVar::InternalSetIntValue(int nValue)
{
    if(nValue == ((int)m_Value.m_nValue ^ (int)this))
        return;

    float fValue = (float)nValue;
    if(ClampValue(fValue)) {
        nValue = (int)(fValue);
    }

    // Redetermine value
    float flOldValue = m_Value.m_fValue;
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = *(uint32_t*)&nValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        char tempVal[32];
        snprintf(tempVal, sizeof(tempVal), "%d", m_Value.m_nValue);
        ChangeStringValue(tempVal, flOldValue);
    } else {
        //assert(m_fnChangeCallbacks.Count() == 0);
    }
}

void ConVar::InternalSetColorValue(Color cValue)
{
    int color = (int)cValue.GetRawColor();
    InternalSetIntValue(color);
}

void ConVar::Create(const char *pName, const char *pDefaultValue, int flags /*= 0*/,
    const char *pHelpString /*= NULL*/, bool bMin /*= false*/, float fMin /*= 0.0*/,
    bool bMax /*= false*/, float fMax /*= false*/, FnChangeCallback_t callback /*= NULL*/)
{
    static const char *empty_string = "";

    m_pParent = this;

    // Name should be static data
    m_pszDefaultValue = pDefaultValue ? pDefaultValue : empty_string;

    m_Value.m_StringLength = strlen(m_pszDefaultValue) + 1;
    m_Value.m_pszString = new char[m_Value.m_StringLength];
    memcpy(m_Value.m_pszString, m_pszDefaultValue, m_Value.m_StringLength);

    m_bHasMin = bMin;
    m_fMinVal = fMin;
    m_bHasMax = bMax;
    m_fMaxVal = fMax;

    if(callback)
        m_fnChangeCallbacks.AddToTail(callback);

    float value = (float)atof(m_Value.m_pszString);

    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&value ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = *(uint32_t*)&value ^ (uint32_t)this;

    BaseClass::Create(pName, pHelpString, flags);
}

void ConVar::SetValue(const char *value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetValue(value);
}
void ConVar::SetValue(float value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetFloatValue(value);
}

void ConVar::SetValue(int value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetIntValue(value);
}

void ConVar::SetValue(Color value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetColorValue(value);
}

void ConVar::Revert(void)
{
    // Force default value again
    ConVar *var = (ConVar *)m_pParent;
    var->SetValue(var->m_pszDefaultValue);
}

bool ConVar::GetMin(float& minVal) const
{
    minVal = m_pParent->m_fMinVal;
    return m_pParent->m_bHasMin;
}

bool ConVar::GetMax(float& maxVal) const
{
    maxVal = m_pParent->m_fMaxVal;
    return m_pParent->m_bHasMax;
}

const char *ConVar::GetDefault(void) const
{
    return m_pParent->m_pszDefaultValue;
}

SpoofedConvar::SpoofedConvar(const char* szCVar)
{
	m_pOriginalCVar = g_CVar->FindVar(szCVar);
	Spoof();
}

SpoofedConvar::SpoofedConvar(ConVar* pCVar)
{
	m_pOriginalCVar = pCVar;
	if (!IsSpoofed() && m_pOriginalCVar)
	{
		//Save old name value and flags so we can restore the cvar lates if needed
		m_iOriginalFlags = m_pOriginalCVar->m_nFlags;
		strcpy(m_szOriginalName, m_pOriginalCVar->m_pszName);
		strcpy(m_szOriginalValue, m_pOriginalCVar->m_pszDefaultValue);

		sprintf_s(m_szDummyName, 128, "d_%s", m_szOriginalName);

		//Create the dummy cvar
		m_pDummyCVar = (ConVar*)malloc(sizeof(ConVar));
		if (!m_pDummyCVar) return;
		memcpy(m_pDummyCVar, m_pOriginalCVar, sizeof(ConVar));

		m_pDummyCVar->m_pNext = nullptr;
		//Register it
		g_CVar->RegisterConCommand(m_pDummyCVar);

		//Fix "write access violation" bullshit
		DWORD dwOld;
		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, PAGE_READWRITE, &dwOld);

		//Rename the cvar
		strcpy((char*)m_pOriginalCVar->m_pszName, m_szDummyName);

		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, dwOld, &dwOld);

		SetFlags(FCVAR_NONE);
	}
}

SpoofedConvar::~SpoofedConvar()
{
	Restore();
}

bool SpoofedConvar::IsSpoofed()
{
	return m_pDummyCVar != nullptr;
}

void SpoofedConvar::Spoof()
{
	if (!IsSpoofed() && m_pOriginalCVar)
	{
		//Save old name value and flags so we can restore the cvar lates if needed
		m_iOriginalFlags = m_pOriginalCVar->m_nFlags;
		strcpy(m_szOriginalName, m_pOriginalCVar->m_pszName);
		strcpy(m_szOriginalValue, m_pOriginalCVar->m_pszDefaultValue);
		sprintf_s(m_szDummyName, 128, "d_%s", m_szOriginalName);

		m_pDummyCVar = (ConVar*)malloc(sizeof(ConVar));
		if (!m_pDummyCVar) return;
		memcpy(m_pDummyCVar, m_pOriginalCVar, sizeof(ConVar));

		m_pDummyCVar->m_pNext = nullptr;
		g_CVar->RegisterConCommand(m_pDummyCVar);

		DWORD dwOld;
		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, PAGE_READWRITE, &dwOld);

		strcpy((char*)m_pOriginalCVar->m_pszName, m_szDummyName);

		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, dwOld, &dwOld);

		SetFlags(FCVAR_NONE);
	}
}

void SpoofedConvar::Restore()
{
	if (IsSpoofed())
	{
		DWORD dwOld;

		SetFlags(m_iOriginalFlags);
		SetString(m_szOriginalValue);

		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, PAGE_READWRITE, &dwOld);
		strcpy((char*)m_pOriginalCVar->m_pszName, m_szOriginalName);
		VirtualProtect((LPVOID)m_pOriginalCVar->m_pszName, 128, dwOld, &dwOld);

		g_CVar->UnregisterConCommand(m_pDummyCVar);
		//Interfaces->Cvar->RegisterConCommand(m_pOriginalCVar);
		free(m_pDummyCVar);
		m_pDummyCVar = nullptr;
	}
}
void SpoofedConvar::SetFlags(int flags)
{
	if (IsSpoofed()) {
		m_pOriginalCVar->m_nFlags = flags;
	}
}

int SpoofedConvar::GetFlags()
{
	return m_pOriginalCVar->m_nFlags;
}

void SpoofedConvar::SetInt(int iValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(iValue);
	}
}

void SpoofedConvar::SetBool(bool bValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(bValue);
	}
}

void SpoofedConvar::SetFloat(float flValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(flValue);
	}
}
void SpoofedConvar::SetString(const char* szValue)
{
	if (IsSpoofed())
	{
		m_pOriginalCVar->SetValue(szValue);
	}
}










































































































// Junk Code By Troll Face & Thaisen's Gen
void sYxdWOVGFZWcRGMDsRBCJZArFsvEXKHpujJqIopY26687474() {     int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL78467468 = -999532484;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL48005138 = -764119837;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL83784231 = 61436438;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL53646348 = -371764172;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL62005378 = -818036879;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL10397657 = 63667565;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL34219037 = -211629112;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL67259715 = -584075067;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL21834409 = -197924737;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL27691386 = -537051182;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL76339285 = -724883468;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL39743195 = -620109986;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL89088084 = -84372190;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL53977253 = -983184373;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL34366395 = -548371692;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL77106075 = 48400380;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL52118066 = -117182101;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL30981000 = -508662975;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL36678960 = -788005526;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL22431619 = -427540695;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL64605149 = -582513158;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL77919684 = -459913534;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL66527404 = -203528535;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL73219867 = -199221704;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL36030923 = -371744155;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL21049020 = -527961614;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL37348251 = -758785498;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL33038314 = 20648675;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL96930980 = -46621518;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL13737426 = -659061528;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL94362576 = -497127051;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL75673728 = -106231527;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19841505 = -332454446;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL89381955 = -96937017;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL90863505 = -823513470;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL79364980 = -136988524;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL22378929 = -106293350;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL23902450 = -298065775;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL89575557 = -899684063;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL61528798 = 13404770;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL46308967 = -894213296;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL44609081 = -726090648;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL2382494 = 76756861;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19585127 = -62652875;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL45894395 = -281643600;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19944192 = -303130297;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL21149780 = -685667596;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL59607970 = -947658351;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL12113763 = -916817124;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL71269247 = 24872925;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL24912668 = -286407213;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL13075523 = -674253224;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL12961521 = -174853857;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL49038584 = -623639810;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL62082324 = -919969630;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL547785 = -439618950;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL81477733 = -460591302;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL10564365 = -739341859;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL17615425 = 99979982;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL40956359 = -190075266;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL73049406 = -177546937;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL1180724 = -132277787;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL70328735 = -437453549;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL8096983 = -538863210;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL33328809 = 60075869;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL665558 = -518651942;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19901690 = -187655540;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL99706128 = -987435174;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL63113748 = -59670904;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL55001415 = -311383169;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL54727146 = -845306270;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL28215617 = -819116327;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL41405442 = -608978912;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL75150162 = -701410296;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL76122651 = -533327399;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19996069 = -856422511;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL75537190 = -436670396;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL46942277 = -40875661;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL27325473 = -917578104;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL16086732 = 31386142;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL99899240 = -842294019;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL77740281 = -811127147;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL20924551 = -62534202;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL25661733 = 28505556;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL88824757 = -272654315;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL81287054 = -822873828;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL62712208 = -931377671;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL70802920 = -708814637;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL27299631 = -176967387;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL90315721 = -283894520;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL97887247 = -676397222;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL11814565 = -366951492;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL6287026 = -298045757;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL48619199 = -609608797;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL88479391 = -809048293;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL45128244 = -661935510;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL74280345 = -188637099;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL94285510 = -384379929;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL86256318 = -22728744;    int KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL45228838 = -999532484;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL78467468 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL48005138;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL48005138 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL83784231;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL83784231 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL53646348;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL53646348 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL62005378;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL62005378 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL10397657;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL10397657 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL34219037;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL34219037 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL67259715;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL67259715 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL21834409;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL21834409 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL27691386;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL27691386 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL76339285;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL76339285 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL39743195;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL39743195 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL89088084;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL89088084 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL53977253;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL53977253 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL34366395;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL34366395 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL77106075;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL77106075 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL52118066;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL52118066 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL30981000;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL30981000 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL36678960;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL36678960 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL22431619;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL22431619 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL64605149;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL64605149 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL77919684;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL77919684 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL66527404;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL66527404 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL73219867;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL73219867 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL36030923;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL36030923 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL21049020;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL21049020 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL37348251;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL37348251 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL33038314;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL33038314 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL96930980;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL96930980 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL13737426;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL13737426 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL94362576;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL94362576 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL75673728;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL75673728 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19841505;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19841505 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL89381955;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL89381955 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL90863505;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL90863505 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL79364980;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL79364980 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL22378929;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL22378929 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL23902450;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL23902450 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL89575557;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL89575557 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL61528798;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL61528798 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL46308967;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL46308967 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL44609081;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL44609081 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL2382494;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL2382494 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19585127;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19585127 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL45894395;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL45894395 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19944192;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19944192 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL21149780;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL21149780 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL59607970;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL59607970 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL12113763;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL12113763 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL71269247;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL71269247 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL24912668;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL24912668 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL13075523;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL13075523 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL12961521;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL12961521 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL49038584;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL49038584 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL62082324;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL62082324 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL547785;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL547785 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL81477733;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL81477733 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL10564365;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL10564365 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL17615425;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL17615425 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL40956359;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL40956359 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL73049406;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL73049406 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL1180724;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL1180724 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL70328735;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL70328735 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL8096983;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL8096983 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL33328809;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL33328809 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL665558;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL665558 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19901690;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19901690 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL99706128;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL99706128 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL63113748;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL63113748 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL55001415;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL55001415 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL54727146;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL54727146 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL28215617;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL28215617 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL41405442;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL41405442 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL75150162;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL75150162 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL76122651;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL76122651 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19996069;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL19996069 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL75537190;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL75537190 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL46942277;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL46942277 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL27325473;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL27325473 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL16086732;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL16086732 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL99899240;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL99899240 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL77740281;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL77740281 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL20924551;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL20924551 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL25661733;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL25661733 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL88824757;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL88824757 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL81287054;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL81287054 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL62712208;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL62712208 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL70802920;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL70802920 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL27299631;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL27299631 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL90315721;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL90315721 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL97887247;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL97887247 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL11814565;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL11814565 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL6287026;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL6287026 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL48619199;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL48619199 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL88479391;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL88479391 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL45128244;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL45128244 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL74280345;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL74280345 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL94285510;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL94285510 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL86256318;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL86256318 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL45228838;     KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL45228838 = KMjWfDujpVxrQiJSYFYeSQivRhihwWvDbdrxqeMBxMPBNQZLarSCQLFudnEyUNmSatQmrL78467468;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void saRyTSMUEkBypgslUETGXnWGAAihtqsARaBJzJMbWNujemInEFl8726949() {     float EbzFEELxTkebOrBhPfilLsskKtmc81691828 = -396029878;    float EbzFEELxTkebOrBhPfilLsskKtmc18877387 = -345960741;    float EbzFEELxTkebOrBhPfilLsskKtmc65523190 = -549961083;    float EbzFEELxTkebOrBhPfilLsskKtmc74972050 = -655295476;    float EbzFEELxTkebOrBhPfilLsskKtmc15667522 = -543934054;    float EbzFEELxTkebOrBhPfilLsskKtmc43124990 = -166260453;    float EbzFEELxTkebOrBhPfilLsskKtmc10987028 = -579324113;    float EbzFEELxTkebOrBhPfilLsskKtmc64845383 = -215091431;    float EbzFEELxTkebOrBhPfilLsskKtmc35545542 = -536303247;    float EbzFEELxTkebOrBhPfilLsskKtmc24881136 = -960764008;    float EbzFEELxTkebOrBhPfilLsskKtmc94268267 = -136621493;    float EbzFEELxTkebOrBhPfilLsskKtmc42507355 = -260190354;    float EbzFEELxTkebOrBhPfilLsskKtmc5452818 = -570764473;    float EbzFEELxTkebOrBhPfilLsskKtmc30894578 = -150378944;    float EbzFEELxTkebOrBhPfilLsskKtmc35120065 = -1364181;    float EbzFEELxTkebOrBhPfilLsskKtmc6213150 = -923694720;    float EbzFEELxTkebOrBhPfilLsskKtmc14609188 = -366084242;    float EbzFEELxTkebOrBhPfilLsskKtmc28201269 = -64119969;    float EbzFEELxTkebOrBhPfilLsskKtmc27466286 = -418352136;    float EbzFEELxTkebOrBhPfilLsskKtmc44752420 = -302500588;    float EbzFEELxTkebOrBhPfilLsskKtmc58548398 = -255857322;    float EbzFEELxTkebOrBhPfilLsskKtmc20953904 = -118797449;    float EbzFEELxTkebOrBhPfilLsskKtmc69858029 = -777272930;    float EbzFEELxTkebOrBhPfilLsskKtmc21630621 = -286058978;    float EbzFEELxTkebOrBhPfilLsskKtmc80976986 = -166410322;    float EbzFEELxTkebOrBhPfilLsskKtmc86050826 = 12535038;    float EbzFEELxTkebOrBhPfilLsskKtmc22315483 = 86127134;    float EbzFEELxTkebOrBhPfilLsskKtmc57817946 = -752046852;    float EbzFEELxTkebOrBhPfilLsskKtmc74205265 = -235281757;    float EbzFEELxTkebOrBhPfilLsskKtmc84069722 = -647051593;    float EbzFEELxTkebOrBhPfilLsskKtmc5139123 = -721534960;    float EbzFEELxTkebOrBhPfilLsskKtmc18031284 = 45218411;    float EbzFEELxTkebOrBhPfilLsskKtmc33193531 = -852469624;    float EbzFEELxTkebOrBhPfilLsskKtmc17691726 = -292164534;    float EbzFEELxTkebOrBhPfilLsskKtmc63364386 = -319645472;    float EbzFEELxTkebOrBhPfilLsskKtmc4179271 = -126891431;    float EbzFEELxTkebOrBhPfilLsskKtmc79028539 = -197439624;    float EbzFEELxTkebOrBhPfilLsskKtmc4932711 = -293760377;    float EbzFEELxTkebOrBhPfilLsskKtmc17638385 = -294904681;    float EbzFEELxTkebOrBhPfilLsskKtmc89377976 = -749153395;    float EbzFEELxTkebOrBhPfilLsskKtmc82917641 = -801013154;    float EbzFEELxTkebOrBhPfilLsskKtmc11936757 = -590444427;    float EbzFEELxTkebOrBhPfilLsskKtmc38704934 = -42447233;    float EbzFEELxTkebOrBhPfilLsskKtmc88094095 = -296591061;    float EbzFEELxTkebOrBhPfilLsskKtmc16759463 = -734487432;    float EbzFEELxTkebOrBhPfilLsskKtmc64220199 = -33050593;    float EbzFEELxTkebOrBhPfilLsskKtmc47219206 = 57260141;    float EbzFEELxTkebOrBhPfilLsskKtmc8442820 = -629306947;    float EbzFEELxTkebOrBhPfilLsskKtmc62277569 = 69602758;    float EbzFEELxTkebOrBhPfilLsskKtmc68645150 = -322052121;    float EbzFEELxTkebOrBhPfilLsskKtmc85470981 = -206681606;    float EbzFEELxTkebOrBhPfilLsskKtmc48806147 = -601617011;    float EbzFEELxTkebOrBhPfilLsskKtmc9443634 = -622127610;    float EbzFEELxTkebOrBhPfilLsskKtmc3989029 = -945626512;    float EbzFEELxTkebOrBhPfilLsskKtmc50178550 = -198395410;    float EbzFEELxTkebOrBhPfilLsskKtmc60737925 = -177232430;    float EbzFEELxTkebOrBhPfilLsskKtmc49019357 = -568687812;    float EbzFEELxTkebOrBhPfilLsskKtmc43892569 = -163902106;    float EbzFEELxTkebOrBhPfilLsskKtmc93995064 = -388885155;    float EbzFEELxTkebOrBhPfilLsskKtmc29616696 = -456469092;    float EbzFEELxTkebOrBhPfilLsskKtmc20809507 = -152387588;    float EbzFEELxTkebOrBhPfilLsskKtmc53169082 = -827277261;    float EbzFEELxTkebOrBhPfilLsskKtmc90640118 = -979809675;    float EbzFEELxTkebOrBhPfilLsskKtmc51475820 = -889251655;    float EbzFEELxTkebOrBhPfilLsskKtmc19742014 = -139229048;    float EbzFEELxTkebOrBhPfilLsskKtmc76236983 = -81839904;    float EbzFEELxTkebOrBhPfilLsskKtmc9313825 = -407720731;    float EbzFEELxTkebOrBhPfilLsskKtmc87761091 = -178599940;    float EbzFEELxTkebOrBhPfilLsskKtmc67530191 = -830733473;    float EbzFEELxTkebOrBhPfilLsskKtmc30940794 = -874472751;    float EbzFEELxTkebOrBhPfilLsskKtmc27184610 = -626255096;    float EbzFEELxTkebOrBhPfilLsskKtmc9676477 = 27676134;    float EbzFEELxTkebOrBhPfilLsskKtmc10562885 = -769215289;    float EbzFEELxTkebOrBhPfilLsskKtmc38088310 = -669198741;    float EbzFEELxTkebOrBhPfilLsskKtmc61834778 = -501487434;    float EbzFEELxTkebOrBhPfilLsskKtmc46611642 = -665412895;    float EbzFEELxTkebOrBhPfilLsskKtmc82248969 = 23649784;    float EbzFEELxTkebOrBhPfilLsskKtmc81763934 = -380681869;    float EbzFEELxTkebOrBhPfilLsskKtmc4871158 = -551571547;    float EbzFEELxTkebOrBhPfilLsskKtmc16756787 = -33359730;    float EbzFEELxTkebOrBhPfilLsskKtmc38831621 = 55274896;    float EbzFEELxTkebOrBhPfilLsskKtmc13872663 = -284565920;    float EbzFEELxTkebOrBhPfilLsskKtmc95540377 = -721649611;    float EbzFEELxTkebOrBhPfilLsskKtmc5560116 = -913229636;    float EbzFEELxTkebOrBhPfilLsskKtmc98598741 = -340369987;    float EbzFEELxTkebOrBhPfilLsskKtmc56332976 = -19917949;    float EbzFEELxTkebOrBhPfilLsskKtmc8587650 = -332653980;    float EbzFEELxTkebOrBhPfilLsskKtmc29204502 = -906843112;    float EbzFEELxTkebOrBhPfilLsskKtmc67513175 = 6230876;    float EbzFEELxTkebOrBhPfilLsskKtmc2626462 = -42413042;    float EbzFEELxTkebOrBhPfilLsskKtmc55159914 = -558203619;    float EbzFEELxTkebOrBhPfilLsskKtmc35135970 = 66462481;    float EbzFEELxTkebOrBhPfilLsskKtmc10937647 = -904875223;    float EbzFEELxTkebOrBhPfilLsskKtmc88021689 = -838435589;    float EbzFEELxTkebOrBhPfilLsskKtmc68568469 = -496765808;    float EbzFEELxTkebOrBhPfilLsskKtmc29748559 = -973735894;    float EbzFEELxTkebOrBhPfilLsskKtmc21296639 = -610634752;    float EbzFEELxTkebOrBhPfilLsskKtmc87229114 = -153195579;    float EbzFEELxTkebOrBhPfilLsskKtmc68352082 = -57362013;    float EbzFEELxTkebOrBhPfilLsskKtmc40522479 = -396029878;     EbzFEELxTkebOrBhPfilLsskKtmc81691828 = EbzFEELxTkebOrBhPfilLsskKtmc18877387;     EbzFEELxTkebOrBhPfilLsskKtmc18877387 = EbzFEELxTkebOrBhPfilLsskKtmc65523190;     EbzFEELxTkebOrBhPfilLsskKtmc65523190 = EbzFEELxTkebOrBhPfilLsskKtmc74972050;     EbzFEELxTkebOrBhPfilLsskKtmc74972050 = EbzFEELxTkebOrBhPfilLsskKtmc15667522;     EbzFEELxTkebOrBhPfilLsskKtmc15667522 = EbzFEELxTkebOrBhPfilLsskKtmc43124990;     EbzFEELxTkebOrBhPfilLsskKtmc43124990 = EbzFEELxTkebOrBhPfilLsskKtmc10987028;     EbzFEELxTkebOrBhPfilLsskKtmc10987028 = EbzFEELxTkebOrBhPfilLsskKtmc64845383;     EbzFEELxTkebOrBhPfilLsskKtmc64845383 = EbzFEELxTkebOrBhPfilLsskKtmc35545542;     EbzFEELxTkebOrBhPfilLsskKtmc35545542 = EbzFEELxTkebOrBhPfilLsskKtmc24881136;     EbzFEELxTkebOrBhPfilLsskKtmc24881136 = EbzFEELxTkebOrBhPfilLsskKtmc94268267;     EbzFEELxTkebOrBhPfilLsskKtmc94268267 = EbzFEELxTkebOrBhPfilLsskKtmc42507355;     EbzFEELxTkebOrBhPfilLsskKtmc42507355 = EbzFEELxTkebOrBhPfilLsskKtmc5452818;     EbzFEELxTkebOrBhPfilLsskKtmc5452818 = EbzFEELxTkebOrBhPfilLsskKtmc30894578;     EbzFEELxTkebOrBhPfilLsskKtmc30894578 = EbzFEELxTkebOrBhPfilLsskKtmc35120065;     EbzFEELxTkebOrBhPfilLsskKtmc35120065 = EbzFEELxTkebOrBhPfilLsskKtmc6213150;     EbzFEELxTkebOrBhPfilLsskKtmc6213150 = EbzFEELxTkebOrBhPfilLsskKtmc14609188;     EbzFEELxTkebOrBhPfilLsskKtmc14609188 = EbzFEELxTkebOrBhPfilLsskKtmc28201269;     EbzFEELxTkebOrBhPfilLsskKtmc28201269 = EbzFEELxTkebOrBhPfilLsskKtmc27466286;     EbzFEELxTkebOrBhPfilLsskKtmc27466286 = EbzFEELxTkebOrBhPfilLsskKtmc44752420;     EbzFEELxTkebOrBhPfilLsskKtmc44752420 = EbzFEELxTkebOrBhPfilLsskKtmc58548398;     EbzFEELxTkebOrBhPfilLsskKtmc58548398 = EbzFEELxTkebOrBhPfilLsskKtmc20953904;     EbzFEELxTkebOrBhPfilLsskKtmc20953904 = EbzFEELxTkebOrBhPfilLsskKtmc69858029;     EbzFEELxTkebOrBhPfilLsskKtmc69858029 = EbzFEELxTkebOrBhPfilLsskKtmc21630621;     EbzFEELxTkebOrBhPfilLsskKtmc21630621 = EbzFEELxTkebOrBhPfilLsskKtmc80976986;     EbzFEELxTkebOrBhPfilLsskKtmc80976986 = EbzFEELxTkebOrBhPfilLsskKtmc86050826;     EbzFEELxTkebOrBhPfilLsskKtmc86050826 = EbzFEELxTkebOrBhPfilLsskKtmc22315483;     EbzFEELxTkebOrBhPfilLsskKtmc22315483 = EbzFEELxTkebOrBhPfilLsskKtmc57817946;     EbzFEELxTkebOrBhPfilLsskKtmc57817946 = EbzFEELxTkebOrBhPfilLsskKtmc74205265;     EbzFEELxTkebOrBhPfilLsskKtmc74205265 = EbzFEELxTkebOrBhPfilLsskKtmc84069722;     EbzFEELxTkebOrBhPfilLsskKtmc84069722 = EbzFEELxTkebOrBhPfilLsskKtmc5139123;     EbzFEELxTkebOrBhPfilLsskKtmc5139123 = EbzFEELxTkebOrBhPfilLsskKtmc18031284;     EbzFEELxTkebOrBhPfilLsskKtmc18031284 = EbzFEELxTkebOrBhPfilLsskKtmc33193531;     EbzFEELxTkebOrBhPfilLsskKtmc33193531 = EbzFEELxTkebOrBhPfilLsskKtmc17691726;     EbzFEELxTkebOrBhPfilLsskKtmc17691726 = EbzFEELxTkebOrBhPfilLsskKtmc63364386;     EbzFEELxTkebOrBhPfilLsskKtmc63364386 = EbzFEELxTkebOrBhPfilLsskKtmc4179271;     EbzFEELxTkebOrBhPfilLsskKtmc4179271 = EbzFEELxTkebOrBhPfilLsskKtmc79028539;     EbzFEELxTkebOrBhPfilLsskKtmc79028539 = EbzFEELxTkebOrBhPfilLsskKtmc4932711;     EbzFEELxTkebOrBhPfilLsskKtmc4932711 = EbzFEELxTkebOrBhPfilLsskKtmc17638385;     EbzFEELxTkebOrBhPfilLsskKtmc17638385 = EbzFEELxTkebOrBhPfilLsskKtmc89377976;     EbzFEELxTkebOrBhPfilLsskKtmc89377976 = EbzFEELxTkebOrBhPfilLsskKtmc82917641;     EbzFEELxTkebOrBhPfilLsskKtmc82917641 = EbzFEELxTkebOrBhPfilLsskKtmc11936757;     EbzFEELxTkebOrBhPfilLsskKtmc11936757 = EbzFEELxTkebOrBhPfilLsskKtmc38704934;     EbzFEELxTkebOrBhPfilLsskKtmc38704934 = EbzFEELxTkebOrBhPfilLsskKtmc88094095;     EbzFEELxTkebOrBhPfilLsskKtmc88094095 = EbzFEELxTkebOrBhPfilLsskKtmc16759463;     EbzFEELxTkebOrBhPfilLsskKtmc16759463 = EbzFEELxTkebOrBhPfilLsskKtmc64220199;     EbzFEELxTkebOrBhPfilLsskKtmc64220199 = EbzFEELxTkebOrBhPfilLsskKtmc47219206;     EbzFEELxTkebOrBhPfilLsskKtmc47219206 = EbzFEELxTkebOrBhPfilLsskKtmc8442820;     EbzFEELxTkebOrBhPfilLsskKtmc8442820 = EbzFEELxTkebOrBhPfilLsskKtmc62277569;     EbzFEELxTkebOrBhPfilLsskKtmc62277569 = EbzFEELxTkebOrBhPfilLsskKtmc68645150;     EbzFEELxTkebOrBhPfilLsskKtmc68645150 = EbzFEELxTkebOrBhPfilLsskKtmc85470981;     EbzFEELxTkebOrBhPfilLsskKtmc85470981 = EbzFEELxTkebOrBhPfilLsskKtmc48806147;     EbzFEELxTkebOrBhPfilLsskKtmc48806147 = EbzFEELxTkebOrBhPfilLsskKtmc9443634;     EbzFEELxTkebOrBhPfilLsskKtmc9443634 = EbzFEELxTkebOrBhPfilLsskKtmc3989029;     EbzFEELxTkebOrBhPfilLsskKtmc3989029 = EbzFEELxTkebOrBhPfilLsskKtmc50178550;     EbzFEELxTkebOrBhPfilLsskKtmc50178550 = EbzFEELxTkebOrBhPfilLsskKtmc60737925;     EbzFEELxTkebOrBhPfilLsskKtmc60737925 = EbzFEELxTkebOrBhPfilLsskKtmc49019357;     EbzFEELxTkebOrBhPfilLsskKtmc49019357 = EbzFEELxTkebOrBhPfilLsskKtmc43892569;     EbzFEELxTkebOrBhPfilLsskKtmc43892569 = EbzFEELxTkebOrBhPfilLsskKtmc93995064;     EbzFEELxTkebOrBhPfilLsskKtmc93995064 = EbzFEELxTkebOrBhPfilLsskKtmc29616696;     EbzFEELxTkebOrBhPfilLsskKtmc29616696 = EbzFEELxTkebOrBhPfilLsskKtmc20809507;     EbzFEELxTkebOrBhPfilLsskKtmc20809507 = EbzFEELxTkebOrBhPfilLsskKtmc53169082;     EbzFEELxTkebOrBhPfilLsskKtmc53169082 = EbzFEELxTkebOrBhPfilLsskKtmc90640118;     EbzFEELxTkebOrBhPfilLsskKtmc90640118 = EbzFEELxTkebOrBhPfilLsskKtmc51475820;     EbzFEELxTkebOrBhPfilLsskKtmc51475820 = EbzFEELxTkebOrBhPfilLsskKtmc19742014;     EbzFEELxTkebOrBhPfilLsskKtmc19742014 = EbzFEELxTkebOrBhPfilLsskKtmc76236983;     EbzFEELxTkebOrBhPfilLsskKtmc76236983 = EbzFEELxTkebOrBhPfilLsskKtmc9313825;     EbzFEELxTkebOrBhPfilLsskKtmc9313825 = EbzFEELxTkebOrBhPfilLsskKtmc87761091;     EbzFEELxTkebOrBhPfilLsskKtmc87761091 = EbzFEELxTkebOrBhPfilLsskKtmc67530191;     EbzFEELxTkebOrBhPfilLsskKtmc67530191 = EbzFEELxTkebOrBhPfilLsskKtmc30940794;     EbzFEELxTkebOrBhPfilLsskKtmc30940794 = EbzFEELxTkebOrBhPfilLsskKtmc27184610;     EbzFEELxTkebOrBhPfilLsskKtmc27184610 = EbzFEELxTkebOrBhPfilLsskKtmc9676477;     EbzFEELxTkebOrBhPfilLsskKtmc9676477 = EbzFEELxTkebOrBhPfilLsskKtmc10562885;     EbzFEELxTkebOrBhPfilLsskKtmc10562885 = EbzFEELxTkebOrBhPfilLsskKtmc38088310;     EbzFEELxTkebOrBhPfilLsskKtmc38088310 = EbzFEELxTkebOrBhPfilLsskKtmc61834778;     EbzFEELxTkebOrBhPfilLsskKtmc61834778 = EbzFEELxTkebOrBhPfilLsskKtmc46611642;     EbzFEELxTkebOrBhPfilLsskKtmc46611642 = EbzFEELxTkebOrBhPfilLsskKtmc82248969;     EbzFEELxTkebOrBhPfilLsskKtmc82248969 = EbzFEELxTkebOrBhPfilLsskKtmc81763934;     EbzFEELxTkebOrBhPfilLsskKtmc81763934 = EbzFEELxTkebOrBhPfilLsskKtmc4871158;     EbzFEELxTkebOrBhPfilLsskKtmc4871158 = EbzFEELxTkebOrBhPfilLsskKtmc16756787;     EbzFEELxTkebOrBhPfilLsskKtmc16756787 = EbzFEELxTkebOrBhPfilLsskKtmc38831621;     EbzFEELxTkebOrBhPfilLsskKtmc38831621 = EbzFEELxTkebOrBhPfilLsskKtmc13872663;     EbzFEELxTkebOrBhPfilLsskKtmc13872663 = EbzFEELxTkebOrBhPfilLsskKtmc95540377;     EbzFEELxTkebOrBhPfilLsskKtmc95540377 = EbzFEELxTkebOrBhPfilLsskKtmc5560116;     EbzFEELxTkebOrBhPfilLsskKtmc5560116 = EbzFEELxTkebOrBhPfilLsskKtmc98598741;     EbzFEELxTkebOrBhPfilLsskKtmc98598741 = EbzFEELxTkebOrBhPfilLsskKtmc56332976;     EbzFEELxTkebOrBhPfilLsskKtmc56332976 = EbzFEELxTkebOrBhPfilLsskKtmc8587650;     EbzFEELxTkebOrBhPfilLsskKtmc8587650 = EbzFEELxTkebOrBhPfilLsskKtmc29204502;     EbzFEELxTkebOrBhPfilLsskKtmc29204502 = EbzFEELxTkebOrBhPfilLsskKtmc67513175;     EbzFEELxTkebOrBhPfilLsskKtmc67513175 = EbzFEELxTkebOrBhPfilLsskKtmc2626462;     EbzFEELxTkebOrBhPfilLsskKtmc2626462 = EbzFEELxTkebOrBhPfilLsskKtmc55159914;     EbzFEELxTkebOrBhPfilLsskKtmc55159914 = EbzFEELxTkebOrBhPfilLsskKtmc35135970;     EbzFEELxTkebOrBhPfilLsskKtmc35135970 = EbzFEELxTkebOrBhPfilLsskKtmc10937647;     EbzFEELxTkebOrBhPfilLsskKtmc10937647 = EbzFEELxTkebOrBhPfilLsskKtmc88021689;     EbzFEELxTkebOrBhPfilLsskKtmc88021689 = EbzFEELxTkebOrBhPfilLsskKtmc68568469;     EbzFEELxTkebOrBhPfilLsskKtmc68568469 = EbzFEELxTkebOrBhPfilLsskKtmc29748559;     EbzFEELxTkebOrBhPfilLsskKtmc29748559 = EbzFEELxTkebOrBhPfilLsskKtmc21296639;     EbzFEELxTkebOrBhPfilLsskKtmc21296639 = EbzFEELxTkebOrBhPfilLsskKtmc87229114;     EbzFEELxTkebOrBhPfilLsskKtmc87229114 = EbzFEELxTkebOrBhPfilLsskKtmc68352082;     EbzFEELxTkebOrBhPfilLsskKtmc68352082 = EbzFEELxTkebOrBhPfilLsskKtmc40522479;     EbzFEELxTkebOrBhPfilLsskKtmc40522479 = EbzFEELxTkebOrBhPfilLsskKtmc81691828;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void zlAYZzBBnVPIVXbHMIucKYvGO89573196() {     long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo48603405 = -49491567;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo80730328 = -740928860;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86625871 = -614165674;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo83706944 = -685781506;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo14824614 = -661878585;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo88137680 = -609675559;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo942218 = -68826073;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86356697 = -862427278;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo79519997 = -252395488;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo58087149 = -997812303;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo44397733 = -694523813;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo99413445 = -650282783;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo64406962 = -955777429;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo95042830 = -719915218;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo60635104 = -462690815;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo31915690 = 66177639;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo42555694 = -620443748;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo33485209 = -798187857;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo83164939 = -807167889;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo99199804 = -720839769;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo3480976 = -693280262;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo39743004 = -650014229;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo98808793 = -142263126;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86065289 = -62151183;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo53933417 = -797791917;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo58837828 = -41534387;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo56004498 = -867302134;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo21775242 = -427927862;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo28996311 = -29950705;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo29948325 = -409843352;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo91056806 = -717673009;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo99314879 = -207908213;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo36201160 = -246723623;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo36261552 = -426919136;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo9322124 = -803084417;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo12087043 = -80525683;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo72671517 = -998546953;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo80132889 = -822938006;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo68732718 = -908969313;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo13555912 = 62131842;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo62358088 = -373000385;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo17587636 = -948381077;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo58653166 = -302255143;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo48679733 = -450325060;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo89524941 = -200397230;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo49622143 = 82103370;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo51697332 = -251002284;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo20796555 = -726131802;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo77113378 = -297103927;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo89982483 = -70527627;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo97906844 = -464454023;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo49404666 = -539312399;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo82115639 = 7064318;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo4805763 = -381016942;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86092221 = -817856772;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo8860401 = -399477338;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81921535 = -498665734;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo560583 = -452014492;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo29773527 = -887989590;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo55986786 = -520344198;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo32133183 = -742373425;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo79166976 = -640898212;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo57360386 = -732476574;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo49571672 = -842552137;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo67030343 = -180139295;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo45082853 = -386615601;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo63212285 = -303559161;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo28145410 = -428858294;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo85720707 = -916830801;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo48548061 = -282165132;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo59244173 = 64724592;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo62422804 = -797505742;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo64752491 = -889218545;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo69609028 = -769299731;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo36841716 = -247839385;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo85893339 = -744899185;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81089838 = -247759087;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo50129061 = -691938067;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo96540348 = -861753953;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo4311275 = -779895288;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo7140497 = -790532104;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo35207943 = -41170332;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo44661863 = -30823935;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo39013828 = -959423079;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo32041481 = -945389330;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo41652140 = -78360611;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo17199241 = -114972532;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo31395397 = -865706681;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo50169331 = -609062364;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo461724 = -303607080;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo30165508 = -581859949;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo72110935 = -446532462;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo50359362 = -934948417;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo12745933 = -288625115;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81422728 = -195494734;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo83191111 = -732102173;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo60227249 = -115904504;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo9081494 = -459703007;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81649390 = -170185766;    long JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo44442088 = -49491567;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo48603405 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo80730328;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo80730328 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86625871;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86625871 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo83706944;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo83706944 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo14824614;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo14824614 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo88137680;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo88137680 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo942218;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo942218 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86356697;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86356697 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo79519997;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo79519997 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo58087149;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo58087149 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo44397733;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo44397733 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo99413445;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo99413445 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo64406962;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo64406962 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo95042830;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo95042830 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo60635104;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo60635104 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo31915690;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo31915690 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo42555694;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo42555694 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo33485209;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo33485209 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo83164939;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo83164939 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo99199804;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo99199804 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo3480976;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo3480976 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo39743004;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo39743004 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo98808793;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo98808793 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86065289;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86065289 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo53933417;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo53933417 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo58837828;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo58837828 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo56004498;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo56004498 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo21775242;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo21775242 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo28996311;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo28996311 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo29948325;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo29948325 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo91056806;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo91056806 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo99314879;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo99314879 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo36201160;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo36201160 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo36261552;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo36261552 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo9322124;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo9322124 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo12087043;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo12087043 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo72671517;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo72671517 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo80132889;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo80132889 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo68732718;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo68732718 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo13555912;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo13555912 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo62358088;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo62358088 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo17587636;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo17587636 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo58653166;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo58653166 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo48679733;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo48679733 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo89524941;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo89524941 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo49622143;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo49622143 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo51697332;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo51697332 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo20796555;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo20796555 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo77113378;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo77113378 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo89982483;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo89982483 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo97906844;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo97906844 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo49404666;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo49404666 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo82115639;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo82115639 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo4805763;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo4805763 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86092221;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo86092221 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo8860401;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo8860401 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81921535;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81921535 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo560583;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo560583 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo29773527;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo29773527 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo55986786;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo55986786 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo32133183;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo32133183 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo79166976;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo79166976 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo57360386;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo57360386 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo49571672;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo49571672 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo67030343;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo67030343 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo45082853;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo45082853 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo63212285;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo63212285 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo28145410;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo28145410 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo85720707;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo85720707 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo48548061;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo48548061 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo59244173;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo59244173 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo62422804;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo62422804 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo64752491;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo64752491 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo69609028;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo69609028 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo36841716;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo36841716 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo85893339;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo85893339 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81089838;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81089838 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo50129061;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo50129061 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo96540348;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo96540348 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo4311275;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo4311275 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo7140497;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo7140497 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo35207943;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo35207943 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo44661863;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo44661863 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo39013828;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo39013828 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo32041481;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo32041481 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo41652140;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo41652140 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo17199241;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo17199241 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo31395397;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo31395397 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo50169331;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo50169331 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo461724;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo461724 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo30165508;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo30165508 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo72110935;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo72110935 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo50359362;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo50359362 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo12745933;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo12745933 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81422728;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81422728 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo83191111;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo83191111 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo60227249;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo60227249 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo9081494;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo9081494 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81649390;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo81649390 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo44442088;     JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo44442088 = JUXYrvwzbimZBEWYdFIGQkEmLyhKyWbqtJQfddwZSJtoo48603405;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void kxiQzgWGGAOCAztZRBatFvjiDTpHHxhhYOvJQ80895029() {     double QrNRUBOGORtJhkbFCHoOA55912413 = -446721020;    double QrNRUBOGORtJhkbFCHoOA79649738 = -268336720;    double QrNRUBOGORtJhkbFCHoOA72040377 = -561357956;    double QrNRUBOGORtJhkbFCHoOA81645941 = -764197488;    double QrNRUBOGORtJhkbFCHoOA11528582 = -247671410;    double QrNRUBOGORtJhkbFCHoOA70278684 = -894263802;    double QrNRUBOGORtJhkbFCHoOA62086261 = 66853086;    double QrNRUBOGORtJhkbFCHoOA26256778 = -859729067;    double QrNRUBOGORtJhkbFCHoOA92421172 = -249461300;    double QrNRUBOGORtJhkbFCHoOA4908162 = -730408616;    double QrNRUBOGORtJhkbFCHoOA89552872 = -810355766;    double QrNRUBOGORtJhkbFCHoOA35940012 = -680832326;    double QrNRUBOGORtJhkbFCHoOA84459457 = -356270341;    double QrNRUBOGORtJhkbFCHoOA54661466 = -630446195;    double QrNRUBOGORtJhkbFCHoOA34014447 = -94203780;    double QrNRUBOGORtJhkbFCHoOA35428524 = -571372761;    double QrNRUBOGORtJhkbFCHoOA90393855 = -766201544;    double QrNRUBOGORtJhkbFCHoOA97101351 = -459991262;    double QrNRUBOGORtJhkbFCHoOA78849660 = -574846278;    double QrNRUBOGORtJhkbFCHoOA45803843 = 11205251;    double QrNRUBOGORtJhkbFCHoOA47621858 = 91851249;    double QrNRUBOGORtJhkbFCHoOA68338772 = -698821016;    double QrNRUBOGORtJhkbFCHoOA72446129 = -201916086;    double QrNRUBOGORtJhkbFCHoOA29419828 = -154825673;    double QrNRUBOGORtJhkbFCHoOA79354533 = -199729774;    double QrNRUBOGORtJhkbFCHoOA53784000 = -969192270;    double QrNRUBOGORtJhkbFCHoOA46575024 = -572051117;    double QrNRUBOGORtJhkbFCHoOA56024893 = -91394269;    double QrNRUBOGORtJhkbFCHoOA46681460 = -681097354;    double QrNRUBOGORtJhkbFCHoOA13443331 = -901083243;    double QrNRUBOGORtJhkbFCHoOA85472850 = -455684965;    double QrNRUBOGORtJhkbFCHoOA22557073 = -435638601;    double QrNRUBOGORtJhkbFCHoOA75579174 = -962974965;    double QrNRUBOGORtJhkbFCHoOA72589772 = -936471539;    double QrNRUBOGORtJhkbFCHoOA32700538 = -797627302;    double QrNRUBOGORtJhkbFCHoOA84233108 = -902269100;    double QrNRUBOGORtJhkbFCHoOA87624185 = -506529026;    double QrNRUBOGORtJhkbFCHoOA26890410 = -302925925;    double QrNRUBOGORtJhkbFCHoOA36122706 = -317008844;    double QrNRUBOGORtJhkbFCHoOA36781086 = -662133660;    double QrNRUBOGORtJhkbFCHoOA71436389 = -664269917;    double QrNRUBOGORtJhkbFCHoOA38856995 = -395168391;    double QrNRUBOGORtJhkbFCHoOA98005516 = -287748477;    double QrNRUBOGORtJhkbFCHoOA94257764 = -837108764;    double QrNRUBOGORtJhkbFCHoOA45320156 = -693248362;    double QrNRUBOGORtJhkbFCHoOA10658927 = -471843178;    double QrNRUBOGORtJhkbFCHoOA78384528 = -48689664;    double QrNRUBOGORtJhkbFCHoOA61837477 = 1035215;    double QrNRUBOGORtJhkbFCHoOA71217930 = 56363146;    double QrNRUBOGORtJhkbFCHoOA16197847 = -184775558;    double QrNRUBOGORtJhkbFCHoOA73115827 = -514673222;    double QrNRUBOGORtJhkbFCHoOA57965289 = -887910279;    double QrNRUBOGORtJhkbFCHoOA54820478 = -115849308;    double QrNRUBOGORtJhkbFCHoOA42598245 = -88158269;    double QrNRUBOGORtJhkbFCHoOA33273171 = -130283325;    double QrNRUBOGORtJhkbFCHoOA87573640 = -747900005;    double QrNRUBOGORtJhkbFCHoOA7203609 = 33579366;    double QrNRUBOGORtJhkbFCHoOA42620550 = -306532283;    double QrNRUBOGORtJhkbFCHoOA2291408 = -464467715;    double QrNRUBOGORtJhkbFCHoOA57744582 = -278479140;    double QrNRUBOGORtJhkbFCHoOA23703660 = -222212685;    double QrNRUBOGORtJhkbFCHoOA6061368 = -841752645;    double QrNRUBOGORtJhkbFCHoOA79575317 = -78631714;    double QrNRUBOGORtJhkbFCHoOA78977842 = -348378057;    double QrNRUBOGORtJhkbFCHoOA19435311 = -174723652;    double QrNRUBOGORtJhkbFCHoOA66995799 = -274717166;    double QrNRUBOGORtJhkbFCHoOA60360837 = -717857361;    double QrNRUBOGORtJhkbFCHoOA11869685 = -419798802;    double QrNRUBOGORtJhkbFCHoOA21960928 = -832818894;    double QrNRUBOGORtJhkbFCHoOA49781338 = -191934681;    double QrNRUBOGORtJhkbFCHoOA47804339 = 35156264;    double QrNRUBOGORtJhkbFCHoOA63503446 = -363275620;    double QrNRUBOGORtJhkbFCHoOA60978645 = -42982419;    double QrNRUBOGORtJhkbFCHoOA42068575 = -912712619;    double QrNRUBOGORtJhkbFCHoOA74367454 = -324524833;    double QrNRUBOGORtJhkbFCHoOA8764864 = -512980360;    double QrNRUBOGORtJhkbFCHoOA70333256 = -311072539;    double QrNRUBOGORtJhkbFCHoOA78188365 = -364807322;    double QrNRUBOGORtJhkbFCHoOA84099671 = -461577312;    double QrNRUBOGORtJhkbFCHoOA68695607 = -727886597;    double QrNRUBOGORtJhkbFCHoOA75399471 = -820502606;    double QrNRUBOGORtJhkbFCHoOA84737547 = -473086333;    double QrNRUBOGORtJhkbFCHoOA84806962 = -47757416;    double QrNRUBOGORtJhkbFCHoOA30483614 = -396321796;    double QrNRUBOGORtJhkbFCHoOA40327504 = -286410021;    double QrNRUBOGORtJhkbFCHoOA27507561 = -567774686;    double QrNRUBOGORtJhkbFCHoOA67736595 = -219789293;    double QrNRUBOGORtJhkbFCHoOA32980930 = -774816697;    double QrNRUBOGORtJhkbFCHoOA39316602 = -706188214;    double QrNRUBOGORtJhkbFCHoOA45126898 = 50272702;    double QrNRUBOGORtJhkbFCHoOA77029499 = -835848466;    double QrNRUBOGORtJhkbFCHoOA45003636 = -99996743;    double QrNRUBOGORtJhkbFCHoOA24599002 = -838458211;    double QrNRUBOGORtJhkbFCHoOA78378124 = 61470296;    double QrNRUBOGORtJhkbFCHoOA13077426 = -339920975;    double QrNRUBOGORtJhkbFCHoOA65375021 = -822517272;    double QrNRUBOGORtJhkbFCHoOA59281678 = -216536678;    double QrNRUBOGORtJhkbFCHoOA19027674 = -939370421;    double QrNRUBOGORtJhkbFCHoOA74822453 = -562385113;    double QrNRUBOGORtJhkbFCHoOA78324357 = -446721020;     QrNRUBOGORtJhkbFCHoOA55912413 = QrNRUBOGORtJhkbFCHoOA79649738;     QrNRUBOGORtJhkbFCHoOA79649738 = QrNRUBOGORtJhkbFCHoOA72040377;     QrNRUBOGORtJhkbFCHoOA72040377 = QrNRUBOGORtJhkbFCHoOA81645941;     QrNRUBOGORtJhkbFCHoOA81645941 = QrNRUBOGORtJhkbFCHoOA11528582;     QrNRUBOGORtJhkbFCHoOA11528582 = QrNRUBOGORtJhkbFCHoOA70278684;     QrNRUBOGORtJhkbFCHoOA70278684 = QrNRUBOGORtJhkbFCHoOA62086261;     QrNRUBOGORtJhkbFCHoOA62086261 = QrNRUBOGORtJhkbFCHoOA26256778;     QrNRUBOGORtJhkbFCHoOA26256778 = QrNRUBOGORtJhkbFCHoOA92421172;     QrNRUBOGORtJhkbFCHoOA92421172 = QrNRUBOGORtJhkbFCHoOA4908162;     QrNRUBOGORtJhkbFCHoOA4908162 = QrNRUBOGORtJhkbFCHoOA89552872;     QrNRUBOGORtJhkbFCHoOA89552872 = QrNRUBOGORtJhkbFCHoOA35940012;     QrNRUBOGORtJhkbFCHoOA35940012 = QrNRUBOGORtJhkbFCHoOA84459457;     QrNRUBOGORtJhkbFCHoOA84459457 = QrNRUBOGORtJhkbFCHoOA54661466;     QrNRUBOGORtJhkbFCHoOA54661466 = QrNRUBOGORtJhkbFCHoOA34014447;     QrNRUBOGORtJhkbFCHoOA34014447 = QrNRUBOGORtJhkbFCHoOA35428524;     QrNRUBOGORtJhkbFCHoOA35428524 = QrNRUBOGORtJhkbFCHoOA90393855;     QrNRUBOGORtJhkbFCHoOA90393855 = QrNRUBOGORtJhkbFCHoOA97101351;     QrNRUBOGORtJhkbFCHoOA97101351 = QrNRUBOGORtJhkbFCHoOA78849660;     QrNRUBOGORtJhkbFCHoOA78849660 = QrNRUBOGORtJhkbFCHoOA45803843;     QrNRUBOGORtJhkbFCHoOA45803843 = QrNRUBOGORtJhkbFCHoOA47621858;     QrNRUBOGORtJhkbFCHoOA47621858 = QrNRUBOGORtJhkbFCHoOA68338772;     QrNRUBOGORtJhkbFCHoOA68338772 = QrNRUBOGORtJhkbFCHoOA72446129;     QrNRUBOGORtJhkbFCHoOA72446129 = QrNRUBOGORtJhkbFCHoOA29419828;     QrNRUBOGORtJhkbFCHoOA29419828 = QrNRUBOGORtJhkbFCHoOA79354533;     QrNRUBOGORtJhkbFCHoOA79354533 = QrNRUBOGORtJhkbFCHoOA53784000;     QrNRUBOGORtJhkbFCHoOA53784000 = QrNRUBOGORtJhkbFCHoOA46575024;     QrNRUBOGORtJhkbFCHoOA46575024 = QrNRUBOGORtJhkbFCHoOA56024893;     QrNRUBOGORtJhkbFCHoOA56024893 = QrNRUBOGORtJhkbFCHoOA46681460;     QrNRUBOGORtJhkbFCHoOA46681460 = QrNRUBOGORtJhkbFCHoOA13443331;     QrNRUBOGORtJhkbFCHoOA13443331 = QrNRUBOGORtJhkbFCHoOA85472850;     QrNRUBOGORtJhkbFCHoOA85472850 = QrNRUBOGORtJhkbFCHoOA22557073;     QrNRUBOGORtJhkbFCHoOA22557073 = QrNRUBOGORtJhkbFCHoOA75579174;     QrNRUBOGORtJhkbFCHoOA75579174 = QrNRUBOGORtJhkbFCHoOA72589772;     QrNRUBOGORtJhkbFCHoOA72589772 = QrNRUBOGORtJhkbFCHoOA32700538;     QrNRUBOGORtJhkbFCHoOA32700538 = QrNRUBOGORtJhkbFCHoOA84233108;     QrNRUBOGORtJhkbFCHoOA84233108 = QrNRUBOGORtJhkbFCHoOA87624185;     QrNRUBOGORtJhkbFCHoOA87624185 = QrNRUBOGORtJhkbFCHoOA26890410;     QrNRUBOGORtJhkbFCHoOA26890410 = QrNRUBOGORtJhkbFCHoOA36122706;     QrNRUBOGORtJhkbFCHoOA36122706 = QrNRUBOGORtJhkbFCHoOA36781086;     QrNRUBOGORtJhkbFCHoOA36781086 = QrNRUBOGORtJhkbFCHoOA71436389;     QrNRUBOGORtJhkbFCHoOA71436389 = QrNRUBOGORtJhkbFCHoOA38856995;     QrNRUBOGORtJhkbFCHoOA38856995 = QrNRUBOGORtJhkbFCHoOA98005516;     QrNRUBOGORtJhkbFCHoOA98005516 = QrNRUBOGORtJhkbFCHoOA94257764;     QrNRUBOGORtJhkbFCHoOA94257764 = QrNRUBOGORtJhkbFCHoOA45320156;     QrNRUBOGORtJhkbFCHoOA45320156 = QrNRUBOGORtJhkbFCHoOA10658927;     QrNRUBOGORtJhkbFCHoOA10658927 = QrNRUBOGORtJhkbFCHoOA78384528;     QrNRUBOGORtJhkbFCHoOA78384528 = QrNRUBOGORtJhkbFCHoOA61837477;     QrNRUBOGORtJhkbFCHoOA61837477 = QrNRUBOGORtJhkbFCHoOA71217930;     QrNRUBOGORtJhkbFCHoOA71217930 = QrNRUBOGORtJhkbFCHoOA16197847;     QrNRUBOGORtJhkbFCHoOA16197847 = QrNRUBOGORtJhkbFCHoOA73115827;     QrNRUBOGORtJhkbFCHoOA73115827 = QrNRUBOGORtJhkbFCHoOA57965289;     QrNRUBOGORtJhkbFCHoOA57965289 = QrNRUBOGORtJhkbFCHoOA54820478;     QrNRUBOGORtJhkbFCHoOA54820478 = QrNRUBOGORtJhkbFCHoOA42598245;     QrNRUBOGORtJhkbFCHoOA42598245 = QrNRUBOGORtJhkbFCHoOA33273171;     QrNRUBOGORtJhkbFCHoOA33273171 = QrNRUBOGORtJhkbFCHoOA87573640;     QrNRUBOGORtJhkbFCHoOA87573640 = QrNRUBOGORtJhkbFCHoOA7203609;     QrNRUBOGORtJhkbFCHoOA7203609 = QrNRUBOGORtJhkbFCHoOA42620550;     QrNRUBOGORtJhkbFCHoOA42620550 = QrNRUBOGORtJhkbFCHoOA2291408;     QrNRUBOGORtJhkbFCHoOA2291408 = QrNRUBOGORtJhkbFCHoOA57744582;     QrNRUBOGORtJhkbFCHoOA57744582 = QrNRUBOGORtJhkbFCHoOA23703660;     QrNRUBOGORtJhkbFCHoOA23703660 = QrNRUBOGORtJhkbFCHoOA6061368;     QrNRUBOGORtJhkbFCHoOA6061368 = QrNRUBOGORtJhkbFCHoOA79575317;     QrNRUBOGORtJhkbFCHoOA79575317 = QrNRUBOGORtJhkbFCHoOA78977842;     QrNRUBOGORtJhkbFCHoOA78977842 = QrNRUBOGORtJhkbFCHoOA19435311;     QrNRUBOGORtJhkbFCHoOA19435311 = QrNRUBOGORtJhkbFCHoOA66995799;     QrNRUBOGORtJhkbFCHoOA66995799 = QrNRUBOGORtJhkbFCHoOA60360837;     QrNRUBOGORtJhkbFCHoOA60360837 = QrNRUBOGORtJhkbFCHoOA11869685;     QrNRUBOGORtJhkbFCHoOA11869685 = QrNRUBOGORtJhkbFCHoOA21960928;     QrNRUBOGORtJhkbFCHoOA21960928 = QrNRUBOGORtJhkbFCHoOA49781338;     QrNRUBOGORtJhkbFCHoOA49781338 = QrNRUBOGORtJhkbFCHoOA47804339;     QrNRUBOGORtJhkbFCHoOA47804339 = QrNRUBOGORtJhkbFCHoOA63503446;     QrNRUBOGORtJhkbFCHoOA63503446 = QrNRUBOGORtJhkbFCHoOA60978645;     QrNRUBOGORtJhkbFCHoOA60978645 = QrNRUBOGORtJhkbFCHoOA42068575;     QrNRUBOGORtJhkbFCHoOA42068575 = QrNRUBOGORtJhkbFCHoOA74367454;     QrNRUBOGORtJhkbFCHoOA74367454 = QrNRUBOGORtJhkbFCHoOA8764864;     QrNRUBOGORtJhkbFCHoOA8764864 = QrNRUBOGORtJhkbFCHoOA70333256;     QrNRUBOGORtJhkbFCHoOA70333256 = QrNRUBOGORtJhkbFCHoOA78188365;     QrNRUBOGORtJhkbFCHoOA78188365 = QrNRUBOGORtJhkbFCHoOA84099671;     QrNRUBOGORtJhkbFCHoOA84099671 = QrNRUBOGORtJhkbFCHoOA68695607;     QrNRUBOGORtJhkbFCHoOA68695607 = QrNRUBOGORtJhkbFCHoOA75399471;     QrNRUBOGORtJhkbFCHoOA75399471 = QrNRUBOGORtJhkbFCHoOA84737547;     QrNRUBOGORtJhkbFCHoOA84737547 = QrNRUBOGORtJhkbFCHoOA84806962;     QrNRUBOGORtJhkbFCHoOA84806962 = QrNRUBOGORtJhkbFCHoOA30483614;     QrNRUBOGORtJhkbFCHoOA30483614 = QrNRUBOGORtJhkbFCHoOA40327504;     QrNRUBOGORtJhkbFCHoOA40327504 = QrNRUBOGORtJhkbFCHoOA27507561;     QrNRUBOGORtJhkbFCHoOA27507561 = QrNRUBOGORtJhkbFCHoOA67736595;     QrNRUBOGORtJhkbFCHoOA67736595 = QrNRUBOGORtJhkbFCHoOA32980930;     QrNRUBOGORtJhkbFCHoOA32980930 = QrNRUBOGORtJhkbFCHoOA39316602;     QrNRUBOGORtJhkbFCHoOA39316602 = QrNRUBOGORtJhkbFCHoOA45126898;     QrNRUBOGORtJhkbFCHoOA45126898 = QrNRUBOGORtJhkbFCHoOA77029499;     QrNRUBOGORtJhkbFCHoOA77029499 = QrNRUBOGORtJhkbFCHoOA45003636;     QrNRUBOGORtJhkbFCHoOA45003636 = QrNRUBOGORtJhkbFCHoOA24599002;     QrNRUBOGORtJhkbFCHoOA24599002 = QrNRUBOGORtJhkbFCHoOA78378124;     QrNRUBOGORtJhkbFCHoOA78378124 = QrNRUBOGORtJhkbFCHoOA13077426;     QrNRUBOGORtJhkbFCHoOA13077426 = QrNRUBOGORtJhkbFCHoOA65375021;     QrNRUBOGORtJhkbFCHoOA65375021 = QrNRUBOGORtJhkbFCHoOA59281678;     QrNRUBOGORtJhkbFCHoOA59281678 = QrNRUBOGORtJhkbFCHoOA19027674;     QrNRUBOGORtJhkbFCHoOA19027674 = QrNRUBOGORtJhkbFCHoOA74822453;     QrNRUBOGORtJhkbFCHoOA74822453 = QrNRUBOGORtJhkbFCHoOA78324357;     QrNRUBOGORtJhkbFCHoOA78324357 = QrNRUBOGORtJhkbFCHoOA55912413;}
// Junk Finished
