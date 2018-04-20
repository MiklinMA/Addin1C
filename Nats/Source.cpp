
#include "Header.h"

void addinInitialization(Addin1C::AddinManager& manager) {
	manager.registerObject<OnesNats>();
}

Addin1C::Variant OnesNats::connect(Addin1C::VariantParameters& p) {
	

	if (host.length() == 0) return std::wstring(L"Server HOST is not set.");
	if (port == 0)			return std::wstring(L"Server PORT is not set.");

	return (std::wstring) L"Connection thread started to " + host + L":" + std::to_wstring((long long)port);
}