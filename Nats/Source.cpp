
#include "Header.h"

void addinInitialization(Addin1C::AddinManager& manager) {
	manager.registerObject<OnesNats>();
}

void gen_random(char *s, const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}

int OnesNats::nats_send(const char *text) {
	if (!nc) return -1;

	::send(nc, text, strlen(text), NULL);
	::send(nc, CRLF, 2, NULL);
	return 0;
}

int OnesNats::nats_recv(const char *text) {
	std::string s = text;
	std::smatch m;
	std::regex r;

	r.assign("INFO (.*)\r\n");
	if (std::regex_search(s, m, r)) {
		nats_send(CONNECT_STRING);
		callback(std::wstring(L"OnesNats"), (std::wstring) L"Connected to server:" + std::to_wstring((long long)port));
		return 0;
	}

	r.assign("PING\r\n");
	if (std::regex_search(s, m, r)) {
		nats_send("PONG");
		return 0;
	}

	r.assign("MSG (\\S+) (\\S+) (\\d+)\r\n(.*)");
	if (std::regex_search(s, m, r)) {
		std::string subject = m[1];
		std::string sid = m[2];
		size_t size = std::stoul(m[3]);
		std::string data = m[4];

		callback(subject, data);

		return 0;
	}
	
	return 1;
}

int OnesNats::nats_connect() {
	if (nc) ::closesocket(nc);

	size_t size, c_size;
	size = wcslen(this->host.c_str()) * 2 + 2;
	char * host = new char[size];
	wcstombs_s(&c_size, host, size, this->host.c_str(), size);

	WSADATA wsadata;
	if (WSAStartup(0x0202, &wsadata)) return -1;
	if (wsadata.wVersion != 0x0202) {
		WSACleanup();
		return wsadata.wVersion;
	}

	SOCKADDR_IN target;
	target.sin_family = AF_INET;
	target.sin_port = htons(port);
	target.sin_addr.s_addr = inet_addr(host);
	nc = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (nc == INVALID_SOCKET) return nc;

	if (::connect(nc, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR) {
		callback(std::wstring(L"OnesNats"),
			(std::wstring) L"Connection to server:" + std::to_wstring((long long)port) + L" failed! ");
		return -1;
	}

	char buf[MAXLEN];
	buf[0] = 0;
	while (1) {
		size = recv(nc, buf, MAXLEN, 0);
		if (!nc || size <= 0 || !strlen(buf)) break;
		buf[size] = 0;
		nats_recv(buf);
	}

	::closesocket(nc);
	return 0;
}


/*
 * PUBLIC
 */
Addin1C::Variant OnesNats::connect(Addin1C::VariantParameters& p) {
	if (host.length() == 0) return std::wstring(L"Server HOST is not set.");
	if (port == 0)			return std::wstring(L"Server PORT is not set.");

	DWORD tid = 0;
	HANDLE t = CreateThread(NULL, 0, thread_starter, (void*)this, 0, &tid);

	if (t == NULL)
		return (std::wstring) L"Connection thread is NOT started to " + host + L":" + std::to_wstring((long long)port);
	else
		return (std::wstring) L"Connection thread is started to " + host + L":" + std::to_wstring((long long)port);
}

Addin1C::Variant OnesNats::subscribe(Addin1C::VariantParameters& p) {
	std::wstring subject = p[0];

	size_t size, c_size;
	size = wcslen(subject.c_str()) * 2 + 2;
	char *c_subject = new char[size];
	wcstombs_s(&c_size, c_subject, size, subject.c_str(), size);

	char sid[17];
	gen_random(sid, 16);

	::send(nc, "SUB ", 4, NULL);
	::send(nc, c_subject, strlen(c_subject), NULL);
	::send(nc, " ", 1, NULL);
	::send(nc, sid, 16, NULL);
	::send(nc, CRLF, 2, NULL);

	return true;
}

Addin1C::Variant OnesNats::publish(Addin1C::VariantParameters& p) {
	return true;
}

Addin1C::Variant OnesNats::request(Addin1C::VariantParameters& p) {
	return true;
}
