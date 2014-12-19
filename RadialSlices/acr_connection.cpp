#include "acr_connection.h"
#include <sstream>

acr_connection::acr_connection()
{

}

acr_connection::~acr_connection()
{
	if (this->m_channel != nullptr)
		this->m_channel->Disconnect();
	CoUninitialize();
}

long acr_connection::getLong(long index)
{
	long lBound = 0;
	CComVariant vItem;

	auto param_address = this->m_channel->GetParmAddr(index);
	auto ret = this->m_channel->GetACRMemory(0, param_address, 1);

	SafeArrayGetElement(ret.parray, &lBound, &vItem);

	return vItem.lVal;
}





acr_connection::ptr make_connection(std::string ip)
{
	try
	{
		COMInit com_resource;

		CComPtr<IUnknown> unknown;

		auto res = unknown.CoCreateInstance(L"ComACRServer.Channel.1");

		if (unknown)
		{
			auto ret = std::make_unique<acr_connection>();
			ret->m_channel = unknown;
			auto& channel = ret->m_channel;
			if (channel == nullptr)
			{
				return acr_connection::ptr();
			}

			long bps[3] = { 9600, 19200, 38400 };
			long transport = 3;
			long port_number = 1;
			long port_bit_speed = 2;

			// Connect to Control Interface 
			channel->put_nPort(port_number);
			channel->put_nBPS(bps[port_bit_speed]);
			channel->put_bstrIP(_bstr_t(ip.c_str()));

			channel->Connect(transport, 0);

			VARIANT_BOOL pVal;

			channel->get_isOffline(&pVal);

			if (pVal == VARIANT_TRUE)
			{
				return acr_connection::ptr();
			}

			com_resource.success = true;

			return ret;
		}
		else
		{
			return acr_connection::ptr();
		}
	}
	catch (...)
	{
		return acr_connection::ptr();
	}
}