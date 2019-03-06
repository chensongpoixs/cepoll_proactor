#include <iostream>

void StopLoginFrame(int i)
{
	chen::g_server.set_stoping());
}

void RegisterSignal()
{
	signal(SIGINT, StopLoginFrame);
	signal(SIGTERM, StopLoginFrame);
}
int main(int argc, char *argv[])
{
	RegisterSignal();
	int port = 0;
	if(argc == 2)
	{
		port = argv[2];
	}
	bool init = chen::g_server.init(port);
	if(init)
	{
		init = chen::g_server.Loop();
	}	

	chen::g_server.destroy();

	if(!init)
	{
		return 1;
	}
	return 0;
	
	return EXIT_SUCCESS;
}