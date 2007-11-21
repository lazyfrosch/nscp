/**************************************************************************
*   Copyright (C) 2004-2007 by Michael Medin <michael@medin.name>         *
*                                                                         *
*   This code is part of NSClient++ - http://trac.nakednuns.org/nscp      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#pragma once

typedef BOOL (WINAPI *PFGlobalMemoryStatusEx)(LPMEMORYSTATUSEX lpBuffer);
typedef BOOL (WINAPI *PFGlobalMemoryStatus)(LPMEMORYSTATUS lpBuffer);

namespace CheckMemMethod
{
	const int None		= 0x0;
	const int Extended	= 0x1;
	const int Normal	= 0x2;
} 

class CheckMemoryException {
private:
	std::string name_;
	std::string msg_;
	unsigned int error_;
public:
	CheckMemoryException(std::string name,std::string msg,unsigned int error) : name_(name), error_(error), msg_(msg) {};

	std::string getError() {
		return "Service: " + name_ + " caused: " + msg_;
	}
	unsigned int getErrorCode() { return error_; }
};

class CheckMemory {
private:
	typedef struct {
		unsigned long long total;
		unsigned long long avail;
	} memType;
public:
	typedef struct {
		unsigned long dwMemoryLoad;  
		memType phys;
		memType pageFile;
		memType virtualMem;
		double availExtendedVirtual;
	} memData;

public:
	CheckMemory() : FEGlobalMemoryStatusEx(NULL), FEGlobalMemoryStatus(NULL), method_(CheckMemMethod::None)
	{
		hKernel32 = ::LoadLibrary(_TEXT("Kernel32"));
		if (hKernel32)  
		{
			FEGlobalMemoryStatusEx = (PFGlobalMemoryStatusEx)::GetProcAddress(hKernel32, _TEXT("GlobalMemoryStatusEx"));
			FEGlobalMemoryStatus = (PFGlobalMemoryStatus)::GetProcAddress(hKernel32, _TEXT("GlobalMemoryStatus"));
		}
		int method = getAvailableMethods();
		if ((method&CheckMemMethod::Extended)==CheckMemMethod::Extended)
			method_ = CheckMemMethod::Extended;
		else
			method_ = method;
	}
	virtual ~CheckMemory() {
		if (hKernel32) FreeLibrary(hKernel32);
	}

	int getAvailableMethods() {
		int ret = CheckMemMethod::None;
		if (FEGlobalMemoryStatusEx)
			ret |= CheckMemMethod::Extended;
		if (FEGlobalMemoryStatus)
			ret |= CheckMemMethod::Normal;
		return ret;
	}
	int getSuggestedMethod();
	int setMethod(int method) {
		if ((getAvailableMethods()&method) != 0)
			method_ = method;
		return method_;
	}

	memData getMemoryStatus() {
		memData ret;
		if (method_ == CheckMemMethod::Extended) {
			MEMORYSTATUSEX buffer;
			buffer.dwLength = sizeof(buffer);
			if (!FEGlobalMemoryStatusEx(&buffer))
				throw CheckMemoryException("CheckMemory", "GlobalMemoryStatusEx failed", GetLastError());
			ret.phys.total = buffer.ullTotalPhys;
			ret.phys.avail = buffer.ullAvailPhys;
			ret.virtualMem.total = buffer.ullTotalVirtual;
			ret.virtualMem.avail = buffer.ullAvailVirtual;
			ret.pageFile.total = buffer.ullTotalPageFile;
			ret.pageFile.avail = buffer.ullAvailPageFile;
		} else if (method_ == CheckMemMethod::Normal) {
			MEMORYSTATUS buffer;
			buffer.dwLength = sizeof(buffer);
			if (!FEGlobalMemoryStatus(&buffer))
				throw CheckMemoryException("CheckMemory", "GlobalMemoryStatus failed", GetLastError());
			ret.phys.total = buffer.dwTotalPhys;
			ret.phys.avail = buffer.dwAvailPhys;
			ret.virtualMem.total = buffer.dwTotalVirtual;
			ret.virtualMem.avail = buffer.dwAvailVirtual;
			ret.pageFile.total = buffer.dwTotalPageFile;
			ret.pageFile.avail = buffer.dwAvailPageFile;
		}
		return ret;
	}
private:
	int method_;
	HMODULE hKernel32;
	PFGlobalMemoryStatusEx	FEGlobalMemoryStatusEx;
	PFGlobalMemoryStatus	FEGlobalMemoryStatus;
};