#pragma once

#include <string>

#ifdef ERROR
#undef ERROR
#endif

enum Severity
{
	DEBUG,
	WARNING,
	ERROR,
	FATAL
};

class Error
{
	int _check;
	std::string _message;
	Severity _level;
	std::string _stackTrace;

	static Error _lastError;

	// Default Constructor
	Error() : _check(0), _message("NO ERROR"), _level(DEBUG) {}

public:
	Error(int check, Severity level, std::string message);

	static bool Check(int check, Severity level, std::string message)
	{
		if (check == 0)
		{
			switch (level)
			{
			case DEBUG:
				{
				#ifdef _DEBUG
					_lastError = Error(check, level, message).HandleDebug();
					return true;
				#else
					return false;
				#endif
				}
				break;
			case WARNING:
				{
					_lastError = Error(check, level, message).HandleWarning();
					return true;
				}
				break;
			case ERROR:
				{
					_lastError = Error(check, level, message).HandleError();
					return true;
				}
				break;
			case FATAL:
				{
					_lastError = Error(check, level, message).HandleFatal();
					throw _lastError;
				}
				break;
			}
		}

		return false;
	}

	static Error GetLastError()
	{
		return _lastError;
	}

	std::string What()
	{
		return _message;
	}

	std::string StackTrace()
	{
		return _stackTrace;
	}

private:
	Error HandleDebug();
	Error HandleWarning();
	Error HandleError();
	Error HandleFatal();
};