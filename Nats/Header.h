
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32")

#include <regex>

#include <AddinObject.h>

#define MAXLEN 8192
#define CRLF "\r\n"
#define SLEEP 30 * 1000
#define DEF_HOST L"localhost"
#define DEF_PORT 4222
#define CONNECT_STRING "CONNECT {\"name\": \"Ones\", \"lang\": \"C++\", \"version\": \"0.1.0\", \"pedantic\": false, \"verbose\": false, \"protocol\": 1}"

class OnesNats : public Addin1C::AddinObject<OnesNats> {
	std::wstring host;
	unsigned short port;

	SOCKET nc;

	bool connected;

	std::map<std::string, std::string> subjects;

public:
	OnesNats() : host(DEF_HOST), port(DEF_PORT), connected(false) {}

	static std::wstring getName() {
		return L"OnesNats";
	}

	static void getMetadata(Metadata& md) {
		md.addProperty(L"host", L"Хост", &OnesNats::setHost, &OnesNats::getHost);
		md.addProperty(L"port", L"Порт", &OnesNats::setPort, &OnesNats::getPort);
		md.addProperty(L"connected", L"Подключен", &OnesNats::setConnected, &OnesNats::getConnected);

		md.addFunction(L"connect", L"Подключить", 0, &OnesNats::connect);
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

private:
	static DWORD WINAPI thread_starter(void *pSelf) {
		while (true) {
			((OnesNats*)pSelf)->nats_connect();
			Sleep(SLEEP);
		}
	}
	int nats_connect();
	int nats_send(const char *text);
	int nats_recv(const char *text);
	int nats_subscribe(const char *subject);
	int nats_publish(const char *subject, const char *data);
};
