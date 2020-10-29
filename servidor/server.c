#include "open62541/build/open62541.h"
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

static volatile UA_Boolean running = true;
int n;
int identificador_variables[5];

/*************************funcion para detener servidor****************************/
static void stopHandler(int sig) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "received ctrl-c");
    running = false;
}

/*******************funciones para buscar id de variables del sensor **************************/
static UA_StatusCode nodeIter(UA_NodeId childId, UA_Boolean isInverse, UA_NodeId referenceTypeId, void *handle) {
    if(isInverse)
        return UA_STATUSCODE_GOOD;
    UA_NodeId *parent = (UA_NodeId *)handle;
    n=n+1;
    identificador_variables[n]=childId.identifier.numeric;
    return UA_STATUSCODE_GOOD;
}

static void buscar_id_nodos(UA_Server *server, char *name){
    n=0;   
    UA_NodeId *parent = UA_NodeId_new();
    *parent = UA_NODEID_NUMERIC(1, 1000);
    UA_Server_forEachChildNodeCall(server, UA_NODEID_STRING(1, name),
                                   nodeIter, (void *) parent);
    UA_NodeId_delete(parent);
}

/*******************funcion para crear un nuevo tipo de objetos, Sensor_Type **************************/
static void Definir_Sensor_type(UA_Server *server) {
    //se crea un nuevo tipo de objeto objetType que denominaremos SensorType
    UA_ObjectTypeAttributes stAttr = UA_ObjectTypeAttributes_default;
    stAttr.description = UA_LOCALIZEDTEXT("en-US", "Objeto del tipo sensor");
    stAttr.displayName = UA_LOCALIZEDTEXT("en-US", "SensorType");
    UA_Server_addObjectTypeNode(server, UA_NODEID_NUMERIC(1, 10000),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                UA_QUALIFIEDNAME(1, "SensorType"), stAttr, NULL, NULL); 
    
    //se crea una variable asociada al SensorType que guarda el tiempo de la lectura del sensor  
    UA_VariableAttributes TimeAttr = UA_VariableAttributes_default;
    TimeAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es la marca de tiempo en que se realiza la lectura");
    TimeAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Tiempo_lectura");
    TimeAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_DateTime Tiempo = UA_DateTime_now();
    UA_Variant_setScalar(&TimeAttr.value, &Tiempo, &UA_TYPES[UA_TYPES_DATETIME]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10001),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Tiempo_lectura"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), TimeAttr, NULL, NULL);
    
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10001),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
                            
    //se crea una variable asociada al SensorType que guarda la lectura de temperatura por el metodo directo                          
    UA_VariableAttributes TFDAttr = UA_VariableAttributes_default;
    TFDAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la temperatura por el metodo directo");
    TFDAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "TF_direct");
    TFDAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_TFD = 0;
    UA_Variant_setScalar(&TFDAttr.value, &Valor_TFD, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10002),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "TF_direct"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), TFDAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10002),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);  
    
    //se crea una variable asociada al SensorType que guarda la lectura de temperatura por el metodo de recuperacion espectral                          
    UA_VariableAttributes TFRSAttr = UA_VariableAttributes_default;
    TFRSAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la temperatura por el metodo de recuperación espectral");
    TFRSAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "TF_rec_spect");
    TFRSAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_TFRS = 0;
    UA_Variant_setScalar(&TFRSAttr.value, &Valor_TFRS, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10003),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "TF_rec_spect"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), TFRSAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10003),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);  
    
    //se crea una variable asociada al SensorType que guarda la lectura de radiacion por temperatura con metodo directo                          
    UA_VariableAttributes RTFDAttr = UA_VariableAttributes_default;
    RTFDAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la radicacion global desde temperatura con metodo directo");
    RTFDAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Radg_TF_direct");
    RTFDAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_RTFD = 0;
    UA_Variant_setScalar(&RTFDAttr.value, &Valor_RTFD, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10004),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Radg_TF_direct"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), RTFDAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10004),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura de radiacion por temperatura de recontruccion espectral                         
    UA_VariableAttributes RTFRSAttr = UA_VariableAttributes_default;
    RTFRSAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la radicacion global desde temperatura con metodo de recuperación espectral");
    RTFRSAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Radg_TF_rec_spect");
    RTFRSAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_RTFRS = 0;
    UA_Variant_setScalar(&RTFRSAttr.value, &Valor_RTFRS, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10005),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Radg_TF_rec_spect"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), RTFRSAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10005),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura de radiacion con recuperacion espectral                          
    UA_VariableAttributes RRSAttr = UA_VariableAttributes_default;
    RRSAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la radiación global por el metodo de recuperación espectral");
    RRSAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Radg_rec_spect");
    RRSAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_RRS = 0;
    UA_Variant_setScalar(&RRSAttr.value, &Valor_RRS, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10006),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Radg_rec_spect"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), RRSAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10006),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura de radiacion con espectrometro                          
    UA_VariableAttributes RTAttr = UA_VariableAttributes_default;
    RTAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la radiacion total por metodo de area bajo la curva del espectro");
    RTAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Radt");
    RTAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_RT = 0;
    UA_Variant_setScalar(&RTAttr.value, &Valor_RT, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10007),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Radt"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), RTAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10007),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura de propension de hollin                           
    UA_VariableAttributes SPAttr = UA_VariableAttributes_default;
    SPAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de propensión de hollin");
    SPAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Soot_propensity");
    SPAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_SP = 0;
    UA_Variant_setScalar(&SPAttr.value, &Valor_SP, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10008),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), SPAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10008),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda el estatus de la camara
    UA_VariableAttributes ECAttr = UA_VariableAttributes_default;
    ECAttr.description =  UA_LOCALIZEDTEXT("en-US", "Estado de la camara false:inactiva-true:activa");
    ECAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Estatus camara");
    ECAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_Boolean EC = UA_FALSE;
    UA_Variant_setScalar(&ECAttr.value, &EC, &UA_TYPES[UA_TYPES_BOOLEAN]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10009),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Estatus camara"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), ECAttr, NULL, NULL);
                              
      UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10009),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);    
    
    //se crea una variable asociada al SensorType que guarda el estatus del espectrometro
    UA_VariableAttributes EEAttr = UA_VariableAttributes_default;
    EEAttr.description =  UA_LOCALIZEDTEXT("en-US", "Estado del espectrometro false:inactivo-true:activo");
    EEAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Estatus espectrometro");
    EEAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_Boolean EE = UA_FALSE;
    UA_Variant_setScalar(&EEAttr.value, &EE, &UA_TYPES[UA_TYPES_BOOLEAN]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10010),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Estatus espectrometro"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), EEAttr, NULL, NULL);
                              
      UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10010),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);    

}

/*******************funcion para crear un nuevo tipo de objetos, Sensor_Type **************************/
static void Crear_variable_sensor(UA_Server *server, char *name) {    
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.description = UA_LOCALIZEDTEXT("en-US", "Sensor de medición de la planta");
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);
    UA_Server_addObjectNode(server, UA_NODEID_STRING(1, name),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                            UA_QUALIFIEDNAME(1, name), UA_NODEID_NUMERIC(1, 10000),
                            oAttr, NULL, NULL);           
   buscar_id_nodos(server,name);
   UA_Variant myVar;
   UA_Variant_init(&myVar);
   
     
   /*********************escribir tiempo de la variable**********************/
   UA_DateTime Tiempo = UA_DateTime_now();
   UA_Variant_setScalarCopy(&myVar, &Tiempo, &UA_TYPES[UA_TYPES_DATETIME]);
   UA_Server_writeValue(server, UA_NODEID_NUMERIC(1, identificador_variables[1]), myVar);
   
   
   /*********************escribir Valor de la variable**********************/
   /*UA_Variant_setScalarCopy(&myVar, &Valor, &UA_TYPES[UA_TYPES_UINT32]);
   UA_Server_writeValue(server, UA_NODEID_NUMERIC(1, identificador_variables[4]), myVar);
   
   /*********************escribir Status de la variable**********************/
   /*UA_Boolean status = UA_FALSE;
   UA_Variant_setScalarCopy(&myVar, &status, &UA_TYPES[UA_TYPES_BOOLEAN]);
   UA_Server_writeValue(server, UA_NODEID_NUMERIC(1, identificador_variables[5]), myVar);*/
}



/****************************************************************/

int main(void) {
	   	signal(SIGINT, stopHandler);//funciones de inicio
    	signal(SIGTERM, stopHandler);//funciones de termino

    	UA_Server *server = UA_Server_new(); //se crea el servidor
    	UA_ServerConfig *config = UA_Server_getConfig(server); //configuracion del servidor
    	UA_ServerConfig_setDefault(config);//definicion de parametros 

    
      Definir_Sensor_type(server);
      Crear_variable_sensor(server, "sensor 10");
	
   
    	 UA_StatusCode retval = UA_Server_run(server, &running);//inicio del servidor


    	UA_Server_delete(server);
    	return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}

