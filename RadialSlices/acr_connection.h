#pragma once
#include <atlbase.h>
#include <atlcom.h>
#include <string>
#include <memory>
#import "ComACRServer.tlb" no_namespace named_guids

struct COMInit
{
	bool success;
	COMInit()
		: success(false)
	{
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	}
	~COMInit()
	{
		if (!this->success)
		{
			CoUninitialize();
		}
	}
};

class acr_connection
{
public:
	acr_connection();
	~acr_connection();
	using ptr = std::unique_ptr<acr_connection>;
	friend acr_connection::ptr make_connection(std::string ip);

	long getLong(long param_number);
private:
	CComQIPtr<IChannel> m_channel;
};
