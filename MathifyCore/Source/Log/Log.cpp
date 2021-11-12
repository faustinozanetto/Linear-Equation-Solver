﻿#include "Log.h"
#include <stack>
#include <iostream>
#include <stdarg.h>
#include "TerminalColor.h"

namespace Mathify::Log
{
    static Level logLevel = Level::INFO;

	static std::stack<std::string> subjects;
	static std::string lastSubject = "";
	static bool newSubject = false;

	static std::string delimiter = "\n";

	static FILE* logFile = nullptr;

	static void logTogether(const char* message, va_list args) {
		vprintf(message, args);
		if(logFile) {
			vfprintf(logFile, message, args);
			fflush(logFile);
		}
	}

// the va_list must be recreated for both print and fprint
#define va_logf(format, message) va_list args; va_start(args, format); vprintf(message.c_str(), args); va_end(args); if(logFile) {va_start(args, format); vfprintf(logFile, message.c_str(), args); va_end(args); fflush(logFile);}

	void init(const char* logFileName) {
#ifdef _MSC_VER
		fopen_s(&logFile, logFileName, "w");
#else
		logFile = fopen(logFileName, "w");
#endif
	}
	void stop() {
		fclose(logFile);
	}

	std::string topSubject() {
		if (subjects.empty())
			return "";

		return subjects.top();
	}

	Subject::Subject(const std::string& title) {
		if (subjects.empty()) {
			subjects.push(title);
			newSubject = title != lastSubject;
		} else {
			if (title == subjects.top()) {
				newSubject = false;
			} else if (title == lastSubject) {
				newSubject = false;
			} else {
				newSubject = true;
			}
			subjects.push(title);
		}
	}

	Subject::~Subject() {
		lastSubject = subjects.top();
		subjects.pop();
		newSubject = lastSubject != topSubject();
	}

	bool emptySubject() {
		return topSubject().empty();
	}

	void setLogLevel(Level level) {
		logLevel = level;
	}

	Level getLogLevel() {
		return logLevel;
	}

	static void printSubject(std::string subject) {
		setColor(Mathify::Log::Color::SUBJECT);
		printf(subject.c_str());
		fprintf(logFile, subject.c_str());
		fflush(logFile);
		setColor(Mathify::Log::Color::NORMAL);
		newSubject = false;
	}

	static void addSubjectIfNeeded() {
		if(newSubject && !topSubject().empty()) printSubject("\n[" + topSubject() + "]:\n");
		if(!emptySubject()) printSubject("|\t");
	}

	void setDelimiter(std::string delimiter) {
		Log::delimiter = delimiter;
	}

	void debug(std::string format, ...) {
		if (logLevel != Level::NONE) {
			addSubjectIfNeeded();
			std::string message = "[DEBUG]: " + format + delimiter;
			setColor(Mathify::Log::Color::DEBUG);
			va_logf(format, message);
			setColor(Mathify::Log::Color::NORMAL);
		}
	}

	void info(std::string format, ...) {
		if (logLevel <= Level::INFO) {
			addSubjectIfNeeded();
			std::string message = "[INFO]: " + format + delimiter;
			setColor(Mathify::Log::Color::INFO);
			va_logf(format, message);
			setColor(Mathify::Log::Color::NORMAL);
		}
	}

	void warn(std::string format, ...) {
		if (logLevel <= Level::WARNING) {
			addSubjectIfNeeded();
			std::string message = "[WARN]: " + format + delimiter;
			setColor(Mathify::Log::Color::WARNING);
			va_logf(format, message);
			setColor(Mathify::Log::Color::NORMAL);
		}
	}

	void error(std::string format, ...) {
		if (logLevel <= Level::ERROR) {
			addSubjectIfNeeded();
			std::string message = "[ERROR]: " + format + delimiter;
			setColor(Mathify::Log::Color::ERROR);
			va_logf(format, message);
			setColor(Mathify::Log::Color::NORMAL);
		}
	}

	void fatal(std::string format, ...) {
		if (logLevel <= Level::FATAL) {
			addSubjectIfNeeded();
			std::string message = "[FATAL]: " + format + delimiter;
			setColor(Mathify::Log::Color::FATAL);
			va_logf(format, message);
			setColor(Mathify::Log::Color::NORMAL);
		}
	}

	void print(std::string format,  ...) {
		setColor(Mathify::Log::Color::NORMAL);
		va_logf(format, format);
	}

	void print(TerminalColorPair color, std::string format, ...) {
		setColor(color);
		va_logf(format, format);
	}
}
