
inicio: 
	gcc -o writer writer.c -lpthread
	gcc -o inicio inicializador.c 
	gcc -o reader_e reader_egoista.c -lpthread
	gcc -o espia espia.c 
	gcc -o fin finalizador.c 
