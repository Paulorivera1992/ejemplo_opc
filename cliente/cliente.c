
#include "open62541.h"
#include <stdlib.h>

UA_Boolean running = true;

static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT, "Received Ctrl-C");
    running = 0;
}

int main(void) {
	UA_Int32 valor=0;
	signal(SIGINT, stopHandler); /* catches ctrl-c */
   
	UA_Client *client = UA_Client_new();
	UA_ClientConfig_setDefault(UA_Client_getConfig(client));
	UA_StatusCode retval = UA_Client_connect_username(client, "opc.tcp://localhost:4840", "paula", "paula123");
    if(retval != UA_STATUSCODE_GOOD){
		UA_Client_delete(client);
		return (int)retval;
		} 
     
     	
 	UA_Variant value; /* Variants can hold scalar values and arrays of any type */
    UA_Variant_init(&value);

	const UA_NodeId nodeId = UA_NODEID_STRING(1, "V1");
	retval= UA_Client_readValueAttribute(client, nodeId, &value);
	if(retval== UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT32])){
		valor= *(UA_Int32*)value.data;
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Valor es %i\n",valor);
		}
	  
    	
	   /*clean up*/
	   UA_Variant_clear(&value);
    	UA_Client_delete(client); /* Disconnects the client internally */
    	return EXIT_SUCCESS;
}

