#pragma once

#include "IAppSystem.hpp"
#include "IConVar.hpp"
#include "..\misc\Convar.hpp"
#include <fstream>
#include <string>

class ConCommandBase;
class ConCommand;
class ConVar;

class IConsoleDisplayFunc
{
public:
    virtual void ColorPrint(const uint8_t* clr, const char *pMessage) = 0;
    virtual void Print(const char *pMessage) = 0;
    virtual void DPrint(const char *pMessage) = 0;
};

class ICvar : public IAppSystem
{
public:
    virtual int						   AllocateDLLIdentifier() = 0; 
    virtual void                       RegisterConCommand(ConCommandBase *pCommandBase) = 0;
    virtual void                       UnregisterConCommand(ConCommandBase *pCommandBase) = 0;
    virtual void                       UnregisterConCommands(int id) = 0;
    virtual const char*                GetCommandLineValue(const char* pVariableName) = 0;
    virtual ConCommandBase*            FindCommandBase(const char* name) = 0;
    virtual const ConCommandBase*      FindCommandBase(const char* name) const = 0;
    virtual ConVar*                    FindVar(const char* var_name) = 0;
    virtual const ConVar*              FindVar(const char* var_name) const = 0;
    virtual ConCommand*                FindCommand(const char* name) = 0;
    virtual const ConCommand*          FindCommand(const char* name) const = 0;
    virtual void                       InstallGlobalChangeCallback(FnChangeCallback_t callback) = 0;
    virtual void                       RemoveGlobalChangeCallback(FnChangeCallback_t callback) = 0;
    virtual void                       CallGlobalChangeCallbacks(ConVar* var, const char* pOldString, float flOldValue) = 0;
    virtual void                       InstallConsoleDisplayFunc(IConsoleDisplayFunc* pDisplayFunc) = 0;
    virtual void                       RemoveConsoleDisplayFunc(IConsoleDisplayFunc* pDisplayFunc) = 0;
    virtual void                       ConsoleColorPrintf(const Color& clr, const char* pFormat, ...) const = 0;
    virtual void                       ConsolePrintf(const char* pFormat, ...) const = 0;
    virtual void                       ConsoleDPrintf(const char* pFormat, ...) const = 0;
    virtual void                       RevertFlaggedConVars(int nFlag) = 0;
protected:	class ICVarIteratorInternal;
public:
	/// Iteration over all cvars. 
	/// (THIS IS A SLOW OPERATION AND YOU SHOULD AVOID IT.)
	/// usage: 
	/// { ICVar::Iterator iter(g_pCVar); 
	///   for ( iter.SetFirst() ; iter.IsValid() ; iter.Next() )
	///   {  
	///       ConCommandBase *cmd = iter.Get();
	///   } 
	/// }
	/// The Iterator class actually wraps the internal factory methods
	/// so you don't need to worry about new/delete -- scope takes care
	//  of it.
	/// We need an iterator like this because we can't simply return a 
	/// pointer to the internal data type that contains the cvars -- 
	/// it's a custom, protected class with unusual semantics and is
	/// prone to change.
	class Iterator
	{
	public:
		inline Iterator(ICvar* icvar);
		inline ~Iterator(void);
		inline void		SetFirst(void);
		inline void		Next(void);
		inline bool		IsValid(void);
		inline ConCommandBase* Get(void);
	private:
		ICVarIteratorInternal* m_pIter;
	};

protected:
	// internals for  ICVarIterator
	class ICVarIteratorInternal
	{
	public:
		// This should be a virtual destructor to avoid undefined behavior and many warnings, but doing so
		// causes plugins to break. It's not worth the trouble at this point.
		//virtual ~ICVarIteratorInternal() {}
		virtual void		SetFirst(void) = 0;
		virtual void		Next(void) = 0;
		virtual	bool		IsValid(void) = 0;
		virtual ConCommandBase* Get(void) = 0;
	};

	virtual ICVarIteratorInternal* FactoryInternalIterator(void) = 0;
	friend class Iterator;
};

inline ICvar::Iterator::Iterator(ICvar* icvar)
{
	m_pIter = icvar->FactoryInternalIterator();
}

inline ICvar::Iterator::~Iterator(void)
{
	delete m_pIter;
}

inline void ICvar::Iterator::SetFirst(void)
{
	m_pIter->SetFirst();
}

inline void ICvar::Iterator::Next(void)
{
	m_pIter->Next();
}

inline bool ICvar::Iterator::IsValid(void)
{
	return m_pIter->IsValid();
}

inline ConCommandBase* ICvar::Iterator::Get(void)
{
	return m_pIter->Get();
}
