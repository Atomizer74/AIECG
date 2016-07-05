#include "Error.h"

#include <iostream>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

Error Error::_lastError;

Error::Error(int check, Severity level, std::string message) : _check(check), _level(level), _message(message)
{
}

Error Error::HandleDebug()
{
	//_stackTrace = InfoStack::GetStackTrace();
	std::cerr << "(DEBUG) " << _message << std::endl;
	return *this;
}

Error Error::HandleWarning()
{
	//_stackTrace = InfoStack::GetStackTrace();
	std::cerr << "(WARNING) " << _message << std::endl;
	return *this;
}

Error Error::HandleError()
{
	//_stackTrace = InfoStack::GetStackTrace();
	std::cerr << "(ERROR) " << _message << std::endl;
	return *this;
}

Error Error::HandleFatal()
{
	//_stackTrace = InfoStack::GetStackTrace();
	std::cerr << "(FATAL ERROR) " << _message << std::endl;
#ifdef WIN32
	MessageBox(NULL, _message.c_str(), "FATAL ERROR", MB_ICONEXCLAMATION | MB_OK);
#endif
	return *this;
}