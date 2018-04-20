
#include <AddinObject.h>

class OnesNats : public Addin1C::AddinObject<OnesNats> {
	std::wstring host;
	unsigned short port;

	bool connected;

public:
	OnesNats() : host(L"localhost"), port(4222), connected(false) {}

	static void getMetadata(Metadata& md) {
		md.addProperty(L"host", L"Хост", &OnesNats::setHost, &OnesNats::getHost);
		md.addProperty(L"port", L"Порт", &OnesNats::setPort, &OnesNats::getPort);
		md.addProperty(L"connected", L"Подключен", &OnesNats::setConnected, &OnesNats::getConnected);

		md.addFunction(L"connect", L"Подключить", 2, &OnesNats::connect);
		md.addFunction(L"subscribe", L"Слушать", 1, &OnesNats::subscribe);
		md.addFunction(L"publish", L"Отправить", 2, &OnesNats::publish);
		md.addFunction(L"request", L"Запросить", 2, &OnesNats::request);
	}

	void setHost(Addin1C::Variant value) { host = value; }
	void setPort(Addin1C::Variant value) {
		if ((const long)value < 0) return;

		port = static_cast<unsigned short>((long)value);
	}
	void setConnected(Addin1C::Variant value) { return; }

	Addin1C::Variant getHost() { return host; }
	Addin1C::Variant getPort() { return (const long)port; }
	Addin1C::Variant getConnected() { return connected; }

	Addin1C::Variant connect(Addin1C::VariantParameters& p);
	Addin1C::Variant subscribe(Addin1C::VariantParameters& p);
	Addin1C::Variant publish(Addin1C::VariantParameters& p);
	Addin1C::Variant request(Addin1C::VariantParameters& p);
};
