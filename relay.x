/* Archivo: relay.x 						*/
/* Descripción de la Interface (en IDL) del Servicio  RPC 	*/
/* RELAY RPC QUE NO CONSIDERA CADUCIDAD DE MENSAJES	  	*/
/* SINTAXIS:							*/
/*	void 		manda( 	cadena_ip , 	cadena_mensaje )*/
/*	cadena_mensaje	retira(	cadena_ip )			*/

typedef string cadena<>;
/* arreglo de cadena de caracteres de longitud arbitraria */

program RELAY{
	version VRELAY{
		void manda(cadena, cadena)=1;
		cadena retira(cadena)=2;
	}=1;
}=0x20000009;
/* Archivo: relay.x 						*/
