#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define SERVER_PORT 1234
#define QUEUE_SIZE 5

//struktura zawierająca dane, które zostaną przekazane do wątku
struct thread_data_t
{
	int epoll_desc;
};

//funkcja opisującą zachowanie wątku - musi przyjmować argument typu (void *) i zwracać (void *)
void *ThreadBehavior(void *t_data)
{
    pthread_detach(pthread_self());
    struct thread_data_t *th_data = (struct thread_data_t*)t_data;
	struct epoll_event event;
	
	char buf[201];
	int size;
	
    while(1) {
        epoll_wait(th_data->epoll_desc, &event, 1, -1); //1 = maksymalnie jedno zdarzenie, -1 = bez timeout
        if ( event.events & EPOLLIN ) {
			size = read(event.data.fd, buf, sizeof(buf) - 1);
			buf[size] = '\0';
			
			if(size == 0) {
				epoll_ctl(th_data->epoll_desc, EPOLL_CTL_DEL, event.data.fd, NULL);
				
				continue;
			}
			
			printf(buf);
        }
    }

    pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
   int server_socket_descriptor;
   int connection_socket_descriptor;
   int bind_result;
   int listen_result;
   char reuse_addr_val = 1;
   struct sockaddr_in server_address;

   //inicjalizacja gniazda serwera
   
   memset(&server_address, 0, sizeof(struct sockaddr));
   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = htonl(INADDR_ANY);
   server_address.sin_port = htons(SERVER_PORT);

   server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
   if (server_socket_descriptor < 0)
   {
       fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda..\n", argv[0]);
       exit(1);
   }
   setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

   bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
   if (bind_result < 0)
   {
       fprintf(stderr, "%s: Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n", argv[0]);
       exit(1);
   }

   listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
   if (listen_result < 0) {
       fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", argv[0]);
       exit(1);
   }

   int epoll = epoll_create1(0);
   struct epoll_event event;
   memset(&event, 0, sizeof(event));
   event.events = EPOLLIN;
   
   if (epoll < 0) {
	   printf("Błąd przy próbie utworzenie epoll");
	   exit(-1);
   }

   struct thread_data_t thread_data;
   thread_data.epoll_desc = epoll;

   int create_result = 0;
   pthread_t thread1;
   create_result = pthread_create(&thread1, NULL, ThreadBehavior, (void *) &thread_data);
   if (create_result){
      printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
      exit(-1);
   }

   while(1)
   {   
       connection_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
       if (connection_socket_descriptor < 0)
       {
           fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
           exit(1);
       }

		event.data.fd = connection_socket_descriptor;
		epoll_ctl(epoll, EPOLL_CTL_ADD, connection_socket_descriptor, &event);
   }

   close(server_socket_descriptor);
   return(0);
}
