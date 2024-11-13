#include "ft_irc.hpp"

// PORT and PASSWORD ar just placeholders to be replaced with actual variables

int initialize_listener(PORT)
{
	int listener;
	struct protoent* protocol = getprotobyname(/*"TCP"*/);

	if (!protocol)
		return (-1);

	if (-1 == socket(AF_INET, SOCK_STREAM, protocol->p_proto);//(IPv4, necesario para TCP, TCP en si)
		return (-1);
	if (-1 == bind(listener, /**/, /**/))
		return (-1);
	//mas posible configuracion;

	if (-1 == listen(listener, /*whatever number*/))
		return (-1);
	return (listener);	
}
int cycle(struct pollfd* monitored, PASSWORD)
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
	if (-1 == listener)
	{
		//possibly use my pterror from minishell _glopez-m
		return (1);//good that no malloc was done yet
	}
	monitored = malloc(2 * sizeof(struct pollfd);
	if (!monitored)
		//error
		/* este que seria por no tener memoria al intentar crearlo siquiera, saldriamos
			pero cuando sea por intentar aceptar una nueva conexion, entonces
			pasaremos de ella y continuaremos funcionando
		*/
	cycle(monitored, PASSWORD);
	
	return (0);
}
