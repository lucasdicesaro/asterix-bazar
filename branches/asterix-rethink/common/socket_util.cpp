#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
//#include <errno.h>

#include "socket_util.h"
#include "types.h"
#include "tools.h"

/*
* Conecta con un servidor remoto a traves de socket TCP
*/
int SocketUtil::cliente_abrir_conexion_tcp (const char *serverIP, int serverPort)
{
	int sock;
	//const char *serverIP = (server_ip != NULL)? server_ip : "127.0.0.1";
	//int serverPort = PORT_TCP;			
	struct sockaddr_in serverAddr;	
	char *logBuffer = new char[BUFFER_SIZE];

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		perror("SocketUtil: socket() failed");
		exit(1);
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family      = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);
	serverAddr.sin_port        = htons(serverPort);

	if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		sprintf(logBuffer, "SocketUtil: No se pudo conectar a la ip [%s] con puerto %d", serverIP, serverPort);
		Tools::warn(logBuffer);	
		
		close (sock);
		//exit(1);
		sock = SOCK_ERRONEO; // Envio un sock erroneo		
	}

	return sock;
}

char *SocketUtil::recibir_mensaje(int socket)
{
	char *echoBuffer = new char[BUFFER_SIZE];		// Buffer for echo string	
	int bytesRcvd;						// Bytes read in single recv()
	
	memset(echoBuffer, 0, sizeof(echoBuffer));
	if ((bytesRcvd = recv(socket, echoBuffer, BUFFER_SIZE, 0)) < 0)
	{
		Tools::error("SocketUtil: recv() failed or connection closed prematurely");
		raise(SIGTERM);
	}
	
	return echoBuffer;
}


int SocketUtil::enviar_mensaje(int socket, std::string mensaje)
{	
	int size = mensaje.length()+1;
	char *logBuffer = new char[BUFFER_SIZE];
	int size_enviado;
	if ((size_enviado = send(socket, mensaje.c_str(), size, 0)) != size)
	{
		sprintf(logBuffer, "SocketUtil: enviar_mensaje:  send() sent a different number of bytes than expected. Se esperaba enviar %d y se envio %d. Socket: %d. Mensaje: [%s]", size, size_enviado, socket, mensaje.c_str());
		Tools::warn(logBuffer);		
		if (size_enviado == -1)
		{
			Tools::debug("Se cerro el socket mientras se tomaba da decision de enviarle un mensaje");
			socket = SOCK_ERRONEO;
		}
		else 
		{
			raise(SIGTERM);
		}		
	}
	return socket;
}


/*
 * Prepara un socket para un cliente UDP.
 * Asocia un socket a un cliente UDP
 * Devuelve el descriptor del socket que debe usar o -1 si ha habido algún error.
 */
int SocketUtil::cliente_abrir_conexion_udp (struct sockaddr_in *localAddr)
{
	int sock;
	int localPort  = PORT_UDP;

	if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		Tools::error("SocketUtil: socket() failed");
	}

	memset(localAddr, 0, sizeof(struct sockaddr_in));
	localAddr->sin_family = AF_INET;
	localAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr->sin_port = htons(localPort);
	
	return sock;
}




/*
* Se le pasa un socket de servidor y acepta en el una conexion de cliente.
* devuelve el descriptor del socket del cliente o -1 si hay problemas.
*/
int SocketUtil::servidor_aceptar_conexion_cliente(int sock)
{
	socklen_t clienteLen;
	struct sockaddr remoteaddr;
	int sockCliente;

	clienteLen = sizeof (remoteaddr);
	if ((sockCliente = accept (sock, &remoteaddr, &clienteLen)) < 0)
	{
		Tools::error("SocketUtil: accept() failed");
		//exit(1);
	}

	return sockCliente;
}

/*
* Abre un socket servidor de tipo AF_INET. Devuelve el descriptor
*	del socket o -1 si hay probleamas
*/
int SocketUtil::servidor_abrir_conexion_tcp(int listenerPort)
{
	//Tools::debug_label_value ("SocketUtil: servidor_abrir_conexion_tcp: listenerPort", listenerPort);
    struct sockaddr_in myaddr;     // server address
    //struct sockaddr_in remoteaddr; // client address
    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    socklen_t addrlen;
    int servSock;

    // get the servSock
    if ((servSock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        Tools::error("SocketUtil: socket");
        exit(1);
    }

    // lose the pesky "address already in use" error message
    if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		close (servSock);
		Tools::error("SocketUtil: setsockopt");
        exit(1);
    }

    // bind
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    //myaddr.sin_port = htons(PORT_TCP);
	myaddr.sin_port = htons(listenerPort);
    memset(&(myaddr.sin_zero), '\0', 8);
    if (bind(servSock, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
		close (servSock);
		Tools::error("SocketUtil: bind");
        exit(1);
    }

    // listen
    if (listen(servSock, 10) == -1) {
		close (servSock);
		Tools::error("SocketUtil: listen");
        exit(1);
    }

    return servSock;
}

/**
 * Abre un socket inet de udp.
 * Devuelve el descriptor del socket abierto o -1 si ha habido algún error.
 */
int SocketUtil::servidor_abrir_conexion_udp(struct sockaddr_in *localAddr)
{
    int sock;
    int localPort = PORT_UDP;

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        Tools::error("SocketUtil: socket() failed");
		exit(1);
    }

    memset(localAddr, 0, sizeof(struct sockaddr_in));
    localAddr->sin_family = AF_INET;
    localAddr->sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr->sin_port = htons(localPort);

    if (bind(sock, (struct sockaddr *) localAddr, sizeof(struct sockaddr_in)) < 0)
    {
		close (sock);
		Tools::error("SocketUtil: bind() failed");
		exit(1);
    }
	
    return sock;
}



/*
 * Crea un nuevo socket cliente.
 * Se le pasa el socket servidor y el array de clientes, con el número de
 * clientes ya conectados.
 */
void SocketUtil::servidor_nuevo_cliente(int sockServidor, int *clientes, int *nClientes)
{
	char logBuffer[BUFFER_SIZE];
	/* Acepta la conexión con el cliente, guardándola en el array */
	if ((clientes[*nClientes] = servidor_aceptar_conexion_cliente(sockServidor)) < 0)
	{
		Tools::error ("SocketUtil: No se puede abrir socket de cliente");
		exit (-1);
	}

	(*nClientes)++;

	/* Si se ha superado el maximo de clientes, se cierra la conexión,
	 * se deja todo como estaba y se vuelve. */
	if ((*nClientes) >= MAX_CONNECTED)
	{
		Tools::debug("SocketUtil: Se supero el tope de clientes. Se rechaza el pedido");
		close (clientes[(*nClientes) -1]);
		(*nClientes)--;
		return;
	}
		
	/* Envía su número de cliente al cliente */
	//escribir_socket_tcp(clientes[(*nClientes)-1], (char *)nClientes);

	/* Escribe en pantalla que ha aceptado al cliente y vuelve */
	sprintf(logBuffer, "SocketUtil: Se acepto al cliente %d", *nClientes);
	Tools::debug(logBuffer);			
	return;
}



/*
 * Función
 que devuelve el valor máximo en la tabla.
 * Supone que los valores válidos de la tabla son positivos y mayores que 0.
 * Devuelve 0 si n es 0 o la tabla es NULL */
int SocketUtil::obtener_maximo_fd(int *tabla, int n)
{
	int i;
	int max;

	if ((tabla == NULL) || (n<1))
		return 0;
		
	max = tabla[0];
	for (i=0; i<n; i++)
		if (tabla[i] > max)
			max = tabla[i];

	return max;
}

/*
 * Busca en array todas las posiciones con -1 y las elimina, copiando encima
 * las posiciones siguientes.
 * Ejemplo, si la entrada es (3, -1, 2, -1, 4) con *n=5
 * a la salida tendremos (3, 2, 4) con *n=3
 */
void SocketUtil::limpiar_tabla_clientes(int *tabla, int *n)
{
	int i,j;

	if ((tabla == NULL) || ((*n) == 0))
		return;

	j=0;
	for (i=0; i<(*n); i++)
	{
		if (tabla[i] != -1)
		{
			tabla[j] = tabla[i];
			j++;
		}
	}
	*n = j;
}



