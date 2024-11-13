#include "ft_irc.hpp"

// PORT and PASSWORD ar just placeholders to be replaced with actual variables

int initialize_listener(PORT)
{
	int listener;
	struct protoent* protocol = getprotobyname("TCP");

	if (!protocol)
		//error;

	if (-1 == socket(AF_INET, SOCK_STREAM, protocol->p_proto);//(IPv4, necesario para TCP, TCP en si)
		//error;
	if (-1 == bind(listener, /**/, /**/))
		//error;
	//mas posible configuracion;

	if (-1 == listen(listener, /*whatever number*/))
		//error;
	return (listener);	
}
int cycle(struct pollfd* monitored)
{
	int pollret;

	while (1)
	{
		pollret = poll(monitored, /**/, /**/);	
		/*
			identify which fds we want to do something with
			DO IT
		*/
	}
	return (0);//in case we want to return errors
}

int main (int argc, char** argv)
{
	int listner;
	struct pollfd* monitored;

	if (argc !=3)
		//error
	listener = initialize_listener(PORT);

	monitored = malloc(2 * sizeof(struct pollfd);
	if (!monitored)
		//error
		/* este que seria por no tener memoria al intentar crearlo siquiera, saldriamos
			pero cuando sea por intentar aceptar una nueva conexion, entonces
			pasaremos de ella y continuaremos funcionando
		*/
	cycle(monitored);
	
	return (0);
}
