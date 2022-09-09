// networking.cpp : Defines the entry point for the application.
//

#if _WIN32
#define _WIN32_WINT 0x0A00
#elif 
#endif // _WIN32

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include <iostream>

#if _WIN32
__forceinline void TRY_EC(const asio::error_code ec) {
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		__debugbreak();
	}
}
#elif 
inline void TRY_EC(const asio::error_code ec) {

}
#endif // _WIN32

#include <string>
#include <vector>

#define Str std::string
#define vec_s8 std::vector<char>

#define AssertRaw(exp) if (!(exp)) { *(int*)0=0; }
#define Kilobyte(x) ((x)*1024)

#define global_variable static

vec_s8 vBuffer(Kilobyte(20));

static void GrabSomeData(asio::ip::tcp::socket& socket) {
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t length) 
		{
			if (!ec)
			{
				std::cout << "\n\nRead" << length << " bytes\n\n";

				for (std::size_t i = 0; i < length; i++)
					std::cout << vBuffer[i];

				GrabSomeData(socket);
			}
		}
	);
}

int main(int argc, char** argv)
{
	
	asio::error_code ec = {};

	asio::io_context context = {}; 

	asio::io_context::work idleWork(context);

	std::thread thrContext{ std::thread([&]() { context.run(); }) };

	// Get the address of somewhere we wsh to connect to
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);
	
	asio::ip::tcp::socket socket(context);

	socket.connect(endpoint, ec);

	TRY_EC(ec);

	if (socket.is_open())
	{
		GrabSomeData(socket);

		Str sRequest{
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n"
		};

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		TRY_EC(ec);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);

		context.stop();
		if (thrContext.joinable())
		{
			thrContext.join();
		}

	}

	return(0);
}
