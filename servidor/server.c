#include "open62541.h"
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <python2.7/Python.h>

int32_t numberOfParts = 0;//variable contador
#define SLEEP_TIME_MILLIS 1//tiempo de muestreo
static volatile UA_Boolean running = true;

/****************funcion para leer codigo python***********************/
int Usar_python (int x, int i) {
	
	int resultado;
	Py_Initialize();

	PyObject *retorno, *modulo, *clase, *metodo, *argumentos, *objeto;
	modulo = PyImport_ImportModule("script");
	clase = PyObject_GetAttrString(modulo, "Numeros");
	argumentos = Py_BuildValue("ii",x,i);
	objeto = PyEval_CallObject(clase, argumentos);
	metodo = PyObject_GetAttrString(objeto, "suma");
	argumentos = Py_BuildValue("()");
	retorno = PyEval_CallObject(metodo,argumentos);
	PyArg_Parse(retorno, "i", &resultado);
	
	Py_Finalize();
	return resultado;

}

/******************************************************/
static void stopHandler(int sig) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "received ctrl-c");
    running = false;
}

/************usuarios y contraseñas definidos****************/
static UA_UsernamePasswordLogin logins[2] = {
    {UA_STRING_STATIC("peter"), UA_STRING_STATIC("peter123")},
    {UA_STRING_STATIC("paula"), UA_STRING_STATIC("paula123")}
};

/*******************funcion para generar datos**************************/
//Para poder monitorizar el sensor de paso en paralelo crearemos un thread.
void *leer_memoria(void *ptr) {
	UA_Server * server = ptr;
	int utime = SLEEP_TIME_MILLIS;
	while (running == 1){
		numberOfParts = numberOfParts + 1;
		int vp=Usar_python(numberOfParts,5);
		printf("Actualizado sumador: %i\n", vp);
		UA_Variant value;
		UA_Int32 myvalor = (UA_Int32) vp;
		
		UA_Variant_setScalarCopy(&value, &myvalor, &UA_TYPES[UA_TYPES_INT32]);
		UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "V1");
		UA_Server_writeValue(server, myIntegerNodeId, value);
		
		sleep(utime);
		}
}

static void Crear_Variable(UA_Server *server) {	
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "V1");
    UA_QualifiedName DTHName = UA_QUALIFIEDNAME(1,"V1_name");
    /* Define the attribute of the myInteger variable node */
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.description = UA_LOCALIZEDTEXT("en-US","Variable numero 1");
    attr.displayName = UA_LOCALIZEDTEXT("en-US","V1 variable");
    attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    UA_Int32 myIntege = 43;
    UA_Variant_setScalarCopy(&attr.value, &myIntege, &UA_TYPES[UA_TYPES_INT32]);
    
     attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_Server_addVariableNode(server, myIntegerNodeId, UA_NODEID_NUMERIC(0,UA_NS0ID_OBJECTSFOLDER),
                              UA_NODEID_NUMERIC(0,UA_NS0ID_ORGANIZES), DTHName,
                              UA_NODEID_NULL, attr, NULL, NULL);
}

/****************************************************************/

int main(void) {
		int ret;
		pthread_t threadSensor;
    	signal(SIGINT, stopHandler);//funciones de inicio
    	signal(SIGTERM, stopHandler);//funciones de termino

    	UA_Server *server = UA_Server_new(); //se crea el servidor
    	UA_ServerConfig *config = UA_Server_getConfig(server); //configuracion del servidor
    	UA_ServerConfig_setDefault(config);//definicion de parametros 

	 /* verificar usuario y contraseña*/
    	UA_StatusCode retval = UA_AccessControl_default(config, false,
        	&config->securityPolicies[config->securityPoliciesSize-1].policyUri, 2, logins);
    	if(retval != UA_STATUSCODE_GOOD)
        	goto cleanup;
    
		//Creamos he iniciamos una variable en el servidor
    	Crear_Variable(server);
    	//Iniciamor la adquisicion de datos
		if(pthread_create( &threadSensor, NULL, leer_memoria, server)) {
			fprintf(stderr,"Error - pthread_create(): %d\n",ret);
			exit(EXIT_FAILURE);
		}	

    	retval = UA_Server_run(server, &running);//inicio del servidor

 	cleanup:  //cerramos el servidor
    	UA_Server_delete(server);
    	return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}

